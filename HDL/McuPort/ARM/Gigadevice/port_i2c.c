#include "hdl_iface.h"

#define WRK_STATE_START    0
#define WRK_STATE_ADDR     1
#define WRK_STATE_DATA     2
#define WRK_STATE_STOP     3
#define WRK_STATE_COMPLETE 4
#define HDL_I2C_MESSAGE_FAULT_MASK 0x7F00

#define I2C_ERROR_CLEAR_MASK (uint32_t)(I2C_STAT0_SMBALT | I2C_STAT0_SMBTO | I2C_STAT0_PECERR |\
                                        I2C_STAT0_OUERR | I2C_STAT0_AERR | I2C_STAT0_LOSTARB |\
                                        I2C_STAT0_BERR)   

typedef struct {
  __IO uint32_t CTL0;            /*!< I2C Control phyister 0,                Address offset: 0x00 */
  __IO uint32_t CTL1;            /*!< I2C Control phyister 1,                Address offset: 0x04 */
  __IO uint32_t SADDR0;          /*!< I2C Slave address phyister 0,          Address offset: 0x08 */
  __IO uint32_t SADDR1;          /*!< I2C Slave address phyister 1,          Address offset: 0x0C */
  __IO uint32_t DATA;            /*!< I2C Transfer buffer phyister,          Address offset: 0x10 */
  __IO uint32_t STAT0;           /*!< I2C Transfer status phyister 0,        Address offset: 0x14 */
  __IO uint32_t STAT1;           /*!< I2C Transfer status phyister 1,        Address offset: 0x18 */
  __IO uint32_t CKCFG;           /*!< I2C Clock configure phyister,          Address offset: 0x1C */
  __IO uint32_t RT;              /*!< I2C Rise time phyister,                Address offset: 0x20*/
  __IO uint32_t _dummy24_7C[23]; 
  __IO uint32_t SAMCS;           /*!< I2C SAM control and status phyister,   Address offset: 0x80 */
  __IO uint32_t _dummy84_8C[3];
  __IO uint32_t FMPCFG;          /*!< I2C Fast mode plus configure phyister, Address offset: 0x90 */
} i2c_periph_t;

typedef struct {
  hdl_delegate_t ev_isr;
  hdl_delegate_t er_isr;
  coroutine_t i2c_worker;
  hdl_i2c_message_t *message;
  uint32_t wc_timeout;
  uint32_t wc_ts;
  __IO uint32_t *wc_phy;
  uint32_t wc_flags;
  uint8_t wc_flags_is_set;
  int8_t wc_state;
  uint8_t wrk_state;
  uint8_t wrk_state_substate;
  uint8_t is_master;
  uint8_t ch_amount;
  const hdl_transceiver_t *transceiver;
} hdl_i2c_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_i2c_var_t, *((hdl_i2c_mcu_t *)0)->obj_var, HDL_I2C_VAR_SIZE, hdl_i2c.h);

static void _i2c_clear_error(i2c_periph_t *i2c_periph) {
  i2c_periph->STAT0 &= ~(I2C_ERROR_CLEAR_MASK);
}

static void event_i2c_ev_isr(uint32_t event, void *sender, void *context) {
  (void) event; (void) sender;
  hdl_i2c_mcu_t *i2c = (hdl_i2c_mcu_t *)context;
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  hdl_i2c_config_hw_t *hwc = (hdl_i2c_config_hw_t *)i2c->config->hwc;
  i2c_periph_t *i2c_periph = (i2c_periph_t *)hwc->phy;
  uint32_t tmp = i2c_periph->STAT0;
  uint32_t tmp1 = 0;
  uint8_t data = 0;
  const hdl_transceiver_t *transceiver = i2c_var->transceiver;
  /* START condition */
  if(tmp & I2C_STAT0_SBSEND) { 
    i2c_periph->CTL0 |= (I2C_CTL0_ACKEN);
  }
  if(tmp & I2C_STAT0_ADDSEND) {
    /* Clear bit ADDSEND*/
    tmp1 = i2c_periph->STAT1;
    uint8_t ch = (tmp1 & I2C_STAT1_DUMODF)? 1: 0;
    transceiver = NULL;
    if(ch < i2c_var->ch_amount) transceiver = i2c->config->channels[ch]->transceiver;
    i2c_var->transceiver = transceiver;
    if((transceiver == NULL) || (transceiver->rx_data == NULL) || 
      ((transceiver->rx_available != NULL) && (transceiver->rx_available(transceiver->proto_context) == 0))) {
      i2c_periph->CTL0 &= ~(I2C_CTL0_ACKEN);
    }
  }
  /* Read from slave */
  if(tmp & I2C_STAT0_TBE) {
    if((transceiver != NULL) && (transceiver->tx_empty != NULL)) {
        transceiver->tx_empty(transceiver->proto_context, &data, 1);
        i2c_periph->DATA = data;
    }
  }
  /* Write to slave */
  if(tmp & I2C_STAT0_RBNE) {
    data = i2c_periph->DATA;
    if((transceiver == NULL) || (transceiver->rx_data == NULL) || 
      ((transceiver->rx_available != NULL) && (transceiver->rx_available(transceiver->proto_context) == 0))) {
      i2c_periph->CTL0 &= ~(I2C_CTL0_ACKEN);
    }
    else {
      transceiver->rx_data(transceiver->proto_context, &data, 1);
    }
  }
  /* STOP condition */
  if(tmp & I2C_STAT0_STPDET) { 
    if((transceiver != NULL) && (transceiver->end_of_transmission != NULL))
      transceiver->end_of_transmission(transceiver->proto_context);
    i2c_periph->CTL0 |= (I2C_CTL0_ACKEN);
  }
}

static void event_i2c_er_isr(uint32_t event, void *sender, void *context) {
  (void)event; (void) sender;
  hdl_i2c_mcu_t *i2c = (hdl_i2c_mcu_t *)context;
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  hdl_i2c_config_hw_t *hwc = (hdl_i2c_config_hw_t *)i2c->config->hwc;
  i2c_periph_t *i2c_periph = (i2c_periph_t *)hwc->phy;
  const hdl_transceiver_t *transceiver = i2c_var->transceiver;
  if(i2c_periph->STAT0 & I2C_STAT0_AERR) {
    if((transceiver != NULL) && (transceiver->end_of_transmission != NULL))
      transceiver->end_of_transmission(transceiver->proto_context);
    i2c_periph->CTL0 |= (I2C_CTL0_ACKEN);
  }
  _i2c_clear_error(i2c_periph);
}

#define WC_STATE_AWAITING    0
#define WC_STATE_TIMEOUT    -1
#define WC_STATE_HIT         1

static void _i2c_phy_set_wait_condition(hdl_i2c_mcu_t *i2c, __IO uint32_t *phy, uint32_t flags, uint8_t is_set, uint32_t timeout) {
  hdl_time_counter_t *timer = (hdl_time_counter_t *)i2c->dependencies[4];
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  i2c_var->wc_ts = hdl_time_counter_get(timer);
  i2c_var->wc_flags = flags;
  i2c_var->wc_flags_is_set = is_set;
  i2c_var->wc_timeout = timeout;
  i2c_var->wc_phy = phy;
  i2c_var->wc_state = WC_STATE_AWAITING;
}

static void _i2c_phy_wait_condition(hdl_i2c_mcu_t *i2c) {
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  if(i2c_var->wc_state == WC_STATE_AWAITING) {
    hdl_time_counter_t *timer = (hdl_time_counter_t *)i2c->dependencies[4];
    uint32_t now = hdl_time_counter_get(timer);
    if(i2c_var->wc_flags_is_set) {
      if((*i2c_var->wc_phy & i2c_var->wc_flags) == i2c_var->wc_flags) i2c_var->wc_state = WC_STATE_HIT;
    }
    else {
      uint32_t flags = ~i2c_var->wc_flags;
      if((*i2c_var->wc_phy | flags) == flags) i2c_var->wc_state = WC_STATE_HIT;
    }
    if(TIME_ELAPSED(i2c_var->wc_ts, i2c_var->wc_timeout, now)) i2c_var->wc_state = WC_STATE_TIMEOUT;
  }
}

static void _i2c_set_bus_to_default(i2c_periph_t *i2c_periph) {
  i2c_periph->CTL0 &= ~I2C_CTL0_ACKEN;
  uint32_t tmp = i2c_periph->STAT0;
  tmp = i2c_periph->STAT1; 
  while (i2c_periph->STAT0 & I2C_STAT0_RBNE)
    tmp = i2c_periph->DATA;
  (void)tmp;
}

static uint8_t _i2c_msg_start_handler(hdl_i2c_mcu_t *i2c) {
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  hdl_i2c_config_hw_t *hwc = (hdl_i2c_config_hw_t *)i2c->config->hwc;
  i2c_periph_t *i2c_periph = (i2c_periph_t *)hwc->phy;
  if(i2c_var->wrk_state_substate == 1) {
    if(i2c_var->wc_state == WC_STATE_TIMEOUT) {
      i2c_periph->CTL0 &= ~I2C_CTL0_START;
      i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_BUS_ERROR;
    }
    else
      i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_START_ON_BUS;
    return HDL_TRUE;
  }
  if(i2c_var->wrk_state_substate == 0) {
    i2c_periph->CTL1 &= ~(I2C_CTL1_BUFIE | I2C_CTL1_EVIE | I2C_CTL1_ERRIE);
    i2c_ack_config(((uint32_t)i2c_periph), I2C_ACK_DISABLE);
    i2c_periph->CTL0 &= ~(I2C_CTL0_SS | I2C_CTL0_STOP);
    //_i2c_set_bus_to_default(i2c_periph);
    /* send a start condition to I2C bus */
    i2c_periph->CTL0 |= I2C_CTL0_START;
    /* wait until SBSEND bit is set */
    _i2c_phy_set_wait_condition(i2c, &i2c_periph->STAT0, I2C_STAT0_SBSEND, 1, 10);
    i2c_var->wrk_state_substate = 1;
  }
  return HDL_FALSE;
}

static hdl_i2c_message_status_t _i2c_msg_addr_handler(hdl_i2c_mcu_t *i2c) {
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  hdl_i2c_config_hw_t *hwc = (hdl_i2c_config_hw_t *)i2c->config->hwc;
  i2c_periph_t *i2c_periph = (i2c_periph_t *)hwc->phy;
  if(i2c_var->wrk_state_substate == 1) {
    if(i2c_periph->STAT0 & I2C_STAT0_AERR) {
      i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_NACK;
    }
    i2c_var->message->status |= (i2c_var->wc_state == WC_STATE_TIMEOUT)? HDL_I2C_MESSAGE_FAULT_BUS_ERROR : HDL_I2C_MESSAGE_STATUS_ADDR_SENT;
    return HDL_TRUE;
  }
  if(i2c_var->wrk_state_substate == 0) {
    if(!(i2c_periph->STAT0 & I2C_STAT0_SBSEND)) { 
      i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_BAD_STATE;
      return HDL_TRUE;
    }
    /* send slave address */
    i2c_periph->DATA = (i2c_var->message->address << 1) | ((i2c_var->message->options & HDL_I2C_MESSAGE_MRSW)? I2C_RECEIVER: 0);
    /* wait until ADDSEND bit is set */
    _i2c_phy_set_wait_condition(i2c, &i2c_periph->STAT0, I2C_STAT0_ADDSEND, 1, 10);
    i2c_var->wrk_state_substate = 1;
  }
  return HDL_FALSE;
}

static uint8_t _i2c_msg_data_receiver_handler(hdl_i2c_mcu_t *i2c) {
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  hdl_i2c_config_hw_t *hwc = (hdl_i2c_config_hw_t *)i2c->config->hwc;
  i2c_periph_t *i2c_periph = (i2c_periph_t *)hwc->phy;
  if(i2c_var->wrk_state_substate == 1) { /* Awaiting rx byte */
    if(i2c_var->wc_state == WC_STATE_TIMEOUT) {
      i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_BAD_STATE | HDL_I2C_MESSAGE_FAULT_BUS_ERROR;
      return HDL_TRUE;
    }
    i2c_var->wrk_state_substate = 2;
  }

  if(i2c_var->wrk_state_substate == 2) {
    if(i2c_periph->STAT0 & I2C_STAT0_RBNE) {
      if((i2c_var->message->transferred == (i2c_var->message->length - 3)) && !(i2c_var->message->options & HDL_I2C_MESSAGE_NACK_LAST)) {
        return HDL_TRUE;
      }
      i2c_var->message->buffer[i2c_var->message->transferred] = i2c_periph->DATA;
      i2c_var->message->transferred++;
      if((i2c_var->message->transferred == (i2c_var->message->length - 2)) && (i2c_var->message->options & HDL_I2C_MESSAGE_NACK_LAST)) {
        i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_NACK;
        i2c_periph->CTL0 &= ~I2C_CTL0_ACKEN;
      }
      i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_DATA;
      if(i2c_var->message->transferred == i2c_var->message->length) {
        return HDL_TRUE;
      }
    }
    _i2c_phy_set_wait_condition(i2c, &i2c_periph->STAT0, I2C_STAT0_RBNE, 1, 10);
    i2c_var->wrk_state_substate = 1;
  }
  if(i2c_var->wrk_state_substate == 0) {
    i2c_var->message->transferred = 0;
    if(i2c_periph->STAT0 & I2C_STAT0_ADDSEND) {
      i2c_periph->CTL0 |= I2C_CTL0_ACKEN; 
      if((i2c_var->message->options & HDL_I2C_MESSAGE_NACK_LAST) || (i2c_var->message->length > 3)) { 
        // Clear I2C_STAT0_ADDSEND
        uint32_t tmp = i2c_periph->STAT0;
        tmp = i2c_periph->STAT1;
        (void)tmp;
        _i2c_phy_set_wait_condition(i2c, &i2c_periph->STAT0, I2C_STAT0_RBNE, 1, 10);
      }
      else { 
        i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_DATA;
        return HDL_TRUE;
      }
    }
    _i2c_phy_set_wait_condition(i2c, &i2c_periph->STAT0, I2C_STAT0_RBNE, 1, 10);
    i2c_var->wrk_state_substate = 1;
  }
  return HDL_FALSE;
}

static uint8_t _i2c_msg_data_transmitter_handler(hdl_i2c_mcu_t *i2c) {
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  hdl_i2c_config_hw_t *hwc = (hdl_i2c_config_hw_t *)i2c->config->hwc;
  i2c_periph_t *i2c_periph = (i2c_periph_t *)hwc->phy;
  if(i2c_var->wrk_state_substate == 0) {
    i2c_var->message->transferred = 0;
    if(!(i2c_periph->STAT1 & I2C_STAT1_TR)) {
      i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_BAD_STATE;
      return HDL_TRUE;
    }
    i2c_var->wrk_state_substate = 2;
  }

  if(i2c_var->wrk_state_substate == 1) { /* Awaiting TBE */
    if(i2c_periph->STAT0 & I2C_STAT0_AERR) {
      i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_NACK;
      return HDL_TRUE;
    }
    if(i2c_var->wc_state == WC_STATE_TIMEOUT) {
      i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_BUS_ERROR;
      return HDL_TRUE;
    }
    i2c_var->wrk_state_substate = 2;
  }

  if(i2c_var->wrk_state_substate == 2) {
    while((i2c_periph->STAT0 & I2C_STAT0_TBE) && (i2c_var->message->transferred < i2c_var->message->length)) {
      i2c_periph->DATA = i2c_var->message->buffer[i2c_var->message->transferred];
      i2c_var->message->transferred++;
    }
    if(i2c_var->message->transferred == i2c_var->message->length) {
      i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_DATA;
      return HDL_TRUE;
    }
    else {
      _i2c_phy_set_wait_condition(i2c, &i2c_periph->STAT0, I2C_STAT0_TBE, 1, 10);
      i2c_var->wrk_state_substate = 1;
    }
  }
  return HDL_FALSE;
}


static uint8_t _i2c_msg_data_handler(hdl_i2c_mcu_t *i2c) {
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  if(i2c_var->message->options & HDL_I2C_MESSAGE_MRSW) {
    return _i2c_msg_data_receiver_handler(i2c);
  }
  else {
    return _i2c_msg_data_transmitter_handler(i2c);
  }
}

static uint8_t _i2c_msg_stop_handler(hdl_i2c_mcu_t *i2c) {
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  hdl_i2c_config_hw_t *hwc = (hdl_i2c_config_hw_t *)i2c->config->hwc;
  i2c_periph_t *i2c_periph = (i2c_periph_t *)hwc->phy;
  if(i2c_var->wrk_state_substate == 1) {
    i2c_var->wrk_state_substate = 2;
  }
  if(i2c_var->wrk_state_substate == 0) {
    if(i2c_periph->STAT1 & I2C_STAT1_TR) {
      _i2c_phy_set_wait_condition(i2c, &i2c_periph->STAT0, I2C_STAT0_BTC, 1, 10);
      i2c_var->wrk_state_substate = 1;
    }
    else {
      i2c_var->wrk_state_substate = 2;
    }
  }
  if(i2c_var->wrk_state_substate == 2) {
    _i2c_set_bus_to_default(i2c_periph);
    if(i2c_periph->STAT1 & I2C_STAT1_MASTER) {
      i2c_periph->CTL0 |= I2C_CTL0_STOP;
      i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_STOP_ON_BUS;
    }
    else 
      i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_BAD_STATE;
    if(!(hwc->stretch_enable))
      i2c_periph->CTL0 |= I2C_SCLSTRETCH_DISABLE;
    i2c_ack_config(((uint32_t)i2c_periph), (i2c_var->ch_amount)? I2C_ACK_ENABLE: I2C_ACK_DISABLE);
    i2c_periph->CTL1 |= (I2C_CTL1_BUFIE | I2C_CTL1_EVIE | I2C_CTL1_ERRIE);
    i2c_var->is_master = HDL_FALSE;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}



static uint8_t _i2c_client_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_i2c_mcu_t *i2c = (hdl_i2c_mcu_t *)arg;
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  if(i2c_var->message != NULL) {
    if(i2c_var->wc_state == WC_STATE_AWAITING) {
      _i2c_phy_wait_condition(i2c);
    } else {
      if(i2c_var->wrk_state == WRK_STATE_START)
        if(!(i2c_var->message->options & HDL_I2C_MESSAGE_START) || _i2c_msg_start_handler(i2c)) { i2c_var->wrk_state++; i2c_var->wrk_state_substate = 0; }
      if(i2c_var->message->status & HDL_I2C_MESSAGE_FAULT_MASK) i2c_var->wrk_state = WRK_STATE_STOP;
      else i2c_var->is_master = HDL_TRUE;

      if(i2c_var->wrk_state == WRK_STATE_ADDR)
        if(!(i2c_var->message->options & HDL_I2C_MESSAGE_ADDR) || _i2c_msg_addr_handler(i2c)) { i2c_var->wrk_state++; i2c_var->wrk_state_substate = 0; }
      if(i2c_var->message->status & HDL_I2C_MESSAGE_FAULT_MASK) i2c_var->wrk_state = WRK_STATE_STOP;

      if(i2c_var->wrk_state == WRK_STATE_DATA)
        if(!((i2c_var->message->length > 0) && (i2c_var->message->buffer != NULL)) || _i2c_msg_data_handler(i2c)) { i2c_var->wrk_state++; i2c_var->wrk_state_substate = 0; }
      if(i2c_var->message->status & HDL_I2C_MESSAGE_FAULT_MASK) i2c_var->wrk_state = WRK_STATE_STOP;

      if(i2c_var->wrk_state == WRK_STATE_STOP)
        if(!(i2c_var->message->options & HDL_I2C_MESSAGE_STOP) || _i2c_msg_stop_handler(i2c)) { i2c_var->wrk_state++; i2c_var->wrk_state_substate = 0; }

      if(i2c_var->wrk_state == WRK_STATE_COMPLETE) {
        i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_COMPLETE;
        i2c_var->message = NULL;
      }
    }
  }
  return cancel;
}

static uint8_t _hdl_i2c_transfer(const void *desc, hdl_i2c_message_t *message) {
  hdl_i2c_mcu_t *i2c = (hdl_i2c_mcu_t *)desc;
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  if((hdl_state(i2c) != HDL_MODULE_UNLOADED) && (i2c_var->message == NULL)) {
    message->status = 0;
    message->transferred = 0;
    i2c_var->message = message;
    i2c_var->wrk_state = WRK_STATE_START;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static hdl_module_state_t _hdl_i2c(const void *desc, uint8_t enable) {
  hdl_i2c_mcu_t *i2c = (hdl_i2c_mcu_t *)desc;
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  hdl_i2c_config_hw_t *hwc = (hdl_i2c_config_hw_t *)i2c->config->hwc;
  i2c_periph_t *i2c_periph = (i2c_periph_t *)hwc->phy;
  i2c_deinit((uint32_t)i2c_periph);
  if(enable) {
    rcu_periph_clock_enable(hwc->rcu);
    //i2c_clock_config((uint32_t)_i2c->module.phy, _i2c->config->speed, _i2c->config->dtcy);
    
    hdl_clock_t *clk = (hdl_clock_t *)i2c->dependencies[2];
    uint32_t freq_mhz;
    uint32_t freq_hz;
    {
      hdl_clock_freq_t fr;
      hdl_clock_get(clk, &fr);
      freq_hz = (fr.num/fr.denom);
      freq_mhz = freq_hz / 1000000;
    }
    if(freq_mhz > 36) freq_mhz = 36;
    HDL_REG_MODIFY(i2c_periph->CTL1, I2C_CTL1_I2CCLK, freq_mhz);

    if(100000U >= hwc->speed) {
      /* the maximum SCL rise time is 1000ns in standard mode */
      uint32_t risetime = (uint32_t)(freq_mhz + 1U);
      if(risetime > 36) i2c_periph->RT = 36;
      else if(risetime < 2) i2c_periph->RT = 2;
      else i2c_periph->RT = risetime;
      uint32_t clkc = (uint32_t)(freq_hz/(hwc->speed * 2U)); 
      if(clkc < 4) clkc = 0x04U; /* the CLKC in standard mode minmum value is 4 */
      HDL_REG_MODIFY(i2c_periph->CKCFG, I2C_CKCFG_CLKC, clkc);
    }
    else if(400000U >= hwc->speed) {
      /* the maximum SCL rise time is 300ns in fast mode */
      i2c_periph->RT = (((freq_mhz * 300UL) / 1000UL) + 1UL);
      uint32_t clkc = 1;
      if(I2C_DTCY_2 == hwc->dtcy) { /* I2C duty cycle is 2 */
        clkc = (uint32_t)(freq_hz/(hwc->speed * 3U));
        i2c_periph->CKCFG &= ~I2C_CKCFG_DTCY;
      }
      else { /* I2C duty cycle is 16/9 */
        clkc = (uint32_t)(freq_hz/(hwc->speed * 25U));
        i2c_periph->CKCFG |= I2C_CKCFG_DTCY;
      }
      if(0U == (clkc & I2C_CKCFG_CLKC)) clkc |= 0x0001U; /* the CLKC in fast mode minmum value is 1 */
      i2c_periph->CKCFG |= I2C_CKCFG_FAST;
      i2c_periph->CKCFG |= clkc;
    }

    for(uint32_t no = 0, ch = 0; no < 2; no++) {
      uint16_t addr = 0;
      if((i2c->config->channels != NULL) && (i2c->config->channels[ch] != NULL)) addr = i2c->config->channels[ch++]->address;
      i2c_var->ch_amount = ch;
      if(no == 0) {
        i2c_mode_addr_config((uint32_t)i2c_periph, I2C_I2CMODE_ENABLE, 
          (hwc->addr_10_bits? I2C_ADDFORMAT_10BITS: I2C_ADDFORMAT_7BITS), (addr << 1));
      }
      else {
        i2c_periph->SADDR1 = (addr << 1);
        i2c_periph->SADDR1 |= (hwc->dual_address? I2C_SADDR1_DUADEN : 0);
      }
    }
    i2c_periph->CTL0 |= 
      (hwc->general_call_enable? I2C_GCEN_ENABLE : I2C_GCEN_DISABLE) |
      (hwc->stretch_enable? I2C_SCLSTRETCH_ENABLE : I2C_SCLSTRETCH_DISABLE) |
      I2C_I2CMODE_ENABLE;   
    i2c_var->ev_isr.context = i2c;
    i2c_var->ev_isr.handler = &event_i2c_ev_isr;
    i2c_var->er_isr.context = i2c;
    i2c_var->er_isr.handler = &event_i2c_er_isr; 
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)i2c->dependencies[3];
    hdl_event_subscribe(&hwc->err_interrupt->event, &i2c_var->er_isr);
    hdl_interrupt_request(ic, hwc->err_interrupt);
    hdl_event_subscribe(&hwc->ev_interrupt->event, &i2c_var->ev_isr);
    hdl_interrupt_request(ic, hwc->ev_interrupt);

    i2c_periph->CTL1 |= (I2C_CTL1_BUFIE | I2C_CTL1_EVIE | I2C_CTL1_ERRIE);
    i2c_periph->CTL0 |= I2C_CTL0_I2CEN;

    i2c_ackpos_config((uint32_t)i2c_periph, I2C_ACKPOS_CURRENT);
    i2c_ack_config(((uint32_t)i2c_periph), (i2c_var->ch_amount)? I2C_ACK_ENABLE: I2C_ACK_DISABLE);
    i2c_var->wc_state = WC_STATE_HIT;
    coroutine_add(&i2c_var->i2c_worker, &_i2c_client_worker, (void*)i2c);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&i2c_var->i2c_worker);
  rcu_periph_clock_disable(hwc->rcu);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_i2c_transceiver_set(const void *desc, const hdl_transceiver_t *transceiver, uint32_t channel) {
  hdl_i2c_mcu_t *i2c = (hdl_i2c_mcu_t *)desc;
  if(i2c->config->channels != NULL) {
    uint32_t ch = 0;
    while (ch <= channel) {
      if(i2c->config->channels[ch] == NULL) return HDL_FALSE;
      ch++;
    }
    ch--;
    i2c->config->channels[ch]->transceiver = transceiver;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

const hdl_i2c_iface_t hdl_i2c_iface = {
  .init = &_hdl_i2c,
  .transfer = &_hdl_i2c_transfer,
  .transceiver_set = &_hdl_i2c_transceiver_set
};
