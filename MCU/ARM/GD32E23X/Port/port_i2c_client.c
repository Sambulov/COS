// typedef struct {
//   /* private */
//   PRIVATE(hdl, HDL_I2C_MESSAGE_PRV_SIZE);
//   uint16_t transfered;
//   hdl_i2c_message_state_t state;
//   uint32_t ovn;
//   /* public */
//   uint16_t address;
//   uint8_t *buffer;
//   uint16_t buffer_size;
//   hdl_i2c_message_options_t options;
// } hdl_i2c_message_private_t;


// // _Static_assert(sizeof(hdl_i2c_message_private_t) == sizeof(hdl_i2c_message_t), "In hdl_i2c.h data structure size of hdl_i2c_message_t doesn't match, check HDL_I2C_MESSAGE_PRV_SIZE");
// // _Static_assert((offsetof(hdl_i2c_message_private_t, address) == offsetof(hdl_i2c_message_t, address)) ||
// //                (offsetof(hdl_i2c_message_private_t, options) == offsetof(hdl_i2c_message_t, options)) ||
// //                (offsetof(hdl_i2c_message_private_t, buffer) == offsetof(hdl_i2c_message_t, buffer)) ||
// //                (offsetof(hdl_i2c_message_private_t, buffer_size) == offsetof(hdl_i2c_message_t, buffer_size)), "In hdl_i2c.h hdl_i2c_message_t properties order doesn't match");

// // _Static_assert(sizeof(hdl_i2c_transaction_private_t) == sizeof(hdl_i2c_transaction_t), "In hdl_i2c.h data structure size of hdl_i2c_transaction_t doesn't match, check HDL_I2C_TRANSACTION_PRV_SIZE");

// // _Static_assert(sizeof(hdl_i2c_client_private_t) == sizeof(hdl_i2c_client_t), "In hdl_i2c.h data structure size of hdl_i2c_client_t doesn't match, check HDL_I2C_PRV_SIZE");
// // _Static_assert((offsetof(hdl_i2c_client_private_t, hw_conf) == offsetof(hdl_i2c_client_t, hw_conf)) , "In hdl_i2c.h hdl_i2c_client_t properties order doesn't match");

#include "hdl_portable.h"
#include "CodeLib.h"

typedef struct {
  __IO uint32_t CTL0;            /*!< I2C Control register 0,                Address offset: 0x00 */
  __IO uint32_t CTL1;            /*!< I2C Control register 1,                Address offset: 0x04 */
  __IO uint32_t SADDR0;          /*!< I2C Slave address register 0,          Address offset: 0x08 */
  __IO uint32_t SADDR1;          /*!< I2C Slave address register 1,          Address offset: 0x0C */
  __IO uint32_t DATA;            /*!< I2C Transfer buffer register,          Address offset: 0x10 */
  __IO uint32_t STAT0;           /*!< I2C Transfer status register 0,        Address offset: 0x14 */
  __IO uint32_t STAT1;           /*!< I2C Transfer status register 1,        Address offset: 0x18 */
  __IO uint32_t CKCFG;           /*!< I2C Clock configure register,          Address offset: 0x1C */
  __IO uint32_t RT;              /*!< I2C Rise time register,                Address offset: 0x20*/
  __IO uint32_t _dummy24_7C[23]; 
  __IO uint32_t SAMCS;           /*!< I2C SAM control and status register,   Address offset: 0x80 */
  __IO uint32_t _dummy84_8C[3];
  __IO uint32_t FMPCFG;          /*!< I2C Fast mode plus configure register, Address offset: 0x90 */
} i2c_periph_t;

typedef struct {
  hdl_module_t module;
  hdl_i2c_client_config_t *config;
  /* private */
  hdl_delegate_t ev_isr;
  hdl_delegate_t er_isr;
  coroutine_desc_static_t i2c_client_task_buff;
  hdl_i2c_message_t *message;
} hdl_i2c_client_private_t;

_Static_assert(sizeof(hdl_i2c_client_private_t) == sizeof(hdl_i2c_client_t), "In hdl_i2c.h data structure size of hdl_i2c_client_t doesn't match, check HDL_I2C_CLIENT_PRV_SIZE");

static void event_i2c_ev_isr(uint32_t event, void *sender, void *context) {
  hdl_i2c_client_private_t *i2c = (hdl_i2c_client_private_t *)context;
}

static void event_i2c_er_isr(uint32_t event, void *sender, void *context) {
  hdl_i2c_client_private_t *i2c = (hdl_i2c_client_private_t *)context;
}

static uint8_t _hdl_reg_wait_condition(hdl_timer_t *timer, __IO uint32_t *reg, uint32_t flags, uint8_t is_set, uint32_t timeout) {
  uint32_t time_stamp = hdl_timer_get(timer);
  if(is_set) {
    while ((*reg & flags) != flags){
      if(TIME_ELAPSED(time_stamp, timeout, hdl_timer_get(timer)))
        return 0;
    }
    return 1;
  }
  else {
    flags = ~flags;
    while ((*reg | flags) != flags) {
      if(TIME_ELAPSED(time_stamp, timeout, hdl_timer_get(timer)))
        return 0;
    }
    return 1;
  }
  return 0;
}

void _i2c_set_bus_to_default(i2c_periph_t *i2c_periph) {
  uint32_t tmp = 0;
  i2c_periph->CTL0 &= ~I2C_CTL0_ACKEN;
  tmp = i2c_periph->STAT0;
  tmp = i2c_periph->STAT1; 
  while (i2c_periph->STAT0 & I2C_STAT0_RBNE)
    tmp = i2c_periph->DATA;
}

hdl_i2c_message_status_t _i2c_msg_start_handler(hdl_i2c_client_private_t *i2c) {
  hdl_timer_t *timer = (hdl_timer_t *)i2c->module.dependencies[4];
  i2c_periph_t *i2c_periph = (i2c_periph_t *)i2c->module.reg;
  i2c_periph->CTL0 &= ~I2C_CTL0_SS;
  _i2c_set_bus_to_default(i2c_periph);
  /* send a start condition to I2C bus */
  i2c_periph->CTL0 |= I2C_CTL0_START;
  /* wait until SBSEND bit is set */
  if(!(_hdl_reg_wait_condition(timer, &i2c_periph->STAT0, I2C_STAT0_SBSEND, 1, 10))) {
    i2c_periph->CTL0 &= ~I2C_CTL0_START;
    return HDL_I2C_MESSAGE_FAULT_BUS_ERROR;
  }
  return HDL_I2C_MESSAGE_STATUS_START_ON_BUS;
}

hdl_i2c_message_status_t _i2c_msg_addr_handler(hdl_i2c_client_private_t *i2c, hdl_i2c_message_t *message) {
  hdl_timer_t *timer = (hdl_timer_t *)i2c->module.dependencies[4];
  i2c_periph_t *i2c_periph = (i2c_periph_t *)i2c->module.reg;
  if(!(i2c_periph->STAT0 & I2C_STAT0_SBSEND)) return HDL_I2C_MESSAGE_FAULT_BAD_STATE;
  /* send slave address */
  i2c_periph->DATA = (message->address << 1) | ((message->options & HDL_I2C_MESSAGE_MRSW)? I2C_RECEIVER: 0);
  /* wait until ADDSEND bit is set */
  if(!(_hdl_reg_wait_condition(timer, &i2c_periph->STAT0, I2C_STAT0_ADDSEND, 1, 10))) 
    return HDL_I2C_MESSAGE_FAULT_BUS_ERROR;
  return HDL_I2C_MESSAGE_STATUS_ADDR_SENT;
}

hdl_i2c_message_status_t _i2c_msg_data_handler(hdl_i2c_client_private_t *i2c, hdl_i2c_message_t *message) {
  hdl_timer_t *timer = (hdl_timer_t *)i2c->module.dependencies[4];
  i2c_periph_t *i2c_periph = (i2c_periph_t *)i2c->module.reg;
  uint32_t tmp = 0;
  hdl_i2c_message_status_t result = HDL_I2C_MESSAGE_STATUS_DATA;
  if(message->options & HDL_I2C_MESSAGE_MRSW) {
    if (!(i2c_periph->STAT0 & I2C_STAT0_ADDSEND))
      if(!(_hdl_reg_wait_condition(timer, &i2c_periph->STAT0, I2C_STAT0_RBNE, 1, 10)))
        return result | HDL_I2C_MESSAGE_FAULT_BAD_STATE | HDL_I2C_MESSAGE_FAULT_BUS_ERROR;
    if(i2c_periph->STAT0 & I2C_STAT0_ADDSEND) {
      i2c_periph->CTL0 |= I2C_CTL0_ACKEN; 
      message->transfered = 0;
      if((message->options & HDL_I2C_MESSAGE_NACK_LAST) || (message->length > 3)) { 
        tmp = i2c_periph->STAT0;
        tmp = i2c_periph->STAT1;
        //if(!(_hdl_reg_wait_condition(timer, &i2c_periph->STAT0, I2C_STAT0_ADDSEND, 0, 10)))
        //  return ; 
      }
    }
    if(!(i2c_periph->STAT0 & I2C_STAT0_ADDSEND)) {
      for (uint32_t i = 0; i < message->length; i++) {
          if((i == (message->length - 3)) && !(message->options & HDL_I2C_MESSAGE_NACK_LAST)) {
            break;
          }
          if((i == (message->length - 1)) && (message->options & HDL_I2C_MESSAGE_NACK_LAST)) {
            result |= HDL_I2C_MESSAGE_STATUS_NACK;
            i2c_periph->CTL0 &= ~I2C_CTL0_ACKEN;
          }
          /* wait until byte received */
          if(!(_hdl_reg_wait_condition(timer, &i2c_periph->STAT0, I2C_STAT0_RBNE, 1, 10))) {
            result |= HDL_I2C_MESSAGE_FAULT_BUS_ERROR;
            break;
          }
          message->buffer[i] = i2c_periph->DATA;
      }
    }
  }
  else {
    if(i2c_periph->STAT1 & I2C_STAT1_TR) {
      for (uint32_t i = 0; i < message->length; i++) {
        if(!(_hdl_reg_wait_condition(timer, &i2c_periph->STAT0, I2C_STAT0_TBE, 1, 10)))
          return result | HDL_I2C_MESSAGE_FAULT_BUS_ERROR;
        i2c_periph->DATA = message->buffer[i];
      }
    }
    else {
      result |= HDL_I2C_MESSAGE_FAULT_BAD_STATE;
    }
  }
  return result;
}

hdl_i2c_message_status_t _i2c_msg_stop_handler(hdl_i2c_client_private_t *i2c) {
  hdl_timer_t *timer = (hdl_timer_t *)i2c->module.dependencies[4];
  i2c_periph_t *i2c_periph = (i2c_periph_t *)i2c->module.reg;
  hdl_i2c_message_status_t result = HDL_I2C_MESSAGE_STATUS_INITIAL;
  if(i2c_periph->STAT1 & I2C_STAT1_TR)
    _hdl_reg_wait_condition(timer, &i2c_periph->STAT0, I2C_STAT0_BTC, 1, 10);
  /* send stop condition to I2C bus */
  _i2c_set_bus_to_default(i2c_periph);
  if(i2c_periph->STAT1 & I2C_STAT1_MASTER) {
    i2c_periph->CTL0 |= I2C_CTL0_STOP;
    result = HDL_I2C_MESSAGE_STATUS_STOP_ON_BUS;
  }
  else 
    result |= HDL_I2C_MESSAGE_FAULT_BAD_STATE;
  if(!(i2c->config->stretch_enable))
    i2c_periph->CTL0 |= I2C_SCLSTRETCH_DISABLE;
  return result;
}

// typedef enum {
//   HDL_I2C_MESSAGE_STATUS_INITIAL           = 0x0000,
//   HDL_I2C_MESSAGE_STATUS_START_ON_BUS      = 0x0001,
//   HDL_I2C_MESSAGE_STATUS_ADDR_SENT         = 0x0002,
//   HDL_I2C_MESSAGE_STATUS_DATA              = 0x0004,
//   HDL_I2C_MESSAGE_STATUS_NACK              = 0x0008,
//   HDL_I2C_MESSAGE_STATUS_STOP_ON_BUS       = 0x0010,
//   HDL_I2C_MESSAGE_FAULT_ARBITRATION_LOST   = 0x0100,
//   HDL_I2C_MESSAGE_FAULT_BUS_ERROR          = 0x0200,
//   HDL_I2C_MESSAGE_FAULT_BAD_STATE          = 0x0400,
//   HDL_I2C_MESSAGE_FAULT_MASK               = 0x7F00,
//   HDL_I2C_MESSAGE_STATUS_COMPLETE          = 0x8000
// } hdl_i2c_message_status_t;

static uint8_t _i2c_client_worker(coroutine_desc_t this, uint8_t cancel, void *arg) {
  hdl_i2c_client_private_t *i2c = (hdl_i2c_client_private_t *) arg;
  while(i2c->message != NULL) {
    hdl_timer_t *timer = (hdl_timer_t *)i2c->module.dependencies[4];
    i2c_periph_t *i2c_periph = (i2c_periph_t *)i2c->module.reg;
    uint32_t tmp = 0;

    i2c->message->status |= HDL_I2C_MESSAGE_STATUS_COMPLETE;

    if(i2c->message->options & HDL_I2C_MESSAGE_START) {
      i2c->message->status |= _i2c_msg_start_handler(i2c);
    }

    if(i2c->message->status & HDL_I2C_MESSAGE_FAULT_MASK) break;

    if(i2c->message->options & HDL_I2C_MESSAGE_ADDR) {
      i2c->message->status |= _i2c_msg_addr_handler(i2c, i2c->message);
    }
    
    if(i2c->message->status & HDL_I2C_MESSAGE_FAULT_MASK) break;

    if((i2c->message->length > 0) && (i2c->message->buffer != NULL)) { 
      i2c->message->status |= _i2c_msg_data_handler(i2c, i2c->message);
    }

    if(i2c->message->status & HDL_I2C_MESSAGE_FAULT_MASK) break;

    if(i2c->message->options & HDL_I2C_MESSAGE_STOP) {
      i2c->message->status |= _i2c_msg_stop_handler(i2c);
    }
  }
  i2c->message = NULL;

  return cancel;
}

uint8_t hdl_i2c_transfer_message(hdl_i2c_client_t *i2c, hdl_i2c_message_t *message) {
  hdl_i2c_client_private_t *_i2c = (hdl_i2c_client_private_t *)i2c;
  if((message != NULL) && (i2c != NULL) && (hdl_state(&i2c->module) == HDL_MODULE_INIT_OK) && (_i2c->message == NULL)) {
    message->status = 0;
    message->transfered = 0;
    _i2c->message = message;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

hdl_module_state_t hdl_i2c_client(void *i2c, uint8_t enable) {
  hdl_i2c_client_private_t *_i2c = (hdl_i2c_client_private_t *)i2c;
  rcu_periph_enum rcu;
  switch ((uint32_t)_i2c->module.reg) {
    case I2C0:
      rcu = RCU_I2C0;
      break;
    case I2C1:
      rcu = RCU_I2C1;
      break;  
    default:
      break;
  }
  i2c_deinit((uint32_t)_i2c->module.reg);
  if(enable) {
    rcu_periph_clock_enable(rcu);
    i2c_clock_config((uint32_t)_i2c->module.reg, _i2c->config->speed, _i2c->config->dtcy);
    i2c_mode_addr_config((uint32_t)_i2c->module.reg, I2C_I2CMODE_ENABLE, 
      (_i2c->config->addr_10_bits? I2C_ADDFORMAT_10BITS: I2C_ADDFORMAT_7BITS), _i2c->config->addr0);
    i2c_ack_config(((uint32_t)_i2c->module.reg), I2C_ACK_DISABLE);
    i2c_ackpos_config((uint32_t)_i2c->module.reg, I2C_ACKPOS_CURRENT);
    I2C_SADDR1((uint32_t)_i2c->module.reg) = (_i2c->config->addr1 & 0xFE);
    I2C_SADDR1((uint32_t)_i2c->module.reg) |= (_i2c->config->dual_address? I2C_SADDR1_DUADEN : 0);
    I2C_CTL0((uint32_t)_i2c->module.reg) |= 
      (_i2c->config->general_call_enable? I2C_GCEN_ENABLE : I2C_GCEN_DISABLE) |
      (_i2c->config->stretch_enable? I2C_SCLSTRETCH_ENABLE : I2C_SCLSTRETCH_DISABLE) |
      I2C_I2CMODE_ENABLE;   
    _i2c->ev_isr.context = i2c;
    _i2c->ev_isr.handler = &event_i2c_ev_isr;
    _i2c->er_isr.context = i2c;
    _i2c->er_isr.handler = &event_i2c_er_isr; 
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)_i2c->module.dependencies[3];
    hdl_interrupt_request(ic, _i2c->config->err_interrupt, &_i2c->er_isr);
    hdl_interrupt_request(ic, _i2c->config->ev_interrupt, &_i2c->ev_isr);
    I2C_CTL0((uint32_t)_i2c->module.reg) |= I2C_CTL0_I2CEN;
    coroutine_add_static(&_i2c->i2c_client_task_buff, &_i2c_client_worker, (void*)_i2c);
    return HDL_MODULE_INIT_OK;
  }
  coroutine_cancel(&_i2c->i2c_client_task_buff);
  rcu_periph_clock_disable(rcu);
  return HDL_MODULE_DEINIT_OK;
}
