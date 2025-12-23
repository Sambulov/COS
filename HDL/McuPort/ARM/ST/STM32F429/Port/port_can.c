#include "hdl_iface.h"

typedef struct {
  //hdl_delegate_t can_err_isr;
  hdl_can_message_t *msg[3];
  hdl_can_message_t msg_buf;
  hdl_event_t event;
  coroutine_t worker;
  uint8_t init_begin    : 1,
          init_ready    : 1,
          init_exit     : 1,
          init_complete : 1,
          reset         : 1;
  uint32_t prescaler; 
  uint32_t time_seg1; 
  uint32_t time_seg2;
} hdl_can_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_can_var_t, *((hdl_can_mcu_t *)0)->obj_var, HDL_CAN_VAR_SIZE, port_can.h);

static inline uint8_t _init_begin(CAN_TypeDef *periph) {
  periph->MCR &= (~(uint32_t)CAN_MCR_SLEEP); /* Exit from sleep mode */
  uint32_t time = 10000;
  periph->MCR |= CAN_MCR_INRQ; /* Request initialisation */
  while((periph->MSR & CAN_MSR_INAK) != CAN_MSR_INAK) if(!time--) return HDL_FALSE;
  return HDL_TRUE;
}

static inline uint8_t _init_exit(CAN_TypeDef *periph) {
  uint32_t time = 10000;
  periph->MCR &= ~(uint32_t)CAN_MCR_INRQ; /* Request leave initialisation */
  while((periph->MSR & CAN_MSR_INAK) == CAN_MSR_INAK) if(!time--) return HDL_FALSE;
  CAN1->FMR &= ~((uint32_t)CAN_FMR_FINIT); /* Leave the initialisation mode for the filter */
  return HDL_TRUE;
}

static uint8_t _hdl_can_init(CAN_TypeDef *periph, const hdl_can_mcu_config_t *config, hdl_can_var_t *can_var) {
  if (config->time_triggered_mode) periph->MCR |= CAN_MCR_TTCM; /* Set the time triggered communication mode */
  if (config->auto_bus_off) periph->MCR |= CAN_MCR_ABOM; /* Set the automatic bus-off management */
  if (config->auto_wake_up) periph->MCR |= CAN_MCR_AWUM; /* Set the automatic wake-up mode */
  if (config->auto_retransmission) periph->MCR |= CAN_MCR_NART; /* Set the no automatic retransmission */
  if (config->receive_fifo_locked) periph->MCR |= CAN_MCR_RFLM; /* Set the receive FIFO locked mode */
  if (config->transmit_fifo_priority) periph->MCR |= CAN_MCR_TXFP; /* Set the transmit FIFO priority */
  /* Set the bit timing register */
  periph->BTR = config->mode | config->sync_jump_width | can_var->time_seg1 | can_var->time_seg2 | 
    ((can_var->prescaler - 1U) & CAN_BTR_BRP);
  return HDL_TRUE;
}

static uint8_t can_receive(CAN_TypeDef *can, hdl_can_message_t *msg) {
  if(!(can->RF0R & CAN_RF0R_FMP0)) return HDL_FALSE;  /* Check pending message */
  msg->options = 0;
  msg->id = (can->sFIFOMailBox[0].RIR >> 21U) & HDL_CAN_ID_MASK;
  if(CAN_RI0R_IDE & can->sFIFOMailBox[0].RIR) {
    msg->options |= HDL_CAN_MESSAGE_IDE;
    msg->id = (can->sFIFOMailBox[0].RIR >> 3U) & HDL_CAN_IDE_MASK;
  }
  if(CAN_RI0R_RTR & can->sFIFOMailBox[0].RIR) msg->options |= HDL_CAN_MESSAGE_RTR;
  msg->dlc = CAN_RDT0R_DLC & can->sFIFOMailBox[0].RDTR;
  //uint8_t filter = (CAN_RDT0R_FMI & can->sFIFOMailBox[0].RDTR) >> 8;
  ((uint32_t *)msg->payload)[0] = can->sFIFOMailBox[0].RDLR;
  ((uint32_t *)msg->payload)[1] = can->sFIFOMailBox[0].RDHR;
  can->RF0R = CAN_RF0R_RFOM0;
  //can->RF1R = CAN_RF1R_RFOM1; /* FIFO1 unused */
  return HDL_TRUE;
}

static uint8_t can_transmit(CAN_TypeDef *can, hdl_can_var_t *can_var) {
  for(uint32_t i = 0; i < 3; i++) {
    hdl_can_message_t *msg = can_var->msg[i];
    if(msg != NULL) {
      if(msg->status == HDL_CAN_MESSAGE_STATUS_INITIAL) {
        /* Set up the Id */
        can->sTxMailBox[i].TIR &= CAN_TI0R_TXRQ;
        if (msg->options & HDL_CAN_MESSAGE_IDE) can->sTxMailBox[i].TIR |= (msg->id << 3U) | CAN_TI0R_IDE;
        else can->sTxMailBox[i].TIR |= (msg->id << 21U);
        if(msg->options & HDL_CAN_MESSAGE_RTR) can->sTxMailBox[i].TIR |= CAN_TI0R_RTR;
        /* Set up the DLC */
        can->sTxMailBox[i].TDTR &= (uint32_t)0xFFFFFFF0U;
        can->sTxMailBox[i].TDTR |= (msg->dlc & 0x0000000FU);
        /* Set up the data field */
        can->sTxMailBox[i].TDLR = ((uint32_t *)msg->payload)[0];
        can->sTxMailBox[i].TDHR = ((uint32_t *)msg->payload)[1];
        /* Request transmission */
        can->sTxMailBox[i].TIR |= CAN_TI0R_TXRQ;
        can_var->msg[i]->status |= HDL_CAN_MESSAGE_STATUS_PENDING;
      }
      // todo: timeout
      if(can->TSR & (CAN_TSR_TME0 << i)) {
        msg->status = HDL_CAN_MESSAGE_STATUS_COMPLETE;
        if(can->TSR & (CAN_TSR_TERR0 << (4 * i))) msg->status |= HDL_CAN_MESSAGE_FAULT_XFER_ERROR;
        if(can->TSR & (CAN_TSR_ALST0 << (4 * i))) msg->status |= HDL_CAN_MESSAGE_FAULT_ARBITRATION_LOST;
        can->TSR |= (CAN_TSR_RQCP0 << (4 * i));
        can_var->msg[i] = NULL;
      }
    }
  }
  return 0;
}

static uint8_t _can_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this; (void)arg;
  hdl_can_mcu_t *can = (hdl_can_mcu_t *) arg;
  hdl_can_var_t *can_var = (hdl_can_var_t *)can->obj_var;
  CAN_TypeDef *periph = (CAN_TypeDef *)can->config->phy;
  if(can_var->reset) {
    periph->TSR |= CAN_TSR_ABRQ0 | CAN_TSR_ABRQ1 | CAN_TSR_ABRQ2;
    for(uint32_t i = 0; i < 3; i++) {
      if(can_var->msg[i] != NULL) {
        can_var->msg[i]->status = HDL_CAN_MESSAGE_FAULT_ABORT | HDL_CAN_MESSAGE_STATUS_COMPLETE;
        can_var->msg[i] = NULL;
      }
    }
    can_var->init_complete = 0;
    can_var->init_begin    = 0,
    can_var->init_ready    = 0,
    can_var->init_exit     = 0,
    can_var->init_complete = 0,
    can_var->reset         = 0;
  }
  if(can_var->init_complete) {
    if(can_receive(periph, &can_var->msg_buf)) hdl_event_raise(&can_var->event, can, &can_var->msg_buf);
    //if(can_error(periph))  hdl_event_raise(&can_var->event, can, HDL_CAN_EVENT_ERR);
    can_transmit(periph, can_var);
  }
  else {
    if(!can_var->init_begin) can_var->init_begin = _init_begin(periph);
    if(!can_var->init_ready) 
      can_var->init_ready = _hdl_can_init(periph, can->config, can_var);
    if(!can_var->init_exit) can_var->init_exit = _init_exit(periph);
    can_var->init_complete = can_var->init_begin & can_var->init_ready & can_var->init_exit;
  }
  return cancel;
}

static uint8_t _hdl_can_filter_set(const void *desc, const hdl_can_filter_t * const *filters) {
  if(filters == NULL) return HDL_FALSE;
  hdl_can_mcu_t *can = (hdl_can_mcu_t *) desc;
  CAN1->FMR |= (uint32_t)CAN_FMR_FINIT; /* Initialisation mode for the filter */
  uint8_t f11b_slot = 0;
  uint8_t f11b_ind = 0;
  uint8_t f29b_slot = 0;
  uint8_t f29b_ind = 0;
  uint8_t f11b_mslot = 0;
  uint8_t f11b_mind = 0;
  uint8_t filter = (CAN1->FMR & CAN_FMR_CAN2SB) >> 8;
  uint32_t c2fm = (1UL << filter) - 1;
  if(can->config->phy == (uint32_t)CAN2) CAN1->FA1R &= c2fm; /* Filter deactivation */
  else {
    filter = 0;
    CAN1->FA1R &= ~c2fm; /* Filter deactivation */
  }
  while(*filters) {
    if((filter == 28) || 
      ((can->config->phy == (uint32_t)CAN1) && (filter == ((CAN1->FMR & CAN_FMR_CAN2SB) >> 8)))) break;
    uint32_t filternbrbitpos = ((uint32_t)1U) << filter;
    if ((*filters)->id_ex) {
      uint32_t id = ((*filters)->id << 3) | 4/* IDE bit */ | (((*filters)->rtr? 2/* RTR bit */:0));
      if((*filters)->masked) {
        uint32_t mask = ((*filters)->id_mask << 3) | 4/* IDE bit */ | (((*filters)->rtr_mask? 2/* RTR bit */:0));
        CAN1->sFilterRegister[filter].FR1 = id;
        CAN1->sFilterRegister[filter].FR2 = mask;
        filter++;
      }
      else {
        if(f29b_slot) {
          filternbrbitpos = (1UL << f29b_ind);
          CAN1->sFilterRegister[f29b_ind].FR2 = id;
          f29b_slot = 0;
        }
        else {
          f29b_ind = filter;
          f29b_slot = 1;
          CAN1->sFilterRegister[f29b_ind].FR1 = id;
          CAN1->sFilterRegister[f29b_ind].FR2 = id;
          filter++;
        }
      }
      CAN1->FS1R |= filternbrbitpos; /* 32-bit scale for the filter */
    }
    else {
      uint32_t id = (((uint16_t)((*filters)->id << 5)) | (((*filters)->rtr? 16/* RTR bit */:0)));
      if((*filters)->masked) {
        uint32_t mask = (((uint16_t)((*filters)->id_mask << 5)) | (((*filters)->rtr_mask? 16/* RTR bit */:0)));
        if(f11b_mslot) {
          filternbrbitpos = (1UL << f11b_mind);
          CAN1->sFilterRegister[f11b_mind].FR2 = (mask << 16) | id;
          f11b_mslot = 0;
        }
        else {
          f11b_mind = filter;
          CAN1->sFilterRegister[f11b_mind].FR1 = (mask << 16) | id;
          CAN1->sFilterRegister[f11b_mind].FR2 = (mask << 16) | id;
          f11b_mslot = 1;
          filter++;
        }
      }
      else {
        if(f11b_slot) {
          filternbrbitpos = (1UL << f11b_ind);
          uint32_t mask = 0x0000ffff;
          if(f11b_slot & 1) {
            id <<= 16;
            mask <<= 16;
          }
          if(f11b_slot > 2) CL_REG_MODIFY(CAN1->sFilterRegister[f11b_ind].FR1, mask, id);
          else CL_REG_MODIFY(CAN1->sFilterRegister[f11b_ind].FR2, mask, id);
          f11b_slot--;
        }
        else {
          f11b_ind = filter;
          CAN1->sFilterRegister[f11b_ind].FR1 = (id << 16) | id;
          CAN1->sFilterRegister[f11b_ind].FR2 = (id << 16) | id;
          f11b_slot = 3;
          filter++;
        }
      }
      CAN1->FS1R &= ~(uint32_t)filternbrbitpos; /* 16-bit scale for the filter */
    }
    CAN1->FFA1R &= ~(uint32_t)filternbrbitpos; /* FIFO 0 assignation for the filter */    
    if ((*filters)->masked) CAN1->FM1R &= ~(uint32_t)filternbrbitpos; /*Id/Mask mode for the filter*/
    else CAN1->FM1R |= (uint32_t)filternbrbitpos; /* Identifier list mode for the filter*/
    CAN1->FA1R |= filternbrbitpos; /* Filter activation */
    filters++;
  }
  CAN1->FMR &= ~((uint32_t)CAN_FMR_FINIT); /* Leave the initialisation mode for the filter */
  return HDL_TRUE;
}

static hdl_module_state_t _hdl_can_base(const void *desc, uint8_t enable) {
  hdl_can_mcu_base_t *can = (hdl_can_mcu_base_t*)desc;
  CL_REG_SET(RCC->APB1RSTR, RCC_APB1ENR_CAN1EN);
  CL_REG_SET(RCC->APB1RSTR, RCC_APB1ENR_CAN2EN);
  CL_REG_CLEAR(RCC->APB1RSTR, RCC_APB1ENR_CAN1EN);
  CL_REG_CLEAR(RCC->APB1RSTR, RCC_APB1ENR_CAN2EN);
  if(enable) {
    CL_REG_SET(RCC->APB1ENR, RCC_APB1ENR_CAN1EN); /* Clock enable */
    CL_REG_MODIFY(CAN1->FMR, CAN_FMR_CAN2SB, ((uint32_t)can->config << 8U)); /* Select the start slave bank */
    return HDL_MODULE_ACTIVE;
  }
  CL_REG_CLEAR(RCC->APB1ENR, RCC_APB1ENR_CAN1EN); /* Clock enable */
  return HDL_MODULE_UNLOADED;
}

const hdl_module_base_iface_t hdl_can_mcu_base_iface = {
  .init = &_hdl_can_base
};

static hdl_module_state_t _hdl_can(const void *desc, uint8_t enable) {
  hdl_can_mcu_t *can = (hdl_can_mcu_t*)desc;
  hdl_can_var_t *can_var = (hdl_can_var_t *)can->obj_var;
  CAN_TypeDef *periph = (CAN_TypeDef *)can->config->phy;
  if(!IS_CAN_ALL_INSTANCE(periph)) return HDL_MODULE_FAULT;
  if(enable) {
    CL_REG_SET(RCC->APB1ENR, can->config->rcc); /* Clock enable */
    can_var->prescaler = can->config->prescaler; 
    can_var->time_seg1 = can->config->time_seg1; 
    can_var->time_seg2 = can->config->time_seg2;
    can_var->reset = 1;
    can_var->msg[0] = NULL;
    can_var->msg[1] = NULL;
    can_var->msg[2] = NULL;
    _hdl_can_filter_set(can, can->config->filters);
    coroutine_add(&can_var->worker, &_can_worker, can);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&can_var->worker);
  periph->MCR |= CAN_MCR_SLEEP; /* sleep mode */
  /* CAN1 clock common for both CAN interfaces, disabled in can_base */
  if(can->config->phy == (uint32_t)CAN2) CL_REG_CLEAR(RCC->APB1ENR, can->config->rcc);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_can_transmit(const void *desc, hdl_can_message_t *message) {
  hdl_can_mcu_t *can = (hdl_can_mcu_t *) desc;
  hdl_can_var_t *can_var = (hdl_can_var_t *)can->obj_var;
  CAN_TypeDef *periph = (CAN_TypeDef *)can->config->phy;
  if(!can_var->init_complete || can_var->reset) return HDL_FALSE;
  hdl_can_message_t **slot = NULL;
  for(uint32_t i = 0; i < 3; i++) {
    if(!slot && (periph->TSR & (CAN_TSR_TME0 << i)) && (can_var->msg[i] == NULL)) 
      slot = &can_var->msg[i];
    if(can_var->msg[i] == message) 
      return HDL_TRUE;
  }
  if(slot) {
    *slot = message;
    message->status = HDL_CAN_MESSAGE_STATUS_INITIAL;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static uint8_t _hdl_can_cancel(const void *desc, hdl_can_message_t *message) {
  hdl_can_mcu_t *can = (hdl_can_mcu_t *) desc;
  hdl_can_var_t *can_var = (hdl_can_var_t *)can->obj_var;
  CAN_TypeDef *periph = (CAN_TypeDef *)can->config->phy;
  for(uint32_t i = 0; i < 3; i++) {
    if(can_var->msg[i] == message) {
      can_var->msg[i]->status = HDL_CAN_MESSAGE_FAULT_ABORT | HDL_CAN_MESSAGE_STATUS_COMPLETE;
      periph->TSR |= (CAN_TSR_ABRQ0 << (8 * i));
      can_var->msg[i] = NULL;
      break;
    }
  }
  return HDL_TRUE;
}

static uint8_t _hdl_can_set(const void *desc, uint32_t qt, uint8_t prop, uint8_t phase1, uint8_t phase2) {
  hdl_can_mcu_t *can = (hdl_can_mcu_t*)desc;
  hdl_can_var_t *can_var = (hdl_can_var_t *)can->obj_var;
  if(((prop + phase1) < 2) || (prop + phase1) > 16) return HDL_FALSE;
  if(!phase2 || (phase2 > 8)) return HDL_FALSE;
  hdl_can_mcu_base_t *can_base = (hdl_can_mcu_base_t *)can->dependencies[0];
  hdl_clock_mcu_t *can_clk = (hdl_clock_mcu_t *)can_base->dependencies[0];
  hdl_clock_freq_t f;
  hdl_clock_get(can_clk, &f);
  uint32_t prescaler = ((f.num + ((qt*f.denom) >> 1)) / (qt * f.denom));
  if(prescaler > CAN_BTR_BRP) return HDL_FALSE;
  uint32_t time_seg1 = (prop + phase1 - 2) << 16;
  uint32_t time_seg2 = (phase2 - 1) << 20;
  can_var->prescaler = prescaler;
  can_var->time_seg1 = time_seg1;
  can_var->time_seg2 = time_seg2;
  can_var->reset = 1;
  return HDL_TRUE;
}

static void _hdl_can_subscribe(const void *desc, hdl_delegate_t *delegate) {
  hdl_can_mcu_t *can = (hdl_can_mcu_t*)desc;
  hdl_can_var_t *can_var = (hdl_can_var_t *)can->obj_var;
  hdl_event_subscribe(&can_var->event, delegate);
}

const hdl_can_iface_t hdl_can_mcu_iface = {
  .init = &_hdl_can,
  .set = &_hdl_can_set,
  .set_filter = &_hdl_can_filter_set,
  .transmit = &_hdl_can_transmit,
  .cancel = &_hdl_can_cancel,
  .subscribe = &_hdl_can_subscribe
};
