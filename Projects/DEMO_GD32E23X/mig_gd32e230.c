/*
  Module Init Graph (MIG)
*/

#include "hdl.h"


#define HDL_I2C_SLAVE_ADDR           0x50

#define HDL_HXTAL_CLOCK              8000000
#define HDL_LXTAL_CLOCK              32768
#define HDL_HXTAL_2_PLLSEL_PREDIV    1                         /* Can be 1 to 16 */
#define HDL_PLLMUL                   9                         /* Can be 2 to 32 */
#define HDL_AHB_PREDIV               1                         /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
#define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
#define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
#define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
#define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel         /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
#define HDL_SYS_CLOCK                mod_clock_pll             /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

#define HDL_SYSTICK_COUNTER_RELOAD   72000 - 1                  /* Clocked by AHB   */

//_Static_assert( (HDL_SYS_CLOCK == mod_clock_pll) && (HDL_HXTAL_CLOCK * HDL_PLLMUL / HDL_HXTAL_2_PLLSEL_PREDIV) <= MAX_SYS_CLOCK, "Sys overclocking!");

#define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

const hdl_core_config_t mod_sys_core_cnf = {
  .flash_latency = WS_WSCNT_2 /* WS_WSCNT_0: sys_clock <= 24MHz, WS_WSCNT_1: sys_clock <= 48MHz, WS_WSCNT_2: sys_clock <= 72MHz */
};

hdl_core_t mod_sys_core = {
  .module.init = &hdl_core,
  .module.dependencies = NULL,
  .module.reg = (void *)SCB_BASE,
  .config = &mod_sys_core_cnf
};

/**************************************************************
 *  NVIC, IRQ, EXTI
 *************************************************************/

hdl_interrupt_t mod_irq_systick = {
  .irq_type = HDL_NVIC_EXCEPTION_SysTick,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_spi_0 = {
  .irq_type = HDL_NVIC_IRQ25_SPI0,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_i2c0_ev = {
  .irq_type = HDL_NVIC_IRQ23_I2C0_EV,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_i2c0_er = {
  .irq_type = HDL_NVIC_IRQ32_I2C0_ER,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_i2c1_ev = {
  .irq_type = HDL_NVIC_IRQ24_I2C1_EV,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_i2c1_er = {
  .irq_type = HDL_NVIC_IRQ34_I2C1_ER,
  .priority = 0,
  .priority_group = 0,
};

extern const hdl_interrupt_controller_config_t mod_nvic_cnf;

const void* const irq_vector[] __attribute__((aligned(HDL_VTOR_TAB_ALIGN))) = {
  &mod_nvic_cnf,
  &reset_handler,
  &nmi_handler,
  &hard_fault_handler,
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  &svc_handler,
  ((void *)0),
  ((void *)0),
  &pend_sv_handler,
  &systick_handler,
  &wwdgt_handler,                    /* IRQ0 */ 
  &lvd_handler,                      /* IRQ1 */
  &rtc_handler,                      /* IRQ2 */
  &fmc_handler,                      /* IRQ3 */
  &rcu_handler,                      /* IRQ4 */
  &exti0_1_handler,                  /* IRQ5 */
  &exti2_3_IRQHandler,               /* IRQ6 */
  &exti4_15_handler,                 /* IRQ7 */
  &irq_n_handler,                    /* IRQ8 */
  &dma_channel0_handler,             /* IRQ9 */
  &dma_channel1_2_handler,           /* IRQ10 */
  &dma_channel3_4_handler,           /* IRQ11 */
  &adc_cmp_handler,                  /* IRQ12 */
  &timer0_brk_up_trg_com_handler,    /* IRQ13 */
  &timer0_channel_handler,           /* IRQ14 */
  &irq_n_handler,                    /* IRQ15 */
  &timer2_handler,                   /* IRQ16 */
  &timer5_handler,                   /* IRQ17 */
  &irq_n_handler,                    /* IRQ18 */
  &timer13_handler,                  /* IRQ19 */
  &timer14_handler,                  /* IRQ21 */
  &timer15_handler,                  /* IRQ22 */
  &timer16_handler,                  /* IRQ23 */
  &i2c0_ev_handler,                  /* IRQ24 */
  &i2c1_ev_handler,                  /* IRQ25 */
  &spi0_handler,                     /* IRQ26 */
  &spi1_handler,                     /* IRQ27 */
  &usart0_handler,                   /* IRQ28 */
  &usart1_handler,                   /* IRQ29 */
  &irq_n_handler,                    /* IRQ30 */
  &irq_n_handler,                    /* IRQ31 */
  &irq_n_handler,                    /* IRQ32 */
  &i2c0_er_handler,                  /* IRQ33 */
  &irq_n_handler,                    /* IRQ34 */
  &i2c1_er_handler,                  /* IRQ35 */
};

const hdl_interrupt_controller_config_t mod_nvic_cnf = {
  .vector = &irq_vector,
  .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
  .irq_latency = 0, /* TODO: define static assert */
  .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_spi_0,
                               &mod_irq_i2c0_ev, &mod_irq_i2c0_er, &mod_irq_i2c1_ev, &mod_irq_i2c1_er),
};

hdl_interrupt_controller_t mod_nvic = {
  .module.init = &hdl_interrupt_controller,
  .module.dependencies = hdl_module_dependencies(&mod_sys_core.module),
  .module.reg = NVIC,
  .config = &mod_nvic_cnf,
};

/**************************************************************
 *  Clock
 *************************************************************/

hdl_clock_t mod_clock_irc8m = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_IRC8M, 
    .property.freq = 8000000
  }})
};

hdl_clock_t mod_clock_hxtal = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_HXTAL, 
    .property.freq = HDL_HXTAL_CLOCK
  }})
};

hdl_clock_t mod_clock_lxtal = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_LXTAL, 
    .property.freq = HDL_LXTAL_CLOCK
  }})
};

hdl_clock_t mod_clock_irc28m = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_IRC28M, 
    .property.freq = 28000000
  }})
};

hdl_clock_t mod_clock_irc40k = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_IRC40K, 
    .property.freq = 40000
  }})
};

hdl_clock_t mod_clock_pll_sel = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&HDL_PLL_MUL_CLOCK.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_PLL_SEL, 
    /* If source IRC8M prescaler fixed on 2 */
    .property.div = HDL_HXTAL_2_PLLSEL_PREDIV
  }})
};

hdl_clock_t mod_clock_pll = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_pll_sel.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_PLL, 
    /* If source IRC8M prescaler fixed on 2 */
    .property.mul = HDL_PLLMUL
  }})
};

hdl_clock_t mod_clock_system = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_sys_core.module, &mod_clock_irc8m.module, &HDL_SYS_CLOCK.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_SYS_SEL
  }})
};

hdl_clock_t mod_clock_rtc = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&HDL_RTC_CLOCK.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_RTC_SEL
  }})
};

hdl_clock_t mod_clock_ahb = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_system.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_AHB,
    .property.div = HDL_AHB_PREDIV
  }})
};

hdl_clock_t mod_clock_apb1 = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_APB1,
    .property.div = HDL_APB1_PREDIV
  }})
};

hdl_clock_t mod_clock_apb2 = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_APB2,
    .property.div = HDL_APB2_PREDIV
  }})
};

const hdl_tick_counter_systick_config_t mod_systick_counter_cnf = {
  .period = HDL_SYSTICK_COUNTER_RELOAD
};

hdl_tick_counter_t mod_systick_counter = {
  .module.init = &hdl_tick_counter,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)SysTick,
  .config.systick = &mod_systick_counter_cnf
};

const hdl_time_counter_config_t mod_timer_ms_cnf = {
  .reload_interrupt = &mod_irq_systick,
};

hdl_time_counter_t mod_timer_ms = {
  .module.init = hdl_time_counter,
  .module.dependencies = hdl_module_dependencies(&mod_systick_counter.module, &mod_nvic.module),
  .module.reg = NULL,
  .config = &mod_timer_ms_cnf
};

/**************************************************************
 *  GPIO
 *************************************************************/
const hdl_gpio_port_config_t hdl_port_config_a = {
  .rcu = RCU_GPIOA
};

const hdl_gpio_port_config_t hdl_port_config_b = {
  .rcu = RCU_GPIOB
};

const hdl_gpio_port_config_t hdl_port_config_c = {
  .rcu = RCU_GPIOC
};

const hdl_gpio_port_config_t hdl_port_config_f = {
  .rcu = RCU_GPIOF
};

hdl_gpio_port_t hdl_gpio_port_a = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOA,
  .config = &hdl_port_config_a
};

hdl_gpio_port_t hdl_gpio_port_b = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOB,
  .config = &hdl_port_config_b
};

hdl_gpio_port_t hdl_gpio_port_c = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOC,
  .config = &hdl_port_config_c
};

hdl_gpio_port_t hdl_gpio_port_f = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOF,
  .config = &hdl_port_config_f
};

const hdl_gpio_pin_hw_config_t mod_gpio_output_pp_mode = {
  .type = GPIO_MODE_OUTPUT,
  .otype = GPIO_OTYPE_PP,
  .ospeed = GPIO_OSPEED_2MHZ,
};

const hdl_gpio_pin_hw_config_t mod_gpio_input_pullup_mode = {
  .type = GPIO_MODE_INPUT,
  .pull = GPIO_PUPD_PULLUP,
};

const hdl_gpio_pin_hw_config_t mod_gpio_input_analog = {
  .type = GPIO_MODE_ANALOG,
  .pull = GPIO_PUPD_NONE,
};

const hdl_gpio_pin_hw_config_t mod_gpio_alternate_swd_mode = {
  .af = GPIO_AF_0,
  .type = GPIO_MODE_AF,
  .otype = GPIO_OTYPE_PP,
  .ospeed = GPIO_OSPEED_2MHZ,
};

const hdl_gpio_pin_hw_config_t mod_gpio_i2c_0_mode = {
  .af = GPIO_AF_1,
  .type = GPIO_MODE_AF,
  .otype = GPIO_OTYPE_OD,
  .ospeed = GPIO_OSPEED_2MHZ
};

const hdl_gpio_pin_hw_config_t mod_gpio_i2c_1_mode = {
  .af = GPIO_AF_0,
  .type = GPIO_MODE_AF,
  .otype = GPIO_OTYPE_OD,
  .ospeed = GPIO_OSPEED_2MHZ
};

const hdl_gpio_pin_hw_config_t hdl_gpio_spi_mode = {
  .type = GPIO_MODE_AF,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_50MHZ,
  .af = GPIO_AF_0,
  .otype = GPIO_OTYPE_PP,
};

hdl_gpio_pin_t uspd20k_ai1_ntc_pu = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_8,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PA8)

hdl_gpio_pin_t uspd20k_ai1_1k_pd = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_9,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PA9)

hdl_gpio_pin_t uspd20k_ai1_cur_scr_low = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_15,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PA15)

hdl_gpio_pin_t uspd20k_ai1_cur_scr_high = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_3,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PB3)

hdl_gpio_pin_t uspd20k_ai1_150r_pd = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_10,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PA10)

hdl_gpio_pin_t uspd20k_ai1_4K3_pd = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_11,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PA11)

hdl_gpio_pin_t uspd20k_ai2_ntc_pu = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_c.module),
  .module.reg = (void *)GPIO_PIN_13,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PC13)

hdl_gpio_pin_t uspd20k_ai2_1k_pd = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_4,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PB4)

hdl_gpio_pin_t uspd20k_ai2_cur_scr_low = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_7,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PB7)

hdl_gpio_pin_t uspd20k_ai2_cur_scr_high = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_15,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PB15)

hdl_gpio_pin_t uspd20k_ai2_150r_pd = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_5,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PB5)

hdl_gpio_pin_t uspd20k_ai2_4K3_pd = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_6,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PB6)

hdl_gpio_pin_t uspd20k_ai3_ntc_pu = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_14,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PB14)

hdl_gpio_pin_t uspd20k_ai3_1k_pd = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_12,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PB12)

hdl_gpio_pin_t uspd20k_ai3_cur_scr_low = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_13,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PB13)

hdl_gpio_pin_t uspd20k_ai3_cur_scr_high = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_2,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PB2)

hdl_gpio_pin_t uspd20k_ai3_150r_pd = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_11,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PB11)

hdl_gpio_pin_t uspd20k_ai3_4K3_pd = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_10,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PB10)

hdl_gpio_pin_t uspd20k_ai4_ntc_pu = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_1,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PB1)

hdl_gpio_pin_t uspd20k_ai4_1k_pd = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_1,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PA1)

hdl_gpio_pin_t uspd20k_ai4_cur_scr_low = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_0,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PA0)

hdl_gpio_pin_t uspd20k_ai4_cur_scr_high = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_0,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PB0)

hdl_gpio_pin_t uspd20k_ai4_150r_pd = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_2,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PA2)

hdl_gpio_pin_t uspd20k_ai4_4K3_pd = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_3,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PA3)

hdl_gpio_pin_t uspd20k_som_int = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
  .module.reg = (void *)GPIO_PIN_12,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_output_pp_mode, .inactive_default = HDL_GPIO_LOW)
}; //(PA12)

hdl_gpio_pin_t mod_pin_12c0_scl = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_8,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_i2c_0_mode, .inactive_default = HDL_GPIO_HIGH)
}; //(PB8)

hdl_gpio_pin_t mod_pin_12c0_sda = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
  .module.reg = (void *)GPIO_PIN_9,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_i2c_0_mode, .inactive_default = HDL_GPIO_HIGH)
}; //(PB9)

hdl_gpio_pin_t mod_pin_12c1_scl = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_6,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_i2c_1_mode, .inactive_default = HDL_GPIO_HIGH)
}; //(PF6)

hdl_gpio_pin_t mod_pin_12c1_sda = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_f.module),
  .module.reg = (void *)GPIO_PIN_7,
  .config = hdl_gpio_pin_config(.hwc = &mod_gpio_i2c_1_mode, .inactive_default = HDL_GPIO_HIGH)
}; //(PF7)

hdl_gpio_pin_t mod_spi_0_mosi = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_7,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_spi_mode)
}; //(PA7)

hdl_gpio_pin_t mod_spi_0_miso = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_6,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_spi_mode)
}; //(PA6)

hdl_gpio_pin_t mod_spi_0_sck = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_5,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &hdl_gpio_spi_mode)
}; //(PA5)

hdl_gpio_pin_t mod_spi_0_cs = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_4,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_output_pp_mode)
}; //(PA4)

/**************************************************************
 *  I2C
 *************************************************************/

const hdl_i2c_config_t mod_i2c0_cnf = {
  .addr_10_bits = 0,
  .dtcy = I2C_DTCY_2,
  .dual_address = 0,
  .err_interrupt = &mod_irq_i2c0_er,
  .ev_interrupt = &mod_irq_i2c0_ev,
  .general_call_enable = 0,
  .speed = 400000,
  .stretch_enable = 1,
  .rcu = RCU_I2C0
};

hdl_i2c_t mod_i2c0 = {
  .module.init = &hdl_i2c,
  .module.dependencies = hdl_module_dependencies(&mod_pin_12c0_scl.module, &mod_pin_12c0_sda.module,
                                                 &mod_clock_apb1.module, &mod_nvic.module, &mod_timer_ms.module),
  .module.reg = (void *)I2C0,
  .config = &mod_i2c0_cnf,
};

hdl_i2c_channel_t i2c1_ch0 = {.address = HDL_I2C_SLAVE_ADDR};

const hdl_i2c_config_t mod_i2c1_cnf = {
  .channels = hdl_i2c_channels(&i2c1_ch0),
  .addr_10_bits = 0,
  .dtcy = I2C_DTCY_2,
  .dual_address = 0,
  .err_interrupt = &mod_irq_i2c1_er,
  .ev_interrupt = &mod_irq_i2c1_ev,
  .general_call_enable = 0,
  .speed = 400000,
  .stretch_enable = 1,
  .rcu = RCU_I2C1
};

hdl_i2c_t mod_i2c1 = {
  .module.init = &hdl_i2c,
  .module.dependencies = hdl_module_dependencies(&mod_pin_12c1_scl.module, &mod_pin_12c1_sda.module,
                                                 &mod_clock_apb1.module, &mod_nvic.module, &mod_timer_ms.module),
  .module.reg = (void *)I2C1,
  .config = &mod_i2c1_cnf,
};

/**************************************************************
 *  SPI
 *************************************************************/

const hdl_spi_client_config_t mod_spi0_cnf = {
  .rcu = RCU_SPI0,
  .endian = HDL_SPI_ENDIAN_MSB,
  .polarity = SPI_CK_PL_HIGH_PH_2EDGE,
  .prescale = HDL_SPI_PSC_2,
  .spi_interrupt = &mod_irq_spi_0
};

hdl_spi_client_t mod_spi0 = {
  .module.init = &hdl_spi_client,
  .module.dependencies = hdl_module_dependencies(&mod_spi_0_mosi.module, &mod_spi_0_miso.module, &mod_spi_0_sck.module,
                                                 &mod_clock_apb1.module, &mod_nvic.module),
  .module.reg = (void *)SPI0,
  .config = &mod_spi0_cnf
};

const hdl_spi_client_ch_config_t mod_spi0_ch0_cnf = {
  .cs_min_delay = 90
};

hdl_spi_client_ch_t mod_spi0_ch0 = {
  .module.init = &hdl_spi_ch,
  .module.dependencies = hdl_module_dependencies(&mod_spi0.module, &mod_spi_0_cs.module, &mod_systick_counter.module),
  .config = &mod_spi0_ch0_cnf
};


extern hdl_gpio_pin_t uspd20k_adc_rdy      __attribute__ ((alias ("mod_spi_0_miso")));
extern hdl_spi_client_ch_t uspd20k_adc_spi __attribute__ ((alias ("mod_spi0_ch0")));
extern hdl_i2c_t uspd20k_i2c_som           __attribute__ ((alias ("mod_i2c1")));
extern hdl_i2c_t uspd20k_i2c_eeprom        __attribute__ ((alias ("mod_i2c0")));
