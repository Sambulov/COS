#ifndef HDL_TIMER_H_
#define HDL_TIMER_H_

#define HDL_TIMER_VAR_SIZE         40

typedef enum {
  HDL_TIMER_EVENT_IDLE,
  HDL_TIMER_EVENT_LOOP,
  HDL_TIMER_EVENT_SINGLE
} hdl_timer_mode_t;

typedef struct {
  hdl_timer_mode_t mode;
  uint32_t delay;
} hdl_timer_config_t;

typedef uint8_t (*hdl_timer_reset_t)(const void *timer, uint32_t delay, hdl_timer_mode_t mode);

typedef struct {
  hdl_module_initializer_t init;
  hdl_event_subscribe_t subscribe;
  hdl_timer_reset_t reset;
} hdl_timer_iface_t;

/* depends on:
  time_counter
 */
hdl_module_new_t(hdl_timer_t, HDL_TIMER_VAR_SIZE, hdl_timer_config_t, hdl_timer_iface_t);

extern hdl_timer_iface_t hdl_timer_iface;

__STATIC_INLINE uint8_t hdl_timer_reset(const void *timer, uint32_t delay, hdl_timer_mode_t mode) {
  return ((hdl_timer_t *)timer)->iface->reset(timer, delay, mode);
}

#endif /* HDL_TIMER_H_ */
