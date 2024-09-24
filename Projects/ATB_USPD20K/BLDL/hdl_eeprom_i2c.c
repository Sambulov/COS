
#include "app.h"
#include "CodeLib.h"

typedef struct {
  hdl_module_t module;
  const hdl_eeprom_i2c_config_t *config;
  /* private */
  coroutine_t worker;
  hdl_i2c_message_t i2c_msg;
  hdl_eeprom_i2c_message_t *eeprom_msg;
  uint16_t mem_addr;
  uint8_t state;
  uint32_t burn_time;
} hdl_eeprom_i2c_private_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_eeprom_i2c_private_t, hdl_eeprom_i2c_t, HDL_EEPROM_I2C_PRV_SIZE, hdl_eeprom_i2c.h);

#define EE_STATE_IDLE                  0
#define EE_STATE_SET_MEM_ADDR_MSG      1
#define EE_STATE_TX_MEM_ADDR_MSG       2
#define EE_STATE_AWAIT_MEM_ADDR_MSG    3
#define EE_STATE_TX_DATA_MSG           4
#define EE_STATE_AWAIT_DATA_MSG        5
#define EE_STATE_AWAIT_BURNING         6


static uint8_t _eeprom_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  hdl_eeprom_i2c_private_t *eeprom = (hdl_eeprom_i2c_private_t *)arg;
  switch (eeprom->state) {
    case EE_STATE_SET_MEM_ADDR_MSG: {
      eeprom->i2c_msg.address = eeprom->config->chip_address;
      uint16_t addr_msb = eeprom->eeprom_msg->offset + eeprom->eeprom_msg->transferred;
      eeprom->mem_addr = (addr_msb >> 8) | (addr_msb << 8);
      eeprom->i2c_msg.buffer = (uint8_t *)&(eeprom->mem_addr);
      volatile uint8_t x = eeprom->i2c_msg.buffer[0];
      volatile uint8_t y = eeprom->i2c_msg.buffer[1];
      eeprom->i2c_msg.length = 2;
      eeprom->i2c_msg.options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR;
      eeprom->state = EE_STATE_TX_MEM_ADDR_MSG;
      break;
    }

    case EE_STATE_TX_MEM_ADDR_MSG: {
      hdl_i2c_t *bus = (hdl_i2c_t *)eeprom->module.dependencies[0]; 
      if(hdl_i2c_transfer_message(bus, &eeprom->i2c_msg)) eeprom->state = EE_STATE_AWAIT_MEM_ADDR_MSG;
      break;
    }

    case EE_STATE_AWAIT_MEM_ADDR_MSG: {
      if(eeprom->i2c_msg.status & HDL_I2C_MESSAGE_STATUS_COMPLETE) {
        if(eeprom->i2c_msg.status & (HDL_I2C_MESSAGE_FAULT_ARBITRATION_LOST | HDL_I2C_MESSAGE_FAULT_BUS_ERROR | HDL_I2C_MESSAGE_FAULT_BAD_STATE)) {
          eeprom->eeprom_msg->state |= HDL_EEPROM_MSG_ERROR_BUS | HDL_EEPROM_MSG_STATUS_COMPLETE;
          eeprom->state = EE_STATE_IDLE;
          eeprom->eeprom_msg = NULL;
        }
        else if(eeprom->i2c_msg.status & HDL_I2C_MESSAGE_STATUS_NACK) {
          eeprom->eeprom_msg->state |= HDL_EEPROM_MSG_ERROR_NO_RESPONSE | HDL_EEPROM_MSG_STATUS_COMPLETE;
          eeprom->state = EE_STATE_IDLE;
          eeprom->eeprom_msg = NULL;
        }
        else {
          eeprom->i2c_msg.address = eeprom->config->chip_address;
          eeprom->i2c_msg.buffer = (eeprom->eeprom_msg->buffer + eeprom->eeprom_msg->transferred);
          eeprom->i2c_msg.length = eeprom->eeprom_msg->count - eeprom->eeprom_msg->transferred;
          eeprom->i2c_msg.options = HDL_I2C_MESSAGE_STOP;
          if(!(eeprom->eeprom_msg->state & HDL_EEPROM_MSG_OPTION_WRITE)) {
            eeprom->i2c_msg.options |= HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR | HDL_I2C_MESSAGE_MRSW | HDL_I2C_MESSAGE_NACK_LAST;
          }
          else {
            if(eeprom->i2c_msg.length > eeprom->config->block_size) {
              eeprom->i2c_msg.length = eeprom->config->block_size;
            }
          }
          eeprom->state = EE_STATE_TX_DATA_MSG;
        }
      }
      break;
    }

    case EE_STATE_TX_DATA_MSG: {
      hdl_i2c_t *bus = (hdl_i2c_t *)eeprom->module.dependencies[0]; 
      if(hdl_i2c_transfer_message(bus, &eeprom->i2c_msg)) eeprom->state = EE_STATE_AWAIT_DATA_MSG;
      break;
    }

    case EE_STATE_AWAIT_DATA_MSG: {
      if(eeprom->i2c_msg.status & HDL_I2C_MESSAGE_STATUS_COMPLETE) {
        hdl_eeprom_i2c_message_t *msg = eeprom->eeprom_msg;
        msg->transferred += eeprom->i2c_msg.transferred;
        eeprom->state = EE_STATE_IDLE;
        eeprom->eeprom_msg = NULL;
        msg->state |= HDL_EEPROM_MSG_STATUS_COMPLETE;
        if(eeprom->i2c_msg.status & (HDL_I2C_MESSAGE_FAULT_ARBITRATION_LOST | HDL_I2C_MESSAGE_FAULT_BUS_ERROR | HDL_I2C_MESSAGE_FAULT_BAD_STATE)) {
          msg->state |= HDL_EEPROM_MSG_ERROR_BUS;
          break;
        }
        if(eeprom->i2c_msg.status & HDL_I2C_MESSAGE_STATUS_NACK) {
          msg->state |= HDL_EEPROM_MSG_ERROR_NACK;
          break;
        }
        if(msg->state & HDL_EEPROM_MSG_OPTION_WRITE) {
          eeprom->state = EE_STATE_AWAIT_BURNING;
          eeprom->eeprom_msg = msg;
          msg->state &= ~HDL_EEPROM_MSG_STATUS_COMPLETE;
          hdl_time_counter_t *time_cnt = (hdl_time_counter_t *)eeprom->module.dependencies[1];
          eeprom->burn_time = hdl_time_counter_get(time_cnt);
        }
      }
      break;
    }

    case EE_STATE_AWAIT_BURNING: {
      hdl_time_counter_t *time_cnt = (hdl_time_counter_t *)eeprom->module.dependencies[1];
      uint32_t now = hdl_time_counter_get(time_cnt);
      if(TIME_ELAPSED(eeprom->burn_time, eeprom->config->write_time, now)) {
        if(eeprom->eeprom_msg->transferred < eeprom->eeprom_msg->count) {
          eeprom->state = EE_STATE_SET_MEM_ADDR_MSG;
        }
        else {
          eeprom->eeprom_msg->state |= HDL_EEPROM_MSG_STATUS_COMPLETE;
          eeprom->state = EE_STATE_IDLE;
          eeprom->eeprom_msg = NULL;
        }
      }
      break;
    }

    case EE_STATE_IDLE:
    default:
      if(eeprom->eeprom_msg != NULL) {
        eeprom->state = EE_STATE_SET_MEM_ADDR_MSG;
      }
      break;
  }
  return cancel;
}

hdl_module_state_t hdl_eeprom_i2c(void *desc, uint8_t enable) {
  hdl_eeprom_i2c_private_t *eeprom = (hdl_eeprom_i2c_private_t *)desc;
  if(enable) {
    coroutine_add(&eeprom->worker, &_eeprom_worker, (void*)eeprom);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&eeprom->worker);
  return HDL_MODULE_UNLOADED;
}

uint8_t hdl_eeprom_i2c_transfer(hdl_eeprom_i2c_t *desc, hdl_eeprom_i2c_message_t *msg) {
  hdl_eeprom_i2c_private_t *eeprom = (hdl_eeprom_i2c_private_t *)desc;
  if((msg == NULL) || (msg->count == 0) || (msg->buffer == NULL) || 
     (eeprom == NULL) || (hdl_state(&eeprom->module) == HDL_MODULE_FAULT) || (eeprom->eeprom_msg != NULL))
    return HDL_FALSE;
  msg->state |= HDL_EEPROM_MSG_STATUS_PROCESSING;
  msg->transferred = 0;
  eeprom->eeprom_msg = msg;
  return HDL_TRUE;
}
