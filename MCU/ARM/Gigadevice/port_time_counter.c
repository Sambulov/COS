#include "hdl_portable.h"

typedef struct {
  hdl_module_t module;
  const hdl_time_counter_config_t *config;
  hdl_delegate_t reload_isr;
  uint32_t count;
} hdl_time_counter_private_t;

_Static_assert(sizeof(hdl_time_counter_private_t) == sizeof(hdl_time_counter_t), "In hdl_time_counter.h data structure size of hdl_time_counter_t doesn't match, check HDL_TIME_COUNTER_PRV_SIZE");

static void event_timer_isr(uint32_t event, void *sender, void *context) {
  ((hdl_time_counter_private_t *)context)->count++;
}

hdl_module_state_t hdl_time_counter(void *desc, const uint8_t enable) {
  hdl_time_counter_private_t *timer = (hdl_time_counter_private_t *)desc;
  if(enable) {
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)timer->module.dependencies[1];
    timer->reload_isr.context = desc;
    timer->reload_isr.handler = &event_timer_isr;
    timer->count = 0;
    if(hdl_interrupt_request(ic, timer->config->reload_interrupt, &timer->reload_isr))
      return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_UNLOADED;
}

uint32_t hdl_time_counter_get(hdl_time_counter_t *desc) {
  if((desc != NULL) && (hdl_state(&desc->module) == HDL_MODULE_ACTIVE)) {
    return ((hdl_time_counter_private_t *)desc)->count;
  }
  return 0;
}
