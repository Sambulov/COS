#ifndef HDL_CLK_SOURCE_H_
#define HDL_CLK_SOURCE_H_

typedef struct {
  uint32_t num;                   /* frequency numerator  */
  uint32_t denom;                 /* frequency denominator */
} hdl_clock_freq_t;

typedef void (*hdl_clock_get_t)(const void *, hdl_clock_freq_t *);

typedef struct {
  hdl_module_initializer_t init;
  hdl_clock_get_t get;
} hdl_clock_iface_t;

hdl_module_new_t(hdl_clock_t, 0, void, hdl_clock_iface_t);

__STATIC_INLINE void hdl_clock_get(const void *desc, hdl_clock_freq_t *out_frec) {
  MODULE_ASSERT(desc, );
  ((hdl_clock_iface_t *)((hdl_module_base_t *)desc)->iface)->get(desc, out_frec);
}

void hdl_clock_calc_mul(hdl_clock_freq_t *in, int32_t factor, hdl_clock_freq_t *out);
void hdl_clock_calc_div(hdl_clock_freq_t *in, int32_t factor, hdl_clock_freq_t *out);

#endif // HDL_CLK_SOURCE_H_
