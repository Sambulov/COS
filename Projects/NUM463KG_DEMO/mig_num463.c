#include "hdl.h"

#define HDL_INTERRUPT_PRIO_GROUP_BITS     __NVIC_PRIO_BITS

#define HDL_LXTAL_CLOCK                   32768
#define HDL_HXTAL_CLOCK                   12000000
#define HDL_PLL_MUL_CLOCK                 mod_clock_hxtal               /* Can be clocked by: mod_clock_irc, mod_clock_hxtal */
#define HDL_SYS_CLOCK                     mod_clock_pll_p_prescaler     /* Can be clocked by: mod_clock_pll_p_prescaler, mod_clock_hxtal, mod_clock_irc */
#define HDL_PLL_VCO_PRESCALER             4                             /* Can be 2, 3 .. 63 */
#define HDL_PLL_N_MULTIPLY                96                            /* Note that, don`t excceed 500MHz; Can be 64, 65 .. 500 */ 
#define HDL_PLL_P_PRESCALER               2                             /* Note that, don`t excceed 240MHz; Can be 2, 4, 6, 8 */
#define HDL_PLL_Q_PRESCALER               10                            /* Note that, don`t excceed 48MHz; Can be 2, 3 .. 15 */
#define HDL_AHB_PRESCALER                 1                             /* Note that, don`t excceed 200MHz; Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
#define HDL_APB1_PRESCALER                4                             /* Note that, don`t excceed 60MHz; Can be 1, 2, 4, 8, 16 */
#define HDL_APB2_PRESCALER                4                             /* Note that, don`t excceed 120MHz; Can be 1, 2, 4, 8, 16 */


const hdl_core_config_t mod_sys_core_cnf = {

};

hdl_core_t mod_sys_core = {
  .module.init = &hdl_core,
  .module.dependencies = NULL,
  .module.reg = (void *)SCB_BASE,
  .config = &mod_sys_core_cnf
};

/***********************************************************
 *                          IRQ
***********************************************************/
hdl_interrupt_t mod_irq_systick = {
  .irq_type = HDL_NVIC_EXCEPTION_SysTick,
  .priority = 0,
  .priority_group = 0,
};

/***********************************************************
 *                          NVIC
***********************************************************/
extern const hdl_interrupt_controller_config_t mod_nvic_cnf;

const void* const irq_vector[] __attribute__((aligned(HDL_VTOR_TAB_ALIGN))) = {
	&_estack,            /* Top of Stack */
	&reset_handler,         /* Reset Handler */
	&nmi_handler,           /* NMI Handler */
	&hard_fault_handler,     /* Hard Fault Handler */
	&mem_manage_handler,     /* MPU Fault Handler */
	&bus_fault_handler,      /* Bus Fault Handler */
	&usage_fault_handler,    /* Usage Fault Handler */
	((void *)0),            /* Reserved */
	((void *)0),            /* Reserved */
	((void *)0),            /* Reserved */
	((void *)0),            /* Reserved */
	&svc_handler,           /* SVCall Handler */
	&debug_mon_handler,      /* Debug Monitor Handler */
	((void *)0),            /* Reserved */
	&pend_sv_handler,        /* PendSV Handler */
	&systick_handler,       /* SysTick Handler */
	/* External interrupts */
	&BOD_IRQHandler,        /*  0: BOD                        */
	&IRC_IRQHandler,        /*  1: IRC                        */
	&PWRWU_IRQHandler,      /*  2: PWRWU                      */
	&RAMPE_IRQHandler,      /*  3: RAMPE                      */
	&CKFAIL_IRQHandler,     /*  4: CKFAIL                     */
	&ISP_IRQHandler,        /*  5: ISP                   */
	&RTC_IRQHandler,        /*  6: RTC                        */
	&TAMPER_IRQHandler,     /*  7: TAMPER                     */
	&WDT_IRQHandler,        /*  8: WDT                        */
	&WWDT_IRQHandler,       /*  9: WWDT                       */
	&EINT0_IRQHandler,      /* 10: EINT0                      */
	&EINT1_IRQHandler,      /* 11: EINT1                      */
	&EINT2_IRQHandler,      /* 12: EINT2                      */
	&EINT3_IRQHandler,      /* 13: EINT3                      */
	&EINT4_IRQHandler,      /* 14: EINT4                      */
	&EINT5_IRQHandler,      /* 15: EINT5                      */
	&GPA_IRQHandler,        /* 16: GPA                        */
	&GPB_IRQHandler,        /* 17: GPB                        */
	&GPC_IRQHandler,        /* 18: GPC                        */
	&GPD_IRQHandler,        /* 19: GPD                        */
	&GPE_IRQHandler,        /* 20: GPE                        */
	&GPF_IRQHandler,        /* 21: GPF                        */
	&QSPI0_IRQHandler,      /* 22: QSPI0                      */
	&SPI0_IRQHandler,       /* 23: SPI0                       */
	&BRAKE0_IRQHandler,     /* 24: BRAKE0                     */
	&EPWM0P0_IRQHandler,    /* 25: EPWM0P0                    */
	&EPWM0P1_IRQHandler,    /* 26: EPWM0P1                    */
	&EPWM0P2_IRQHandler,    /* 27: EPWM0P2                    */
	&BRAKE1_IRQHandler,     /* 28: BRAKE1                     */
	&EPWM1P0_IRQHandler,    /* 29: EPWM1P0                    */
	&EPWM1P1_IRQHandler,    /* 30: EPWM1P1                    */
	&EPWM1P2_IRQHandler,    /* 31: EPWM1P2                    */
	&TMR0_IRQHandler,       /* 32: TIMER0                     */
	&TMR1_IRQHandler,       /* 33: TIMER1                     */
	&TMR2_IRQHandler,       /* 34: TIMER2                     */
	&TMR3_IRQHandler,       /* 35: TIMER3                     */
	&UART0_IRQHandler,      /* 36: UART0                      */
	&UART1_IRQHandler,      /* 37: UART1                      */
	&I2C0_IRQHandler,       /* 38: I2C0                       */
	&I2C1_IRQHandler,       /* 39: I2C1                       */
	&PDMA0_IRQHandler,      /* 40: PDMA0                      */
	&DAC_IRQHandler,        /* 41: DAC                        */
	&EADC00_IRQHandler,     /* 42: EADC00                     */
	&EADC01_IRQHandler,     /* 43: EADC01                     */
	&ACMP01_IRQHandler,     /* 44: ACMP01                     */
	&ACMP23_IRQHandler,     /* 45: ACMP23                     */
	&EADC02_IRQHandler,     /* 46: EADC02                     */
	&EADC03_IRQHandler,     /* 47: EADC03                     */
	&UART2_IRQHandler,      /* 48: UART2                      */
	&UART3_IRQHandler,      /* 49: UART3                      */
	&QSPI1_IRQHandler,      /* 50: QSPI1                      */
	&SPI1_IRQHandler,       /* 51: SPI1                       */
	&SPI2_IRQHandler,       /* 52: SPI2                       */
	&USBD_IRQHandler,       /* 53: USBD                       */
	&OHCI_IRQHandler,       /* 54: OHCI                       */
	&USBOTG_IRQHandler,     /* 55: OTG                        */
	&BMC_IRQHandler,        /* 56: BMC                        */
	&SPI5_IRQHandler,       /* 57: SPI5                       */
	&SC0_IRQHandler,        /* 58: SC0                        */
	&SC1_IRQHandler,        /* 59: SC1                        */
	&SC2_IRQHandler,        /* 60: SC2                        */
	&GPJ_IRQHandler,        /* 61: GPJ                        */
	&SPI3_IRQHandler,       /* 62: SPI3                       */
	&SPI4_IRQHandler,       /* 63: SPI4                       */
	&SDH0_IRQHandler,       /* 64: SDH0                       */
	&USBD20_IRQHandler,     /* 65: HSUSBD                     */
	&EMAC0_IRQHandler,      /* 66: EMAC0                      */
	((void *)0),            /* 67: Reserved                   */
	&I2S0_IRQHandler,       /* 68: I2S0                       */
	&I2S1_IRQHandler,       /* 69: I2S1                       */
	&SPI6_IRQHandler,       /* 70: SPI6                       */
	&CRPT_IRQHandler,       /* 71: CRPT                       */
	&GPG_IRQHandler,        /* 72: GPG                        */
	&EINT6_IRQHandler,      /* 73: EINT6                      */
	&UART4_IRQHandler,      /* 74: UART4                      */
	&UART5_IRQHandler,      /* 75: UART5                      */
	&USCI0_IRQHandler,      /* 76: USCI0                      */
	&SPI7_IRQHandler,       /* 77: SPI7                       */
	&BPWM0_IRQHandler,      /* 78: BPWM0                      */
	&BPWM1_IRQHandler,      /* 79: BPWM1                      */
	&SPIM_IRQHandler,       /* 80: SPIM                       */
	&CCAP_IRQHandler,       /* 81: CCAP                       */
	&I2C2_IRQHandler,       /* 82: I2C2                       */
	&I2C3_IRQHandler,       /* 83: I2C3                       */
	&EQEI0_IRQHandler,      /* 84: EQEI0                      */
	&EQEI1_IRQHandler,      /* 85: EQEI1                      */
	&ECAP0_IRQHandler,      /* 86: ECAP0                      */
	&ECAP1_IRQHandler,      /* 87: ECAP1                      */
	&GPH_IRQHandler,        /* 88: GPH                        */
	&EINT7_IRQHandler,      /* 89: EINT7                      */
	&SDH1_IRQHandler,       /* 90: SDH1                       */
	&PSIO_IRQHandler,       /* 91: PSIO                       */
	&EHCI_IRQHandler,       /* 92: EHCI                       */
	&USBOTG20_IRQHandler,   /* 93: HSOTG                      */
	&ECAP2_IRQHandler,      /* 94: ECAP2                      */
	&ECAP3_IRQHandler,      /* 95: ECAP3                      */
	&KPI_IRQHandler,        /* 96: KPI                        */
	&HBI_IRQHandler,        /* 97: HBI                        */
	&PDMA1_IRQHandler,      /* 98: PDMA1                      */
	&UART8_IRQHandler,      /* 99: UART8                      */
	&UART9_IRQHandler,      /* 100: UART9                     */
	&TRNG_IRQHandler,       /* 101: TRNG                      */
	&UART6_IRQHandler,      /* 102: UART6                     */
	&UART7_IRQHandler,      /* 103: UART7                     */
	&EADC10_IRQHandler,     /* 104: EADC10                    */
	&EADC11_IRQHandler,     /* 105: EADC11                    */
	&EADC12_IRQHandler,     /* 106: EADC12                    */
	&EADC13_IRQHandler,     /* 107: EADC13                    */
	&SPI8_IRQHandler,       /* 108: SPI8                      */
	&KS_IRQHandler,         /* 109: KS                        */
	&GPI_IRQHandler,        /* 110: GPI                       */
	&SPI9_IRQHandler,       /* 111: SPI9                      */
	&CANFD00_IRQHandler,    /* 112: CANFD00                   */
	&CANFD01_IRQHandler,    /* 113: CANFD01                   */
	&CANFD10_IRQHandler,    /* 114: CANFD10                   */
	&CANFD10_IRQHandler,    /* 115: CANFD10                   */
	&EQEI2_IRQHandler,      /* 116: EQEI2                     */
	&EQEI3_IRQHandler,      /* 117: EQEI3                     */
	&I2C4_IRQHandler,       /* 118: I2C4                      */
	&SPI10_IRQHandler,      /* 119: SPI10                     */
	&CANFD20_IRQHandler,    /* 120: CANFD20                   */
	&CANFD21_IRQHandler,    /* 121: CANFD21                   */
	&CANFD30_IRQHandler,    /* 122: CANFD30                   */
	&CANFD30_IRQHandler,    /* 123: CANFD30                   */
	&EADC20_IRQHandler,     /* 124: EADC20                    */
	&EADC21_IRQHandler,     /* 125: EADC21                    */
	&EADC22_IRQHandler,     /* 126: EADC22                    */
	&EADC23_IRQHandler,     /* 127: EADC23                    */
};

const hdl_interrupt_controller_config_t mod_nvic_cnf = {
  .vector = &irq_vector,
  .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
  .interrupts = hdl_interrupts(&mod_irq_systick),
};

hdl_interrupt_controller_t mod_nvic = {
  .module.init = &hdl_interrupt_controller,
  .module.dependencies = hdl_module_dependencies(&mod_sys_core.module),
  .module.reg = NVIC,
  .config = &mod_nvic_cnf
};

/***********************************************************
 *                          CLOCK
***********************************************************/
hdl_clock_t mod_clock_irc = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)CLK,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_IRC12M, 
    .property.freq = 12000000
  }})
};

hdl_clock_t mod_clock_irc48m = { //TODO: ???????????????
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)CLK,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_IRC48M, 
    .property.freq = 48000000
  }})
};

hdl_clock_t mod_clock_irc10k = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)CLK,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_IRC10K, 
    .property.freq = 10000
  }})
};

hdl_clock_t mod_clock_hxtal = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)CLK,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_IRC12M, 
    .property.freq = HDL_HXTAL_CLOCK
  }})
};

hdl_clock_t mod_clock_lxtal = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)CLK,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_LXTAL, 
    .property.freq = HDL_LXTAL_CLOCK
  }})
};

// hdl_clock_prescaler_t mod_pll_sel = {
//   .module.init = &hdl_clock_selector_pll,
//   .module.dependencies = hdl_module_dependencies(&HDL_PLL_MUL_CLOCK.module),
//   .module.reg = (void *)CLK,
//   .muldiv_factor = 1,
// };

// hdl_clock_prescaler_t mod_clock_pll_vco_prescaler = {
//   .module.init = &hdl_clock_selector_pll_vco,
//   .module.dependencies = hdl_module_dependencies(&mod_pll_sel.module),
//   .module.reg = (void *)CLK,
//   .muldiv_factor = HDL_PLL_VCO_PRESCALER,
// };

// hdl_clock_prescaler_t mod_clock_pll_n_multiply = {
//   .module.init = &hdl_clock_pll_n,
//   .module.dependencies = hdl_module_dependencies(&mod_clock_pll_vco_prescaler.module),
//   .module.reg = (void *)CLK,
//   .muldiv_factor = HDL_PLL_N_MULTIPLY,
// };

// hdl_clock_prescaler_t mod_clock_pll_p_prescaler = {
//   .module.init = &hdl_clock_pll_p,
//   .module.dependencies = hdl_module_dependencies(&mod_clock_pll_n_multiply.module),
//   .module.reg = (void *)CLK,
//   .muldiv_factor = HDL_PLL_P_PRESCALER,
// };

// hdl_clock_prescaler_t mod_clock_pll_q_prescaler = {
//   .module.init = &hdl_clock_pll_q,
//   .module.dependencies = hdl_module_dependencies(&mod_clock_pll_n_multiply.module),
//   .module.reg = (void *)CLK,
//   .muldiv_factor = HDL_PLL_Q_PRESCALER,
// };

// hdl_clock_prescaler_t mod_clock_sys_clock = {
//   .module.init = &hdl_clock_system,
//   .module.dependencies = hdl_module_dependencies(&mod_sys_core.module, &HDL_SYS_CLOCK.module),
//   .module.reg = (void *)CLK,
//   .muldiv_factor = HDL_PLL_Q_PRESCALER,
// };

// hdl_clock_prescaler_t mod_clock_ahb = {
//   .module.init = &hdl_clock_ahb,
//   .module.dependencies = hdl_module_dependencies(&mod_clock_sys_clock.module),
//   .module.reg = (void *)CLK,
//   .muldiv_factor = HDL_AHB_PRESCALER,
// };

// hdl_clock_prescaler_t mod_clock_apb1 = {
//   .module.init = &hdl_clock_apb1,
//   .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
//   .module.reg = (void *)CLK,
//   .muldiv_factor = HDL_APB1_PRESCALER,
// };

// hdl_clock_prescaler_t mod_clock_apb2 = {
//   .module.init = &hdl_clock_apb2,
//   .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
//   .module.reg = (void *)CLK,
//   .muldiv_factor = HDL_APB2_PRESCALER,
// };


const hdl_gpio_port_config_t gpio_port_a_cnf = {
	.rcu = CLK_AHBCLK0_GPACKEN_Msk
};

const hdl_gpio_port_config_t gpio_port_b_cnf = {
	.rcu = CLK_AHBCLK0_GPBCKEN_Msk
};

const hdl_gpio_port_config_t gpio_port_c_cnf = {
	.rcu = CLK_AHBCLK0_GPCCKEN_Msk
};

const hdl_gpio_port_config_t gpio_port_d_cnf = {
	.rcu = CLK_AHBCLK0_GPDCKEN_Msk
};

const hdl_gpio_port_config_t gpio_port_e_cnf = {
	.rcu = CLK_AHBCLK0_GPECKEN_Msk
};

const hdl_gpio_port_config_t gpio_port_f_cnf = {
	.rcu = CLK_AHBCLK0_GPFCKEN_Msk
};

const hdl_gpio_port_config_t gpio_port_g_cnf = {
	.rcu = CLK_AHBCLK0_GPGCKEN_Msk
};

const hdl_gpio_port_config_t gpio_port_h_cnf = {
	.rcu = CLK_AHBCLK0_GPHCKEN_Msk
};

const hdl_gpio_pin_hw_config_t gpio_pin_input_pu_slow_cnf = {
	.func = GPIO_MODE_INPUT,
	.pull_mode = GPIO_PUSEL_PULL_UP,
	.slew_mode = GPIO_SLEWCTL_NORMAL
};

const hdl_gpio_pin_hw_config_t gpio_pin_output_slow_cnf = {
	.func = GPIO_MODE_OUTPUT,
	.pull_mode = GPIO_PUSEL_DISABLE,
	.slew_mode = GPIO_SLEWCTL_NORMAL
};

hdl_gpio_port_t mod_gpio_port_h = {
  .module.init = &hdl_gpio_port,
  .module.reg = (void*)PH,
  //.module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .config = &gpio_port_h_cnf
};

hdl_gpio_port_t mod_gpio_port_e = {
  .module.init = &hdl_gpio_port,
  .module.reg = (void *)PE,
  //.module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .config = &gpio_port_e_cnf
};

const hdl_gpio_pin_config_t gpio_pin_out_low_cnf = {
  .hwc = &gpio_pin_output_slow_cnf,
  .inactive_default = HDL_GPIO_HIGH
};

hdl_gpio_pin_t mod_gpio_pin_ph4 = {    //LED_R
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_port_h.module),
  .module.reg = (void *)BIT4,
  .config = &gpio_pin_out_low_cnf,
};

hdl_gpio_pin_t mod_gpio_pin_ph5 = {    //LED_Y
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_port_h.module),
  .module.reg = (void *)BIT5,
  .config = &gpio_pin_out_low_cnf,
};

hdl_gpio_pin_t mod_gpio_pin_ph6 = {    //LED_G
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_port_h.module),
  .module.reg = (void *)BIT6,
  .config = &gpio_pin_out_low_cnf,
};

const hdl_gpio_pin_config_t gpio_pin_in_high_cnf = {
  .hwc = &gpio_pin_input_pu_slow_cnf,
  .inactive_default = HDL_GPIO_HIGH
};

hdl_gpio_pin_t mod_gpio_pin_pe8 = {    //BTN_0
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_port_e.module),
  .module.reg = (void *)BIT8,
  .config = &gpio_pin_in_high_cnf,
};

hdl_gpio_pin_t mod_gpio_pin_pe9 = {    //BTN_1
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_port_e.module),
  .module.reg = (void *)BIT9,
  .config = &gpio_pin_in_high_cnf,
};

extern hdl_gpio_pin_t mod_gpio_pin_btn_0      __attribute__ ((alias ("mod_gpio_pin_pe8")));
extern hdl_gpio_pin_t mod_gpio_pin_btn_1      __attribute__ ((alias ("mod_gpio_pin_pe9")));
extern hdl_gpio_pin_t mod_gpio_pin_led_r      __attribute__ ((alias ("mod_gpio_pin_ph4")));
extern hdl_gpio_pin_t mod_gpio_pin_led_y      __attribute__ ((alias ("mod_gpio_pin_ph5")));
extern hdl_gpio_pin_t mod_gpio_pin_led_g      __attribute__ ((alias ("mod_gpio_pin_ph6")));

