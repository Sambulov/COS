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
  uint32_t freq;                  /* Whole parat of frequency  */
  uint32_t div;                   /* Fraction part of frequency */
  uint32_t muldiv_factor;         /* Prescale value or multiply coefficient */
} hdl_clock_prescaler_t;

void clock_calc_mul(hdl_clock_prescaler_t *presc, hdl_clock_t *source);
void clock_calc_div(hdl_clock_prescaler_t *presc, hdl_clock_t *source);

#endif // HDL_CLK_SOURCE_H_

