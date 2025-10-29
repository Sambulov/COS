#ifndef HDL_PWM_H_
#define HDL_PWM_H_

#include "hdl_tick_counter.h"

typedef uint8_t (*hdl_pwm_get_set_t)(const void  *desc, uint32_t *threshold);
typedef uint8_t (*hdl_pwm_stop_t)(const void *desc);
typedef hdl_tick_counter_t *(*hdl_pwm_get_tickcounter_t)(const void *desc);

typedef struct {
  hdl_module_initializer_t init;
  hdl_pwm_get_tickcounter_t get_tickcounter;
  hdl_pwm_get_set_t get;
  hdl_pwm_get_set_t set;
} hdl_pwm_iface_t;

hdl_module_new_t(hdl_pwm_t, 0, void*, hdl_pwm_iface_t);

__STATIC_INLINE uint8_t hdl_pwm_set(const void *desc, uint32_t threshold) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_pwm_iface_t *)((hdl_module_base_t *)desc)->iface)->set(desc, &threshold);
}

__STATIC_INLINE uint8_t hdl_pwm_get(const void *desc, uint32_t *threshold) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_pwm_iface_t *)((hdl_module_base_t *)desc)->iface)->get(desc, threshold);
}

__STATIC_INLINE hdl_tick_counter_t *hdl_pwm_tickcounter(const void *desc) {
  MODULE_ASSERT(desc, NULL);
  return ((hdl_pwm_iface_t *)((hdl_module_base_t *)desc)->iface)->get_tickcounter(desc);
}

__STATIC_INLINE uint32_t hdl_pwm_period(const void *desc) {
  return hdl_tick_counter_get_period(hdl_pwm_tickcounter(desc));
}

#endif /* HDL_PWM_H_ */