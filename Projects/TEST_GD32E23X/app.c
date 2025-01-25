#include "app.h"
#include "CodeLib.h"

extern hdl_time_counter_t mod_timer_ms;
extern hdl_gpio_pin_t uspd20k_ai4_ntc_pu;

void main() {
  __NOP();
  hdl_enable(&mod_timer_ms);
  hdl_enable(&uspd20k_ai4_ntc_pu);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  uint32_t time = hdl_time_counter_get((hdl_module_base_t *)&mod_timer_ms);
  uint32_t now = time;

  hdl_gpio_toggle((hdl_module_base_t *)&uspd20k_ai4_ntc_pu);
  while (1) {
    cooperative_scheduler(false);
    now = hdl_time_counter_get((hdl_module_base_t *)&mod_timer_ms);
    if(TIME_ELAPSED(time, 500, now)) {
      hdl_gpio_toggle((hdl_module_base_t *)&uspd20k_ai4_ntc_pu);
      time = now;
    }
  }

}
