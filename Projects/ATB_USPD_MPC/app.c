#include "app.h"
#include "CodeLib.h"

hdl_timer_t mod_timer_x = {
  .module.dependencies = hdl_module_dependencies(&mod_timer_ms.module),
  .module.init = &hdl_timer,
};

hdl_module_t mod_app = {
  .dependencies = hdl_module_dependencies(&mod_timer_x.module, &mod_gpio_a15_led1.module, &mod_gpio_c10_led2.module, &mod_gpio_c11_led3.module,
                                          &mod_gpio_c12_led4.module, &mod_gpio_d0_led5.module, &mod_gpio_d1_led6.module, &mod_gpio_d2_led7.module,
                                          &mod_gpio_d3_led8.module, &mod_gpio_d4_led9.module, &mod_gpio_d5_led10.module, &mod_gpio_d6_led11.module,
                                          &mod_gpio_d7_led12.module)
};

void blink_hundler(uint32_t event_trigger, void *sender, void *context) {
  static uint8_t i = 1;
  hdl_gpio_pin_t *pin = (hdl_gpio_pin_t *)mod_app.dependencies[i++];
  if(pin == NULL) {
    i = 1;
    pin = (hdl_gpio_pin_t *)mod_app.dependencies[i++];
  }
  hdl_gpio_toggle(pin);
}

void main() {
  hdl_delegate_t blink_timer = {
    .context = NULL,
    .handler = blink_hundler
  };
  hdl_enable(&mod_app);
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  hdl_timer_subscribe(&mod_timer_x, &blink_timer);
  hdl_timer_set(&mod_timer_x, 10, HDL_TIMER_EVENT_LOOP);
  while (1) {
    cooperative_scheduler(false);
  }
  while (1);
}
