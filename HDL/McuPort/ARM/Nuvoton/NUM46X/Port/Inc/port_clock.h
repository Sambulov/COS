#ifndef PORT_CLOCK_H_
#define PORT_CLOCK_H_

#define HDL_CLOCK_PRV_SIZE 8

typedef enum {
  HDL_CLOCK_TYPE_HXTAL,          /* property: freq (can be 4 ~ 24MHz)*/
  HDL_CLOCK_TYPE_LXTAL,          /* property: freq 32768 const */
  HDL_CLOCK_TYPE_IRC12M,         /* property: freq 12000000 const */
  HDL_CLOCK_TYPE_IRC48M,         /* property: freq 48000000 const */
  HDL_CLOCK_TYPE_IRC10K,         /* property: freq 10000 const */

  HDL_CLOCK_TYPE_PLL_REF,        /* property: div (can be 1, 2, 3, ... 32), depends on HXTAL or IRC12M, PLL_REF: 4~8MHz */
  HDL_CLOCK_TYPE_PLL_VCO,        /* property: mul (can be 4, 6, 8 ... 1028), depends on PLL_REF, PLL_VCO: 200~500MHz */
  HDL_CLOCK_TYPE_PLL,            /* property: div (can be 1, 2, 4), depends on PLL_VCO, PLL_OUT: 50~500MHz */

  HDL_CLOCK_TYPE_PLLFN_REF,      /* property: div (can be 1, 2, 3, ... 32), depends on HXTAL or IRC12M, PLLFN_REF: 1~8MHz */
  HDL_CLOCK_TYPE_PLLFN_VCO,      /* property: mul (can be 12, 14, 16, 18 ... 510), depends on PLLFN_REF, PLLFN_VCO: 200~500MHz */
  HDL_CLOCK_TYPE_PLLFN_VCOFR,    /* property: mul (can be 1, 2, 3, 4 ... 4095), depends on PLLFN_VCO, PLLFN_VCOFR/4096: 200~500MHz */
  HDL_CLOCK_TYPE_PLLFN,          /* property: div (can be 1, 2, 4), depends on PLL_VCOFR, PLLFN: 50~500MHz */

  HDL_CLOCK_TYPE_SYS,            /* property: div (can be 1, 2, 4), depends on IRC12M or IRC10K or HXTAL or LXTAL or PLL, SYS: 0~200MHz??? */

  HDL_CLOCK_TYPE_APB0,           /* property: div (can be 1, 2, 4, 8, 16), depends on SYS, APB0: 0~200MHz??? */
  HDL_CLOCK_TYPE_APB1,           /* property: div (can be 1, 2, 4, 8, 16), depends on SYS, APB1: 0~200MHz??? */
  
  HDL_CLOCK_TYPE_SYSTICK_REF,    /* property: div (1 or 2 for HXTAL; 1 for LXTAL; 2 for other), depends on either: HXTAL, LXTAL, IRC12M, SYS */

  HDL_CLOCK_TYPE_SYSTICK,        /* property: NA, depends on SYS or SYSTICK_REF */
  
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

#endif
