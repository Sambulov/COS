#ifndef HDL_SYS_TIMER_H_
#define HDL_SYS_TIMER_H_

typedef struct {
  hdl_module_t module;
  hdl_nvic_irq_n_t reload_iterrupt;
  uint32_t val;
}hdl_sys_timer_t;

hdl_module_state_t hdl_sys_timer(void *desc, const uint8_t enable);
static inline uint32_t hdl_sys_timer_get(hdl_sys_timer_t *desc) {
  return desc->val;
}

#define millis(desc)  hdl_sys_timer_get(desc)

#endif // HDL_SYS_TIMER_H_
