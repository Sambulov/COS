#include "app.h"
#include "CodeLib.h"

void main() {
  //hdl_enable(&mod_timer_ms.module);
  //hdl_enable(&mod_i2c1.module);
  //hdl_enable(&mod_gpo_led.module);
  //hdl_enable(&sw_timer.module);
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  while (1) {
    cooperative_scheduler(false);
  }
}
