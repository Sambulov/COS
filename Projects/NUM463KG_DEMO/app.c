#include "app.h"
#include "CodeLib.h"


void main() {
  hdl_enable(&mod_timer_ms.module);
  hdl_enable(&mod_gpio_pin_btn_0.module);
  hdl_enable(&mod_gpio_pin_btn_1.module);
  hdl_enable(&mod_gpio_pin_led_r.module);
  hdl_enable(&mod_gpio_pin_led_y.module);
  hdl_enable(&mod_gpio_pin_led_g.module);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  hdl_gpio_set_active(&mod_gpio_pin_led_r);

  while (1) {
    cooperative_scheduler(false);
    if(hdl_gpio_is_active(&mod_gpio_pin_btn_0)){
      hdl_gpio_set_active(&mod_gpio_pin_led_g);
    }
    else {
      hdl_gpio_set_inactive(&mod_gpio_pin_led_g);
    }
    if(hdl_gpio_is_active(&mod_gpio_pin_btn_1)){
      hdl_gpio_set_active(&mod_gpio_pin_led_y);
    }
    else {
      hdl_gpio_set_inactive(&mod_gpio_pin_led_y);
    }
  }
}
