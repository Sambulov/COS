#ifndef HDL_TICK_COUNTER_H_
#define HDL_TICK_COUNTER_H_

#include "port_tick_counter.h"

typedef void (*hdl_tick_counter_set_t)(const hdl_module_base_t *counter, uint32_t value, uint32_t period);
typedef uint32_t (*hdl_tick_counter_get_t)(const hdl_module_base_t *counter);
typedef void (*hdl_tick_counter_stop_t)(const hdl_module_base_t *counter);

typedef struct {
  hdl_module_initializer_t init;
  hdl_tick_counter_set_t set;
  hdl_tick_counter_get_t get;
  hdl_tick_counter_stop_t stop;
} hdl_tick_counter_iface_t;

/* depends on:
   hdl_clock_t
 */
hdl_module_new_t(hdl_tick_counter_t, 0, hdl_tick_counter_config_t, hdl_tick_counter_iface_t);

extern const hdl_tick_counter_iface_t hdl_tick_counter_iface;

__STATIC_INLINE uint32_t hdl_tick_counter_get(const hdl_module_base_t *counter) {
  return ((hdl_tick_counter_iface_t *)counter->iface)->get(counter);
}

__STATIC_INLINE void hdl_tick_counter_set(const hdl_module_base_t *counter, uint32_t value, uint32_t period) {
  ((hdl_tick_counter_iface_t *)counter->iface)->set(counter, value, period);
}

__STATIC_INLINE void hdl_tick_counter_stop(const hdl_module_base_t *counter) {
  ((hdl_tick_counter_iface_t *)counter->iface)->stop(counter);
}

#endif /* HDL_TICK_COUNTER_H_ */ 
