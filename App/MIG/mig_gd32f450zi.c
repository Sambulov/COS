/*
  Module Init Graph (MIG)
*/

#include "bldl.h"

#if defined ( ATB_3500 )

#define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

  // #define HDL_HXTAL_CLOCK              16000000
  // #define HDL_LXTAL_CLOCK              32768
  // #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  // #define HDL_PLLMUL                   2
  // #define HDL_AHB_PREDIV               1
  // #define HDL_APB1_PREDIV              1
  // #define HDL_APB2_PREDIV              1
  // #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  // #define HDL_PLL_MUL_CLOCK            mod_clock_pll_prescaler   /* Can be clocked by: mod_clock_pll_prescaler, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  // #define HDL_SYS_CLOCK                mod_clock_pll_mul         /* Can be clocked by: mod_clock_pll_mul, mod_clock_irc8m, mod_clock_hxtal */

  // #define HDL_SYSTICK_COUNTER_RELOAD  32000 - 1                  /* Clocked by AHB   */
  // #define HDL_TIMER0_COUNTER_RELOAD   32000 - 1                  /* Clocked by APB 2 */
  // #define HDL_TIMER2_COUNTER_RELOAD   32000 - 1                  /* Clocked by APB 1 */

#define HDL_HXTAL_CLOCK                   20000000
#define HDL_PLL_MUL_CLOCK                 mod_clock_hxtal               /* Can be clocked by: mod_clock_irc16m, mod_clock_hxtal */
#define HDL_SYS_CLOCK                     mod_clock_pll_p_prescaler     /* Can be clocked by: mod_clock_pll_p_prescaler, mod_clock_hxtal, mod_clock_irc16m */
#define HDL_PLL_VCO_PRESCALER             20                            /* Can be 2, 3 .. 63 */
#define HDL_PLL_N_MULTIPLY                64                            /* Note that, don`t excceed 500MHz; Can be 64, 65 .. 500 */ 
#define HDL_PLL_P_PRESCALER               2                             /* Note that, don`t excceed 240MHz; Can be 2, 4, 6, 8 */
#define HDL_PLL_Q_PRESCALER               2                             /* Note that, don`t excceed 48MHz; Can be 2, 3 .. 15 */
#define HDL_AHB_PRESCALER                 1                             /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
#define HDL_APB1_PRESCALER                2                             /* Can be 1, 2, 4, 8, 16 */
#define HDL_APB2_PRESCALER                2                             /* Can be 1, 2, 4, 8, 16 */

hdl_core_t mod_sys_core = {
  .module.init = &hdl_core,
  .module.dependencies = NULL,
  .module.reg = (void *)SCB_BASE,
  .flash_latency = WS_WSCNT_2 /* WS_WSCNT_0: sys_clock <= 24MHz, WS_WSCNT_1: sys_clock <= 48MHz, WS_WSCNT_2: sys_clock <= 72MHz */
  /* TODO: ... */
};
/***********************************************************
 *                          IRQ
***********************************************************/
hdl_nvic_interrupt_t mod_irq_systick = {
  .irq_type = HDL_NVIC_EXCEPTION_SysTick,
  .priority = 0,
  .priority_group = 0,
};
hdl_nvic_interrupt_t mod_irq_timer0 = {
  .irq_type = HDL_NVIC_IRQ25_TIMER0_UP_TIMER9,
  .priority = 0,
  .priority_group = 1,
};
hdl_nvic_interrupt_t mod_irq_timer1 = {
  .irq_type = HDL_NVIC_IRQ28_TIMER1,
  .priority = 0,
  .priority_group = 2,
};

hdl_nvic_interrupt_t mod_irq_exti_0 = {
  .irq_type = HDL_NVIC_IRQ6_EXTI0,
  .priority = 0,
  .priority_group = 1,
};
/***********************************************************
 *                          EXTI
***********************************************************/
hdl_nvic_exti_t mod_nvic_exti_line_0 = {
  .line = HDL_EXTI_LINE_0,
  .mode = HDL_EXTI_MODE_INTERRUPT,
  .source = HDL_EXTI_SOURCE_PE,
  .trigger = HDL_EXTI_TRIGGER_RISING
};
/***********************************************************
 *                          NVIC
***********************************************************/
hdl_nvic_t mod_nvic = {
  .module.init = &hdl_nvic,
  .module.dependencies = hdl_module_dependencies(&mod_sys_core.module),
  .module.reg = NVIC,
  .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
  .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_timer0, &mod_irq_timer1, &mod_irq_exti_0),
  .exti_lines = hdl_exti_lines(&mod_nvic_exti_line_0)
};
/***********************************************************
 *                          CLOCK
***********************************************************/
hdl_clock_t mod_clock_irc16m = {
  .module.init = &hdl_clock_irc16m,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .freq = 16000000,
  .div = 1
};
hdl_clock_t mod_clock_hxtal = {
  .module.init = &hdl_clock_hxtal,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .freq = HDL_HXTAL_CLOCK,
  .div = 1
};
hdl_clock_prescaler_t mod_pll_sel = {
  .module.init = &hdl_clock_selector_pll,
  .module.dependencies = hdl_module_dependencies(&HDL_PLL_MUL_CLOCK.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = 1,
};
hdl_clock_prescaler_t mod_clock_pll_vco_prescaler = {
  .module.init = &hdl_clock_selector_pll_vco,
  .module.dependencies = hdl_module_dependencies(&mod_pll_sel.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = HDL_PLL_VCO_PRESCALER,
};
hdl_clock_prescaler_t mod_clock_pll_n_multiply = {
  .module.init = &hdl_clock_pll_n,
  .module.dependencies = hdl_module_dependencies(&mod_clock_pll_vco_prescaler.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = HDL_PLL_N_MULTIPLY,
};
hdl_clock_prescaler_t mod_clock_pll_p_prescaler = {
  .module.init = &hdl_clock_pll_p,
  .module.dependencies = hdl_module_dependencies(&mod_clock_pll_n_multiply.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = HDL_PLL_P_PRESCALER,
};
hdl_clock_prescaler_t mod_clock_pll_q_prescaler = {
  .module.init = &hdl_clock_pll_q,
  .module.dependencies = hdl_module_dependencies(&mod_clock_pll_n_multiply.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = HDL_PLL_Q_PRESCALER,
};
hdl_clock_prescaler_t mod_clock_sys_clock = {
  .module.init = &hdl_clock_system,
  .module.dependencies = hdl_module_dependencies(&mod_sys_core.module, &HDL_SYS_CLOCK.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = HDL_PLL_Q_PRESCALER,
};
hdl_clock_prescaler_t mod_clock_ahb = {
  .module.init = &hdl_clock_ahb,
  .module.dependencies = hdl_module_dependencies(&mod_clock_sys_clock.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = HDL_AHB_PRESCALER,
};
hdl_clock_prescaler_t mod_clock_apb1 = {
  .module.init = &hdl_clock_apb1,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = HDL_APB1_PRESCALER,
};
hdl_clock_prescaler_t mod_clock_apb2 = {
  .module.init = &hdl_clock_apb2,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)RCU,
  .muldiv_factor = HDL_APB2_PRESCALER,
};

/***********************************************************
 *                          COUNTER
***********************************************************/
hdl_clock_counter_t mod_systick_counter = {
  .module.init = &hdl_clock_counter,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),  // add ahb
  .module.reg = (void *)SysTick,
  .diction = HDL_DOWN_COUNTER,
  .counter_reload = 32000 - 1,
};
hdl_clock_counter_t mod_timer0_counter = {
  .module.init = &hdl_clock_counter,
  .module.dependencies = hdl_module_dependencies(&mod_clock_apb2.module), // add ahb
  .module.reg = (void *)TIMER0,
  .diction = HDL_DOWN_COUNTER,
  .counter_reload = 16000 - 1,
  };
  hdl_clock_counter_t mod_timer1_counter = {
  .module.init = &hdl_clock_counter,
  .module.dependencies = hdl_module_dependencies(&mod_clock_apb1.module), // add ahb
  .module.reg = (void *)TIMER1,
  .diction = HDL_DOWN_COUNTER,
  .counter_reload = 16000 - 1,
  };
  /***********************************************************
   *                          TIMER
   ***********************************************************/
  hdl_timer_t mod_systick_timer_ms = {
    .module.init = hdl_timer,
    .module.dependencies = hdl_module_dependencies(&mod_systick_counter.module, &mod_nvic.module),
    .module.reg = NULL,
    .reload_iterrupt = HDL_NVIC_EXCEPTION_SysTick,
    .val = 0
  };
    hdl_timer_t mod_timer0_ms = {
    .module.init = hdl_timer,
    .module.dependencies = hdl_module_dependencies(&mod_timer0_counter.module, &mod_nvic.module),
    .module.reg = NULL,
    .reload_iterrupt = HDL_NVIC_IRQ25_TIMER0_UP_TIMER9,
    .val = 0
  };
    hdl_timer_t mod_timer1_ms = {
    .module.init = hdl_timer,
    .module.dependencies = hdl_module_dependencies(&mod_timer1_counter.module, &mod_nvic.module),
    .module.reg = NULL,
    .reload_iterrupt = HDL_NVIC_IRQ28_TIMER1,
    .val = 0
  };
/***********************************************************
 *                          GPIO PORT
***********************************************************/
hdl_gpio_port_t hdl_gpio_port_c = {
  .init = &hdl_gpio_port,
  .dependencies = NULL, // add ahb
  .reg = (void *)GPIOC,
};
/***********************************************************
 *                          GPIO PIN
***********************************************************/
hdl_gpio_mode_t hdl_gpio_mode_input_floating = {
    .type = GPIO_MODE_INPUT,
    .pull = GPIO_PUPD_NONE,
    .ospeed = GPIO_OSPEED_2MHZ
};
hdl_gpio_mode_t hdl_gpio_mode_output_no_pull = {
    .type = GPIO_MODE_OUTPUT,
    .pull = GPIO_PUPD_NONE,
    .ospeed = GPIO_OSPEED_2MHZ,
};
hdl_gpio_mode_t hdl_gpio_mode_analog = {
    .type = GPIO_MODE_ANALOG,
    .pull = GPIO_PUPD_NONE,
    .ospeed = GPIO_OSPEED_2MHZ,
};

hdl_gpio_pin_t mod_led_0_0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_0,
  .mode = &hdl_gpio_mode_output_no_pull,
};

hdl_gpio_pin_t mod_led_0_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_1,
  .mode = &hdl_gpio_mode_output_no_pull,
};

hdl_gpio_pin_t mod_led_0_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_2,
  .mode = &hdl_gpio_mode_output_no_pull,
};

hdl_gpio_pin_t mod_led_1_0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_3,
  .mode = &hdl_gpio_mode_output_no_pull,
};

hdl_gpio_pin_t mod_led_1_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_4,
  .mode = &hdl_gpio_mode_output_no_pull,
};

hdl_gpio_pin_t mod_led_1_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_5,
  .mode = &hdl_gpio_mode_output_no_pull,
};

hdl_gpio_pin_t mod_led_2_0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_6,
  .mode = &hdl_gpio_mode_output_no_pull,
};

hdl_gpio_pin_t mod_led_2_1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_7,
  .mode = &hdl_gpio_mode_output_no_pull,
};

hdl_gpio_pin_t mod_led_2_2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .module.reg = (void *)GPIO_PIN_8,
  .mode = &hdl_gpio_mode_output_no_pull,
};
#endif