#ifndef HDL_TIMER_H_
#define HDL_TIMER_H_

#define HDL_TIMER_VAR_SIZE         40

typedef enum {
  HDL_TIMER_MODE_IDLE,
  HDL_TIMER_MODE_LOOP,
  HDL_TIMER_MODE_SINGLE
} hdl_timer_mode_t;

typedef struct {
  hdl_timer_mode_t mode;
  uint32_t delay;
} hdl_timer_config_t;

typedef uint8_t (*hdl_timer_reset_t)(const void *timer, uint32_t delay, hdl_timer_mode_t mode);
typedef hdl_timer_mode_t (*hdl_timer_mode_get_t)(const void *desc);
typedef uint32_t (*hdl_timer_left_t)(const void *desc);

typedef struct {
  hdl_module_initializer_t init;
  hdl_event_subscribe_t subscribe;
  hdl_timer_reset_t reset;
  hdl_timer_mode_get_t mode;
  hdl_timer_left_t left;
} hdl_timer_iface_t;

/* depends on:
  time_counter
 */
hdl_module_new_t(hdl_timer_t, HDL_TIMER_VAR_SIZE, hdl_timer_config_t, hdl_timer_iface_t);

extern const hdl_timer_iface_t hdl_timer_iface;

__STATIC_INLINE void hdl_timer_subscribe(const void *desc, hdl_delegate_t *delegate) {
  MODULE_ASSERT(desc, );
  ((hdl_timer_t *)desc)->iface->subscribe(desc, delegate);
}

__STATIC_INLINE uint8_t hdl_timer_reset(const void *desc, uint32_t delay, hdl_timer_mode_t mode) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_timer_t *)desc)->iface->reset(desc, delay, mode);
}

__STATIC_INLINE hdl_timer_mode_t hdl_timer_mode(const void *desc) {
  MODULE_ASSERT(desc, HDL_TIMER_MODE_IDLE);
  return ((hdl_timer_t *)desc)->iface->mode(desc);
}

__STATIC_INLINE uint32_t hdl_timer_left(const void *desc) {
  MODULE_ASSERT(desc, 0);
  return ((hdl_timer_t *)desc)->iface->left(desc);
}

#endif /* HDL_TIMER_H_ */
