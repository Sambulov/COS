#ifndef HDL_BUTTON_H_
#define HDL_BUTTON_H_

typedef enum {
  HDL_BTN_UNKNOWN = 0,
  HDL_BTN_RELEASED,
  HDL_BTN_PRESS_DEBOUNCE,
  HDL_BTN_PRESSED,
  HDL_BTN_HOLDING,
  HDL_BTN_CLICKED,
} hdl_btn_state_t;

#define HDL_BUTTON_VAR_SIZE    40

typedef enum {
  HDL_BTN_EVENT_RELEASE = 0x01,
  HDL_BTN_EVENT_PRESS   = 0x02,
  HDL_BTN_EVENT_HOLD    = 0x04,
  HDL_BTN_EVENT_CLICK   = 0x08,
} hdl_btn_event_t;

typedef struct {
  uint32_t debounce_delay;
  uint32_t hold_delay;
} hdl_button_config_t;

typedef hdl_btn_state_t (* hdl_button_state_get_t)(const void *);
typedef uint8_t (* hdl_button_sw_action_t)(const void *);

typedef struct {
  hdl_module_initializer_t init;
  hdl_event_subscribe_t subscribe;
  hdl_button_state_get_t state_get;
  hdl_button_sw_action_t press;
  hdl_button_sw_action_t click;
  hdl_button_sw_action_t release;
} hdl_button_iface_t;

/* depends on:
  gpio
  time_counter
 */
hdl_module_new_t(hdl_button_t, HDL_BUTTON_VAR_SIZE, hdl_button_config_t, hdl_button_iface_t);

extern hdl_button_iface_t hdl_button_iface;

__STATIC_INLINE hdl_btn_state_t hdl_button_state_get(const void *btn) {
  return ((hdl_button_t *)btn)->iface->state_get(btn);
}

__STATIC_INLINE uint8_t hdl_button_sw_press(const void *btn) {
  return ((hdl_button_t *)btn)->iface->press(btn);
}

__STATIC_INLINE uint8_t hdl_button_sw_click(const void *btn) {
  return ((hdl_button_t *)btn)->iface->click(btn);
}

__STATIC_INLINE uint8_t hdl_button_sw_release(const void *btn) {
  return ((hdl_button_t *)btn)->iface->release(btn);
}

__STATIC_INLINE void hdl_button_subscribe(const void *btn, hdl_delegate_t *delegate) {
  ((hdl_button_t *)btn)->iface->subscribe(btn, delegate);
}

#endif /* HDL_BUTTON_H_ */
