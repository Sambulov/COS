#ifndef PORT_CLOCK_H_
#define PORT_CLOCK_H_

#include "hdl_clock.h"

#define HDL_CLOCK_VAR_SIZE   8

typedef enum {
  HDL_CLOCK_TYPE_IRC32K,         /* property: freq = 32000 const */
  HDL_CLOCK_TYPE_HXTAL,          /* property: freq (can be 4 ~ 26MHz)*/
  HDL_CLOCK_TYPE_LXTAL,          /* property: freq (32768)*/
  HDL_CLOCK_TYPE_IRC16M,         /* property: freq = 16000000 const */
  HDL_CLOCK_TYPE_PLL_M,          /* property: div (can be 2, 3 .. 63), module depends on HXTAL or IRC16 */

  /* For PLL clocks also need to be dependency on corresponding hdl_clock_mcu_pll_t */
  HDL_CLOCK_TYPE_PLL_N,          /* property: mul (can be 50, 51 .. 432), module depends on PLL_SEL */
  HDL_CLOCK_TYPE_PLL_P,          /* property: div (can be 2, 4, 6, 8), module depends on PLL_N */
  HDL_CLOCK_TYPE_PLL_Q,          /* property: div (can be 2, 3 .. 15), module depends on PLL_N. Clock source for 48MHz periph */
  HDL_CLOCK_TYPE_PLL_I2S_N,      /* property: mul (can be 50, 51 .. 432), module depends on PLL_SEL */
  HDL_CLOCK_TYPE_PLL_I2S_R,      /* property: div (can be 2, 3 .. 7), module depends on PLL_I2S_N */

  HDL_CLOCK_TYPE_SYS_SEL,        /* property: (NA), module depends on Core, IRC16; optionnally on HXTAL or PLL_P */
  HDL_CLOCK_TYPE_AHB,            /* property: div (can be 1, 2, 4, 8, 16, 64, 128, 256, 512), module depends on SYS_SEL */
  HDL_CLOCK_TYPE_APB1,           /* property: div (can be 1, 2, 4, 8, 16), module depends on AHB */
  HDL_CLOCK_TYPE_APB2,           /* property: div (can be 1, 2, 4, 8, 16), module depends on AHB */

  HDL_CLOCK_TYPE_APB1_TIMERS,    /* property: (NA) if apb1_div != 1 mul 2 else mul 1, module depends on APB1 */
  HDL_CLOCK_TYPE_APB2_TIMERS,    /* property: (NA) if apb2_div != 1 mul 2 else mul 1, module depends on APB2 */

  HDL_CLOCK_TYPE_MCO1,           /* property: div (can be 1, 2, 3, 4, 5), module depends on either LXTAL, HXTAL, IRC16M, PLL_P */
  HDL_CLOCK_TYPE_MCO2,           /* property: div (can be 1, 2, 3, 4, 5), module depends on either SYS_SEL, PLL_I2S_R, HXTAL, PLL_P */

} hdl_clock_type_t;

typedef union {
  uint32_t freq;
  uint32_t mul;
  uint32_t div;
} hdl_clock_property_t;

typedef struct {
  hdl_clock_type_t type;
  hdl_clock_property_t property;
  uint32_t phy;
} hdl_clock_config_t;

hdl_module_new_t(hdl_clock_mcu_t, HDL_CLOCK_VAR_SIZE, hdl_clock_config_t, hdl_clock_iface_t);

extern const hdl_clock_iface_t hdl_clock_iface;

/* depends on
  For main PLL
    hdl_clock pll_n
    hdl_clock pll_p
    hdl_clock pll_q 
  For I2S_PLL
    hdl_clock i2s_pll_n
    hdl_clock i2s_pll_r
*/
hdl_module_new_t(hdl_clock_mcu_pll_t, 0, void, hdl_module_base_iface_t);

extern const hdl_module_base_iface_t hdl_clock_mcu_pll_iface;


#endif