#include "bldl.h"

hdl_module_state_t bldl_uspd_ain_port(void *desc, uint8_t enable) {
  if(enable) {
    return HDL_MODULE_INIT_OK;
  }
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t bldl_uspd_discrete_port(void *desc, uint8_t enable) {
  if(enable) {
    return HDL_MODULE_INIT_OK;
  }
  return HDL_MODULE_DEINIT_OK;
}
