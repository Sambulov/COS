#include "app.h"
#include "CodeLib.h"

hdl_timer_t led_y_timer = {
  .module.dependencies = hdl_module_dependencies(&mod_timer_ms.module),
  .module.init = &hdl_timer,
};

hdl_timer_t led_g_timer = {
  .module.dependencies = hdl_module_dependencies(&mod_timer_ms.module),
  .module.init = &hdl_timer,
};

void led_y_timer_handler(uint32_t event_trigger, void *sender, void *context) {
  hdl_gpio_toggle(&mod_gpio_pin_led_y);
}

void led_g_timer_handler(uint32_t event_trigger, void *sender, void *context) {
  hdl_gpio_toggle(&mod_gpio_pin_led_g);
}

void main() {

  hdl_delegate_t led_y_timer_delegate = {
    .context = NULL,
    .handler = &led_y_timer_handler
  };

  hdl_delegate_t led_g_timer_delegate = {
    .context = NULL,
    .handler = &led_g_timer_handler
  };

  hdl_enable(&mod_timer_ms.module);
  hdl_enable(&led_y_timer.module);
  hdl_enable(&led_g_timer.module);
  hdl_enable(&mod_gpio_pin_btn_0.module);
  hdl_enable(&mod_gpio_pin_btn_1.module);
  hdl_enable(&mod_gpio_pin_led_y.module);
  hdl_enable(&mod_gpio_pin_led_y.module);
  hdl_enable(&mod_gpio_pin_led_g.module);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  hdl_event_subscribe(&led_y_timer.event, &led_y_timer_delegate);
  hdl_event_subscribe(&led_g_timer.event, &led_g_timer_delegate);
  hdl_timer_set(&led_y_timer, 100, HDL_TIMER_EVENT_LOOP);
  hdl_timer_set(&led_g_timer, 500, HDL_TIMER_EVENT_LOOP);

  hdl_gpio_set_active(&mod_gpio_pin_led_y);

  while (1) {
    cooperative_scheduler(false);
    // if(hdl_gpio_is_active(&mod_gpio_pin_btn_0)){
    //   hdl_gpio_set_active(&mod_gpio_pin_led_g);
    // }
    // else {
    //   hdl_gpio_set_inactive(&mod_gpio_pin_led_g);
    // }
    // if(hdl_gpio_is_active(&mod_gpio_pin_btn_1)){
    //   hdl_gpio_set_active(&mod_gpio_pin_led_y);
    // }
    // else {
    //   hdl_gpio_set_inactive(&mod_gpio_pin_led_y);
    // }
  }
}
