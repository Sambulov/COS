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

#define HDL_BUTTON_PRV_SIZE    36

typedef enum {
  HDL_BTN_EVENT_RELEASE = 0x01,
  HDL_BTN_EVENT_PRESS   = 0x02,
  HDL_BTN_EVENT_HOLD    = 0x04,
  HDL_BTN_EVENT_CLICK   = 0x08,
} hdl_btn_event_t;

/* depends on:
  gpio
  time_counter
 */
typedef struct {
  hdl_module_t module;
  uint32_t debounce_delay;
  uint32_t hold_delay;
  hdl_event_t event;
  PRIVATE(hdl, HDL_BUTTON_PRV_SIZE);
} hdl_button_t;

hdl_module_state_t hdl_button(void *desc, uint8_t enable);

hdl_btn_state_t hdl_btn_state_get(hdl_button_t *btn);

uint8_t hdl_btn_sw_press(hdl_button_t *btn);
uint8_t hdl_btn_sw_click(hdl_button_t *btn);
uint8_t hdl_btn_sw_release(hdl_button_t *btn);

#endif /* HDL_BUTTON_H_ */
