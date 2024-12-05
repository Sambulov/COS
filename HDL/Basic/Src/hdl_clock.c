#include "hdl.h"
#include "CodeLib.h"

static void clock_calc(hdl_clock_freq_t *out, hdl_clock_freq_t *in, int32_t mulldiv_factor) {
  out->denom = in->denom;
  out->num = in->num;
  if(mulldiv_factor > 1) { /* mul */
    if(out->denom > 1) {
      uint32_t x = gcd(out->denom, mulldiv_factor);
      if(x > 1) {
        out->denom /= x;
        mulldiv_factor /= x;
      }
    }
    out->num = in->num * mulldiv_factor;
  }
  else if(mulldiv_factor < -1) { /* denom */
    mulldiv_factor = -mulldiv_factor;
    uint32_t x = gcd(out->num, mulldiv_factor);
    out->num = in->num / x;
    out->denom = in->denom * (mulldiv_factor / x);
  }
}

void hdl_clock_calc_mul(hdl_clock_freq_t *in, int32_t factor, hdl_clock_freq_t *out) {
  if((out != NULL) && (in != NULL))
    clock_calc(out, in, factor);
}

void hdl_clock_calc_div(hdl_clock_freq_t *in, int32_t factor, hdl_clock_freq_t *out) {
  if((out != NULL) && (in != NULL))
    clock_calc(out, in, -factor);
}
