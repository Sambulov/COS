#include "hdl_iface.h"

typedef struct {
  hdl_delegate_t ev_isr;
  coroutine_t i2c_worker;
  hdl_i2c_message_t *message;
  uint32_t wc_timer;
  uint32_t wc_ts;
  uint32_t wc_status;
  uint8_t wrk_handler;
  uint8_t wrk_state;
  uint16_t ch_amount;
} hdl_i2c_var_t;

typedef uint8_t (*i2c_master_handler_t)(hdl_i2c_mcu_t *i2c);

HDL_ASSERRT_STRUCTURE_CAST(hdl_i2c_var_t, *((hdl_i2c_mcu_t *)0)->obj_var, HDL_I2C_VAR_SIZE, hdl_i2c.h);

typedef enum {
  I2C_MM_BUS_ERROR                   = 0x00,

  I2C_MM_START                       = 0x08,
  I2C_MM_REPEAT_START                = 0x10,
  I2C_MT_ADDRESS_ACK                 = 0x18,
  I2C_MT_ADDRESS_NACK                = 0x20,
  I2C_MT_DATA_ACK                    = 0x28,
  I2C_MT_DATA_NACK                   = 0x30,
  I2C_MM_ARBITRATION_LOST            = 0x38,
  I2C_MR_ADDRESS_ACK                 = 0x40,
  I2C_MR_ADDRESS_NACK                = 0x48,
  I2C_MR_DATA_ACK                    = 0x50,
  I2C_MR_DATA_NACK                   = 0x58,

  I2C_SR_ADDRESS_ACK                 = 0x60,
  I2C_SR_ARBITRATION_LOST            = 0x68,
  I2C_GC_ADDRESS_ACK                 = 0x70,
  I2C_GC_ARBITRATION_LOST            = 0x78,
  I2C_SR_DATA_ACK                    = 0x80,
  I2C_SR_DATA_NACK                   = 0x88,
  I2C_GC_DATA_ACK                    = 0x90,
  I2C_GC_DATA_NACK                   = 0x98,
  I2C_ST_STOP_REPEAT_START           = 0xA0,
  I2C_ST_ADDRESS_ACK                 = 0xA8,
  I2C_SM_ADDRESS_TX_ARBITRATION_LOST = 0xB0,
  I2C_ST_DATA_ACK                    = 0xB8,
  I2C_ST_DATA_NACK                   = 0xC0,
  I2C_ST_LAST_DATA_ACK               = 0xC8,

  I2C_SR_BMDEN_ACKMEN                = 0xF0,
  I2C_BUS_RELEASED                   = 0xF8
} i2c_status_t;

#define I2C_MM_STATE(state)    ((state <= I2C_MR_DATA_NACK) && (state != I2C_MM_BUS_ERROR))
#define I2C_SM_STATE(state)    ((state >= I2C_SR_ADDRESS_ACK) && (state != I2C_BUS_RELEASED))

#define HANDLER_CONTINUE  0
#define HANDLER_COMPLETE  1
#define HANDLER_FAULT     2

#define WRK_START         0
#define WRK_ADDR          1
#define WRK_DATA          2
#define WRK_STOP          3
#define WRK_COMPLETE      4

static uint8_t _i2c_msg_start_handler(hdl_i2c_mcu_t *i2c) {
  hdl_i2c_config_hw_t *hwc = (hdl_i2c_config_hw_t *)i2c->config->hwc;
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  I2C_T *i2c_periph = (I2C_T *)hwc->phy;
  if(i2c_var->message->options & HDL_I2C_MESSAGE_START) {
    hdl_time_counter_t *timer = (hdl_time_counter_t *)i2c->dependencies[4];
    uint32_t now = hdl_time_counter_get(timer);
    switch (i2c_var->wrk_state) {
      case 0:
        i2c_var->wc_timer = now;
        i2c_var->wrk_state = 1;
        break;
      case 1:
        if(i2c_periph->STATUS0 == I2C_MM_BUS_ERROR) i2c_periph->CTL0 |= I2C_CTL0_SI_Msk;
        if(I2C_MM_STATE(i2c_periph->STATUS0) || (i2c_periph->STATUS0 == I2C_BUS_RELEASED)) {
          i2c_periph->CTL0 &= ~(I2C_CTL0_INTEN_Msk | I2C_CTL0_STA_Msk | I2C_CTL0_STO_Msk | I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk);
          i2c_periph->CTL0 |= I2C_CTL0_STA_Msk | I2C_CTL0_SI_Msk;
          i2c_var->wrk_state = 2;
        }
        else break;
      default:
        if((i2c_periph->STATUS0 == I2C_MM_START) || (i2c_periph->STATUS0 == I2C_MM_REPEAT_START)) {
          i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_START_ON_BUS;
          return HANDLER_COMPLETE;
        }
        break;
    }
    return HANDLER_CONTINUE;
  }
  return HANDLER_COMPLETE;
}

static uint8_t _i2c_msg_stop_handler(hdl_i2c_mcu_t *i2c) {
  hdl_i2c_config_hw_t *hwc = (hdl_i2c_config_hw_t *)i2c->config->hwc;
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  I2C_T *i2c_periph = (I2C_T *)hwc->phy;
  if((i2c_var->message->options & HDL_I2C_MESSAGE_STOP) || 
     (i2c_var->message->status & HDL_I2C_MESSAGE_FAULT_MASK)) {
    hdl_time_counter_t *timer = (hdl_time_counter_t *)i2c->dependencies[4];
    uint32_t now = hdl_time_counter_get(timer);
    switch (i2c_var->wrk_state) {
      case 0:
        i2c_var->wc_timer = now;
        i2c_var->wrk_state = 1;
        break;
      case 1:
        if(I2C_SM_STATE(i2c_periph->STATUS0)) {
          i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_BAD_STATE;
          return HANDLER_FAULT;
        }
        if(((i2c_periph->CTL0 & I2C_CTL0_SI_Msk) && I2C_MM_STATE(i2c_periph->STATUS0)) || 
          (i2c_periph->STATUS0 == I2C_MM_BUS_ERROR) ||
          (i2c_periph->STATUS0 == I2C_BUS_RELEASED)) {
          i2c_periph->CTL0 &= ~(I2C_CTL0_STA_Msk | I2C_CTL0_STO_Msk | I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk);
          i2c_periph->DAT = 0x00;
          i2c_periph->CTL0 |= I2C_CTL0_STO_Msk | I2C_CTL0_SI_Msk;
          i2c_periph->CTL0 &= ~I2C_CTL0_STO_Msk;
          i2c_var->wrk_state = 2;
        }
        else break;
      default:
        if(i2c_periph->STATUS0 == I2C_BUS_RELEASED) {
          if(i2c_periph->STATUS1 & I2C_STATUS1_ONBUSY_Msk) {
            i2c_periph->CTL0 &= ~(I2C_CTL0_I2CEN_Msk | I2C_CTL0_STA_Msk | I2C_CTL0_STO_Msk | I2C_CTL0_AA_Msk);
            i2c_periph->CTL0 |= I2C_CTL0_I2CEN_Msk;
          }
          i2c_periph->CTL0 |= I2C_CTL0_INTEN_Msk | I2C_CTL0_AA_Msk;
          i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_STOP_ON_BUS;
          return HANDLER_COMPLETE;
        }
        break;
    }
    return HANDLER_CONTINUE;
  }
  return HANDLER_COMPLETE;
}

static uint8_t _i2c_msg_addr_handler(hdl_i2c_mcu_t *i2c) {
  hdl_i2c_config_hw_t *hwc = (hdl_i2c_config_hw_t *)i2c->config->hwc;
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  I2C_T *i2c_periph = (I2C_T *)hwc->phy;
  if(i2c_var->message->options & HDL_I2C_MESSAGE_ADDR) {
    hdl_time_counter_t *timer = (hdl_time_counter_t *)i2c->dependencies[4];
    uint32_t now = hdl_time_counter_get(timer);
    switch (i2c_var->wrk_state) {
      case 0:
        i2c_var->wc_timer = now;
        i2c_var->wrk_state = 1;
        break;
      case 1:
        if(i2c_periph->CTL0 & I2C_CTL0_SI_Msk) {
          if((i2c_periph->STATUS0 != I2C_MM_START) && (i2c_periph->STATUS0 != I2C_MM_REPEAT_START)) {            
            i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_BAD_STATE;
            return HANDLER_FAULT;
          }
          i2c_periph->CTL0 &= ~(I2C_CTL0_STA_Msk | I2C_CTL0_STO_Msk | I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk);
          i2c_periph->DAT = (i2c_var->message->address << 1) | 
            ((i2c_var->message->options & HDL_I2C_MESSAGE_MRSW)? 0x01: 0x00);
          i2c_periph->CTL0 |= I2C_CTL0_SI_Msk;
          i2c_var->wrk_state = 2;
          break;
        }
        break;
      default:
        if(i2c_periph->CTL0 & I2C_CTL0_SI_Msk) {
          i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_ADDR_SENT;
          if(((i2c_var->message->options & HDL_I2C_MESSAGE_MRSW) && ((i2c_periph->STATUS0 == I2C_MR_ADDRESS_ACK) || (i2c_periph->STATUS0 == I2C_MR_ADDRESS_NACK))) ||
            (!(i2c_var->message->options & HDL_I2C_MESSAGE_MRSW) && ((i2c_periph->STATUS0 == I2C_MT_ADDRESS_ACK) || (i2c_periph->STATUS0 == I2C_MT_ADDRESS_NACK)))) {
            if((i2c_periph->STATUS0 == I2C_MR_ADDRESS_NACK) || (i2c_periph->STATUS0 == I2C_MT_ADDRESS_NACK)) 
              i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_NACK;
            return HANDLER_COMPLETE;
          }
          i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_ARBITRATION_LOST;
          return HANDLER_FAULT;
        }
        break;
    }
    return HANDLER_CONTINUE;
  }
  return HANDLER_COMPLETE;
}

static uint8_t _i2c_msg_data_handler(hdl_i2c_mcu_t *i2c) {
  hdl_i2c_config_hw_t *hwc = (hdl_i2c_config_hw_t *)i2c->config->hwc;
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  I2C_T *i2c_periph = (I2C_T *)hwc->phy;
  if((i2c_var->message->length > 0) && (i2c_var->message->buffer != NULL)) {
    hdl_time_counter_t *timer = (hdl_time_counter_t *)i2c->dependencies[4];
    uint32_t now = hdl_time_counter_get(timer);
    if(i2c_var->wrk_state == 0) {
        i2c_var->wc_timer = now;
        i2c_var->wrk_state = 1;
    }
    if(i2c_periph->CTL0 & I2C_CTL0_SI_Msk) {
      if(I2C_SM_STATE(i2c_periph->STATUS0) || 
          (i2c_periph->STATUS0 == I2C_MM_START) || 
          (i2c_periph->STATUS0 == I2C_MM_REPEAT_START)) {
        i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_BAD_STATE;
        return HANDLER_FAULT;
      }
      if(i2c_periph->STATUS0 == I2C_MM_ARBITRATION_LOST) {
        i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_ARBITRATION_LOST;
        return HANDLER_FAULT;            
      }
      if(i2c_periph->STATUS0 == I2C_MM_BUS_ERROR) {
        i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_BUS_ERROR;
        return HANDLER_FAULT;
      }
      if((i2c_periph->STATUS0 == I2C_MT_ADDRESS_NACK) ||
         (i2c_periph->STATUS0 == I2C_MR_ADDRESS_NACK)) {
        return HANDLER_COMPLETE;
      }
      uint32_t ack = 0;
      if((i2c_periph->STATUS0 == I2C_MR_ADDRESS_ACK) ||
         (i2c_periph->STATUS0 == I2C_MR_DATA_NACK) ||
         (i2c_periph->STATUS0 == I2C_MR_DATA_ACK)) {
        if(i2c_periph->STATUS0 != I2C_MR_ADDRESS_ACK) {
          i2c_var->message->buffer[i2c_var->message->transferred] = i2c_periph->DAT;
          i2c_var->message->transferred++;
        }
        if(i2c_periph->STATUS0 == I2C_MR_DATA_NACK) {
          i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_NACK;
          return HANDLER_COMPLETE;
        }
        if(!(i2c_var->message->options & HDL_I2C_MESSAGE_NACK_LAST) || 
          (i2c_var->message->transferred < (i2c_var->message->length - 1))) {
          ack = I2C_CTL0_AA_Msk;
        }
      }
      if((i2c_periph->STATUS0 == I2C_MT_DATA_NACK) ||
         (i2c_periph->STATUS0 == I2C_MT_DATA_ACK) ||
         (i2c_periph->STATUS0 == I2C_MT_ADDRESS_ACK)) {
        if(i2c_periph->STATUS0 != I2C_MT_ADDRESS_ACK) i2c_var->message->transferred++;
        if(i2c_periph->STATUS0 == I2C_MT_DATA_NACK) {
          i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_NACK;
          return HANDLER_COMPLETE;
        }
        if(i2c_var->message->transferred >= i2c_var->message->length) {
          return HANDLER_COMPLETE;
        }
        i2c_periph->DAT = i2c_var->message->buffer[i2c_var->message->transferred];
      }
      i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_DATA;
      i2c_periph->CTL0 &= ~(I2C_CTL0_STA_Msk | I2C_CTL0_STO_Msk | I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk);
      i2c_periph->CTL0 |= I2C_CTL0_SI_Msk | ack;
      i2c_var->wc_timer = now;
    }
    return HANDLER_CONTINUE;
  }
  return HANDLER_COMPLETE;
}

static i2c_master_handler_t handlers[] = {&_i2c_msg_start_handler, &_i2c_msg_addr_handler, &_i2c_msg_data_handler, &_i2c_msg_stop_handler};

static uint8_t _i2c_client_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_i2c_mcu_t *i2c = (hdl_i2c_mcu_t *)arg;
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  while(i2c_var->message != NULL) {
    uint8_t wrk_hundler = i2c_var->wrk_handler;
    uint8_t hresult = handlers[wrk_hundler](i2c);
    if(hresult == HANDLER_CONTINUE) {
      hdl_time_counter_t *timer = (hdl_time_counter_t *)i2c->dependencies[4];
      uint32_t now = hdl_time_counter_get(timer);
      if(TIME_ELAPSED(i2c_var->wc_timer, 1500, now)) {
        i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_BUS_ERROR;
        hresult = HANDLER_FAULT;
      }
      else break;
    }
    if(hresult == HANDLER_COMPLETE) wrk_hundler++;
    if(hresult == HANDLER_FAULT) wrk_hundler = ((wrk_hundler == WRK_STOP)? WRK_COMPLETE: WRK_STOP);
    if(wrk_hundler == WRK_COMPLETE) {      
      i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_COMPLETE;
      i2c_var->message = NULL;
      wrk_hundler = WRK_START;
      i2c_var->wrk_state = 0;
    }
    if(wrk_hundler != i2c_var->wrk_handler) i2c_var->wrk_state = 0;
    i2c_var->wrk_handler = wrk_hundler;
    break;
  }
  return cancel;
}

static void event_i2c_isr(uint32_t event, void *sender, void *context) {
  (void)event; (void)sender;
  hdl_i2c_mcu_t *i2c = (hdl_i2c_mcu_t *)context;
  hdl_i2c_config_hw_t *hwc = (hdl_i2c_config_hw_t *)i2c->config->hwc;
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  I2C_T *i2c_periph = (I2C_T *)hwc->phy;
  uint32_t ctrl_flags = I2C_CTL0_DPCIF_Msk;
  uint8_t ch = (i2c_periph->STATUS1 & I2C_STATUS1_ADMAT0_Msk)? 0:
               (i2c_periph->STATUS1 & I2C_STATUS1_ADMAT1_Msk)? 1:
               (i2c_periph->STATUS1 & I2C_STATUS1_ADMAT2_Msk)? 2:
               (i2c_periph->STATUS1 & I2C_STATUS1_ADMAT3_Msk)? 3: -1;
  const hdl_transceiver_t *transceiver = NULL;
  if(ch < i2c_var->ch_amount) transceiver = i2c->config->channels[ch]->transceiver;
  switch (i2c_periph->STATUS0) {
    case I2C_SR_ADDRESS_ACK:
      if((transceiver != NULL) && 
         (transceiver->rx_available != NULL) &&
         (transceiver->rx_data != NULL) &&
         (transceiver->rx_available(transceiver->proto_context))) {
        ctrl_flags |= I2C_CTL0_AA_Msk;
      }
      break;
    case I2C_ST_ADDRESS_ACK:
    case I2C_ST_DATA_ACK: {
      if((i2c_periph->STATUS0 == I2C_ST_ADDRESS_ACK ) && 
        ((transceiver == NULL) || (transceiver->tx_empty == NULL) || (transceiver->tx_available == NULL))) break;
      uint8_t data = 0xff;
      transceiver->tx_empty(transceiver->proto_context, &data, 1);
      i2c_periph->DAT = data;
      if(transceiver->tx_available(transceiver->proto_context))
        ctrl_flags |= I2C_CTL0_AA_Msk;
      break;
    }
    case I2C_SR_DATA_ACK:
      uint8_t data = i2c_periph->DAT;
      transceiver->rx_data(transceiver->proto_context, &data, 1);
      if(transceiver->rx_available(transceiver->proto_context)) {
        ctrl_flags |= I2C_CTL0_AA_Msk;
      }
      break;
    case I2C_ST_DATA_NACK:
    case I2C_SR_DATA_NACK:
    case I2C_ST_STOP_REPEAT_START:
    case I2C_ST_LAST_DATA_ACK:
      if((transceiver != NULL) && (transceiver->end_of_transmission != NULL)) {
        transceiver->end_of_transmission(transceiver->proto_context);
      }
      ctrl_flags |= I2C_CTL0_AA_Msk;
      break;
    case I2C_SR_ARBITRATION_LOST:
    case I2C_SM_ADDRESS_TX_ARBITRATION_LOST:
      ctrl_flags |= I2C_CTL0_AA_Msk;
      break;
    case I2C_GC_ADDRESS_ACK:
    case I2C_GC_ARBITRATION_LOST:
    case I2C_GC_DATA_ACK:
    case I2C_GC_DATA_NACK:
      // not supported yet
    default:
      break;
  }
  i2c_periph->CTL0 &= ~(I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk);
  i2c_periph->CTL0 |= I2C_CTL0_SI_Msk | ctrl_flags;
}

static uint8_t _i2c_clk_reset(I2C_T *i2c_periph, uint8_t en) {
  switch ((uint32_t)i2c_periph) {
    case I2C0_BASE:
      HDL_REG_SET(CLK->APBCLK0, CLK_APBCLK0_I2C0CKEN_Msk);
      HDL_REG_SET(SYS->IPRST1, SYS_IPRST1_I2C0RST_Msk);
      HDL_REG_CLEAR(SYS->IPRST1, SYS_IPRST1_I2C0RST_Msk);      
      if(!en) HDL_REG_CLEAR(CLK->APBCLK0, CLK_APBCLK0_I2C0CKEN_Msk);
      break;
    case I2C1_BASE:
      HDL_REG_SET(CLK->APBCLK0, CLK_APBCLK0_I2C1CKEN_Msk);
      HDL_REG_SET(SYS->IPRST1, SYS_IPRST1_I2C1RST_Msk);
      HDL_REG_CLEAR(SYS->IPRST1, SYS_IPRST1_I2C1RST_Msk);
      if(!en) HDL_REG_CLEAR(CLK->APBCLK0, CLK_APBCLK0_I2C1CKEN_Msk);
      break;
    case I2C2_BASE:
      HDL_REG_SET(CLK->APBCLK0, CLK_APBCLK0_I2C2CKEN_Msk);
      HDL_REG_SET(SYS->IPRST1, SYS_IPRST1_I2C2RST_Msk);
      HDL_REG_CLEAR(SYS->IPRST1, SYS_IPRST1_I2C2RST_Msk);
      if(!en) HDL_REG_CLEAR(CLK->APBCLK0, CLK_APBCLK0_I2C2CKEN_Msk);
      break;
    case I2C3_BASE:
      HDL_REG_SET(CLK->APBCLK0, CLK_APBCLK0_I2C3CKEN_Msk);
      HDL_REG_SET(SYS->IPRST1, SYS_IPRST1_I2C3RST_Msk);
      HDL_REG_CLEAR(SYS->IPRST1, SYS_IPRST1_I2C3RST_Msk);
      if(!en) HDL_REG_CLEAR(CLK->APBCLK0, CLK_APBCLK0_I2C3CKEN_Msk);
      break;
    case I2C4_BASE:
      HDL_REG_SET(CLK->APBCLK1, CLK_APBCLK1_I2C4CKEN_Msk);
      HDL_REG_SET(SYS->IPRST2, SYS_IPRST2_I2C4RST_Msk);
      HDL_REG_CLEAR(SYS->IPRST2, SYS_IPRST2_I2C4RST_Msk);
      if(!en) HDL_REG_CLEAR(CLK->APBCLK1, CLK_APBCLK1_I2C4CKEN_Msk);
      break;
    default:
      return HDL_FALSE;
  }
  return HDL_TRUE;
}

static hdl_module_state_t _hdl_i2c(const void *desc, uint8_t enable) {
  hdl_i2c_mcu_t *i2c = (hdl_i2c_mcu_t *)desc;
  hdl_i2c_config_hw_t *hwc = (hdl_i2c_config_hw_t *)i2c->config->hwc;
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  I2C_T *i2c_periph = (I2C_T *)hwc->phy;
  if(enable) {
    if(!_i2c_clk_reset(i2c_periph, HDL_TRUE)) return HDL_MODULE_FAULT;
    hdl_clock_t *clk = (hdl_clock_t *)i2c->dependencies[0];
    hdl_clock_freq_t freq;
    hdl_clock_get(clk, &freq);
    uint32_t div = (uint32_t)((((freq.num << 3) / (freq.denom * hwc->speed * 4U) + 4U) >> 3) - 1U);
    i2c_periph->CLKDIV = div;
    uint32_t st_limit = (((div * 2 - 6) >> 3) << I2C_TMCTL_STCTL_Pos) & I2C_TMCTL_STCTL_Msk;
    uint32_t ht_limit = (((div * 2 - 9) >> 3) << I2C_TMCTL_HTCTL_Pos) & I2C_TMCTL_HTCTL_Msk;
    i2c_periph->TMCTL = st_limit | ht_limit;
    for(uint32_t no = 0, ch = 0; no < 4; no++) {
      uint16_t addr = 0;
      if((i2c->config->channels != NULL) && (i2c->config->channels[ch] != NULL)) addr = i2c->config->channels[ch++]->address;
      I2C_SetSlaveAddr(i2c_periph, no, addr, I2C_GCMODE_DISABLE);
      i2c_var->ch_amount = ch;
    }
    if(hwc->swap_scl_sda) HDL_REG_SET(i2c_periph->CTL1, I2C_CTL1_SWITCHEN_Msk); // swap scl <-X-> sda
    HDL_REG_SET(i2c_periph->CTL0, I2C_CTL0_SRCINTEN_Msk | I2C_CTL0_INTEN_Msk);
    HDL_REG_SET(i2c_periph->CTL1, I2C_CTL1_UDRIEN_Msk | I2C_CTL1_OVRIEN_Msk);
    i2c_var->ev_isr.context = i2c;
    i2c_var->ev_isr.handler = &event_i2c_isr;
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)i2c->dependencies[3];
    hdl_event_subscribe(&hwc->interrupt->event, &i2c_var->ev_isr);
    hdl_interrupt_request(ic, hwc->interrupt);
    HDL_REG_CLEAR(i2c_periph->CTL0, I2C_CTL0_SRCINTEN_Msk | I2C_CTL0_DPCINTEN_Msk);    
    HDL_REG_SET(i2c_periph->CTL0, I2C_CTL0_I2CEN_Msk);
    HDL_REG_SET(i2c_periph->CTL0, I2C_CTL0_AA_Msk);
    coroutine_add(&i2c_var->i2c_worker, &_i2c_client_worker, (void*)i2c);
    i2c_var->message = NULL;
    return HDL_MODULE_ACTIVE;
  }
  if(i2c_var->message != NULL) {
    i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_BUS_ERROR | HDL_I2C_MESSAGE_STATUS_COMPLETE;
  }

  coroutine_cancel(&i2c_var->i2c_worker);
  _i2c_clk_reset(i2c_periph, HDL_FALSE);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_i2c_transfer(const void *desc, hdl_i2c_message_t *message) {
  hdl_i2c_mcu_t *i2c = (hdl_i2c_mcu_t *)desc;
  hdl_i2c_var_t *i2c_var = (hdl_i2c_var_t *)i2c->obj_var;
  if((message != NULL) && (i2c_var->message == NULL)) {
    message->status = 0;
    message->transferred = 0;
    i2c_var->message = message;
    i2c_var->wrk_handler = WRK_START;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static uint8_t _hdl_i2c_transceiver_set(const void *desc, const hdl_transceiver_t *transceiver, uint32_t channel_id) {
  hdl_i2c_mcu_t *i2c = (hdl_i2c_mcu_t *)desc;
  if((i2c != NULL) && (i2c->config->channels != NULL)) {
    uint32_t ch = 0;
    while (ch <= channel_id) {
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
