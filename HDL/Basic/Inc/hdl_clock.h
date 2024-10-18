#ifndef HDL_CLK_SOURCE_H_
#define HDL_CLK_SOURCE_H_

#include "port_clock.h"

typedef struct {
  hdl_module_t module;
  const hdl_clock_config_t *config;
  PRIVATE(hdl, HDL_CLOCK_PRV_SIZE);
} hdl_clock_t;

typedef struct {
  uint32_t num;                   /* frequency numerator  */
  uint32_t denom;                 /* frequency denominator */
} hdl_clock_freq_t;

hdl_module_state_t hdl_clock(void *desc, uint8_t enable);

void hdl_clock_calc_mul(hdl_clock_freq_t *in, int32_t factor, hdl_clock_freq_t *out);
void hdl_clock_calc_div(hdl_clock_freq_t *in, int32_t factor, hdl_clock_freq_t *out);

void hdl_get_clock(hdl_clock_t *clock, hdl_clock_freq_t *freq);

#endif // HDL_CLK_SOURCE_H_
