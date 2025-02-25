
#include "hdl_iface.h"

typedef struct {
  coroutine_t worker;
  hdl_i2c_message_t i2c_msg;
  hdl_nvm_message_t *nvm_msg;
  uint16_t mem_addr;
  uint8_t state;
  uint32_t burn_time;
} hdl_eeprom_i2c_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_eeprom_i2c_var_t, *((hdl_eeprom_i2c_t *)0)->obj_var, HDL_EEPROM_I2C_VAR_SIZE, hdl_eeprom_i2c.h);

#define EE_STATE_IDLE                  0
#define EE_STATE_SET_MEM_ADDR_MSG      1
#define EE_STATE_TX_MEM_ADDR_MSG       2
#define EE_STATE_AWAIT_MEM_ADDR_MSG    3
#define EE_STATE_TX_DATA_MSG           4
#define EE_STATE_AWAIT_DATA_MSG        5
#define EE_STATE_AWAIT_BURNING         6
#define EE_STATE_COMPLETE              7


static uint8_t _eeprom_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_eeprom_i2c_t *eeprom = (hdl_eeprom_i2c_t *)arg;
  hdl_eeprom_i2c_var_t *eeprom_var = (hdl_eeprom_i2c_var_t *)eeprom->obj_var;
  switch (eeprom_var->state) {
    case EE_STATE_SET_MEM_ADDR_MSG: {
      eeprom_var->i2c_msg.address = eeprom->config->chip_address;
      eeprom_var->mem_addr = eeprom_var->nvm_msg->address + eeprom_var->nvm_msg->transferred;
      eeprom_var->mem_addr = (eeprom_var->mem_addr << 8) | (eeprom_var->mem_addr >> 8);
      eeprom_var->i2c_msg.buffer = (uint8_t *)&(eeprom_var->mem_addr);
      eeprom_var->i2c_msg.length = 2;
      eeprom_var->i2c_msg.options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR;
      eeprom_var->state = EE_STATE_TX_MEM_ADDR_MSG;
      break;
    }

    case EE_STATE_TX_MEM_ADDR_MSG: {
      hdl_i2c_t *bus = (hdl_i2c_t *)eeprom->dependencies[0]; 
      if(hdl_take(bus, eeprom) && hdl_i2c_transfer(bus, &eeprom_var->i2c_msg)) eeprom_var->state = EE_STATE_AWAIT_MEM_ADDR_MSG;
      break;
    }

    case EE_STATE_AWAIT_MEM_ADDR_MSG: {
      if(eeprom_var->i2c_msg.status & HDL_I2C_MESSAGE_STATUS_COMPLETE) {
        if(eeprom_var->i2c_msg.status & (HDL_I2C_MESSAGE_FAULT_ARBITRATION_LOST | HDL_I2C_MESSAGE_FAULT_BUS_ERROR | HDL_I2C_MESSAGE_FAULT_BAD_STATE)) {
          eeprom_var->nvm_msg->status = HDL_NVM_ERROR_BUS_FAULT;
          eeprom_var->state = EE_STATE_COMPLETE;
          eeprom_var->nvm_msg = NULL;
        }
        else if(eeprom_var->i2c_msg.status & HDL_I2C_MESSAGE_STATUS_NACK) {
          eeprom_var->nvm_msg->status = HDL_NVM_ERROR_INTERNAL_FAULT;
          eeprom_var->state = EE_STATE_COMPLETE;
          eeprom_var->nvm_msg = NULL;
        }
        else {
          eeprom_var->i2c_msg.buffer = (eeprom_var->nvm_msg->buffer + eeprom_var->nvm_msg->transferred);
          eeprom_var->i2c_msg.length = eeprom_var->nvm_msg->size - eeprom_var->nvm_msg->transferred;
          eeprom_var->i2c_msg.options = HDL_I2C_MESSAGE_STOP;
          if(!(eeprom_var->nvm_msg->options & HDL_NVM_OPTION_WRITE)) {
            eeprom_var->i2c_msg.options |= HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR | HDL_I2C_MESSAGE_MRSW | HDL_I2C_MESSAGE_NACK_LAST;
          }
          else {
            if(eeprom_var->i2c_msg.length > eeprom->config->page_size) {
              eeprom_var->i2c_msg.length = eeprom->config->page_size;
            }
          }
          eeprom_var->state = EE_STATE_TX_DATA_MSG;
        }
      }
      break;
    }

    case EE_STATE_TX_DATA_MSG: {
      hdl_i2c_t *bus = (hdl_i2c_t *)eeprom->dependencies[0]; 
      if(hdl_i2c_transfer(bus, &eeprom_var->i2c_msg)) eeprom_var->state = EE_STATE_AWAIT_DATA_MSG;
      break;
    }

    case EE_STATE_AWAIT_DATA_MSG: {
      if(eeprom_var->i2c_msg.status & HDL_I2C_MESSAGE_STATUS_COMPLETE) {
        hdl_nvm_message_t *data = eeprom_var->nvm_msg;
        data->transferred += eeprom_var->i2c_msg.transferred;
        eeprom_var->state = EE_STATE_COMPLETE;
        if(eeprom_var->i2c_msg.status & (HDL_I2C_MESSAGE_FAULT_ARBITRATION_LOST | HDL_I2C_MESSAGE_FAULT_BUS_ERROR | HDL_I2C_MESSAGE_FAULT_BAD_STATE)) {
          data->status |= HDL_NVM_ERROR_BUS_FAULT;
          break;
        }
        if(data->options & HDL_NVM_OPTION_WRITE) {
          if(eeprom_var->i2c_msg.status & HDL_I2C_MESSAGE_STATUS_NACK) {
            data->status |= HDL_NVM_ERROR_INTERNAL_FAULT;
            break;
          }
          hdl_i2c_t *bus = (hdl_i2c_t *)eeprom->dependencies[0]; 
          hdl_give(bus, eeprom);
          eeprom_var->state = EE_STATE_AWAIT_BURNING;
          hdl_time_counter_t *time_cnt = (hdl_time_counter_t *)eeprom->dependencies[1];
          eeprom_var->burn_time = hdl_time_counter_get(time_cnt);
        }
      }
      break;
    }

    case EE_STATE_AWAIT_BURNING: {
      hdl_time_counter_t *time_cnt = (hdl_time_counter_t *)eeprom->dependencies[1];
      uint32_t now = hdl_time_counter_get(time_cnt);
      if(TIME_ELAPSED(eeprom_var->burn_time, eeprom->config->write_time, now)) {
        if(eeprom_var->nvm_msg->transferred < eeprom_var->nvm_msg->size) {
          eeprom_var->state = EE_STATE_SET_MEM_ADDR_MSG;
        }
        else {
          eeprom_var->state = EE_STATE_COMPLETE;
        }
      }
      break;
    }

    case EE_STATE_COMPLETE: {
      hdl_i2c_t *bus = (hdl_i2c_t *)eeprom->dependencies[0]; 
      hdl_give(bus, eeprom);
      if(eeprom_var->nvm_msg->options & HDL_NVM_OPTION_WRITE) {
        hdl_gpio_pin_t *wp_pin = (hdl_gpio_pin_t *)eeprom->dependencies[2];
        hdl_gpio_set_inactive(wp_pin);
      }
      eeprom_var->nvm_msg->status |= HDL_NVM_STATE_COMPLETE;
      eeprom_var->nvm_msg->status &= ~HDL_NVM_STATE_BUSY;
      eeprom_var->nvm_msg = NULL;
      eeprom_var->state = EE_STATE_IDLE;
      break;
    }

    case EE_STATE_IDLE:
    default:
      if(eeprom_var->nvm_msg != NULL) {
        if(eeprom_var->nvm_msg->options & HDL_NVM_OPTION_WRITE) {
          hdl_gpio_pin_t *wp_pin = (hdl_gpio_pin_t *)eeprom->dependencies[2];
          if(!hdl_is_null_module(wp_pin) && hdl_gpio_is_active(wp_pin)) break;
        }
        eeprom_var->state = EE_STATE_SET_MEM_ADDR_MSG;
      }
      break;
  }
  return cancel;
}

static hdl_module_state_t _hdl_eeprom_i2c(const void *desc, uint8_t enable) {
  hdl_eeprom_i2c_t *eeprom = (hdl_eeprom_i2c_t *)desc;
  hdl_eeprom_i2c_var_t *eeprom_var = (hdl_eeprom_i2c_var_t *)eeprom->obj_var;
  if(enable) {
    hdl_gpio_pin_t *wp_pin = (hdl_gpio_pin_t *)eeprom->dependencies[2];
    hdl_gpio_set_active(wp_pin);
    coroutine_add(&eeprom_var->worker, &_eeprom_worker, (void*)eeprom);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&eeprom_var->worker);
  return HDL_MODULE_UNLOADED;
}

static hdl_eeprom_i2c_t *_hdl_eeprom_i2c_check(hdl_eeprom_i2c_t *eeprom, hdl_nvm_message_t *message) {
  hdl_eeprom_i2c_var_t *eeprom_var = (hdl_eeprom_i2c_var_t *)eeprom->obj_var;
  if((message == NULL) || (message->size == 0) || (message->buffer == NULL) || 
     (eeprom == NULL) || (hdl_state(eeprom) == HDL_MODULE_FAULT) || (eeprom_var->nvm_msg != NULL))
    return NULL;
  return eeprom;
}

static uint8_t _hdl_eeprom_i2c_transfer(const void *desc, hdl_nvm_message_t *message) {
  hdl_eeprom_i2c_t *eeprom = (hdl_eeprom_i2c_t *)desc;
  hdl_eeprom_i2c_var_t *eeprom_var = (hdl_eeprom_i2c_var_t *)eeprom->obj_var;
  eeprom = _hdl_eeprom_i2c_check(eeprom, message);
  if(eeprom != NULL) {
    message->status = HDL_NVM_STATE_BUSY;
    message->transferred = 0;
    eeprom_var->nvm_msg = message;
    if(message->options & HDL_NVM_OPTION_WRITE) {
      hdl_gpio_pin_t *wp_pin = (hdl_gpio_pin_t *)eeprom->dependencies[2];
      hdl_gpio_set_inactive(wp_pin);
    }
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static uint8_t _hdl_epprom_i2c_info_get(const void *desc, hdl_nvm_info_t *out_info) {
  hdl_eeprom_i2c_t *eeprom = (hdl_eeprom_i2c_t *)desc;
  if(out_info != NULL) {
    out_info->page_size = 1;
    out_info->volume = eeprom->config->size;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

const hdl_nvm_iface_t hdl_eeprom_iface = {
  .init = &_hdl_eeprom_i2c,
  .transfer = &_hdl_eeprom_i2c_transfer,
  .info = &_hdl_epprom_i2c_info_get
};
