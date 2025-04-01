#include "hdl_portable.h"
#include "Macros.h"

typedef struct {
  hdl_clock_freq_t freq;
} hdl_clock_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_clock_var_t, *((hdl_clock_mcu_t *)0)->obj_var, HDL_CLOCK_VAR_SIZE, port_clock.h);

extern hdl_module_state_t __hdl_clock(const void *desc, uint8_t enable);

static uint8_t _hdl_get_clock(const void *desc, hdl_clock_freq_t *freq) {
  if(freq != NULL) {
    hdl_clock_var_t *clk_var = (hdl_clock_var_t *)((hdl_clock_t *)desc)->obj_var;
    freq->num = clk_var->freq.num;
    freq->denom = clk_var->freq.denom;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

const hdl_clock_iface_t hdl_clock_iface = {
  .init = &__hdl_clock,
  .get = &_hdl_get_clock
};
