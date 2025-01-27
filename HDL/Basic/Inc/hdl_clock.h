#ifndef HDL_CLK_SOURCE_H_
#define HDL_CLK_SOURCE_H_

#include "port_clock.h"

typedef struct {
  uint32_t num;                   /* frequency numerator  */
  uint32_t denom;                 /* frequency denominator */
} hdl_clock_freq_t;

typedef void (*hdl_clock_get_t)(const void *, hdl_clock_freq_t *);

typedef struct {
  hdl_module_initializer_t init;
  hdl_clock_get_t get;
} hdl_clock_iface_t;

hdl_module_new_t(hdl_clock_t, HDL_CLOCK_PRV_SIZE, hdl_clock_config_t, hdl_clock_iface_t);

extern const hdl_clock_iface_t hdl_clock_iface;

__STATIC_INLINE void hdl_clock_get(const void *clk, hdl_clock_freq_t *out_frec) {
  ((hdl_clock_iface_t *)((hdl_module_base_t *)clk)->iface)->get(clk, out_frec);
}

void hdl_clock_calc_mul(hdl_clock_freq_t *in, int32_t factor, hdl_clock_freq_t *out);
void hdl_clock_calc_div(hdl_clock_freq_t *in, int32_t factor, hdl_clock_freq_t *out);

#endif // HDL_CLK_SOURCE_H_
