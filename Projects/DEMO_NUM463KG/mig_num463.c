#include "hdl.h"

#define HDL_INTERRUPT_PRIO_GROUP_BITS     __NVIC_PRIO_BITS

#define HDL_LXTAL_CLOCK                   32768
#define HDL_HXTAL_CLOCK                   12000000
#define HDL_PLL_REF_CLOCK                 mod_clock_hxtal               /* Can be clocked by: mod_clock_irc, mod_clock_hxtal */
#define HDL_PLL_REF_PRESCALER             3                             /* Can be 1, 2, 3, ... 32 */
#define HDL_PLL_VCO_PRESCALER             100                           /* Can be 4, 6, 8 ... 1028 */
#define HDL_PLL_PRESCALER                 2                             /* Can be 1, 2, 4 */
#define HDL_SYS_CLOCK                     mod_clock_pll                 /* Can be clocked by: mod_clock_pll, mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc10k, mod_clock_irc */
#define HDL_SYS_PRESCALER                 1                             /* Can be 1, 2, 3, ... 16 */
#define HDL_SYSTICK_REF                   mod_clock_sys                 /* Can be clocked by: mod_clock_sys, mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc */
#define HDL_SYSTICK_REF_PRESCALER         2                             /* Can be 1 or 2 for mod_clock_hxtal, 1 for mod_clock_lxtal, 2 for mod_clock_sys,  mod_clock_irc */
#define HDL_APB0_PRESCALER                4                             /* Note that, don`t excceed 60MHz; Can be 1, 2, 4, 8, 16 */
#define HDL_APB1_PRESCALER                2                             /* Note that, don`t excceed 120MHz; Can be 1, 2, 4, 8, 16 */
#define HDL_SYSTICK_CLOCK                 mod_clock_sys                 /* Can be clocked by: mod_clock_sys, mod_clock_systick_ref*/
#define HDL_SYSTICK_RELOAD                200000 - 1

#define HDL_I2C_SLAVE_ADDR                0x68

const hdl_core_config_t mod_sys_core_cnf = {
 .power_level = SYS_PLCTL_PLSEL_PL0,
 .flash_latency = 8
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

hdl_interrupt_t mod_irq_gpio_e = {
  .irq_type = HDL_NVIC_IRQ20_GPE_IRQn,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_i2c0 = {
  .irq_type = HDL_NVIC_IRQ38_I2C0_IRQn,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_i2c1 = {
  .irq_type = HDL_NVIC_IRQ39_I2C1_IRQn,
  .priority = 0,
  .priority_group = 0,
};

/***********************************************************
 *                          NVIC
***********************************************************/
extern const hdl_interrupt_controller_config_t mod_nvic_cnf;

const void* const irq_vector[] __attribute__((aligned(HDL_VTOR_TAB_ALIGN))) = {
	&mod_nvic_cnf,
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
  .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_gpio_e, &mod_irq_i2c0, &mod_irq_i2c1),
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
    .type = HDL_CLOCK_TYPE_HXTAL, 
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

hdl_clock_t mod_clock_pll_ref = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&HDL_PLL_REF_CLOCK.module),
  .module.reg = (void *)CLK,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_PLL_REF, 
    .property.div = HDL_PLL_REF_PRESCALER
  }})
};

hdl_clock_t mod_clock_pll_vco = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_pll_ref.module),
  .module.reg = (void *)CLK,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_PLL_VCO, 
    .property.mul = HDL_PLL_VCO_PRESCALER
  }})
};

hdl_clock_t mod_clock_pll = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_pll_vco.module),
  .module.reg = (void *)CLK,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_PLL, 
    .property.div = HDL_PLL_PRESCALER
  }})
};

hdl_clock_t mod_clock_sys = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&HDL_SYS_CLOCK.module, &mod_sys_core.module),
  .module.reg = (void *)CLK,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_SYS, 
    .property.div = HDL_SYS_PRESCALER
  }})
};

hdl_clock_t mod_clock_systick_ref = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&HDL_SYSTICK_REF.module),
  .module.reg = (void *)CLK,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_SYSTICK_REF, 
    .property.div = HDL_SYSTICK_REF_PRESCALER
  }})
};

hdl_clock_t mod_clock_apb0 = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_sys.module),
  .module.reg = (void *)CLK,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_APB0, 
    .property.div = HDL_APB0_PRESCALER
  }})
};

hdl_clock_t mod_clock_apb1 = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_sys.module),
  .module.reg = (void *)CLK,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_APB1, 
    .property.div = HDL_APB1_PRESCALER
  }})
};

const hdl_tick_counter_systick_config_t mod_systick_counter_cnf = {
  .period = HDL_SYSTICK_RELOAD
};

hdl_tick_counter_t mod_systick_counter = {
  .module.init = &hdl_tick_counter,
  .module.dependencies = hdl_module_dependencies(&HDL_SYSTICK_CLOCK.module),
  .module.reg = (void *)SysTick,
  .config.systick = &mod_systick_counter_cnf
};

const hdl_time_counter_config_t mod_systick_timer_cnf = {
  .reload_interrupt = &mod_irq_systick,
};

hdl_time_counter_t mod_systick_timer_ms = {
  .module.init = hdl_time_counter,
  .module.dependencies = hdl_module_dependencies(&mod_systick_counter.module, &mod_nvic.module),
  .module.reg = NULL,
  .config = &mod_systick_timer_cnf
};

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

hdl_gpio_port_t mod_gpio_port_a = {
  .module.init = &hdl_gpio_port,
  .module.reg = (void *)PA,
  .module.dependencies = hdl_module_dependencies(&mod_clock_sys.module),
  .config = &gpio_port_a_cnf
};

hdl_gpio_port_t mod_gpio_port_b = {
  .module.init = &hdl_gpio_port,
  .module.reg = (void *)PB,
  .module.dependencies = hdl_module_dependencies(&mod_clock_sys.module),
  .config = &gpio_port_b_cnf
};

hdl_gpio_port_t mod_gpio_port_c = {
  .module.init = &hdl_gpio_port,
  .module.reg = (void *)PC,
  .module.dependencies = hdl_module_dependencies(&mod_clock_sys.module),
  .config = &gpio_port_c_cnf
};

hdl_gpio_port_t mod_gpio_port_d = {
  .module.init = &hdl_gpio_port,
  .module.reg = (void *)PD,
  .module.dependencies = hdl_module_dependencies(&mod_clock_sys.module),
  .config = &gpio_port_d_cnf
};

hdl_gpio_port_t mod_gpio_port_e = {
  .module.init = &hdl_gpio_port,
  .module.reg = (void *)PE,
  .module.dependencies = hdl_module_dependencies(&mod_clock_sys.module),
  .config = &gpio_port_e_cnf
};

hdl_gpio_port_t mod_gpio_port_f = {
  .module.init = &hdl_gpio_port,
  .module.reg = (void *)PF,
  .module.dependencies = hdl_module_dependencies(&mod_clock_sys.module),
  .config = &gpio_port_f_cnf
};

hdl_gpio_port_t mod_gpio_port_g = {
  .module.init = &hdl_gpio_port,
  .module.reg = (void *)PG,
  .module.dependencies = hdl_module_dependencies(&mod_clock_sys.module),
  .config = &gpio_port_g_cnf
};

hdl_gpio_port_t mod_gpio_port_h = {
  .module.init = &hdl_gpio_port,
  .module.reg = (void*)PH,
  .module.dependencies = hdl_module_dependencies(&mod_clock_sys.module),
  .config = &gpio_port_h_cnf
};

const hdl_gpio_pin_hw_config_t gpio_pin_input_pu_slow_int_cnf = {
	.func = GPIO_MODE_INPUT,
	.pull_mode = GPIO_PUSEL_PULL_UP,
	.slew_mode = GPIO_SLEWCTL_NORMAL,
  .int_mode = GPIO_INT_FALLING,
  .func_alternate = 0
};

const hdl_gpio_pin_hw_config_t gpio_pin_input_pu_slow_cnf = {
	.func = GPIO_MODE_INPUT,
	.pull_mode = GPIO_PUSEL_PULL_UP,
	.slew_mode = GPIO_SLEWCTL_NORMAL,
  .int_mode = GPIO_INT_NONE,
  .func_alternate = 0
};

const hdl_gpio_pin_hw_config_t gpio_pin_output_slow_cnf = {
	.func = GPIO_MODE_OUTPUT,
	.pull_mode = GPIO_PUSEL_DISABLE,
	.slew_mode = GPIO_SLEWCTL_NORMAL,
  .func_alternate = 0
};

const hdl_gpio_pin_hw_config_t gpio_pin_hw_i2c0_1_cnf = {
	.func = GPIO_MODE_OPEN_DRAIN,
	.pull_mode = GPIO_PUSEL_PULL_UP,
	.slew_mode = GPIO_SLEWCTL_NORMAL,
  .func_alternate = 9
};

const hdl_gpio_pin_config_t gpio_pin_out_low_cnf = {
  .hwc = &gpio_pin_output_slow_cnf,
  .inactive_default = HDL_GPIO_HIGH
};

const hdl_gpio_pin_config_t gpio_pin_i2c0_1_cnf = {
  .hwc = &gpio_pin_hw_i2c0_1_cnf,
  .inactive_default = HDL_GPIO_HIGH
};

hdl_gpio_pin_t mod_gpio_pin_pc0_i2c_sda = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_port_c.module),
  .module.reg = (void *)0,
  .config = &gpio_pin_i2c0_1_cnf,
};

hdl_gpio_pin_t mod_gpio_pin_pc1_i2c_scl = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_port_c.module),
  .module.reg = (void *)1,
  .config = &gpio_pin_i2c0_1_cnf,
};

hdl_gpio_pin_t mod_gpio_pin_pb0_i2c_sda = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_port_b.module),
  .module.reg = (void *)0,
  .config = &gpio_pin_i2c0_1_cnf,
};

hdl_gpio_pin_t mod_gpio_pin_pb1_i2c_scl = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_port_b.module),
  .module.reg = (void *)1,
  .config = &gpio_pin_i2c0_1_cnf,
};

hdl_gpio_pin_t mod_gpio_pin_ph4 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_port_h.module),
  .module.reg = (void *)4,
  .config = &gpio_pin_out_low_cnf,
};

hdl_gpio_pin_t mod_gpio_pin_ph5 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_port_h.module),
  .module.reg = (void *)5,
  .config = &gpio_pin_out_low_cnf,
};

hdl_gpio_pin_t mod_gpio_pin_ph6 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_port_h.module),
  .module.reg = (void *)6,
  .config = &gpio_pin_out_low_cnf,
};

const hdl_gpio_pin_config_t gpio_pin_in_high_int_cnf = {
  .hwc = &gpio_pin_input_pu_slow_int_cnf,
  .inactive_default = HDL_GPIO_HIGH,
};

const hdl_gpio_pin_config_t gpio_pin_in_high_cnf = {
  .hwc = &gpio_pin_input_pu_slow_cnf,
  .inactive_default = HDL_GPIO_HIGH,
};

hdl_gpio_pin_t mod_gpio_pin_pe8 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_port_e.module),
  .module.reg = (void *)8,
  .config = &gpio_pin_in_high_int_cnf,
};

hdl_gpio_pin_t mod_gpio_pin_pe9 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_port_e.module),
  .module.reg = (void *)9,
  .config = &gpio_pin_in_high_cnf,
};

hdl_i2c_channel_t i2c0_ch0 = {.address = HDL_I2C_SLAVE_ADDR};

hdl_i2c_t mod_i2c0 = {
  .module.init = &hdl_i2c,
  .module.dependencies = hdl_module_dependencies(&mod_clock_apb0.module,
    &mod_gpio_pin_pc1_i2c_scl.module, &mod_gpio_pin_pc0_i2c_sda.module,
    &mod_nvic.module, &mod_systick_timer_ms.module),
  .module.reg = (void*)I2C0,
  .config = hdl_module_config(hdl_i2c_config_t,
    .channels = hdl_i2c_channels(&i2c0_ch0),
    .general_call_enable = 0,
    .swap_scl_sda = 0,
    .addr_10_bits = 0,
    .interrupt = &mod_irq_i2c0,
    .speed = 400000
  )
};

hdl_i2c_t mod_i2c1 = {
  .module.init = &hdl_i2c,
  .module.dependencies = hdl_module_dependencies(&mod_clock_apb1.module,
    &mod_gpio_pin_pb1_i2c_scl.module, &mod_gpio_pin_pb0_i2c_sda.module,
    &mod_nvic.module, &mod_systick_timer_ms.module),
  .module.reg = (void*)I2C1,
  .config = hdl_module_config(hdl_i2c_config_t,
    .general_call_enable = 0,
    .addr_10_bits = 0,
    .swap_scl_sda = 0,
    .interrupt = &mod_irq_i2c1,
    .speed = 400000
  )
};

hdl_nvm_t mod_flash = {
  .module.init = &hdl_nvm,
  .module.dependencies = NULL,
  .config = hdl_module_config(hdl_nvm_config_t,
    .address_start = FLASH_BASE + (128 * 1024),
    .size = (128 * 1024),
    .max_write_unit = -1,
    .min_erase_unit = FMC_FLASH_PAGE_SIZE
  )
};

extern hdl_time_counter_t mod_timer_ms        __attribute__ ((alias ("mod_systick_timer_ms")));
extern hdl_gpio_pin_t mod_gpio_pin_btn_0      __attribute__ ((alias ("mod_gpio_pin_pe8")));
extern hdl_gpio_pin_t mod_gpio_pin_btn_1      __attribute__ ((alias ("mod_gpio_pin_pe9")));
extern hdl_gpio_pin_t mod_gpio_pin_led_r      __attribute__ ((alias ("mod_gpio_pin_ph4")));
extern hdl_gpio_pin_t mod_gpio_pin_led_y      __attribute__ ((alias ("mod_gpio_pin_ph5")));
extern hdl_gpio_pin_t mod_gpio_pin_led_g      __attribute__ ((alias ("mod_gpio_pin_ph6")));

extern hdl_interrupt_controller_t mod_interrupt_controller  __attribute__ ((alias ("mod_nvic")));
extern hdl_interrupt_t mod_irq_gpio_btn_port  __attribute__ ((alias ("mod_irq_gpio_e")));
