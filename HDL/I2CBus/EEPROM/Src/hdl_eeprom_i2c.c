#include "hdl_iface.h"

typedef struct {
  coroutine_t worker;
  hdl_nvm_message_t *nvm_msg;
  uint16_t mem_addr;
  uint8_t mode   : 2,
          cancel : 1,
          state  : 5;
  uint32_t burn_time;
} hdl_eeprom_i2c_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_eeprom_i2c_var_t, *((hdl_eeprom_i2c_t *)0)->obj_var, HDL_EEPROM_I2C_VAR_SIZE, hdl_eeprom_i2c.h);

#define EE_STATE_IDLE                  0
#define EE_STATE_READ                  1
#define EE_STATE_WRITE                 2
#define EE_STATE_ERASE                 3
#define EE_STATE_WP_RESET              4
#define EE_STATE_AWAIT_BUS             5
#define EE_STATE_AWAIT_BURNING         6
#define EE_STATE_COMPLETE              7

#define EE_MODE_READ    1
#define EE_MODE_WRITE   2
#define EE_MODE_ERASE   3

static uint8_t _eeprom_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_eeprom_i2c_t *eeprom = (hdl_eeprom_i2c_t *)arg;
  hdl_eeprom_i2c_var_t *eeprom_var = (hdl_eeprom_i2c_var_t *)eeprom->obj_var;
  switch (eeprom_var->state) {

    case EE_STATE_AWAIT_BURNING: {
      hdl_time_counter_t *time_cnt = (hdl_time_counter_t *)eeprom->dependencies[1];
      uint32_t now = hdl_time_counter_get(time_cnt);
      if(!CL_TIME_ELAPSED(eeprom_var->burn_time, eeprom->config->write_time, now)) 
        break;
      eeprom_var->state = EE_STATE_COMPLETE;
      uint32_t size = eeprom_var->nvm_msg->size - eeprom_var->nvm_msg->synced_size;
      if(eeprom_var->mode == EE_MODE_ERASE) {
        eeprom_var->nvm_msg->synced_size += CL_MIN(16, size);
        if(eeprom_var->nvm_msg->synced_size != eeprom_var->nvm_msg->size)
          eeprom_var->state = EE_STATE_ERASE;
      }
      else {
        if(size > eeprom->config->page_size) size = eeprom->config->page_size;
        eeprom_var->nvm_msg->synced_size += size;
        if(eeprom_var->nvm_msg->synced_size != eeprom_var->nvm_msg->size)
          eeprom_var->state = EE_STATE_WRITE;
      }
      break;
    }

    case EE_STATE_COMPLETE: {
      hdl_gpio_pin_t *wp_pin = (hdl_gpio_pin_t *)eeprom->dependencies[2];
      hdl_gpio_set_active(wp_pin);
      eeprom_var->nvm_msg->out_status |= HDL_NVM_STATE_COMPLETE;
      eeprom_var->nvm_msg->out_status &= ~HDL_NVM_STATE_BUSY;
      eeprom_var->nvm_msg = NULL;
      eeprom_var->state = EE_STATE_IDLE;
      break;
    }

    case EE_STATE_AWAIT_BUS: {
      hdl_i2c_mem_t *mem = (hdl_i2c_mem_t *)eeprom->dependencies[0];
      hdl_i2c_mem_state_t state = hdl_i2c_mem_state(mem);
      if(state & HDL_I2C_MEM_BUSY) break;
      eeprom_var->state = EE_STATE_COMPLETE;
      if(state == HDL_I2C_MEM_XFER_OK) {
        if(eeprom_var->mode == EE_MODE_READ)
          eeprom_var->nvm_msg->synced_size = eeprom_var->nvm_msg->size;
        else {
          hdl_time_counter_t *time_cnt = (hdl_time_counter_t *)eeprom->dependencies[1];
          eeprom_var->burn_time = hdl_time_counter_get(time_cnt);
          eeprom_var->state = EE_STATE_AWAIT_BURNING;
        }
      }
      else
        eeprom_var->nvm_msg->out_status |= HDL_NVM_ERROR_BUS_FAULT;
      hdl_give(mem, eeprom);
      break;
    }

    case EE_STATE_READ: {
      hdl_i2c_mem_t *mem = (hdl_i2c_mem_t *)eeprom->dependencies[0];
      if(!hdl_take(mem, eeprom)) break;
      uint16_t chip_address = eeprom->config->chip_address;
      uint32_t addr = eeprom_var->nvm_msg->address;
      while (addr > 0xffff) {
        addr -= 0x10000;
        chip_address++;
      }
      if(!hdl_i2c_mem_read_r2(mem, chip_address, 
        addr,
        eeprom_var->nvm_msg->data,
        eeprom_var->nvm_msg->size)) break;
      eeprom_var->state = EE_STATE_AWAIT_BUS;
      break;
    }

    case EE_STATE_WRITE: {
      hdl_i2c_mem_t *mem = (hdl_i2c_mem_t *)eeprom->dependencies[0];
      if(!hdl_take(mem, eeprom)) break;
      uint16_t chip_address = eeprom->config->chip_address;
      if(eeprom_var->nvm_msg->address > 0xffff)
        chip_address++;
      uint32_t addr = eeprom_var->nvm_msg->address + eeprom_var->nvm_msg->synced_size;
      uint32_t size = eeprom_var->nvm_msg->size - eeprom_var->nvm_msg->synced_size;
      if(size > eeprom->config->page_size) size = eeprom->config->page_size;
      uint8_t *data_ptr = &eeprom_var->nvm_msg->data[eeprom_var->nvm_msg->synced_size];
      if(!hdl_i2c_mem_write_r2(mem, chip_address, addr, data_ptr, size)) 
        break;
      eeprom_var->state = EE_STATE_AWAIT_BUS;
      break;
    }

    case EE_STATE_ERASE: {
      static const uint8_t dummy[16]= {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, };
      hdl_i2c_mem_t *mem = (hdl_i2c_mem_t *)eeprom->dependencies[0];
      if(!hdl_take(mem, eeprom)) break;
      uint16_t chip_address = eeprom->config->chip_address;
      if(eeprom_var->nvm_msg->address > 0xffff)
        chip_address++;
      uint32_t addr = eeprom_var->nvm_msg->address + eeprom_var->nvm_msg->synced_size;
      uint32_t size = eeprom_var->nvm_msg->size - eeprom_var->nvm_msg->synced_size;
      if(size > 16) size = 16;
      if(!hdl_i2c_mem_write_r2(mem, chip_address, addr, dummy, size)) 
        break;
      eeprom_var->state = EE_STATE_AWAIT_BUS;
      break;
    }
    
    case EE_STATE_WP_RESET: {
      hdl_gpio_pin_t *wp_pin = (hdl_gpio_pin_t *)eeprom->dependencies[2];
      if(!hdl_is_null_module(wp_pin) && hdl_gpio_is_active(wp_pin)) break;
      eeprom_var->state = EE_STATE_WRITE;
      if(eeprom_var->mode == EE_MODE_ERASE) eeprom_var->state = EE_STATE_ERASE;
      break;
    }

    case EE_STATE_IDLE:
    default:
      if(eeprom_var->nvm_msg == NULL) break;
      if((eeprom_var->nvm_msg->size == 0) || 
         ((eeprom_var->nvm_msg->data == NULL) && (eeprom_var->mode != EE_MODE_ERASE))) {
        eeprom_var->nvm_msg->out_status |= HDL_NVM_ERROR_BAD_ARG;
        eeprom_var->state = EE_STATE_COMPLETE;
        break;
      }
      eeprom_var->state = EE_STATE_READ;
      if(eeprom_var->mode != EE_MODE_READ) {
        hdl_gpio_pin_t *wp_pin = (hdl_gpio_pin_t *)eeprom->dependencies[2];
        hdl_gpio_set_inactive(wp_pin);
        eeprom_var->state = EE_STATE_WP_RESET;
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

static uint8_t _hdl_eeprom_i2c_transfer(const void *desc, hdl_nvm_message_t *message, uint8_t mode) {
  hdl_eeprom_i2c_t *eeprom = (hdl_eeprom_i2c_t *)desc;
  hdl_eeprom_i2c_var_t *eeprom_var = (hdl_eeprom_i2c_var_t *)eeprom->obj_var;
  if((message == NULL) || (eeprom_var->nvm_msg != NULL)) return HDL_FALSE;
  message->out_status = HDL_NVM_STATE_BUSY;
  message->synced_size = 0;
  eeprom_var->nvm_msg = message;
  eeprom_var->mode = mode;
  return HDL_TRUE;
}

static uint8_t _hdl_eeprom_i2c_read(const void *desc, hdl_nvm_message_t *message) {
  return _hdl_eeprom_i2c_transfer(desc, message, EE_MODE_READ);
}

static uint8_t _hdl_eeprom_i2c_write(const void *desc, hdl_nvm_message_t *message) {
  return _hdl_eeprom_i2c_transfer(desc, message, EE_MODE_WRITE);
}

static uint8_t _hdl_eeprom_i2c_erase(const void *desc, hdl_nvm_message_t *message) {
  return _hdl_eeprom_i2c_transfer(desc, message, EE_MODE_ERASE);
}

static uint8_t _hdl_eeprom_i2c_cancel(const void *desc) {
  hdl_eeprom_i2c_t *eeprom = (hdl_eeprom_i2c_t *)desc;
  hdl_eeprom_i2c_var_t *eeprom_var = (hdl_eeprom_i2c_var_t *)eeprom->obj_var;
  if(eeprom_var->nvm_msg == NULL) return HDL_TRUE;
  eeprom_var->cancel = 1;
  return HDL_FALSE;
}

static uint8_t _hdl_epprom_i2c_info_get(const void *desc, hdl_nvm_info_t *out_info) {
  hdl_eeprom_i2c_t *eeprom = (hdl_eeprom_i2c_t *)desc;
  if(out_info != NULL) {
    out_info->sector_size = 1;
    out_info->mtu_size = eeprom->config->page_size;
    out_info->volume = eeprom->config->size;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

const hdl_nvm_iface_t hdl_eeprom_iface = {
  .init = &_hdl_eeprom_i2c,
  .read = &_hdl_eeprom_i2c_read,
  .write = &_hdl_eeprom_i2c_write,
  .erase = &_hdl_eeprom_i2c_erase,
  .info = &_hdl_epprom_i2c_info_get,
  .cancel = &_hdl_eeprom_i2c_cancel
};
