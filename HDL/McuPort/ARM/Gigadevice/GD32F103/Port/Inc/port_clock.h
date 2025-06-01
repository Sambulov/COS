#ifndef PORT_CLOCK_H_
#define PORT_CLOCK_H_

#include "hdl_clock.h"

#define IRC8M_CLOCK                  8000000UL
#define IRC40K_CLOCK                 40000UL

#define MAX_SYS_CLOCK             108000000UL
#define MAX_ADC_CLOCK             14000000UL
#define PLL_MAX_FREQ              MAX_SYS_CLOCK
#define APB1_MAX_FREQ             60000000UL
#define APB2_MAX_FREQ             MAX_SYS_CLOCK


#define HDL_CLOCK_PRV_SIZE 8

typedef enum {
  HDL_CLOCK_TYPE_HXTAL,          /* property: freq (can be 4 ~ 16MHz)*/
  HDL_CLOCK_TYPE_LXTAL,          /* property: freq (32768)*/
  HDL_CLOCK_TYPE_IRC8M,          /* property: freq = 8000000 const */
  HDL_CLOCK_TYPE_PLL_SEL,        /* property: div (can be 1 or 2 for HXTAL, 2 for IRC8M), module depends on HXTAL or IRC8M */
  HDL_CLOCK_TYPE_PLL,            /* property: mul (can be 2, 3, 4 .. 32), module depends on PLL_SEL */
  HDL_CLOCK_TYPE_SYS_SEL,        /* property: (NA), module depends on Core, IRC8M and optionnally on HXTAL or PLL */
  HDL_CLOCK_TYPE_AHB,            /* property: div (can be 1, 2, 4, 8, 16, 64, 128, 256, 512), module depends on SYS_SEL */
  HDL_CLOCK_TYPE_APB1,           /* property: div (can be 1, 2, 4, 8, 16), module depends on AHB */
  HDL_CLOCK_TYPE_APB2,           /* property: div (can be 1, 2, 4, 8, 16), module depends on AHB */

  HDL_CLOCK_TYPE_ADC,            /* property: div (can be 2, 4, 6, 8, 12, 16), module depends on HDL_CLOCK_TYPE_APB2 */

  HDL_CLOCK_TYPE_IRC28M,         /* property: freq = 28000000 const */
  HDL_CLOCK_TYPE_RTC_SEL,        /* property: (NA), module depends on IRC40K or LXTAL or HXTAL */
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
  uint32_t phy;
} hdl_clock_config_t;

#define HDL_CLOCK_VAR_SIZE 8

hdl_module_new_t(hdl_clock_mcu_t, HDL_CLOCK_VAR_SIZE, hdl_clock_config_t, hdl_clock_iface_t);

extern const hdl_clock_iface_t hdl_clock_iface;

#endif /* PORT_CLOCK_H_ */
