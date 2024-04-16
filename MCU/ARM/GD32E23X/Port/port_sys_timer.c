#include "hdl_portable.h"

void event_sys_timer_isr(uint32_t event, void *sender, void *context) {
  hdl_sys_timer_t *timer = (hdl_sys_timer_t *)context;
  //hdl_clock_counter_t *counter = (hdl_clock_counter_t *)timer->module.dependencies[0];
  //hdl_clock_t *clock = (hdl_clock_t *)counter->module.dependencies[0];
  // TODO: calculate increment
  timer->val++;
}

hdl_module_state_t hdl_sys_timer(void *desc, const uint8_t enable) {
  hdl_sys_timer_t *timer = (hdl_sys_timer_t *)desc;
  if(enable) {
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)timer->module.dependencies[1];
    if(hdl_interrupt_request(ic, timer->reload_iterrupt, &event_sys_timer_isr, desc))
      return HDL_MODULE_INIT_OK;
  }
  
  return HDL_MODULE_DEINIT_OK;
}
