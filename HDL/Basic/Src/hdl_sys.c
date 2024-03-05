#include "hdl.h"

void hdl_delay(uint32_t ms) {
  uint32_t start_ticks = hdl_millis();
  while (1) {
    if ((hdl_millis() - start_ticks) >= ms) {
      return;
    }
  }
}
