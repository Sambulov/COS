#ifndef HDL_TIMER_H_
#define HDL_TIMER_H_

typedef struct {
  hdl_module_t module;
  hdl_nvic_irq_n_t reload_iterrupt;
  uint32_t val;
}hdl_timer_t;

hdl_module_state_t hdl_timer(void *desc, const uint8_t enable);
static inline uint32_t hdl_timer_get(hdl_timer_t *desc) {
  return desc->val;
}

#endif // HDL_TIMER_H_
