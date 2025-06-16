#ifndef HDL_PWM_H_
#define HDL_PWM_H_

typedef uint8_t (*hdl_pwm_set_t)(void const *desc, uint32_t *width, uint32_t *period);
typedef uint8_t (*hdl_pwm_stop_t)(void const *desc);
typedef uint32_t (*hdl_pwm_get_counter_t)(void const *desc);

typedef struct {
  hdl_module_initializer_t init;
  hdl_pwm_set_t set;
  hdl_pwm_get_counter_t get_counter;
  hdl_pwm_stop_t stop;
} hdl_pwm_iface_t;

hdl_module_new_t(hdl_pwm_t, 0, void, hdl_pwm_iface_t);

__STATIC_INLINE uint8_t hdl_pwm_set(void const *desc, uint32_t *width, uint32_t *period) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_pwm_iface_t *)((hdl_module_base_t *)desc)->iface)->set(desc, width, period);
}

__STATIC_INLINE uint8_t hdl_pwm_stop(void const *desc) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_pwm_iface_t *)((hdl_module_base_t *)desc)->iface)->stop(desc);
}

__STATIC_INLINE uint32_t hdl_pwm_get_counter(void const *desc) {
  MODULE_ASSERT(desc, 0);
  return ((hdl_pwm_iface_t *)((hdl_module_base_t *)desc)->iface)->get_counter(desc);
}

#endif /* HDL_PWM_H_ */