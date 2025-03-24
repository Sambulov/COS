#include "app.h"

#define SWITCH_DELAY              1000

void btn_handler(uint32_t event_trigger, void *sender, void *context) {
  (void)sender;
  static uint32_t delay = SWITCH_DELAY;
  if(event_trigger == HDL_BTN_EVENT_CLICK){
    delay = delay / 2;
    hdl_timer_reset(context, delay, HDL_TIMER_MODE_LOOP);
  }
  if(event_trigger == HDL_BTN_EVENT_HOLD) {
    delay = SWITCH_DELAY;
    hdl_timer_reset(context, delay, HDL_TIMER_MODE_LOOP);
  }
}

void led_handler(uint32_t event_trigger, void *sender, void *context) {
  static uint32_t led_no = 0;
  static const hdl_gpio_pin_t * const leds[] = { &mod_led1_pin, &mod_led2_pin, &mod_led3_pin, &mod_led4_pin };
  (void)sender; (void)event_trigger; (void)context;
  hdl_gpio_toggle(leds[led_no]);
  led_no = (led_no + 1) & 0x3;
}

const hdl_button_t button = {
  .iface = &hdl_button_iface,
  .dependencies = hdl_module_dependencies(&mod_button_pin, &mod_timer_ms),
  .config = hdl_module_config(hdl_button_config_t,
    .debounce_delay = 30,
    .hold_delay = 3000
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_BUTTON_VAR_SIZE)
};

const hdl_timer_t gpio_timer = {
  .iface = &hdl_timer_iface,
  .dependencies= hdl_module_dependencies(&mod_timer_ms),
  .config = hdl_module_config(hdl_timer_config_t,
    .mode = HDL_TIMER_MODE_LOOP,
    .delay = SWITCH_DELAY
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_TIMER_VAR_SIZE)
};

void gpio_test() {
  static hdl_delegate_t btn_del = {
    .context = (void *)&gpio_timer,
    .handler = &btn_handler
  };

  static hdl_delegate_t gpio_delegate = {
    .context = NULL,
    .handler = &led_handler
  };
  hdl_enable(&button);
  hdl_enable(&gpio_timer);
  hdl_button_subscribe(&button, &btn_del);
  hdl_timer_subscribe(&gpio_timer, &gpio_delegate);
}
