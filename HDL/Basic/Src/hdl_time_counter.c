#include "hdl_iface.h"

typedef struct {
  hdl_delegate_t reload_isr;
  uint32_t count;
} hdl_time_counter_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_time_counter_var_t, *((hdl_time_counter_t *)0)->obj_var, HDL_TIME_COUNTER_VAR_SIZE, hdl_time_counter.h);

static void event_timer_isr(uint32_t event, void *sender, void *context) {
  (void)event; (void)sender;
  ((hdl_time_counter_var_t *)context)->count++;
}

static hdl_module_state_t _hdl_time_counter(const void *desc, const uint8_t enable) {
  hdl_time_counter_t *timer = (hdl_time_counter_t *)desc;
  if(enable) {
    hdl_time_counter_var_t *timer_var = (hdl_time_counter_var_t *)timer->obj_var;
    timer_var->reload_isr.context = timer->obj_var;
    timer_var->reload_isr.handler = &event_timer_isr;
    timer_var->count = 0;
    hdl_event_subscribe(&timer->config->reload_interrupt->event, &timer_var->reload_isr);
    if(hdl_interrupt_request(timer->dependencies[1], timer->config->reload_interrupt))
      return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_UNLOADED;
}

static uint32_t _hdl_time_counter_get(const void *desc) {
  if(hdl_state(desc) != HDL_MODULE_FAULT) {
    return ((hdl_time_counter_var_t *)((hdl_time_counter_t *)desc)->obj_var)->count;
  }
  return 0;
}

const hdl_time_counter_iface_t hdl_time_counter_iface = {
  .init = &_hdl_time_counter,
  .get = &_hdl_time_counter_get
};
