#include "hdl_iface.h"

typedef struct {
  hdl_delegate_t can_isr;
  const hdl_transceiver_t *transceiver;
  coroutine_t worker;
  uint8_t tx_data[2];
  uint8_t rx_byte  : 1;
  uint8_t tx_byte  : 1;
} hdl_can_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_can_var_t, *((hdl_can_mcu_t *)0)->obj_var, HDL_CAN_VAR_SIZE, port_can.h);

static uint8_t _can_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this; (void)arg;
  //hdl_can_mcu_t *can = (hdl_can_mcu_t *) arg;
  //hdl_can_var_t *can_var = (hdl_can_var_t *)can->obj_var;
  return cancel;
}

static void event_can_isr(uint32_t event, void *sender, void *context) {
  (void)event; (void)sender; (void)context;
  //hdl_can_mcu_t *can = (hdl_can_mcu_t *)context;
  //hdl_can_var_t *can_var = (hdl_can_var_t *)can->obj_var;
}

static hdl_module_state_t _hdl_can(const void *desc, uint8_t enable) {
  hdl_can_mcu_t *can = (hdl_can_mcu_t*)desc;
  hdl_can_var_t *can_var = (hdl_can_var_t *)can->obj_var;
  //hdl_time_counter_t *timer = (hdl_time_counter_t *)can->dependencies[4];

  //CAN_TypeDef *periph = (CAN_TypeDef *)can->config->phy;
  volatile uint32_t *rcc_en = &RCC->APB1ENR;
  volatile uint32_t *rcc_rst = &RCC->APB1RSTR;
  
  switch (can->config->phy) {
    case (uint32_t)CAN1:
    case (uint32_t)CAN2:
      break;
    default:
      return HDL_MODULE_FAULT;
  }
  CL_REG_SET(*rcc_rst, can->config->rcu);
  CL_REG_CLEAR(*rcc_rst, can->config->rcu);
  if(enable) {
    CL_REG_SET(*rcc_en, can->config->rcu);

    coroutine_add(&can_var->worker, &_can_worker, can);
    //hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)can->dependencies[3];
    can_var->can_isr.context = can;
    can_var->can_isr.handler = &event_can_isr;
    //hdl_event_subscribe(&can->config->interrupt->event, &can_var->can_isr);
    //hdl_interrupt_request(ic, can->config->interrupt);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&can_var->worker);
  CL_REG_CLEAR(*rcc_en, can->config->rcu);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_can_transfer(const void *desc, hdl_can_message_t *message) {
  (void)desc; (void)message;
  //hdl_can_mcu_t *can = (hdl_can_mcu_t *) desc;
  //hdl_can_var_t *can_var = (hdl_can_var_t *)can->obj_var;  
  return HDL_TRUE;
}

const hdl_can_iface_t hdl_can_mcu_iface = {
  .init = &_hdl_can,
  .transfer = &_hdl_can_transfer,
};
