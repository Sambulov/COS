#ifndef PORT_CLOCK_H_
#define PORT_CLOCK_H_

#define MAX_SYS_CLOCK                72000000UL
#define MAX_AHB_CLOCK                72000000UL
#define MAX_APB1_CLOCK               72000000UL
#define MAX_APB2_CLOCK               72000000UL

#define IRC8M_CLOCK                  8000000UL
#define IRC28M_CLOCK                 28000000UL
#define IRC40K_CLOCK                 40000UL


typedef struct {
hdl_module_t module;
  uint32_t freq;
  uint32_t div;
  uint8_t clock_monitor_enable;  /* 0 - clock monitor disabled, !0 - enabled */
} hdl_sys_clock_t;

/* oscillator */
hdl_module_state_t hdl_clock_hxtal(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_irc8m(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_lxtal(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_irc40k(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_irc28m(void *desc, uint8_t enable);

hdl_module_state_t hdl_clock_hxtal_prescaler(void *desc, uint8_t enable);

/* pll selector (irc8m/2 | hdl_clock_pll_prescaler) */
hdl_module_state_t hdl_clock_selector_pll(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_pll(void *desc, uint8_t enable);

hdl_module_state_t hdl_clock_system(void *desc, uint8_t enable);

/* RTC selector */  
hdl_module_state_t hdl_clock_selector_rtc(void *desc, uint8_t enable);

hdl_module_state_t hdl_clock_ahb(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_apb1(void *desc, uint8_t enable);
hdl_module_state_t hdl_clock_apb2(void *desc, uint8_t enable);

#endif // PORT_CLOCK_H_

