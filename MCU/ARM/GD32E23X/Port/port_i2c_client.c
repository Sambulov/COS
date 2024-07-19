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
  uint32_t wc_timeout;
  uint32_t wc_ts;
  __IO uint32_t *wc_reg;
  uint32_t wc_flags;
  uint8_t wc_flags_is_set;
  int8_t wc_state;
  uint8_t wrk_state;
  uint8_t wrk_state_substate;
} hdl_i2c_client_private_t;

_Static_assert(sizeof(hdl_i2c_client_private_t) == sizeof(hdl_i2c_client_t), "In hdl_i2c.h data structure size of hdl_i2c_client_t doesn't match, check HDL_I2C_CLIENT_PRV_SIZE");

static void event_i2c_ev_isr(uint32_t event, void *sender, void *context) {
  hdl_i2c_client_private_t *i2c = (hdl_i2c_client_private_t *)context;
}

static void event_i2c_er_isr(uint32_t event, void *sender, void *context) {
  hdl_i2c_client_private_t *i2c = (hdl_i2c_client_private_t *)context;
}

#define WC_STATE_AWAITING    0
#define WC_STATE_TIMEOUT    -1
#define WC_STATE_HIT         1

static void _i2c_reg_set_wait_condition(hdl_i2c_client_private_t *i2c, __IO uint32_t *reg, uint32_t flags, uint8_t is_set, uint32_t timeout) {
  hdl_timer_t *timer = (hdl_timer_t *)i2c->module.dependencies[4];
  i2c->wc_ts = hdl_timer_get(timer);
  i2c->wc_flags = flags;
  i2c->wc_flags_is_set = is_set;
  i2c->wc_timeout = timeout;
  i2c->wc_reg = reg;
  i2c->wc_state = WC_STATE_AWAITING;
}

static void _i2c_reg_wait_condition(hdl_i2c_client_private_t *i2c) {
  if(i2c->wc_state == WC_STATE_AWAITING) {
    hdl_timer_t *timer = (hdl_timer_t *)i2c->module.dependencies[4];
    uint32_t now = hdl_timer_get(timer);
    if(i2c->wc_flags_is_set) {
      if((*i2c->wc_reg & i2c->wc_flags) == i2c->wc_flags) i2c->wc_state = WC_STATE_HIT;
    }
    else {
      uint32_t flags = ~i2c->wc_flags;
      if((*i2c->wc_reg | flags) == flags) i2c->wc_state = WC_STATE_HIT;
    }
    if(TIME_ELAPSED(i2c->wc_ts, i2c->wc_timeout, now)) i2c->wc_state = WC_STATE_TIMEOUT;
  }
}

static void _i2c_set_bus_to_default(i2c_periph_t *i2c_periph) {
  uint32_t tmp = 0;
  i2c_periph->CTL0 &= ~I2C_CTL0_ACKEN;
  tmp = i2c_periph->STAT0;
  tmp = i2c_periph->STAT1; 
  while (i2c_periph->STAT0 & I2C_STAT0_RBNE)
    tmp = i2c_periph->DATA;
}

static uint8_t _i2c_msg_start_handler(hdl_i2c_client_private_t *i2c) {
  i2c_periph_t *i2c_periph = (i2c_periph_t *)i2c->module.reg;
  if(i2c->wrk_state_substate == 1) {
    if(i2c->wc_state == WC_STATE_TIMEOUT) {
      i2c_periph->CTL0 &= ~I2C_CTL0_START;
      i2c->message->status |= HDL_I2C_MESSAGE_FAULT_BUS_ERROR;
    }
    else
      i2c->message->status |= HDL_I2C_MESSAGE_STATUS_START_ON_BUS;
    return HDL_TRUE;
  }
  if(i2c->wrk_state_substate == 0) {
    i2c_periph->CTL0 &= ~I2C_CTL0_SS;
    //_i2c_set_bus_to_default(i2c_periph);
    /* send a start condition to I2C bus */
    i2c_periph->CTL0 |= I2C_CTL0_START;
    /* wait until SBSEND bit is set */
    _i2c_reg_set_wait_condition(i2c, &i2c_periph->STAT0, I2C_STAT0_SBSEND, 1, 10);
    i2c->wrk_state_substate = 1;
  }
  return HDL_FALSE;
}

static hdl_i2c_message_status_t _i2c_msg_addr_handler(hdl_i2c_client_private_t *i2c) {
  hdl_timer_t *timer = (hdl_timer_t *)i2c->module.dependencies[4];
  i2c_periph_t *i2c_periph = (i2c_periph_t *)i2c->module.reg;
  if(i2c->wrk_state_substate == 1) {
    i2c->message->status |= (i2c->wc_state == WC_STATE_TIMEOUT)? HDL_I2C_MESSAGE_FAULT_BUS_ERROR : HDL_I2C_MESSAGE_STATUS_ADDR_SENT;
    return HDL_TRUE;
  }
  if(i2c->wrk_state_substate == 0) {
    if(!(i2c_periph->STAT0 & I2C_STAT0_SBSEND)) { 
      i2c->message->status |= HDL_I2C_MESSAGE_FAULT_BAD_STATE;
      return HDL_TRUE;
    }
    /* send slave address */
    i2c_periph->DATA = (i2c->message->address << 1) | ((i2c->message->options & HDL_I2C_MESSAGE_MRSW)? I2C_RECEIVER: 0);
    /* wait until ADDSEND bit is set */
    _i2c_reg_set_wait_condition(i2c, &i2c_periph->STAT0, I2C_STAT0_ADDSEND, 1, 10);
    i2c->wrk_state_substate = 1;
  }
  return HDL_FALSE;
}

static uint8_t _i2c_msg_data_receiver_handler(hdl_i2c_client_private_t *i2c) {
  hdl_timer_t *timer = (hdl_timer_t *)i2c->module.dependencies[4];
  i2c_periph_t *i2c_periph = (i2c_periph_t *)i2c->module.reg;
  uint32_t tmp = 0;
  if(i2c->wrk_state_substate == 1) { /* Awaiting rx byte */
    if(i2c->wc_state == WC_STATE_TIMEOUT) {
      i2c->message->status |= HDL_I2C_MESSAGE_FAULT_BAD_STATE | HDL_I2C_MESSAGE_FAULT_BUS_ERROR;
      return HDL_TRUE;
    }
    i2c->wrk_state_substate = 2;
  }

  if(i2c->wrk_state_substate == 2) {
    if(i2c_periph->STAT0 & I2C_STAT0_RBNE) {
      if((i2c->message->transfered == (i2c->message->length - 3)) && !(i2c->message->options & HDL_I2C_MESSAGE_NACK_LAST)) {
        return HDL_TRUE;
      }
      i2c->message->buffer[i2c->message->transfered] = i2c_periph->DATA;
      i2c->message->transfered++;
      if((i2c->message->transfered == (i2c->message->length - 2)) && (i2c->message->options & HDL_I2C_MESSAGE_NACK_LAST)) {
        i2c->message->status |= HDL_I2C_MESSAGE_STATUS_NACK;
        i2c_periph->CTL0 &= ~I2C_CTL0_ACKEN;
      }
      i2c->message->status |= HDL_I2C_MESSAGE_STATUS_DATA;
      if(i2c->message->transfered == i2c->message->length) {
        return HDL_TRUE;
      }
    }
    _i2c_reg_set_wait_condition(i2c, &i2c_periph->STAT0, I2C_STAT0_RBNE, 1, 10);
    i2c->wrk_state_substate = 1;
  }
  if(i2c->wrk_state_substate == 0) {
    i2c->message->transfered = 0;
    if(i2c_periph->STAT0 & I2C_STAT0_ADDSEND) {
      i2c_periph->CTL0 |= I2C_CTL0_ACKEN; 
      if((i2c->message->options & HDL_I2C_MESSAGE_NACK_LAST) || (i2c->message->length > 3)) { 
        // Clear I2C_STAT0_ADDSEND
        tmp = i2c_periph->STAT0;
        tmp = i2c_periph->STAT1;
        _i2c_reg_set_wait_condition(i2c, &i2c_periph->STAT0, I2C_STAT0_RBNE, 1, 10);
      }
      else { 
        i2c->message->status |= HDL_I2C_MESSAGE_STATUS_DATA;
        return HDL_TRUE;
      }
    }
    _i2c_reg_set_wait_condition(i2c, &i2c_periph->STAT0, I2C_STAT0_RBNE, 1, 10);
    i2c->wrk_state_substate = 1;
  }
  return HDL_FALSE;
}

static uint8_t _i2c_msg_data_transmitter_handler(hdl_i2c_client_private_t *i2c) {
  i2c_periph_t *i2c_periph = (i2c_periph_t *)i2c->module.reg;
  if(i2c->wrk_state_substate == 0) {
    i2c->message->transfered = 0;
    if(!(i2c_periph->STAT1 & I2C_STAT1_TR)) {
      i2c->message->status |= HDL_I2C_MESSAGE_FAULT_BAD_STATE;
      return HDL_TRUE;
    }
    i2c->wrk_state_substate = 2;
  }

  if(i2c->wrk_state_substate == 1) { /* Awaiting TBE */
    if(i2c->wc_state == WC_STATE_TIMEOUT) {
      i2c->message->status |= HDL_I2C_MESSAGE_FAULT_BUS_ERROR;
      return HDL_TRUE;
    }
    i2c->wrk_state_substate = 2;
  }

  if(i2c->wrk_state_substate == 2) {
    while((i2c_periph->STAT0 & I2C_STAT0_TBE) && (i2c->message->transfered < i2c->message->length)) {
      i2c_periph->DATA = i2c->message->buffer[i2c->message->transfered];
      i2c->message->transfered++;
    }
    if(i2c->message->transfered == i2c->message->length) {
      i2c->message->status |= HDL_I2C_MESSAGE_STATUS_DATA;
      return HDL_TRUE;
    }
    else {
      _i2c_reg_set_wait_condition(i2c, &i2c_periph->STAT0, I2C_STAT0_TBE, 1, 10);
      i2c->wrk_state_substate = 1;
    }
  }
  return HDL_FALSE;
}


static uint8_t _i2c_msg_data_handler(hdl_i2c_client_private_t *i2c) {
  if(i2c->message->options & HDL_I2C_MESSAGE_MRSW) {
    return _i2c_msg_data_receiver_handler(i2c);
  }
  else {
    return _i2c_msg_data_transmitter_handler(i2c);
  }
}

static hdl_i2c_message_status_t _i2c_msg_stop_handler(hdl_i2c_client_private_t *i2c) {
  hdl_timer_t *timer = (hdl_timer_t *)i2c->module.dependencies[4];
  i2c_periph_t *i2c_periph = (i2c_periph_t *)i2c->module.reg;
  hdl_i2c_message_status_t result = HDL_I2C_MESSAGE_STATUS_INITIAL;
  if(i2c->wrk_state_substate == 1) {
    i2c->wrk_state_substate = 2;
  }
  if(i2c->wrk_state_substate == 0) {
    if(i2c_periph->STAT1 & I2C_STAT1_TR) {
      _i2c_reg_set_wait_condition(i2c, &i2c_periph->STAT0, I2C_STAT0_BTC, 1, 10);
      i2c->wrk_state_substate = 1;
    }
    else {
      i2c->wrk_state_substate = 2;
    }
  }
  if(i2c->wrk_state_substate == 2) {
    _i2c_set_bus_to_default(i2c_periph);
    if(i2c_periph->STAT1 & I2C_STAT1_MASTER) {
      i2c_periph->CTL0 |= I2C_CTL0_STOP;
      i2c->message->status |= HDL_I2C_MESSAGE_STATUS_STOP_ON_BUS;
    }
    else 
      i2c->message->status |= HDL_I2C_MESSAGE_FAULT_BAD_STATE;
    if(!(i2c->config->stretch_enable))
      i2c_periph->CTL0 |= I2C_SCLSTRETCH_DISABLE;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

#define WRK_STATE_START    0
#define WRK_STATE_ADDR     1
#define WRK_STATE_DATA     2
#define WRK_STATE_STOP     3
#define WRK_STATE_COMPLETE 4
#define HDL_I2C_MESSAGE_FAULT_MASK 0x7F00

static uint8_t _i2c_client_worker(coroutine_desc_t this, uint8_t cancel, void *arg) {
  hdl_i2c_client_private_t *i2c = (hdl_i2c_client_private_t *) arg;
  if(i2c->message != NULL) {
    if(i2c->wc_state == WC_STATE_AWAITING) {
      _i2c_reg_wait_condition(i2c);
    } else {
      if(i2c->wrk_state == WRK_STATE_START)
        if(!(i2c->message->options & HDL_I2C_MESSAGE_START) || _i2c_msg_start_handler(i2c)) { i2c->wrk_state++; i2c->wrk_state_substate = 0; }
      if(i2c->message->status & HDL_I2C_MESSAGE_FAULT_MASK) i2c->wrk_state = WRK_STATE_COMPLETE;

      if(i2c->wrk_state == WRK_STATE_ADDR)
        if(!(i2c->message->options & HDL_I2C_MESSAGE_ADDR) || _i2c_msg_addr_handler(i2c)) { i2c->wrk_state++; i2c->wrk_state_substate = 0; }
      if(i2c->message->status & HDL_I2C_MESSAGE_FAULT_MASK) i2c->wrk_state = WRK_STATE_COMPLETE;

      if(i2c->wrk_state == WRK_STATE_DATA)
        if(!((i2c->message->length > 0) && (i2c->message->buffer != NULL)) || _i2c_msg_data_handler(i2c)) { i2c->wrk_state++; i2c->wrk_state_substate = 0; }
      if(i2c->message->status & HDL_I2C_MESSAGE_FAULT_MASK) i2c->wrk_state = WRK_STATE_COMPLETE;

      if(i2c->wrk_state == WRK_STATE_STOP)
        if(!(i2c->message->options & HDL_I2C_MESSAGE_STOP) || _i2c_msg_stop_handler(i2c)) { i2c->wrk_state++; i2c->wrk_state_substate = 0; }

      if(i2c->wrk_state == WRK_STATE_COMPLETE) {
        i2c->message->status |= HDL_I2C_MESSAGE_STATUS_COMPLETE;
        i2c->message = NULL;
      }
    }
  }
  return cancel;
}

uint8_t hdl_i2c_transfer_message(hdl_i2c_client_t *i2c, hdl_i2c_message_t *message) {
  hdl_i2c_client_private_t *_i2c = (hdl_i2c_client_private_t *)i2c;
  if((message != NULL) && (i2c != NULL) && (hdl_state(&i2c->module) == HDL_MODULE_INIT_OK) && (_i2c->message == NULL)) {
    message->status = 0;
    message->transfered = 0;
    _i2c->message = message;
    _i2c->wrk_state = WRK_STATE_START;
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
    _i2c->wc_state = WC_STATE_HIT;
    coroutine_add_static(&_i2c->i2c_client_task_buff, &_i2c_client_worker, (void*)_i2c);
    return HDL_MODULE_INIT_OK;
  }
  coroutine_cancel(&_i2c->i2c_client_task_buff);
  rcu_periph_clock_disable(rcu);
  return HDL_MODULE_DEINIT_OK;
}
