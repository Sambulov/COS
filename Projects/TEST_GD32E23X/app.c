#include "app.h"
#include "CodeLib.h"

extern hdl_time_counter_t mod_timer_ms;
extern hdl_gpio_pin_t uspd20k_ai4_ntc_pu;
extern hdl_button_t button;
extern hdl_gpio_pin_t mod_pin_12c0_scl;
extern hdl_i2c_t mod_i2c0;
extern hdl_i2c_t mod_i2c1;

void btn_handler(uint32_t event_trigger, void *sender, void *context) {
  (void)event_trigger;(void)sender;(void)context;
  __NOP();
}

void main() {
  __NOP();
  hdl_enable(&mod_timer_ms);
  hdl_enable(&uspd20k_ai4_ntc_pu);
  hdl_enable(&button);
  //hdl_enable(&mod_i2c0);
  //hdl_enable(&mod_i2c1);

  hdl_delegate_t btn_del = {
    .context = NULL,
    .handler = &btn_handler
  };

  hdl_button_subscribe(&button, &btn_del);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  uint32_t time = hdl_time_counter_get((hdl_module_base_t *)&mod_timer_ms);
  uint32_t now = time;

  hdl_gpio_toggle((hdl_module_base_t *)&uspd20k_ai4_ntc_pu);
  while (1) {
    cooperative_scheduler(false);
    now = hdl_time_counter_get(&mod_timer_ms);
    if(TIME_ELAPSED(time, 500, now)) {
      hdl_gpio_toggle(&uspd20k_ai4_ntc_pu);
      time = now;
    }
  }

}
