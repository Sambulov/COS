#include "hdl_portable.h"

hdl_module_state_t hdl_adc(void *desc, uint8_t enable) {
  return HDL_MODULE_INIT_FAILED;
}

uint8_t hdl_adc_start(hdl_adc_t *hdl_adc, void *buff) {
  return HDL_FALSE;
}
