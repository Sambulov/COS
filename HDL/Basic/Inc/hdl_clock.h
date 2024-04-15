#ifndef HDL_CLK_SOURCE_H_
#define HDL_CLK_SOURCE_H_

typedef struct _HDL_CLOCK_BASE_ {
  hdl_hardware_t hw;
  uint32_t freq;                  /* Whole parat of frequency  */
  uint32_t div;                   /* Fraction part of frequency */
} hdl_clock_t;

typedef struct {
  hdl_hardware_t hw;
  uint32_t freq;                  /* Whole parat of frequency  */
  uint32_t div;                   /* Fraction part of frequency */
  uint32_t muldiv_factor;         /* Prescale value or multiply coefficient */
} hdl_clock_prescaler_t;

#if defined ( NUM46X )

#endif

#if defined ( GD32E23X )

  typedef struct {
    hdl_hardware_t hw;
    uint32_t freq;
    uint32_t div;
    uint8_t clock_monitor_enable;  /* 0 - clock monitor disabled, !0 - enabled */
  } hdl_sys_clock_t;


typedef void (*event_handler_t)(uint32_t event, void *sender, void *context);

  /* Enable/Disable oscillator */
  hdl_init_state_t hdl_gd_clock_hxtal(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_lxtal(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_irc40k(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_irc28m(void *desc, uint8_t enable);
  /* pll prescaler(after HXTAL) */
  hdl_init_state_t hdl_gd_clock_pll_prescaler(void *desc, uint8_t enable);

  /* pll set source */
  hdl_init_state_t hdl_gd_clock_system_source(void *desc, uint8_t enable);
  
  /* pll selector */
  hdl_init_state_t hdl_gd_clock_selector_pll(void *desc, uint8_t enable);
  
  /* pll multiply coefficient */
  hdl_init_state_t hdl_gd_clock_pll_multiply_coefficient(void *desc, uint8_t enable);

  /* RTC selector */  
  hdl_init_state_t hdl_gd_clock_selector_rtc(void *desc, uint8_t enable);
  

  hdl_init_state_t hdl_gd_clock_pll(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_irc8m(void *desc, uint8_t enable);

  /* PLL setting for hxtal as a source*/
  hdl_init_state_t hdl_gd_clock_hxtal2pllsel(void *desc, uint8_t enable);

  /* PLL setting for IRC8M as a source*/
  hdl_init_state_t hdl_gd_clock_pll_source_irc8m_with_multiply_coefficient(void *desc, uint8_t enable);

  /* Pll multiply coefficient */
  hdl_init_state_t hdl_gd_clock_pll_mf(void *desc, uint8_t enable);


  hdl_init_state_t hdl_gd_clock_sys(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_ahb(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_apb1(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_apb2(void *desc, uint8_t enable);

#endif

#if defined ( STM32L0XX )

#endif

#endif // HDL_CLK_SOURCE_H_

