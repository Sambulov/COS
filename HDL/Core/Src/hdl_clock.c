#include "hdl_iface.h"

void hdl_clock_calc_mul(hdl_clock_freq_t *in, uint32_t factor, hdl_clock_freq_t *out) {
  if((out != NULL) && (in != NULL)) {
    out->denom = in->denom;
    out->num = in->num;
    decimal32_mull(out, factor);
  }
}

void hdl_clock_calc_div(hdl_clock_freq_t *in, uint32_t factor, hdl_clock_freq_t *out) {
  if((out != NULL) && (in != NULL)) {
    out->denom = in->denom;
    out->num = in->num;
    decimal32_div(out, factor);
  }
}
