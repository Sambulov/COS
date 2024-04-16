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

#define HDL_BUTTON_PRV_SIZE    16
	
typedef struct {
  const uint32_t debounce_delay;
  const uint32_t hold_delay;
  const hdl_gpio_state active_state;
  const hdl_gpio_pin_t *btn_gpio;
} hdl_button_hw_t;

typedef enum {
  HDL_BTN_EVENT_RELEASE = 0x01,
  HDL_BTN_EVENT_PRESS   = 0x02,
  HDL_BTN_EVENT_HOLD    = 0x04,
  HDL_BTN_EVENT_CLICK   = 0x08,
} hdl_btn_event_t;

typedef struct {
	uint8_t __private[HDL_BUTTON_PRV_SIZE];
  const hdl_button_hw_t *module;
  hdl_btn_event_t event_mask;
  event_handler_t button_event_cb;
  void *context;
} hdl_button_t;

void hdl_btn_work(hdl_button_t *desc);
hdl_btn_state_t hdl_btn_state_get(hdl_button_t *desc);

uint8_t hdl_btn_press(hdl_button_t *desc);
uint8_t hdl_btn_click(hdl_button_t *desc);
uint8_t hdl_btn_release(hdl_button_t *desc);

#endif // HDL_BUTTON_H_
