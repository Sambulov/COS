/*
  Module Init Graph (MIG)
*/

#include "app.h"

#define TEST_NO 1
/* 
  1  - pll by hxtal/2, sys clock 108MHz SysTick          | w
  2  - pll by hxtal/2, sys clock 72MHz  SysTick          | w
  3  - pll by hxtal/1, sys clock 108MHz SysTick          | w
  4  - pll by irc8m/2, sys clock 108MHz SysTick          | w
  5  - pll by irc8m/2, sys clock 72MHz  SysTick          | w
  6  - pll by irc8m/2, sys clock 36MHz TIMER0            | w
  7  - pll by irc8m/2, sys clock 36MHz APB2/2 TIMER0*2   | w
  8  - pll by irc8m/2, sys clock 108MHz APB2/16 TIMER0*2 | w
  9  - pll by irc8m/2, sys clock 108MHz APB1/16 TIMER1*2 | w
  10 - pll by irc8m/2, sys clock 52MHz  APB1/1 TIMER1*1  | w
  11 - pll by irc8m/2, sys clock 52MHz  APB1/2 TIMER1*2  | w
*/

  #define HDL_HXTAL_CLOCK              8000000
  #define HDL_LXTAL_CLOCK              32768
  //#define HDL_HXTAL_2_PLLSEL_PREDIV    2
  //#define HDL_PLLMUL                   27
  #define HDL_AHB_PREDIV               1
  //#define HDL_APB1_PREDIV              2
  //#define HDL_APB2_PREDIV              1
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  //#define HDL_PLL_MUL_CLOCK            mod_clock_hxtal_prescaler /* Can be clocked by: mod_clock_hxtal_prescaler, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll             /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */
  //#define MS_TIMER_RELOAD_VAL          108000-1

#if TEST_NO == 1
  #define HDL_HXTAL_2_PLLSEL_PREDIV    2
  #define HDL_PLLMUL                   27
  #define HDL_PLL_MUL_CLOCK            mod_clock_hxtal_prescaler
  #define MS_TIMER_RELOAD_VAL          108000-1
  #define HDL_APB1_PREDIV              2
  #define HDL_APB2_PREDIV              1
  #define HDL_ADC_PREDIV               8
#endif

#if TEST_NO == 2
  #define HDL_HXTAL_2_PLLSEL_PREDIV    2
  #define HDL_PLLMUL                   18
  #define HDL_PLL_MUL_CLOCK            mod_clock_hxtal_prescaler
  #define MS_TIMER_RELOAD_VAL          72000-1
  #define HDL_APB1_PREDIV              2
  #define HDL_APB2_PREDIV              1
#endif

#if TEST_NO == 3
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   9
  #define HDL_PLL_MUL_CLOCK            mod_clock_hxtal_prescaler
  #define MS_TIMER_RELOAD_VAL          72000-1
  #define HDL_APB1_PREDIV              2
  #define HDL_APB2_PREDIV              1
#endif

#if TEST_NO == 4
  #define HDL_HXTAL_2_PLLSEL_PREDIV    2
  #define HDL_PLLMUL                   27
  #define HDL_PLL_MUL_CLOCK            mod_clock_irc8m
  #define MS_TIMER_RELOAD_VAL          108000-1
  #define HDL_APB1_PREDIV              2
  #define HDL_APB2_PREDIV              1
#endif

#if TEST_NO == 5
  #define HDL_HXTAL_2_PLLSEL_PREDIV    2
  #define HDL_PLLMUL                   18
  #define HDL_PLL_MUL_CLOCK            mod_clock_irc8m
  #define MS_TIMER_RELOAD_VAL          72000-1
  #define HDL_APB1_PREDIV              2
  #define HDL_APB2_PREDIV              1
#endif

#if TEST_NO == 6
  #define HDL_HXTAL_2_PLLSEL_PREDIV    2
  #define HDL_PLLMUL                   9
  #define HDL_PLL_MUL_CLOCK            mod_clock_irc8m
  #define MS_TIMER_RELOAD_VAL          36000-1
  #define HDL_APB1_PREDIV              2
  #define HDL_APB2_PREDIV              1
#endif

#if TEST_NO == 7
  #define HDL_HXTAL_2_PLLSEL_PREDIV    2
  #define HDL_PLLMUL                   9
  #define HDL_PLL_MUL_CLOCK            mod_clock_irc8m
  #define MS_TIMER_RELOAD_VAL          36000-1
  #define HDL_APB1_PREDIV              2
  #define HDL_APB2_PREDIV              2
  #define HDL_ADC_PREDIV               4
#endif

#if TEST_NO == 8
  #define HDL_HXTAL_2_PLLSEL_PREDIV    2
  #define HDL_PLLMUL                   27
  #define HDL_PLL_MUL_CLOCK            mod_clock_irc8m
  #define MS_TIMER_RELOAD_VAL          13500-1
  #define HDL_APB1_PREDIV              2
  #define HDL_APB2_PREDIV              16
#endif

#if TEST_NO == 9
  #define HDL_HXTAL_2_PLLSEL_PREDIV    2
  #define HDL_PLLMUL                   27
  #define HDL_PLL_MUL_CLOCK            mod_clock_irc8m
  #define MS_TIMER_RELOAD_VAL          13500-1
  #define HDL_APB1_PREDIV              16
  #define HDL_APB2_PREDIV              2
#endif

#if TEST_NO == 10
  #define HDL_HXTAL_2_PLLSEL_PREDIV    2
  #define HDL_PLLMUL                   13
  #define HDL_PLL_MUL_CLOCK            mod_clock_irc8m
  #define MS_TIMER_RELOAD_VAL          52000-1
  #define HDL_APB1_PREDIV              1
  #define HDL_APB2_PREDIV              2
#endif

#if TEST_NO == 11
  #define HDL_HXTAL_2_PLLSEL_PREDIV    2
  #define HDL_PLLMUL                   13
  #define HDL_PLL_MUL_CLOCK            mod_clock_irc8m
  #define MS_TIMER_RELOAD_VAL          52000-1
  #define HDL_APB1_PREDIV              2
  #define HDL_APB2_PREDIV              2
#endif

  #define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

  hdl_clock_t mod_clock_irc8m = {
    .module.init = &hdl_clock_irc8m,
    .module.dependencies = NULL,
    .module.reg = (void *)RCU,
    .freq = 8000000,
    .div = 1
  };

  hdl_clock_t mod_clock_hxtal = {
    .module.init = &hdl_clock_hxtal,
    .module.dependencies = NULL,
    .module.reg = (void *)RCU,
    .freq = HDL_HXTAL_CLOCK,
    .div = 1
  };

  hdl_clock_t mod_clock_lxtal = {
    .module.init = &hdl_clock_lxtal,
    .module.dependencies = NULL,
    .module.reg = (void *)RCU,
    .freq = HDL_LXTAL_CLOCK,
    .div = 1
  };

  hdl_clock_t mod_clock_irc40k = {
    .module.init = &hdl_clock_irc40k,
    .module.dependencies = NULL,
    .freq = 40000,
    .div = 1
  };

  hdl_clock_prescaler_t mod_clock_hxtal_prescaler = {
    .module.init = &hdl_clock_hxtal_prescaler,
    .module.dependencies = hdl_module_dependencies(&mod_clock_hxtal.module),
    .module.reg = (void *)RCU,
    .muldiv_factor = HDL_HXTAL_2_PLLSEL_PREDIV,
  };

  hdl_clock_t mod_clock_selector_pll = {
    .module.init = &hdl_clock_selector_pll,
    /* If source IRC8M before oscillator there is prescaler 2, this logic realized inside driver */
    .module.dependencies = hdl_module_dependencies(&HDL_PLL_MUL_CLOCK.module),
    .module.reg = (void *)RCU,
  };

  hdl_clock_prescaler_t mod_clock_pll = {
    .module.init = &hdl_clock_pll,
    .module.dependencies = hdl_module_dependencies(&mod_clock_selector_pll.module),
    .module.reg = (void *)RCU,
    .muldiv_factor = HDL_PLLMUL
  };

  hdl_core_t mod_sys_core = {
    .module.init = &hdl_core,
    .module.dependencies = NULL,
    .module.reg = (void *)SCB_BASE,
    .flash_latency = WS_WSCNT_2 /* WS_WSCNT_0: sys_clock <= 24MHz, WS_WSCNT_1: sys_clock <= 48MHz, WS_WSCNT_2: sys_clock <= 72MHz */
    /* TODO: latency */
  };

  hdl_clock_prescaler_t mod_clock_system_clock_source = {
    .module.init = &hdl_clock_system,
    .module.dependencies = hdl_module_dependencies(&mod_sys_core.module, &HDL_SYS_CLOCK.module),
    .module.reg = (void *)RCU,
    .muldiv_factor = 1,
  };

  hdl_nvic_interrupt_t mod_irq_systick = {
    .irq_type = HDL_NVIC_EXCEPTION_SysTick,
    .priority = 0,
    .priority_group = 0,
  };

  hdl_nvic_interrupt_t mod_irq_timer0_update = {
    .irq_type = HDL_NVIC_IRQ25_TIMER0_UP_TIMER9,
    .priority = 0,
    .priority_group = 0,
  };

  hdl_nvic_interrupt_t mod_irq_timer1 = {
    .irq_type = HDL_NVIC_IRQ28_TIMER1,
    .priority = 0,
    .priority_group = 0,
  };

  hdl_nvic_t mod_nvic = {
    .module.init = &hdl_interrupt_controller,
    .module.dependencies = hdl_module_dependencies(&mod_sys_core.module),
    .module.reg = NVIC,
    .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
    .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_timer0_update, &mod_irq_timer1),
  };

  hdl_clock_prescaler_t mod_clock_ahb = {
    .module.init = &hdl_clock_ahb,
    .module.dependencies = hdl_module_dependencies(&mod_clock_system_clock_source.module),
    .module.reg = (void *)RCU,
    .muldiv_factor = HDL_AHB_PREDIV,
  };

  hdl_clock_prescaler_t mod_clock_apb1 = {
    .module.init = &hdl_clock_apb1,
    .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .module.reg = (void *)RCU,
    .muldiv_factor = HDL_APB1_PREDIV,
  };

  hdl_clock_prescaler_t mod_clock_apb2 = {
    .module.init = &hdl_clock_apb2,
    .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .module.reg = (void *)RCU,
    .muldiv_factor = HDL_APB2_PREDIV,
  };

  hdl_clock_prescaler_t mod_clock_adc = {
    .module.init = &hdl_clock_adc,
    .module.dependencies = hdl_module_dependencies(&mod_clock_apb2.module),
    .module.reg = (void *)RCU,
    .muldiv_factor = HDL_ADC_PREDIV,
  };

  const hdl_tick_counter_hw_config_t mod_tick_counter0_cnf = {
    .alignedmode = TIMER_COUNTER_EDGE,
    .clockdivision = TIMER_CKDIV_DIV1,
    .counterdirection = TIMER_COUNTER_UP,
    .period = 16000 - 1,
    .prescaler = 0,
    .repetitioncounter = 0,
    .rcu = RCU_TIMER0
  };

  const hdl_tick_counter_hw_config_t mod_tick_counter1_cnf = {
    .alignedmode = TIMER_COUNTER_EDGE,
    .clockdivision = TIMER_CKDIV_DIV1,
    .counterdirection = TIMER_COUNTER_UP,
    .period = 16000 - 1,
    .prescaler = 0,
    .repetitioncounter = 0,
    .rcu = RCU_TIMER1
  };

  const hdl_tick_counter_systick_config_t mod_systick_counter_cnf = {
    .period = 240000 - 1
  };

  hdl_tick_counter_t mod_systick_counter = {
    .module.init = &hdl_tick_counter,
    .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .module.reg = (void *)SysTick,
    .config = &mod_systick_counter_cnf
  };

  hdl_tick_counter_t mod_timer0_counter = {
    .module.init = &hdl_tick_counter,
    .module.dependencies = hdl_module_dependencies(&mod_clock_apb2.module),
    .module.reg = (void *)TIMER0,
    .config = &mod_tick_counter0_cnf
  };

  hdl_tick_counter_t mod_timer1_counter = {
    .module.init = &hdl_tick_counter,
    .module.dependencies = hdl_module_dependencies(&mod_clock_apb1.module),
    .module.reg = (void *)TIMER1,
    .config = &mod_tick_counter1_cnf
  };

#if TEST_NO >= 9
  hdl_timer_t mod_timer_ms = {
    .module.init = hdl_timer,
    .module.dependencies = hdl_module_dependencies(&mod_timer1_counter.module, &mod_nvic.module),
    .module.reg = NULL,
    .reload_iterrupt = HDL_NVIC_IRQ28_TIMER1,
    .val = 0
  };
#elif TEST_NO >= 6
  hdl_timer_t mod_timer_ms = {
    .module.init = hdl_timer,
    .module.dependencies = hdl_module_dependencies(&mod_timer0_counter.module, &mod_nvic.module),
    .module.reg = NULL,
    .reload_iterrupt = HDL_NVIC_IRQ25_TIMER0_UP_TIMER9,
    .val = 0
  };
#else
  hdl_timer_t mod_timer_ms = {
    .module.init = hdl_timer,
    .module.dependencies = hdl_module_dependencies(&mod_systick_counter.module, &mod_nvic.module),
    .module.reg = NULL,
    .reload_iterrupt = HDL_NVIC_EXCEPTION_SysTick,
    .val = 0
  };
#endif

hdl_dma_t mod_dma = {
  .module.init = &hdl_dma,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void*)DMA_BASE,
};

hdl_dma_channel_t mod_adc_dma_ch = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH0,
  .direction = HDL_DMA_DIRECTION_P2M,
  .memory_inc = HDL_DMA_INCREMENT_ON,
  .memory_width = HDL_DMA_SIZE_OF_MEMORY_32_BIT,
  .periph_inc = HDL_DMA_INCREMENT_OFF,
  .periph_width = HDL_DMA_SIZE_OF_MEMORY_16_BIT,
  .mode = HDL_DMA_MODE_CIRCULAR,
  .priority = HDL_DMA_PRIORITY_LOW
};

hdl_dma_channel_t mod_m2m_dma_ch = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH1,
  .direction = HDL_DMA_DIRECTION_M2M,
  .memory_inc = HDL_DMA_INCREMENT_ON,
  .memory_width = HDL_DMA_SIZE_OF_MEMORY_32_BIT,
  .periph_inc = HDL_DMA_INCREMENT_ON,
  .periph_width = HDL_DMA_SIZE_OF_MEMORY_32_BIT,
  .mode = HDL_DMA_MODE_SINGLE,
  .priority = HDL_DMA_PRIORITY_LOW
};

hdl_gpio_port_hw_config_t mod_gpio_port_config_a = {
  .rcu = RCU_GPIOA
};

hdl_gpio_port_hw_config_t mod_gpio_port_config_b = {
  .rcu = RCU_GPIOB
};

hdl_gpio_port_hw_config_t mod_gpio_port_config_c = {
  .rcu = RCU_GPIOC
};

hdl_gpio_port_hw_config_t mod_gpio_port_config_d = {
  .rcu = RCU_GPIOD
};

hdl_gpio_port_hw_config_t mod_gpio_port_config_e = {
  .rcu = RCU_GPIOE
};

hdl_gpio_port_t mod_gpio_a = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOA,
  .config = hdl_gpio_port_config(.hwc = &mod_gpio_port_config_a)
};

hdl_gpio_port_t mod_gpio_b = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOB,
  .config = hdl_gpio_port_config(.hwc = &mod_gpio_port_config_b)
};

hdl_gpio_port_t mod_gpio_c = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOC,
  .config = hdl_gpio_port_config(.hwc = &mod_gpio_port_config_c)
};

hdl_gpio_port_t mod_gpio_d = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOD,
  .config = hdl_gpio_port_config(.hwc = &mod_gpio_port_config_d)
};

hdl_gpio_port_t mod_gpio_e = {
  .module.init = &hdl_gpio_port,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)GPIOE,
  .config = hdl_gpio_port_config(.hwc = &mod_gpio_port_config_e)
};

hdl_gpio_pin_hw_config_t mod_gpio_mode_output_pp = {
  .ospeed = GPIO_OSPEED_2MHZ,
  .type = GPIO_MODE_OUT_PP
};

hdl_gpio_pin_hw_config_t mod_gpio_mode_input_np = {
  .type = GPIO_MODE_IN_FLOATING
};

hdl_gpio_pin_hw_config_t mod_gpio_mode_input_analog = {
  .type = GPIO_MODE_AIN
};

hdl_gpio_pin_t mod_gpio_a15_led1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_a.module),
  .module.reg = (void *)GPIO_PIN_15,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_c10_led2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_c.module),
  .module.reg = (void *)GPIO_PIN_10,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_c11_led3 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_c.module),
  .module.reg = (void *)GPIO_PIN_11,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_c12_led4 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_c.module),
  .module.reg = (void *)GPIO_PIN_12,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_d0_led5 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d.module),
  .module.reg = (void *)GPIO_PIN_0,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_d1_led6 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d.module),
  .module.reg = (void *)GPIO_PIN_1,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_d2_led7 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d.module),
  .module.reg = (void *)GPIO_PIN_2,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_d3_led8 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d.module),
  .module.reg = (void *)GPIO_PIN_3,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_d4_led9 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d.module),
  .module.reg = (void *)GPIO_PIN_4,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_d5_led10 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d.module),
  .module.reg = (void *)GPIO_PIN_5,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_d6_led11 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d.module),
  .module.reg = (void *)GPIO_PIN_6,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_d7_led12 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d.module),
  .module.reg = (void *)GPIO_PIN_7,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_output_pp)
};

hdl_plc_port_config_t mod_uspd_port_default_cnf_led = {
  .desc = (PLC_IO_PORT_DO_OD),
  .default_output = HDL_GPIO_HIGH,
  .default_option = 0
};

bldl_uspd_led_port_t mod_uspd_led_port1 = {
  .module.init = &bldl_uspd_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_a15_led1.module),
  .default_config = &mod_uspd_port_default_cnf_led
};

bldl_uspd_led_port_t mod_uspd_led_port2 = {
  .module.init = &bldl_uspd_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_c10_led2.module),
  .default_config = &mod_uspd_port_default_cnf_led
};

bldl_uspd_led_port_t mod_uspd_led_port3 = {
  .module.init = &bldl_uspd_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_c11_led3.module),
  .default_config = &mod_uspd_port_default_cnf_led
};

bldl_uspd_led_port_t mod_uspd_led_port4 = {
  .module.init = &bldl_uspd_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_c12_led4.module),
  .default_config = &mod_uspd_port_default_cnf_led
};

bldl_uspd_led_port_t mod_uspd_led_port5 = {
  .module.init = &bldl_uspd_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d0_led5.module),
  .default_config = &mod_uspd_port_default_cnf_led
};

bldl_uspd_led_port_t mod_uspd_led_port6 = {
  .module.init = &bldl_uspd_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d1_led6.module),
  .default_config = &mod_uspd_port_default_cnf_led
};

bldl_uspd_led_port_t mod_uspd_led_port7 = {
  .module.init = &bldl_uspd_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d2_led7.module),
  .default_config = &mod_uspd_port_default_cnf_led
};

bldl_uspd_led_port_t mod_uspd_led_port8 = {
  .module.init = &bldl_uspd_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d3_led8.module),
  .default_config = &mod_uspd_port_default_cnf_led
};

bldl_uspd_led_port_t mod_uspd_led_port9 = {
  .module.init = &bldl_uspd_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d4_led9.module),
  .default_config = &mod_uspd_port_default_cnf_led
};

bldl_uspd_led_port_t mod_uspd_led_port10 = {
  .module.init = &bldl_uspd_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d5_led10.module),
  .default_config = &mod_uspd_port_default_cnf_led
};

bldl_uspd_led_port_t mod_uspd_led_port11 = {
  .module.init = &bldl_uspd_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d6_led11.module),
  .default_config = &mod_uspd_port_default_cnf_led
};

bldl_uspd_led_port_t mod_uspd_led_port12 = {
  .module.init = &bldl_uspd_led_port,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d7_led12.module),
  .default_config = &mod_uspd_port_default_cnf_led
};

hdl_gpio_pin_t mod_gpio_e15_do8 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_e.module),
  .module.reg = (void *)GPIO_PIN_15,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_b13_do7 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_b.module),
  .module.reg = (void *)GPIO_PIN_13,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_b15_do6 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_b.module),
  .module.reg = (void *)GPIO_PIN_15,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_d9_do5 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d.module),
  .module.reg = (void *)GPIO_PIN_9,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_d11_do4 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d.module),
  .module.reg = (void *)GPIO_PIN_11,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_d13_do3 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d.module),
  .module.reg = (void *)GPIO_PIN_13,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_d15_do2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d.module),
  .module.reg = (void *)GPIO_PIN_15,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_c7_do1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_c.module),
  .module.reg = (void *)GPIO_PIN_7,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_b12_di8 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_b.module),
  .module.reg = (void *)GPIO_PIN_12,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_input_np)
};

hdl_gpio_pin_t mod_gpio_b14_di7 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_b.module),
  .module.reg = (void *)GPIO_PIN_14,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_input_np)
};

hdl_gpio_pin_t mod_gpio_d8_di6 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d.module),
  .module.reg = (void *)GPIO_PIN_8,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_input_np)
};

hdl_gpio_pin_t mod_gpio_d10_di5 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d.module),
  .module.reg = (void *)GPIO_PIN_10,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_input_np)
};

hdl_gpio_pin_t mod_gpio_d12_di4 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d.module),
  .module.reg = (void *)GPIO_PIN_12,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_input_np)
};

hdl_gpio_pin_t mod_gpio_d14_di3 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d.module),
  .module.reg = (void *)GPIO_PIN_14,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_input_np)
};

hdl_gpio_pin_t mod_gpio_c6_di2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_c.module),
  .module.reg = (void *)GPIO_PIN_6,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_input_np)
};

hdl_gpio_pin_t mod_gpio_c8_di1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_c.module),
  .module.reg = (void *)GPIO_PIN_8,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_HIGH, .hwc = &mod_gpio_mode_input_np)
};

hdl_plc_port_config_t mod_uspd_port_default_cnf_do = {
  .desc = (PLC_IO_PORT_DO_OD),
  .default_output = HDL_GPIO_LOW,
  .default_option = 0
};

bldl_uspd_discrete_port_t mod_uspd_od_port1 = {
  .module.init = &bldl_uspd_discrete_port,
  .module.reg = (void *)1,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_c8_di1.module, &mod_gpio_c7_do1.module),
  .default_config = &mod_uspd_port_default_cnf_do,
  .output = HDL_GPIO_HIGH,
};

bldl_uspd_discrete_port_t mod_uspd_od_port2 = {
  .module.init = &bldl_uspd_discrete_port,
  .module.reg = (void *)2,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_c6_di2.module, &mod_gpio_d15_do2.module),
  .default_config = &mod_uspd_port_default_cnf_do,
  .output = HDL_GPIO_HIGH,
};

bldl_uspd_discrete_port_t mod_uspd_od_port3 = {
  .module.init = &bldl_uspd_discrete_port,
  .module.reg = (void *)3,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d14_di3.module, &mod_gpio_d13_do3.module),
  .default_config = &mod_uspd_port_default_cnf_do,
  .output = HDL_GPIO_HIGH,
};

bldl_uspd_discrete_port_t mod_uspd_od_port4 = {
  .module.init = &bldl_uspd_discrete_port,
  .module.reg = (void *)4,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d12_di4.module, &mod_gpio_d11_do4.module),
  .default_config = &mod_uspd_port_default_cnf_do,
  .output = HDL_GPIO_HIGH,
};

bldl_uspd_discrete_port_t mod_uspd_od_port5 = {
  .module.init = &bldl_uspd_discrete_port,
  .module.reg = (void *)5,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d10_di5.module, &mod_gpio_d9_do5.module),
  .default_config = &mod_uspd_port_default_cnf_do,
  .output = HDL_GPIO_HIGH,
};

bldl_uspd_discrete_port_t mod_uspd_od_port6 = {
  .module.init = &bldl_uspd_discrete_port,
  .module.reg = (void *)6,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_d8_di6.module, &mod_gpio_b15_do6.module),
  .default_config = &mod_uspd_port_default_cnf_do,
  .output = HDL_GPIO_HIGH,
};

bldl_uspd_discrete_port_t mod_uspd_od_port7 = {
  .module.init = &bldl_uspd_discrete_port,
  .module.reg = (void *)7,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_b14_di7.module, &mod_gpio_b13_do7.module),
  .default_config = &mod_uspd_port_default_cnf_do,
  .output = HDL_GPIO_HIGH,
};

bldl_uspd_discrete_port_t mod_uspd_od_port8 = {
  .module.init = &bldl_uspd_discrete_port,
  .module.reg = (void *)8,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_b12_di8.module, &mod_gpio_e15_do8.module),
  .default_config = &mod_uspd_port_default_cnf_do,
  .output = HDL_GPIO_HIGH,
};

hdl_gpio_pin_t mod_gpio_a0_ai0 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_a.module),
  .module.reg = (void *)GPIO_PIN_0,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_input_analog)
};

hdl_gpio_pin_t mod_gpio_a1_ai1 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_a.module),
  .module.reg = (void *)GPIO_PIN_1,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_input_analog)
};

hdl_gpio_pin_t mod_gpio_a2_ai2 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_a.module),
  .module.reg = (void *)GPIO_PIN_2,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_input_analog)
};

hdl_gpio_pin_t mod_gpio_a3_ai3 = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_a.module),
  .module.reg = (void *)GPIO_PIN_3,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_input_analog)
};

hdl_gpio_pin_t mod_gpio_e12_ai0_pu = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_e.module),
  .module.reg = (void *)GPIO_PIN_12,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_e9_ai1_pu = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_e.module),
  .module.reg = (void *)GPIO_PIN_9,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_b2_ai2_pu = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_b.module),
  .module.reg = (void *)GPIO_PIN_2,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_c5_ai3_pu = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_c.module),
  .module.reg = (void *)GPIO_PIN_5,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_e14_ai0_csh = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_e.module),
  .module.reg = (void *)GPIO_PIN_14,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_e11_ai1_csh = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_e.module),
  .module.reg = (void *)GPIO_PIN_11,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_e8_ai2_csh = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_e.module),
  .module.reg = (void *)GPIO_PIN_8,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_b1_ai3_csh = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_b.module),
  .module.reg = (void *)GPIO_PIN_1,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_e13_ai0_pd = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_e.module),
  .module.reg = (void *)GPIO_PIN_13,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_e10_ai1_pd = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_e.module),
  .module.reg = (void *)GPIO_PIN_10,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_e7_ai2_pd = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_e.module),
  .module.reg = (void *)GPIO_PIN_7,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_gpio_pin_t mod_gpio_b0_ai3_pd = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_b.module),
  .module.reg = (void *)GPIO_PIN_0,
  .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_mode_output_pp)
};

hdl_adc_source_t mod_adc_source_0 = {
  .channel = HDL_ADC_CHANNEL_0,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
};

hdl_adc_source_t mod_adc_source_1 = {
  .channel = HDL_ADC_CHANNEL_1,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
};

hdl_adc_source_t mod_adc_source_2 = {
  .channel = HDL_ADC_CHANNEL_2,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
};

hdl_adc_source_t mod_adc_source_3 = {
  .channel = HDL_ADC_CHANNEL_3,
  .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
};

hdl_adc_t mod_adc = {
  .module.init = &hdl_adc,
  .module.dependencies = hdl_module_dependencies(&mod_clock_adc.module, &mod_timer_ms.module, &mod_adc_dma_ch.module),
  .module.reg = (void*)ADC0,
  .data_alignment = HDL_ADC_DATA_ALIGN_RIGHT,
  .init_timeout = 3000,
  .sources = hdl_adc_src(&mod_adc_source_0, &mod_adc_source_1, &mod_adc_source_2, &mod_adc_source_3),
};

hdl_plc_port_config_t mod_uspd_port_default_cnf_ain = {
  .desc = (PLC_IO_PORT_HW_INPUT | PLC_IO_PORT_HW_ANALOG | PLC_IO_PORT_SW_RAW_ADC),
  .default_output = 0,
  .default_option = 0
};

bldl_uspd_ain_port_t mod_uspd_ai_port0 = {
  .module.init = &bldl_uspd_ain_port,
  .module.reg = (void *)&mod_adc_source_0,
  .module.dependencies = hdl_module_dependencies(&mod_adc.module, &mod_gpio_a0_ai0.module, &mod_gpio_e12_ai0_pu.module,
                                                 &mod_gpio_e13_ai0_pd.module, &mod_gpio_e14_ai0_csh.module),
  .default_config = &mod_uspd_port_default_cnf_ain
};

bldl_uspd_ain_port_t mod_uspd_ai_port1 = {
  .module.init = &bldl_uspd_ain_port,
  .module.reg = (void *)&mod_adc_source_1,
  .module.dependencies = hdl_module_dependencies(&mod_adc.module, &mod_gpio_a1_ai1.module, &mod_gpio_e9_ai1_pu.module,
                                                 &mod_gpio_e10_ai1_pd.module, &mod_gpio_e11_ai1_csh.module),
  .default_config = &mod_uspd_port_default_cnf_ain
};

bldl_uspd_ain_port_t mod_uspd_ai_port2 = {
  .module.init = &bldl_uspd_ain_port,
  .module.reg = (void *)&mod_adc_source_2,
  .module.dependencies = hdl_module_dependencies(&mod_adc.module, &mod_gpio_a2_ai2.module, &mod_gpio_b2_ai2_pu.module,
                                                 &mod_gpio_e7_ai2_pd.module, &mod_gpio_e8_ai2_csh.module),
  .default_config = &mod_uspd_port_default_cnf_ain
};

bldl_uspd_ain_port_t mod_uspd_ai_port3 = {
  .module.init = &bldl_uspd_ain_port,
  .module.reg = (void *)&mod_adc_source_3,
  .module.dependencies = hdl_module_dependencies(&mod_adc.module, &mod_gpio_a3_ai3.module, &mod_gpio_c5_ai3_pu.module,
                                                 &mod_gpio_b0_ai3_pd.module, &mod_gpio_b1_ai3_csh.module),
  .default_config = &mod_uspd_port_default_cnf_ain
};

bldl_uspd_port_expander_t mod_uspd_port_expander = {
  .init = &bldl_uspd_port_expander,
  .dependencies = hdl_module_dependencies(&mod_uspd_led_port1.module, &mod_uspd_led_port2.module, &mod_uspd_led_port3.module,
                                          &mod_uspd_led_port4.module, &mod_uspd_led_port5.module, &mod_uspd_led_port6.module,
                                          &mod_uspd_led_port7.module, &mod_uspd_led_port8.module, &mod_uspd_led_port9.module,
                                          &mod_uspd_led_port10.module, &mod_uspd_led_port11.module, &mod_uspd_led_port12.module, 
                                          &mod_uspd_od_port1.module, &mod_uspd_od_port2.module, &mod_uspd_od_port3.module, 
                                          &mod_uspd_od_port4.module, &mod_uspd_od_port5.module, &mod_uspd_od_port6.module, 
                                          &mod_uspd_od_port7.module, &mod_uspd_od_port8.module, &mod_uspd_ai_port0.module, 
                                          &mod_uspd_ai_port1.module, &mod_uspd_ai_port2.module, &mod_uspd_ai_port3.module)
};
