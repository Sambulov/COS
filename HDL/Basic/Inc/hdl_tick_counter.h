#ifndef HDL_TICK_COUNTER_H_
#define HDL_TICK_COUNTER_H_

typedef void (*hdl_tick_counter_set_t)(const void *counter, uint32_t value, uint32_t period);
typedef uint32_t (*hdl_tick_counter_get_t)(const void *counter);
typedef void (*hdl_tick_counter_stop_t)(const void *counter);

typedef struct {
  hdl_module_initializer_t init;
  hdl_tick_counter_set_t set;
  hdl_tick_counter_get_t get;
  hdl_tick_counter_stop_t stop;
} hdl_tick_counter_iface_t;

__STATIC_INLINE uint32_t hdl_tick_counter_get(const void *desc) {
  return ((hdl_tick_counter_iface_t *)((hdl_module_base_t *)desc)->iface)->get(desc);
}

__STATIC_INLINE void hdl_tick_counter_set(const void *desc, uint32_t value, uint32_t period) {
  ((hdl_tick_counter_iface_t *)((hdl_module_base_t *)desc)->iface)->set(desc, value, period);
}

__STATIC_INLINE void hdl_tick_counter_stop(const void *desc) {
  ((hdl_tick_counter_iface_t *)((hdl_module_base_t *)desc)->iface)->stop(desc);
}

#endif /* HDL_TICK_COUNTER_H_ */ 
