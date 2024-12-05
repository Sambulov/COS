#include "app.h"
#include "CodeLib.h"

extern hdl_time_counter_t mod_timer_ms;

void main() {
  __NOP();
  hdl_enable(&mod_timer_ms.module);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  uint32_t time = 0;
  uint32_t now = hdl_time_counter_get(&mod_timer_ms);
  while(!TIME_ELAPSED(0, 2000, now)) {
    now = hdl_time_counter_get(&mod_timer_ms);
  }

  while (1) {
    cooperative_scheduler(false);
  }

}
