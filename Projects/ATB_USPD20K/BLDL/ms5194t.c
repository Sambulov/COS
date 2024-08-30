#include "app.h"

hdl_module_state_t hdl_ms5194t(void *desc, uint8_t enable) {
  if(enable) {
    return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_ACTIVE;
}
