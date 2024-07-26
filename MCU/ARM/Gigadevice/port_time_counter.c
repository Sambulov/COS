#include "hdl_portable.h"

static void event_timer_isr(uint32_t event, void *sender, void *context) {
  hdl_time_counter_t *timer = (hdl_time_counter_t *)context;
  timer->val++;
}

hdl_module_state_t hdl_time_counter(void *desc, const uint8_t enable) {
  hdl_time_counter_t *timer = (hdl_time_counter_t *)desc;
  if(enable) {
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)timer->module.dependencies[1];
    timer->reload_isr.context = desc;
    timer->reload_isr.handler = &event_timer_isr;
    if(hdl_interrupt_request(ic, timer->reload_iterrupt, &timer->reload_isr))
      return HDL_MODULE_INIT_OK;
  }
  return HDL_MODULE_DEINIT_OK;
}
