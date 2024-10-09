#ifndef HDL_CLK_SOURCE_H_
#define HDL_CLK_SOURCE_H_

#include "port_clock.h"

typedef struct _HDL_CLOCK_BASE_ {
  hdl_module_t module;
  uint32_t freq;                  /* Whole parat of frequency  */
  uint32_t div;                   /* Fraction part of frequency */
} hdl_clock_t;

typedef struct {
  hdl_module_t module;
  uint32_t freq;                  /* frequency numerator  */
  uint32_t div;                   /* frequency denominator */
  uint32_t muldiv_factor;         /* Prescale value or multiply coefficient */
} hdl_clock_prescaler_t;

void hdl_clock_calc_mul(hdl_clock_t *presc, hdl_clock_t *source, int32_t factor);
void hdl_clock_calc_div(hdl_clock_t *presc, hdl_clock_t *source, int32_t factor);

float hdl_get_clock(hdl_clock_t *clock);

#endif // HDL_CLK_SOURCE_H_

