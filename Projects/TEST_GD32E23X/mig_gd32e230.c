/*
  Module Init Graph (MIG)
*/

#include "hdl.h"


#define HDL_I2C_SLAVE_ADDR           0x50

#define HDL_HXTAL_CLOCK              16000000
#define HDL_LXTAL_CLOCK              32768
#define HDL_HXTAL_2_PLLSEL_PREDIV    1                         /* Can be 1 to 16 */
#define HDL_PLLMUL                   4                         /* Can be 2 to 32 */
#define HDL_AHB_PREDIV               1                         /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
#define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
#define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
#define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
#define HDL_PLL_MUL_CLOCK            mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
#define HDL_SYS_CLOCK                mod_clock_pll             /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

#define HDL_SYSTICK_COUNTER_RELOAD   72000 - 1                  /* Clocked by AHB   */

//_Static_assert( (HDL_SYS_CLOCK == mod_clock_pll) && (HDL_HXTAL_CLOCK * HDL_PLLMUL / HDL_HXTAL_2_PLLSEL_PREDIV) <= MAX_SYS_CLOCK, "Sys overclocking!");

#define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

const hdl_core_config_t mod_sys_core_cnf = {
  .flash_latency = WS_WSCNT_2, /* WS_WSCNT_0: sys_clock <= 24MHz, WS_WSCNT_1: sys_clock <= 48MHz, WS_WSCNT_2: sys_clock <= 72MHz */
  .reg = (void *)SCB_BASE
};

const hdl_core_t mod_sys_core = {
  .iface = &hdl_core_iface,
  .dependencies = NULL,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .config = &mod_sys_core_cnf
};

/**************************************************************
 *  NVIC, IRQ, EXTI
 *************************************************************/

hdl_interrupt_t mod_irq_systick = {
  .irq_type = HDL_NVIC_EXCEPTION_SysTick,
  .priority = 1,
  .priority_group = 1,
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
  .reg = (void *)NVIC,
};

const hdl_interrupt_controller_t mod_nvic = {
  .iface = &hdl_interrupt_controller_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&mod_sys_core),
  .config = &mod_nvic_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

/**************************************************************
 *  Clock
 *************************************************************/

const hdl_clock_t mod_clock_irc8m = {
  .iface = &hdl_clock_iface,
  .dependencies = NULL,
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_IRC8M, 
    .property.freq = 8000000,
    .reg = (void *)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_PRV_SIZE)
};

const hdl_clock_t mod_clock_hxtal = {
  .iface = &hdl_clock_iface,
  .dependencies = NULL,
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_HXTAL, 
    .property.freq = HDL_HXTAL_CLOCK,
    .reg = (void *)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_PRV_SIZE)
};

const hdl_clock_t mod_clock_lxtal = {
  .iface = &hdl_clock_iface,
  .dependencies = NULL,
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_LXTAL, 
    .property.freq = HDL_LXTAL_CLOCK,
    .reg = (void *)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_PRV_SIZE)
};

const hdl_clock_t mod_clock_irc28m = {
  .iface = &hdl_clock_iface,
  .dependencies = NULL,
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_IRC28M, 
    .property.freq = 28000000,
    .reg = (void *)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_PRV_SIZE)
};

const hdl_clock_t mod_clock_irc40k = {
  .iface = &hdl_clock_iface,
  .dependencies = NULL,
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_IRC40K, 
    .property.freq = 40000,
    .reg = (void *)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_PRV_SIZE)
};

const hdl_clock_t mod_clock_pll_sel = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&HDL_PLL_MUL_CLOCK),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_PLL_SEL, 
    /* If source IRC8M prescaler fixed on 2 */
    .property.div = HDL_HXTAL_2_PLLSEL_PREDIV,
    .reg = (void *)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_PRV_SIZE)
};

const hdl_clock_t mod_clock_pll = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&mod_clock_pll_sel),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_PLL, 
    /* If source IRC8M prescaler fixed on 2 */
    .property.mul = HDL_PLLMUL,
    .reg = (void *)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_PRV_SIZE)
};

const hdl_clock_t mod_clock_system = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&mod_sys_core, 
    (hdl_module_base_t *)&mod_clock_irc8m, (hdl_module_base_t *)&HDL_SYS_CLOCK),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_SYS_SEL,
    .reg = (void *)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_PRV_SIZE)
};

const hdl_clock_t mod_clock_rtc = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&HDL_RTC_CLOCK),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_RTC_SEL,
    .reg = (void *)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_PRV_SIZE)
};

const hdl_clock_t mod_clock_ahb = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&mod_clock_system),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_AHB,
    .property.div = HDL_AHB_PREDIV,
    .reg = (void *)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_PRV_SIZE)
};

const hdl_clock_t mod_clock_apb1 = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&mod_clock_ahb),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_APB1,
    .property.div = HDL_APB1_PREDIV,
    .reg = (void *)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_PRV_SIZE)
};

const hdl_clock_t mod_clock_apb2 = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&mod_clock_ahb),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_APB2,
    .property.div = HDL_APB2_PREDIV,
    .reg = (void *)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_PRV_SIZE)
};

const hdl_tick_counter_systick_config_t mod_systick_counter_cnf = {
  .period = HDL_SYSTICK_COUNTER_RELOAD
};

const hdl_tick_counter_t mod_systick_counter = {
  .iface = &hdl_tick_counter_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&mod_clock_ahb),
  .config = hdl_module_config(hdl_tick_counter_config_t,
    .reg = (void *)SysTick,
    .type.systick = &mod_systick_counter_cnf
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_time_counter_t mod_timer_ms = {
  .iface = &hdl_time_counter_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&mod_systick_counter, (hdl_module_base_t *)&mod_nvic),
  .config = hdl_module_config(hdl_time_counter_config_t, .reload_interrupt = &mod_irq_systick),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_TIME_COUNTER_PRV_SIZE),
};

/**************************************************************
 *  GPIO
 *************************************************************/

const hdl_gpio_port_t hdl_gpio_port_a = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCU_GPIOA,
    .reg = GPIOA
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_t hdl_gpio_port_b = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCU_GPIOB,
    .reg = GPIOB
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_t hdl_gpio_port_c = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCU_GPIOC,
    .reg = GPIOC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_t hdl_gpio_port_f = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCU_GPIOF,
    .reg = GPIOF
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
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

const hdl_gpio_pin_t uspd20k_ai1_ntc_pu = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_8
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PA8)

const hdl_gpio_pin_t uspd20k_ai1_1k_pd = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_9
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PA9)

const hdl_gpio_pin_t uspd20k_ai1_cur_scr_low = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_15
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PA15)

const hdl_gpio_pin_t uspd20k_ai1_cur_scr_high = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_3
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PB3)

const hdl_gpio_pin_t uspd20k_ai1_150r_pd = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_10
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PA10)

const hdl_gpio_pin_t uspd20k_ai1_4K3_pd = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_11
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PA11)

const hdl_gpio_pin_t uspd20k_ai2_ntc_pu = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_13
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PC13)

const hdl_gpio_pin_t uspd20k_ai2_1k_pd = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_4
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PB4)

const hdl_gpio_pin_t uspd20k_ai2_cur_scr_low = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_7
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PB7)

const hdl_gpio_pin_t uspd20k_ai2_cur_scr_high = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_15
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PB15)

const hdl_gpio_pin_t uspd20k_ai2_150r_pd = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_5
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PB5)

const hdl_gpio_pin_t uspd20k_ai2_4K3_pd = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_6
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PB6)

const hdl_gpio_pin_t uspd20k_ai3_ntc_pu = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_14
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PB14)

const hdl_gpio_pin_t uspd20k_ai3_1k_pd = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_12
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PB12)

const hdl_gpio_pin_t uspd20k_ai3_cur_scr_low = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_13
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PB13)

const hdl_gpio_pin_t uspd20k_ai3_cur_scr_high = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_2
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PB2)

const hdl_gpio_pin_t uspd20k_ai3_150r_pd = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_11
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PB11)

const hdl_gpio_pin_t uspd20k_ai3_4K3_pd = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_10
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PB10)

const hdl_gpio_pin_t uspd20k_ai4_ntc_pu = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_1
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PB1)

const hdl_gpio_pin_t uspd20k_ai4_1k_pd = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_1
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PA1)

const hdl_gpio_pin_t uspd20k_ai4_cur_scr_low = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pullup_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_0
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PA0)

const hdl_gpio_pin_t uspd20k_ai4_cur_scr_high = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_0
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PB0)

const hdl_gpio_pin_t uspd20k_ai4_150r_pd = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_2
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PA2)

const hdl_gpio_pin_t uspd20k_ai4_4K3_pd = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_3
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PA3)

const hdl_gpio_pin_t uspd20k_som_int = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_12
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PA12)

const hdl_gpio_pin_t mod_pin_12c0_scl = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_i2c_0_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_8
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PB8)

const hdl_gpio_pin_t mod_pin_12c0_sda = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_i2c_0_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_9
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PB9)

const hdl_gpio_pin_t mod_pin_12c1_scl = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_f),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_i2c_1_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_6
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PF6)

const hdl_gpio_pin_t mod_pin_12c1_sda = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_f),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_i2c_1_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_7
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PF7)

const hdl_gpio_pin_t mod_spi_0_mosi = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &hdl_gpio_spi_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_7
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PA7)

const hdl_gpio_pin_t mod_spi_0_miso = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &hdl_gpio_spi_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_6
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PA6)

const hdl_gpio_pin_t mod_spi_0_sck = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &hdl_gpio_spi_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_5
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PA5)

const hdl_gpio_pin_t mod_spi_0_cs = {
  .iface = &hdl_gpio_pin_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_4
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
}; //(PA4)

const hdl_button_t button = {
  .iface = &hdl_button_iface,
  .dependencies = hdl_module_dependencies((hdl_module_base_t *)&uspd20k_ai4_cur_scr_low, (hdl_module_base_t *)&mod_timer_ms),
  .config = hdl_module_config(hdl_button_config_t,
    .debounce_delay = 30,
    .hold_delay = 3000
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_BUTTON_VAR_SIZE)
};

/**************************************************************
 *  I2C
 *************************************************************/

// const hdl_i2c_config_t mod_i2c0_cnf = {
//   .addr_10_bits = 0,
//   .dtcy = I2C_DTCY_2,
//   .dual_address = 0,
//   .err_interrupt = &mod_irq_i2c0_er,
//   .ev_interrupt = &mod_irq_i2c0_ev,
//   .general_call_enable = 0,
//   .speed = 400000,
//   .stretch_enable = 1,
//   .rcu = RCU_I2C0
// };

// hdl_i2c_t mod_i2c0 = {
//   .module.init = &hdl_i2c,
//   .module.dependencies = hdl_module_dependencies(&mod_pin_12c0_scl.module, &mod_pin_12c0_sda.module,
//                                                  &mod_clock_apb1.module, &mod_nvic.module, &mod_timer_ms.module),
//   .module.reg = (void *)I2C0,
//   .config = &mod_i2c0_cnf,
// };

// hdl_i2c_channel_t i2c1_ch0 = {.address = HDL_I2C_SLAVE_ADDR};

// const hdl_i2c_config_t mod_i2c1_cnf = {
//   .channels = hdl_i2c_channels(&i2c1_ch0),
//   .addr_10_bits = 0,
//   .dtcy = I2C_DTCY_2,
//   .dual_address = 0,
//   .err_interrupt = &mod_irq_i2c1_er,
//   .ev_interrupt = &mod_irq_i2c1_ev,
//   .general_call_enable = 0,
//   .speed = 400000,
//   .stretch_enable = 1,
//   .rcu = RCU_I2C1
// };

// hdl_i2c_t mod_i2c1 = {
//   .module.init = &hdl_i2c,
//   .module.dependencies = hdl_module_dependencies(&mod_pin_12c1_scl.module, &mod_pin_12c1_sda.module,
//                                                  &mod_clock_apb1.module, &mod_nvic.module, &mod_timer_ms.module),
//   .module.reg = (void *)I2C1,
//   .config = &mod_i2c1_cnf,
// };

/**************************************************************
 *  SPI
 *************************************************************/

// const hdl_spi_client_config_t mod_spi0_cnf = {
//   .rcu = RCU_SPI0,
//   .endian = HDL_SPI_ENDIAN_MSB,
//   .polarity = SPI_CK_PL_HIGH_PH_2EDGE,
//   .prescale = HDL_SPI_PSC_2,
//   .spi_interrupt = &mod_irq_spi_0
// };

// hdl_spi_client_t mod_spi0 = {
//   .module.init = &hdl_spi_client,
//   .module.dependencies = hdl_module_dependencies(&mod_spi_0_mosi.module, &mod_spi_0_miso.module, &mod_spi_0_sck.module,
//                                                  &mod_clock_apb1.module, &mod_nvic.module),
//   .module.reg = (void *)SPI0,
//   .config = &mod_spi0_cnf
// };

// const hdl_spi_client_ch_config_t mod_spi0_ch0_cnf = {
//   .cs_min_delay = 90
// };

// hdl_spi_client_ch_t mod_spi0_ch0 = {
//   .module.init = &hdl_spi_ch,
//   .module.dependencies = hdl_module_dependencies(&mod_spi0.module, &mod_spi_0_cs.module, &mod_systick_counter.module),
//   .config = &mod_spi0_ch0_cnf
// };
