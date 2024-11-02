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

const hdl_button_config_t btn1_cnf = {
  .debounce_delay = 50,
  .hold_delay = 3000
};

hdl_button_t btn1 = {
  .module.init = &hdl_button,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_pin_btn_1.module, &mod_timer_ms.module),
  .config = &btn1_cnf,
};

void led_y_timer_handler(uint32_t event_trigger, void *sender, void *context) {
  hdl_gpio_toggle(&mod_gpio_pin_led_y);
}

void led_g_timer_handler(uint32_t event_trigger, void *sender, void *context) {
  hdl_gpio_toggle(&mod_gpio_pin_led_g);
}

void btn0_int_handler(uint32_t event_trigger, void *sender, void *context) {
  if(event_trigger & 0x100)
    hdl_gpio_toggle(&mod_gpio_pin_led_r);
}

void btn1_handler(uint32_t event_trigger, void *sender, void *context) {
  if(event_trigger == HDL_BTN_EVENT_CLICK)
    hdl_gpio_toggle(&mod_gpio_pin_led_r);
  if(event_trigger == HDL_BTN_EVENT_HOLD)
    hdl_gpio_set_active(&mod_gpio_pin_led_r);
}

hdl_delegate_t led_y_timer_delegate = {
  .context = NULL,
  .handler = &led_y_timer_handler
};

hdl_delegate_t led_g_timer_delegate = {
  .context = NULL,
  .handler = &led_g_timer_handler
};

hdl_delegate_t btn0_int_delegate = {
  .context = NULL,
  .handler = &btn0_int_handler
};

hdl_delegate_t btn1_delegate = {
  .context = NULL,
  .handler = &btn1_handler
};

void main() {
  hdl_enable(&mod_timer_ms.module);
  hdl_enable(&led_y_timer.module);
  hdl_enable(&led_g_timer.module);
  hdl_enable(&mod_gpio_pin_btn_0.module);
  hdl_enable(&mod_gpio_pin_btn_1.module);
  hdl_enable(&mod_gpio_pin_led_r.module);
  hdl_enable(&mod_gpio_pin_led_y.module);
  hdl_enable(&mod_gpio_pin_led_g.module);
  hdl_enable(&btn1.module);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  hdl_event_subscribe(&btn1.event, &btn1_delegate);
  hdl_event_subscribe(&led_y_timer.event, &led_y_timer_delegate);
  hdl_event_subscribe(&led_g_timer.event, &led_g_timer_delegate);
  hdl_timer_set(&led_y_timer, 100, HDL_TIMER_EVENT_LOOP);
  hdl_timer_set(&led_g_timer, 100, HDL_TIMER_EVENT_LOOP);

  hdl_interrupt_request(&mod_interrupt_controller, &mod_irq_gpio_btn_port, &btn0_int_delegate);

  hdl_gpio_set_active(&mod_gpio_pin_led_y);

  while (1) {
    cooperative_scheduler(false);
  }
}
