/*
  Module Init Graph (MIG)
*/

#ifdef GD32E230

#include "hdl_iface.h"

#define SWD_ENABLE                   1

#define HDL_I2C_SLAVE_ADDR           0x31
#define HDL_I2C_EEPROM_ADDR          0x50

#define HDL_HXTAL_CLOCK              25000000
#define HDL_LXTAL_CLOCK              32768
#define HDL_HXTAL_2_PLLSEL_PREDIV    5                         /* Can be 1 to 16 */
#define HDL_PLLMUL                   14                        /* Can be 2 to 32 */
#define HDL_AHB_PREDIV               1                         /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
#define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
#define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
#define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
#define HDL_PLL_SEL_CLOCK            mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal or mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
#define HDL_SYS_CLOCK                mod_clock_pll             /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

#define HDL_SYSTICK_COUNTER_RELOAD   70000 - 1                  /* Clocked by AHB   */

const hdl_module_base_t hdl_null_module = {
  .iface = NULL,
  .dependencies = NULL,
  .config = NULL,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

//_Static_assert( (HDL_SYS_CLOCK == mod_clock_pll_mul) && (HDL_HXTAL_CLOCK * HDL_PLLMUL / HDL_HXTAL_2_PLLSEL_PREDIV) <= MAX_SYS_CLOCK, "Sys overclocking!");

#define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

extern const void *_heap_start;
extern const void *_heap_size;

const hdl_mem_block_t hdl_heap_regions[] = {
  { &_heap_start, (uint32_t)&_heap_size },
  { NULL, 0 } /* Terminates the array. */  
};

const hdl_core_config_t mod_sys_core_cnf = {
  .flash_latency = WS_WSCNT_2 /* WS_WSCNT_0: sys_clock <= 24MHz, WS_WSCNT_1: sys_clock <= 48MHz, WS_WSCNT_2: sys_clock <= 72MHz */
};

const hdl_core_t mod_sys_core = {
  .iface = &hdl_core_arm_iface,
  .dependencies = NULL,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .config = hdl_module_config(hdl_core_config_t, 
    .flash_latency = WS_WSCNT_2,
    .phy = SCB_BASE
  )
};

/**************************************************************
 *  NVIC, IRQ, EXTI
 *************************************************************/

hdl_interrupt_t mod_irq_systick = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_EXCEPTION_SysTick,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_spi_0 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ25_SPI0,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_i2c0_ev = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ23_I2C0_EV,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_i2c0_er = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ32_I2C0_ER,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_i2c1_ev = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ24_I2C1_EV,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_i2c1_er = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ34_I2C1_ER,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_usart1 = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ28_USART1,
    .priority = 0,
    .priority_group = 0
  )
};

hdl_interrupt_t mod_irq_adc = {
  .irq_cnf = hdl_module_config(hdl_interrupt_config_t,
    .irq_type = HDL_NVIC_IRQ12_ADC_CMP,
    .priority = 0,
    .priority_group = 2
  )
};

extern const hdl_nvic_config_t mod_nvic_cnf;

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

const hdl_nvic_config_t mod_nvic_cnf = {
  .vector = &irq_vector,
  .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
  .irq_latency = 0, /* TODO: define static assert */
  .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_spi_0, &mod_irq_usart1,
                               &mod_irq_i2c0_ev, &mod_irq_i2c0_er, &mod_irq_i2c1_ev, &mod_irq_i2c1_er,
                               &mod_irq_adc),
  .phy = NVIC,
};

const hdl_nvic_t mod_nvic = {
  .iface = &hdl_nvic_iface,
  .dependencies = hdl_module_dependencies(&mod_sys_core),
  .config = &mod_nvic_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

/**************************************************************
 *  Clock
 *************************************************************/

const hdl_clock_mcu_t mod_clock_irc8m = {
  .iface = &hdl_clock_iface,
  .dependencies = NULL,
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_IRC8M, 
    .property.freq = 8000000,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_hxtal = {
  .iface = &hdl_clock_iface,
  .dependencies = NULL,
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_HXTAL, 
    .property.freq = HDL_HXTAL_CLOCK,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_lxtal = {
  .iface = &hdl_clock_iface,
  .dependencies = NULL,
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_LXTAL, 
    .property.freq = HDL_LXTAL_CLOCK,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_irc28m = {
  .iface = &hdl_clock_iface,
  .dependencies = NULL,
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_IRC28M, 
    .property.freq = 28000000,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_irc40k = {
  .iface = &hdl_clock_iface,
  .dependencies = NULL,
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_IRC40K, 
    .property.freq = 40000,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_pll_sel = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&HDL_PLL_SEL_CLOCK),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_PLL_SEL, 
    /* If source IRC8M prescaler fixed on 2 */
    .property.div = HDL_HXTAL_2_PLLSEL_PREDIV,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_pll = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_pll_sel),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_PLL, 
    /* If source IRC8M prescaler fixed on 2 */
    .property.mul = HDL_PLLMUL,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_system = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_sys_core, &mod_clock_irc8m, &HDL_SYS_CLOCK),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_SYS_SEL,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_rtc = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&HDL_RTC_CLOCK),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_RTC_SEL,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_ahb = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_system),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_AHB,
    .property.div = HDL_AHB_PREDIV,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_apb1 = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_APB1,
    .property.div = HDL_APB1_PREDIV,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_clock_mcu_t mod_clock_apb2 = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_APB2,
    .property.div = HDL_APB2_PREDIV,
    .phy = (uint32_t)RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_tick_counter_t mod_systick_counter = {
  .iface = &hdl_tick_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_tick_counter_config_t,
    .phy = (uint32_t)SysTick,
    .type.systick = hdl_module_config(hdl_tick_counter_systick_config_t, .period = HDL_SYSTICK_COUNTER_RELOAD)
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_time_counter_t mod_systick_timer = {
  .iface = &hdl_time_counter_iface,
  .dependencies = hdl_module_dependencies(&mod_systick_counter, &mod_nvic),
  .config = hdl_module_config(hdl_time_counter_config_t, .reload_interrupt = &mod_irq_systick),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_TIME_COUNTER_VAR_SIZE),
};

/**************************************************************
 *  GPIO
 *************************************************************/
const hdl_gpio_port_mcu_t hdl_gpio_port_a = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCU_GPIOA,
    .phy = GPIOA
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_b = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCU_GPIOB,
    .phy = GPIOB
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_c = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCU_GPIOC,
    .phy = GPIOC
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_gpio_port_mcu_t hdl_gpio_port_f = {
  .iface = &hdl_gpio_port_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = hdl_module_config(hdl_gpio_port_config_t,
    .rcu = RCU_GPIOF,
    .phy = GPIOF
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

/*============================================*/

const hdl_gpio_pin_hw_config_t mod_gpio_output_od_mode = {
  .type = GPIO_MODE_OUTPUT,
  .otype = GPIO_OTYPE_OD,
  .ospeed = GPIO_OSPEED_2MHZ,
};

const hdl_gpio_pin_hw_config_t mod_gpio_input_a_mode = {
  .type = GPIO_MODE_ANALOG,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_2MHZ,
  .af = GPIO_AF_0
};

const hdl_gpio_pin_hw_config_t mod_gpio_output_pp_mode = {
  .type = GPIO_MODE_OUTPUT,
  .otype = GPIO_OTYPE_PP,
  .ospeed = GPIO_OSPEED_2MHZ,
};

const hdl_gpio_pin_hw_config_t mod_gpio_uart1_mode = {
  .af = GPIO_AF_1,
  .type = GPIO_MODE_AF,
  .otype = GPIO_OTYPE_PP,
  .ospeed = GPIO_OSPEED_2MHZ,
  .pull = GPIO_PUPD_PULLUP
};

const hdl_gpio_pin_hw_config_t mod_gpio_input_pullup_mode = {
  .type = GPIO_MODE_INPUT,
  .pull = GPIO_PUPD_PULLUP,
};

const hdl_gpio_pin_hw_config_t mod_gpio_alternate_swd_mode = {
  .af = GPIO_AF_0,
  .type = GPIO_MODE_AF,
  .otype = GPIO_OTYPE_PP,
  .ospeed = GPIO_OSPEED_2MHZ,
};

const hdl_gpio_pin_hw_config_t mod_gpio_i2c1f_mode = {
  .af = GPIO_AF_0,
  .type = GPIO_MODE_AF,
  .otype = GPIO_OTYPE_OD,
  .ospeed = GPIO_OSPEED_2MHZ,
  .pull = GPIO_PUPD_PULLUP
};

const hdl_gpio_pin_hw_config_t mod_gpio_i2c1b_mode = {
  .af = GPIO_AF_1,
  .type = GPIO_MODE_AF,
  .otype = GPIO_OTYPE_OD,
  .ospeed = GPIO_OSPEED_2MHZ,
  .pull = GPIO_PUPD_PULLUP
};

const hdl_gpio_pin_hw_config_t hdl_gpio_spi_mode = {
  .type = GPIO_MODE_AF,
  .pull = GPIO_PUPD_NONE,
  .ospeed = GPIO_OSPEED_50MHZ,
  .af = GPIO_AF_0,
  .otype = GPIO_OTYPE_PP,
};
/*============================================*/

const hdl_gpio_pin_t mod_gpio_pa0 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pullup_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_0)
}; //(PA0)

const hdl_gpio_pin_t mod_gpio_pa1 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_pullup_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_1)
}; //(PA1)

const hdl_gpio_pin_t mod_gpio_pa2 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_2)
}; //(PA2)

const hdl_gpio_pin_t mod_gpio_pa3 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_3)
}; //(PA3)

const hdl_gpio_pin_t mod_gpio_pa2_uart_tx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_uart1_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_2)
}; //(PA2)

const hdl_gpio_pin_t mod_gpio_pa3_uart_rx = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_uart1_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_3)
}; //(PA3)

const hdl_gpio_pin_t mod_gpio_pin_pa4_cs = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_4)
}; //(PA4)

const hdl_gpio_pin_t mod_gpio_pin_pa4_adc = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_a_mode,
    .pin = GPIO_PIN_4)
}; //(PA4)

const hdl_gpio_pin_t mod_gpio_pin_pa5_sck = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &hdl_gpio_spi_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_5)
}; //(PA5)

const hdl_gpio_pin_t mod_gpio_pin_pa5_adc = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_a_mode,
    .pin = GPIO_PIN_5)
}; //(PA5)

const hdl_gpio_pin_t mod_gpio_pin_pa6_miso = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &hdl_gpio_spi_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_6)
}; //(PA6)

const hdl_gpio_pin_t mod_gpio_pin_pa6_adc = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_a_mode,
    .pin = GPIO_PIN_6)
}; //(PA6)

const hdl_gpio_pin_t mod_gpio_pin_pa7_mosi = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &hdl_gpio_spi_mode,
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_7)
}; //(PA7)

const hdl_gpio_pin_t mod_gpio_pin_pa7_adc = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_input_a_mode,
    .pin = GPIO_PIN_7)
}; //(PA7)

const hdl_gpio_pin_t mod_gpio_pa8 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_8)
}; //(PA8)

const hdl_gpio_pin_t mod_gpio_pa9 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_9)
}; //(PA9)

const hdl_gpio_pin_t mod_gpio_pa10 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_10)
}; //(PA10)

const hdl_gpio_pin_t mod_gpio_pa11 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_11)
}; //(PA11)

const hdl_gpio_pin_t mod_gpio_pa12 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_12)
}; //(PA12)

const hdl_gpio_pin_t mod_gpio_pa13 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_13)
}; //(PA13)

const hdl_gpio_pin_t mod_gpio_pa14 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_14)
}; //(PA14)

const hdl_gpio_pin_t mod_gpio_pa15 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_15)
}; //(PA15)

const hdl_gpio_pin_t mod_gpio_pb0 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_0)
}; //(PB0)

const hdl_gpio_pin_t mod_gpio_pb1 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_1)
}; //(PB1)

const hdl_gpio_pin_t mod_gpio_pb2 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_2)
}; //(PB2)

const hdl_gpio_pin_t mod_gpio_pb3 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_3)
}; //(PB3)

const hdl_gpio_pin_t mod_gpio_pb4 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_4)
}; //(PB4)

const hdl_gpio_pin_t mod_gpio_pb5 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_5)
}; //(PB5)

const hdl_gpio_pin_t mod_gpio_pb6 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_6)
}; //(PB6)

const hdl_gpio_pin_t mod_gpio_pb7 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_7)
}; //(PB7)

const hdl_gpio_pin_t mod_gpio_pb8 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_8)
}; //(PB8)

const hdl_gpio_pin_t mod_gpio_pb9 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_9)
}; //(PB9)

const hdl_gpio_pin_t mod_gpio_pin_pb10_scl = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_i2c1b_mode, 
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_10)
}; //(PB10)

const hdl_gpio_pin_t mod_gpio_pin_pb11 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_11)
};   // (PB11)

const hdl_gpio_pin_t mod_gpio_pin_pb11_sda = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_i2c1b_mode, 
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_11)
}; //(PB11)

const hdl_gpio_pin_t mod_gpio_pb12 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_12)
}; //(PB12)

const hdl_gpio_pin_t mod_gpio_pb13 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_13)
}; //(PB13)

const hdl_gpio_pin_t mod_gpio_pb14 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_14)
}; //(PB14)

const hdl_gpio_pin_t mod_gpio_pb15 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_15)
}; //(PB15)

const hdl_gpio_pin_t mod_gpio_pc13 = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_c),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_pp_mode, 
    .inactive_default = HDL_GPIO_LOW,
    .pin = GPIO_PIN_13)
}; //(PC13)

const hdl_gpio_pin_t mod_gpio_pin_pf6_od = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_od_mode, 
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_6)
}; //(PF6)

const hdl_gpio_pin_t mod_gpio_pin_pf7_od = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_output_od_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_7)
}; //(PF7)

const hdl_gpio_pin_t mod_gpio_pin_pf6_scl = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_i2c1f_mode, 
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_6)
}; //(PF6)

const hdl_gpio_pin_t mod_gpio_pin_pf7_sda = {
  .iface = &hdl_gpio_pin_iface,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .dependencies = hdl_module_dependencies(&hdl_gpio_port_f),
  .config = hdl_module_config(hdl_gpio_pin_config_t,
    .hwc = &mod_gpio_i2c1f_mode,
    .inactive_default = HDL_GPIO_HIGH,
    .pin = GPIO_PIN_7)
}; //(PF7)

/**************************************************************
 *  I2C
 *************************************************************/

hdl_i2c_channel_t i2c0_ch0 = {.address = HDL_I2C_SLAVE_ADDR};

const hdl_i2c_config_t mod_i2c0_cnf = {
  .channels = hdl_i2c_channels(&i2c0_ch0),
  .hwc = hdl_module_config(hdl_i2c_config_hw_t,
    .addr_10_bits = 0,
    .dtcy = I2C_DTCY_2,
    .dual_address = 0,
    .err_interrupt = &mod_irq_i2c0_er,
    .ev_interrupt = &mod_irq_i2c0_ev,
    .general_call_enable = 0,
    .speed = 400000,
    .stretch_enable = 1,
    .rcu = RCU_I2C0,
    .phy = I2C0
  )
};
 
const hdl_i2c_mcu_t mod_i2c0 = {
  .iface = &hdl_i2c_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_pin_pf6_scl, &mod_gpio_pin_pf7_sda,
                                          &mod_clock_apb1, &mod_nvic, &mod_systick_timer),
  .config = &mod_i2c0_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_I2C_VAR_SIZE)
};
 
hdl_i2c_channel_t i2c1_ch0 = {.address = HDL_I2C_SLAVE_ADDR};
 
const hdl_i2c_config_t mod_i2c1_cnf = {
  .channels = hdl_i2c_channels(&i2c1_ch0),
  .hwc = hdl_module_config(hdl_i2c_config_hw_t, 
    .addr_10_bits = 0,
    .dtcy = I2C_DTCY_2,
    .dual_address = 0,
    .err_interrupt = &mod_irq_i2c1_er,
    .ev_interrupt = &mod_irq_i2c1_ev,
    .general_call_enable = 0,
    .speed = 400000,
    .stretch_enable = 1,
    .rcu = RCU_I2C1,
    .phy = I2C1
  )
};
 
const hdl_i2c_mcu_t mod_i2c1 = {
  .iface = &hdl_i2c_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_pin_pb10_scl, &mod_gpio_pin_pb11_sda,
                                          &mod_clock_apb1, &mod_nvic, &mod_systick_timer),
  .config = &mod_i2c1_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_I2C_VAR_SIZE)
};
 
const hdl_i2c_config_t mod_i2c_sw_cnf = {
  .channels = NULL,
  .hwc =  hdl_module_config(hdl_i2c_sw_hw_config_t,
    .switch_delay = 70
  )
};
 
const hdl_i2c_sw_t mod_i2c_sw = {
  .iface = &hdl_i2c_sw_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_pin_pf6_od, &mod_gpio_pin_pf7_od,
                                          &mod_systick_counter),
  .config = &mod_i2c_sw_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_I2C_SW_VAR_SIZE)
};
  
/**************************************************************
 *  SPI
 *************************************************************/

const hdl_spi_client_mcu_t mod_spi0 = {
  .iface = &hdl_spi_client_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_pin_pa7_mosi, &mod_gpio_pin_pa6_miso, &mod_gpio_pin_pa5_sck,
                                          &mod_clock_apb1, &mod_nvic),
  .config = hdl_module_config(hdl_spi_client_config_t,
    .phy = SPI0,
    .rcu = RCU_SPI0,
    .endian = HDL_SPI_ENDIAN_MSB,
    .polarity = SPI_CK_PL_HIGH_PH_2EDGE,
    .prescale = HDL_SPI_PSC_2,
    .spi_interrupt = &mod_irq_spi_0
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_SPI_CLIENT_VAR_SIZE)
};

const hdl_spi_client_ch_mcu_t mod_spi0_ch0 = {
  .iface = &hdl_spi_client_ch_iface,
  .dependencies = hdl_module_dependencies(&mod_spi0, &mod_gpio_pin_pa4_cs, &mod_systick_counter),
  .config = hdl_module_config(hdl_spi_client_ch_config_t, .cs_min_delay = 90),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_SPI_CLIENT_CH_VAR_SIZE)
};

const hdl_xl9535_port_t mod_gpio_exp_38 = {
  .iface = &hdl_xl9535_port_iface,
  .config = hdl_module_config(hdl_xl9535_port_config_t, .address = 38),
  .dependencies = hdl_module_dependencies(&mod_i2c_sw, &mod_systick_timer, &hdl_null_module),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_XL9535_PORT_VAR_SIZE),
};

const hdl_xl9535_pin_config_hw_t mod_gpio_exp_pin_output_low_cnf = {
  .output = HDL_XL9535_CNF_OUTPUT_LOW,
  .polarity = HDL_XL9535_CNF_POLARITY_RETAINED,
  .mode = HDL_XL9535_CNF_MODE_OUTPUT
};

const hdl_xl9535_pin_config_hw_t mod_gpio_exp_pin_input_cnf = {
  .polarity = HDL_XL9535_CNF_POLARITY_RETAINED,
  .mode = HDL_XL9535_CNF_MODE_INPUT
};

const hdl_xl9535_pin_t mod_gpio_exp_38_pin00 = {
  .iface = &hdl_xl9535_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_exp_38),
  .config = hdl_module_config(hdl_xl9535_pin_config_t,
    .inactive_default = HDL_GPIO_LOW,
    .pin_no = 0,
    .hwc = &mod_gpio_exp_pin_output_low_cnf),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_xl9535_pin_t mod_gpio_exp_38_pin01 = {
  .iface = &hdl_xl9535_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_exp_38),
  .config = hdl_module_config(hdl_xl9535_pin_config_t,
    .inactive_default = HDL_GPIO_LOW,
    .pin_no = 1,
    .hwc = &mod_gpio_exp_pin_output_low_cnf),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_xl9535_pin_t mod_gpio_exp_38_pin02 = {
  .iface = &hdl_xl9535_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_exp_38),
  .config = hdl_module_config(hdl_xl9535_pin_config_t,
    .inactive_default = HDL_GPIO_LOW,
    .pin_no = 2,
    .hwc = &mod_gpio_exp_pin_output_low_cnf),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_xl9535_pin_t mod_gpio_exp_38_pin03 = {
  .iface = &hdl_xl9535_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_exp_38),
  .config = hdl_module_config(hdl_xl9535_pin_config_t,
    .inactive_default = HDL_GPIO_LOW,
    .pin_no = 3,
    .hwc = &mod_gpio_exp_pin_output_low_cnf),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_xl9535_pin_t mod_gpio_exp_38_pin04 = {
  .iface = &hdl_xl9535_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_exp_38),
  .config = hdl_module_config(hdl_xl9535_pin_config_t,
    .inactive_default = HDL_GPIO_LOW,
    .pin_no = 4,
    .hwc = &mod_gpio_exp_pin_output_low_cnf),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_xl9535_pin_t mod_gpio_exp_38_pin05 = {
  .iface = &hdl_xl9535_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_exp_38),
  .config = hdl_module_config(hdl_xl9535_pin_config_t,
    .inactive_default = HDL_GPIO_LOW,
    .pin_no = 5,
    .hwc = &mod_gpio_exp_pin_input_cnf),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_xl9535_pin_t mod_gpio_exp_38_pin06 = {
  .iface = &hdl_xl9535_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_exp_38),
  .config = hdl_module_config(hdl_xl9535_pin_config_t,
    .inactive_default = HDL_GPIO_LOW,
    .pin_no = 6,
    .hwc = &mod_gpio_exp_pin_output_low_cnf),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_xl9535_pin_t mod_gpio_exp_38_pin07 = {
  .iface = &hdl_xl9535_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_exp_38),
  .config = hdl_module_config(hdl_xl9535_pin_config_t,
    .inactive_default = HDL_GPIO_LOW,
    .pin_no = 7,
    .hwc = &mod_gpio_exp_pin_output_low_cnf),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_xl9535_pin_t mod_gpio_exp_38_pin10 = {
  .iface = &hdl_xl9535_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_exp_38),
  .config = hdl_module_config(hdl_xl9535_pin_config_t,
    .inactive_default = HDL_GPIO_LOW,
    .pin_no = 8,
    .hwc = &mod_gpio_exp_pin_output_low_cnf),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_xl9535_pin_t mod_gpio_exp_38_pin11 = {
  .iface = &hdl_xl9535_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_exp_38),
  .config = hdl_module_config(hdl_xl9535_pin_config_t,
    .inactive_default = HDL_GPIO_LOW,
    .pin_no = 9,
    .hwc = &mod_gpio_exp_pin_output_low_cnf),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_xl9535_pin_t mod_gpio_exp_38_pin12 = {
  .iface = &hdl_xl9535_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_exp_38),
  .config = hdl_module_config(hdl_xl9535_pin_config_t,
    .inactive_default = HDL_GPIO_LOW,
    .pin_no = 10,
    .hwc = &mod_gpio_exp_pin_output_low_cnf),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_xl9535_pin_t mod_gpio_exp_38_pin13 = {
  .iface = &hdl_xl9535_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_exp_38),
  .config = hdl_module_config(hdl_xl9535_pin_config_t,
    .inactive_default = HDL_GPIO_LOW,
    .pin_no = 11,
    .hwc = &mod_gpio_exp_pin_input_cnf),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_xl9535_pin_t mod_gpio_exp_38_pin14 = {
  .iface = &hdl_xl9535_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_exp_38),
  .config = hdl_module_config(hdl_xl9535_pin_config_t,
    .inactive_default = HDL_GPIO_LOW,
    .pin_no = 12,
    .hwc = &mod_gpio_exp_pin_input_cnf),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_xl9535_pin_t mod_gpio_exp_38_pin15 = {
  .iface = &hdl_xl9535_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_exp_38),
  .config = hdl_module_config(hdl_xl9535_pin_config_t,
    .inactive_default = HDL_GPIO_LOW,
    .pin_no = 13,
    .hwc = &mod_gpio_exp_pin_input_cnf),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_xl9535_pin_t mod_gpio_exp_38_pin16 = {
  .iface = &hdl_xl9535_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_exp_38),
  .config = hdl_module_config(hdl_xl9535_pin_config_t,
    .inactive_default = HDL_GPIO_LOW,
    .pin_no = 14,
    .hwc = &mod_gpio_exp_pin_input_cnf),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};

const hdl_xl9535_pin_t mod_gpio_exp_38_pin17 = {
  .iface = &hdl_xl9535_pin_iface,
  .dependencies = hdl_module_dependencies(&mod_gpio_exp_38),
  .config = hdl_module_config(hdl_xl9535_pin_config_t,
    .inactive_default = HDL_GPIO_LOW,
    .pin_no = 15,
    .hwc = &mod_gpio_exp_pin_input_cnf),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};
/*================================================================*/

const hdl_eeprom_i2c_config_t mod_eeprom_i2c_cnf = {
  .chip_address = HDL_I2C_EEPROM_ADDR,
  .page_size = 16,
  .write_time = 6,
  .size = 1024
};

const hdl_eeprom_i2c_t mod_eeprom_i2c = {
  .iface = &hdl_eeprom_iface,
  .dependencies = hdl_module_dependencies(&mod_i2c_sw, &mod_systick_timer, &mod_gpio_pc13),
  .config = &mod_eeprom_i2c_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_EEPROM_I2C_VAR_SIZE)
};

/*================================================================*/

#ifdef ATB_MPCU_ABP

const hdl_clock_mcu_t mod_clock_adc = {
  .iface = &hdl_clock_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_irc28m),
  .config = hdl_module_config(hdl_clock_config_t,
    .type = HDL_CLOCK_TYPE_ADC,
    .property.div = 1,
    .phy = RCU
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_CLOCK_VAR_SIZE)
};

const hdl_dma_config_t mod_dma_config = {
  .rcu = RCU_DMA,
  .phy = DMA
};

const hdl_dma_mcu_t mod_dma = {
  .iface = &hdl_dma_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_ahb),
  .config = &mod_dma_config,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_dma_channel_config_t mod_dma_ch_adc_cnf = {
  .priority = DMA_PRIORITY_LOW,
  .direction = DMA_PERIPHERAL_TO_MEMORY,
  .memory_width = DMA_MEMORY_WIDTH_32BIT,
  .periph_width = DMA_PERIPHERAL_WIDTH_16BIT,
  .memory_inc = 1,
  .periph_inc = 0,
  .circular = 1,
  .ch_no = DMA_CH0
};

hdl_dma_channel_mcu_t mod_dma_ch_adc = {
  .iface = &hdl_dma_channel_iface,
  .dependencies = hdl_module_dependencies(&mod_dma),
  .config = &mod_dma_ch_adc_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
};

const hdl_adc_source_t mod_adc_source_ch4_adc = {
  .channel = HDL_ADC_CHANNEL_4,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5,
};

const hdl_adc_source_t mod_adc_source_ch5_adc = {
  .channel = HDL_ADC_CHANNEL_5,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5,
};

const hdl_adc_source_t mod_adc_source_ch6_adc = {
  .channel = HDL_ADC_CHANNEL_6,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5,
};

const hdl_adc_source_t mod_adc_source_ch7_adc = {
  .channel = HDL_ADC_CHANNEL_7,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5,
};

const hdl_adc_source_t mod_adc_source_ch16_adc = {
  .channel = ADC_CHANNEL_16,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5,
};

const hdl_adc_source_t mod_adc_source_ch17_adc = {
  .channel = ADC_CHANNEL_17,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5,
};

const hdl_adc_config_t mod_adc_mcu_cnf = {
  .phy = ADC,
  .adc_interrupt = &mod_irq_adc,
  .resolution = HDL_ADC_RESOLUTION_12BIT,
  .data_alignment = HDL_ADC_DATA_ALIGN_RIGHT,
  .init_timeout = 3000,
  .sources = hdl_adc_src(&mod_adc_source_ch4_adc, &mod_adc_source_ch5_adc, &mod_adc_source_ch6_adc, &mod_adc_source_ch16_adc,
                         &mod_adc_source_ch17_adc, &mod_adc_source_ch7_adc),
};

const hdl_adc_mcu_t mod_adc_mcu = {
  .iface = &hdl_adc_iface,
  .dependencies = hdl_module_dependencies(&mod_clock_adc, &mod_systick_timer, &mod_dma_ch_adc, &mod_nvic,
    &mod_gpio_pin_pa4_adc, &mod_gpio_pin_pa5_adc, &mod_gpio_pin_pa6_adc, &mod_gpio_pin_pa7_adc
  ),
  .config = &mod_adc_mcu_cnf,
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_ADC_VAR_SIZE)
};

#endif

/**************************************************************
 *  UART
 *************************************************************/

 const hdl_uart_mcu_t mod_uart1 = {
  .iface = &hdl_uart_iface,
  .dependencies = hdl_module_dependencies(
    &mod_gpio_pa3_uart_rx, 
    &mod_gpio_pa2_uart_tx, 
    &mod_clock_apb1,
    &mod_nvic, 
    &mod_systick_timer 
  ),
  .config = hdl_module_config(hdl_uart_config_t,
    .phy = (uint32_t)USART0, 
    .rcu = RCU_USART1,
    .interrupt = &mod_irq_usart1,
    .baudrate = 115200,
    .word_len = USART_WL_8BIT,
    .parity = USART_PM_NONE,
    .stop_bits = USART_STB_1BIT
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_UART_VAR_SIZE)
};

/*================================================================*/

extern const hdl_interrupt_controller_t mod_ic                    __attribute__ ((alias ("mod_nvic")));

extern const hdl_tick_counter_t mod_tick_counter                  __attribute__ ((alias ("mod_systick_counter")));

extern const hdl_time_counter_t mod_timer_ms                      __attribute__ ((alias ("mod_systick_timer")));

extern const hdl_gpio_pin_t mod_output1_pin                       __attribute__ ((alias ("mod_gpio_pa8")));
extern const hdl_gpio_pin_t mod_output2_pin                       __attribute__ ((alias ("mod_gpio_pa9")));

extern const hdl_gpio_pin_t mod_led1_pin                          __attribute__ ((alias ("mod_gpio_pb12")));
extern const hdl_gpio_pin_t mod_led2_pin                          __attribute__ ((alias ("mod_gpio_pb13")));
extern const hdl_gpio_pin_t mod_led3_pin                          __attribute__ ((alias ("mod_gpio_pb14")));
extern const hdl_gpio_pin_t mod_led4_pin                          __attribute__ ((alias ("mod_gpio_pb15")));
extern const hdl_gpio_pin_t mod_button_pin                        __attribute__ ((alias ("mod_gpio_pa0")));

extern const hdl_uart_t mod_uart                                  __attribute__ ((alias ("mod_uart1")));

extern const hdl_spi_client_ch_t mod_spi_client                   __attribute__ ((alias ("mod_spi0_ch0")));


#endif
