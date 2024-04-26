#include "hdl_portable.h"

static void event_timer_isr(uint32_t event, void *sender, void *context) {
  hdl_timer_t *timer = (hdl_timer_t *)context;
  timer->val++;
}

hdl_module_state_t hdl_timer(void *desc, const uint8_t enable) {
  hdl_timer_t *timer = (hdl_timer_t *)desc;
  if(enable) {
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)timer->module.dependencies[1];
    if(hdl_interrupt_request(ic, timer->reload_iterrupt, &event_timer_isr, desc))
      return HDL_MODULE_INIT_OK;
  }
  return HDL_MODULE_DEINIT_OK;
  return HDL_MODULE_INIT_FAILED;
}
