#include "hdl_portable.h"

static void _clock_calc(hdl_clock_t *this, hdl_clock_t *source, int32_t mulldiv_factor) {
  if(mulldiv_factor > 0) {
    this->freq = source->freq * mulldiv_factor;
    this->div = source->div;
  }
  else {
    this->freq = source->freq ;
    this->div = source->div * (-mulldiv_factor);
  }
  /* TODO: Normilize */
}

hdl_module_state_t hdl_clock(void *desc, uint8_t enable) {
  return HDL_MODULE_INIT_FAILED;
}
