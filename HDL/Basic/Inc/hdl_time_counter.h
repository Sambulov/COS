#ifndef HDL_TIME_COUNTER_H_
#define HDL_TIME_COUNTER_H_

#define HDL_TIME_COUNTER_VAR_SIZE         28

typedef struct {
  hdl_interrupt_t *reload_interrupt;
} hdl_time_counter_config_t;

typedef uint32_t (*hdl_time_counter_get_t)(const void *);

typedef struct {
  hdl_module_initializer_t init;
  hdl_time_counter_get_t get;
} hdl_time_counter_iface_t;

__STATIC_INLINE uint32_t hdl_time_counter_get(const void *desc) {
  return ((hdl_time_counter_iface_t *)((hdl_module_base_t *)desc)->iface)->get(desc);
}

hdl_module_new_t(hdl_time_counter_t, HDL_TIME_COUNTER_VAR_SIZE, hdl_time_counter_config_t, hdl_time_counter_iface_t);

extern const hdl_time_counter_iface_t hdl_time_counter_iface;

#endif /* HDL_TIME_COUNTER_H_ */
