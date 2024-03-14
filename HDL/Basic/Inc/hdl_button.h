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

#define HDL_BUTTON_PRV_SIZE    20
	
typedef struct {
  const uint32_t debounce_delay;
  const uint32_t hold_delay;
  const hdl_gpio_state active_state;
  const hdl_gpio_t *btn_gpio;
} hdl_button_hw_t;

typedef struct {
	uint8_t __private[HDL_BUTTON_PRV_SIZE];
  const hdl_callback_t *button_event_cb;
} hdl_button_t;

void hdl_btn_init(hdl_button_t *desc, const hdl_button_hw_t *hw);
void hdl_btn_work(hdl_button_t *desc);
hdl_btn_state_t hdl_btn_state_get(hdl_button_t *desc);

uint8_t hdl_btn_press(hdl_button_t *desc);
uint8_t hdl_btn_click(hdl_button_t *desc);
uint8_t hdl_btn_release(hdl_button_t *desc);

#endif // HDL_BUTTON_H_
