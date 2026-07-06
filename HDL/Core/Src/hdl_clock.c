#include "hdl_iface.h"

void hdl_clock_calc_mul(hdl_clock_freq_t *in, uint32_t factor, hdl_clock_freq_t *out) {
  if((out != NULL) && (in != NULL))
    *out = decimal32_mul(*in, decimal32_from_int(factor));
}

void hdl_clock_calc_div(hdl_clock_freq_t *in, uint32_t factor, hdl_clock_freq_t *out) {
  if((out != NULL) && (in != NULL))
    *out = decimal32_div(*in, decimal32_from_int(factor));
}
