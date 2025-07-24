#ifndef HDL_TICK_COUNTER_H_
#define HDL_TICK_COUNTER_H_

#include "hdl_clock.h"

typedef uint8_t (*hdl_tick_counter_get_set_t)(const void *counter, uint32_t *count, uint32_t *period);
typedef uint8_t (*hdl_tick_counter_stop_t)(const void *counter);

typedef struct {
  hdl_module_initializer_t init;
  hdl_clock_get_t clock_get;
  hdl_tick_counter_get_set_t set;
  hdl_tick_counter_get_set_t get;
  hdl_tick_counter_stop_t stop;
} hdl_tick_counter_iface_t;

hdl_module_new_t(hdl_tick_counter_t, 0, void*, hdl_tick_counter_iface_t);

/* returns timer reload frequency */
__STATIC_INLINE uint8_t hdl_tick_counter_get_clock(const void *desc, hdl_clock_freq_t *freq) {
  MODULE_ASSERT_FAST(desc, HDL_FALSE);
  return ((hdl_tick_counter_iface_t *)((hdl_module_base_t *)desc)->iface)->clock_get(desc, freq);
}

__STATIC_INLINE uint8_t hdl_tick_counter_get(const void *desc, uint32_t *value, uint32_t *period) {
  MODULE_ASSERT_FAST(desc, HDL_FALSE);
  return ((hdl_tick_counter_iface_t *)((hdl_module_base_t *)desc)->iface)->get(desc, value, period);
}

__STATIC_INLINE uint32_t hdl_tick_counter_get_tick(const void *desc) {
  MODULE_ASSERT_FAST(desc, 0);
  uint32_t val;
  ((hdl_tick_counter_iface_t *)((hdl_module_base_t *)desc)->iface)->get(desc, &val, NULL);
  return val;
}

__STATIC_INLINE uint32_t hdl_tick_counter_get_period(const void *desc) {
  MODULE_ASSERT_FAST(desc, 0);
  uint32_t val;
  ((hdl_tick_counter_iface_t *)((hdl_module_base_t *)desc)->iface)->get(desc, NULL, &val);
  return val;
}

__STATIC_INLINE uint8_t hdl_tick_counter_set(const void *desc, uint32_t *value, uint32_t *period) {
  MODULE_ASSERT(desc, HDL_FALSE);
  ((hdl_tick_counter_iface_t *)((hdl_module_base_t *)desc)->iface)->set(desc, value, period);
}

__STATIC_INLINE uint8_t hdl_tick_counter_stop(const void *desc) {
  MODULE_ASSERT(desc, HDL_FALSE);
  ((hdl_tick_counter_iface_t *)((hdl_module_base_t *)desc)->iface)->stop(desc);
}

#endif /* HDL_TICK_COUNTER_H_ */ 
