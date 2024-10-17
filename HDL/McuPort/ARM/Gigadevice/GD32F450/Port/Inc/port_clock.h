#ifndef PORT_CLOCK_H_
#define PORT_CLOCK_H_

#define HDL_CLOCK_PRV_SIZE   8

typedef enum {
  HDL_CLOCK_TYPE_HXTAL,          /* property: freq (can be 4 ~ 32MHz)*/
  HDL_CLOCK_TYPE_LXTAL,          /* property: freq (32768)*/
  HDL_CLOCK_TYPE_IRC16M,         /* property: freq = 16000000 const */
  HDL_CLOCK_TYPE_PLL_SEL,        /* property: div (can be 2, 3 .. 63), module depends on HXTAL or IRC16 */
  HDL_CLOCK_TYPE_PLL_VCO,        /* property: mul (can be 64, 65 .. 500), module depends on PLL_SEL */
  HDL_CLOCK_TYPE_PLL_P,          /* property: div (can be 2, 4, 6, 8), module depends on PLL_VCO */
  HDL_CLOCK_TYPE_PLL_Q,          /* property: div (can be 2, 3 .. 15), module depends on PLL_VCO */
  HDL_CLOCK_TYPE_SYS_SEL,        /* property: (NA), module depends on Core, IRC16 and optionnally on HXTAL or PLL_P */
  HDL_CLOCK_TYPE_AHB,            /* property: div (can be 1, 2, 4, 8, 16, 64, 128, 256, 512), module depends on SYS_SEL */
  HDL_CLOCK_TYPE_APB1,           /* property: div (can be 1, 2, 4, 8, 16), module depends on AHB */
  HDL_CLOCK_TYPE_APB2,           /* property: div (can be 1, 2, 4, 8, 16), module depends on AHB */

  HDL_CLOCK_TYPE_IRC48M,         /* property: freq = 48000000 const */
  HDL_CLOCK_TYPE_IRC40K,         /* property: freq = 40000 const */
} hdl_clock_type_t;

typedef union {
  uint32_t freq;
  uint32_t mul;
  uint32_t div;
} hdl_clock_property_t;

typedef struct {
  hdl_clock_type_t type;
  hdl_clock_property_t property;
} hdl_clock_config_t;


hdl_module_state_t hdl_clock_hxtal(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_lxtal(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_irc16m(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_irc40k(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_irc48m(void *desc, uint8_t enable);

hdl_module_state_t hdl_clock_selector_pll(void *desc, uint8_t enable);

hdl_module_state_t hdl_clock_selector_pll_vco(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_pll_n(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_pll_p(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_pll_q(void *desc, uint8_t enable);

hdl_module_state_t hdl_clock_system(void *desc, uint8_t enable);

hdl_module_state_t hdl_clock_ahb(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_apb1(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_apb2(void *desc, uint8_t enable);

#endif