#ifndef HDL_CLK_SOURCE_H_
#define HDL_CLK_SOURCE_H_


typedef struct _HDL_CLOCK_BASE_ {
  hdl_hardware_t hw;
  float freq;
} hdl_clock_t;

typedef struct {
  hdl_clock_t clock;
  hdl_clock_t *source;
  uint32_t muldiv_factor;
} hdl_clock_prescaler_t;

typedef struct {
  hdl_clock_t clock;
  hdl_clock_t *source;
} hdl_clock_selector_t;

#if defined ( NUM46X )

#endif

#if defined ( GD32E23X )

  typedef struct {
    hdl_clock_t clock;
    hdl_clock_t *source;
    uint8_t clock_monitor_enable;  /* 0 - clock monitor disabled, !0 - enabled */
  } hdl_sys_clock_t;

  hdl_init_state_t hdl_gd_clock_hxtal(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_lxtal(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_irc40k(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_irc8m(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_irc28m(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_rtc(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_hxtal_pll(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_pll_mf(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_pll(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_sys(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_ahb(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_apb1(void *desc, uint8_t enable);
  hdl_init_state_t hdl_gd_clock_apb2(void *desc, uint8_t enable);

#endif

#if defined ( STM32L0XX )

#endif

void hdl_clock_init(hdl_clock_t *clock);
uint8_t hdl_clock_ready(hdl_clock_t *clock);

#endif // HDL_CLK_SOURCE_H_