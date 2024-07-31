#ifndef HDL_TIME_COUNTER_H_
#define HDL_TIME_COUNTER_H_

typedef struct {
  hdl_module_t module;
  hdl_interrupt_t *reload_interrupt;
  hdl_delegate_t reload_isr;
  uint32_t val;
}hdl_time_counter_t;

hdl_module_state_t hdl_time_counter(void *desc, const uint8_t enable);
static inline uint32_t hdl_time_counter_get(hdl_time_counter_t *desc) {
  return desc->val;
}

#endif /* HDL_TIME_COUNTER_H_ */
