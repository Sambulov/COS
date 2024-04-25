#include "hdl.h"
#include "CodeLib.h"

static void clock_calc(hdl_clock_prescaler_t *presc, hdl_clock_t *source, int32_t mulldiv_factor) {
  presc->div = source->div;
  presc->freq = source->freq;
  if(mulldiv_factor > 1) { /* mul */
    if(presc->div > 1) {
      uint32_t x = gcd(presc->div, mulldiv_factor);
      if(x > 1) {
        presc->div /= x;
        mulldiv_factor /= x;
      }
    }
    presc->freq = source->freq * mulldiv_factor;
  }
  else if(mulldiv_factor < -1) { /* div */
    mulldiv_factor = -mulldiv_factor;
    uint32_t x = gcd(presc->freq, mulldiv_factor);
    presc->freq = source->freq / x;
    presc->div = source->div * (mulldiv_factor / x);
  }
}

void clock_calc_mul(hdl_clock_prescaler_t *presc, hdl_clock_t *source) {
  if((presc != NULL) && (source != NULL))
    clock_calc(presc, source, presc->muldiv_factor);
}

void clock_calc_div(hdl_clock_prescaler_t *presc, hdl_clock_t *source) {
  if((presc != NULL) && (source != NULL))
    clock_calc(presc, source, -presc->muldiv_factor);
}
