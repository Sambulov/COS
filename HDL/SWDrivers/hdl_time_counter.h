#ifndef HDL_TIME_COUNTER_H_
#define HDL_TIME_COUNTER_H_

#define HDL_TIME_COUNTER_VAR_SIZE         28

typedef uint32_t (*hdl_time_counter_get_t)(const void *);

typedef struct {
  hdl_module_initializer_t init;
  hdl_time_counter_get_t get;
} hdl_time_counter_iface_t;

__STATIC_INLINE uint32_t hdl_time_counter_get(const void *desc) {
  MODULE_ASSERT(desc, 0);
  return ((hdl_time_counter_iface_t *)((hdl_module_base_t *)desc)->iface)->get(desc);
}

/* depends on
  mod_tick_counter
  mod_irq_reload
*/
hdl_module_new_t(hdl_time_counter_t, HDL_TIME_COUNTER_VAR_SIZE, void *, hdl_time_counter_iface_t);

extern const hdl_time_counter_iface_t hdl_time_counter_iface;

#endif /* HDL_TIME_COUNTER_H_ */
