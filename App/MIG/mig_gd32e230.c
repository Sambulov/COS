/*
  Module Init Graph (MIG)
*/

#include "bldl.h"

#if defined ( ATB_RK3568J_SMC_R1 )

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    2
  #define HDL_PLLMUL                   9
  #define HDL_AHB_PREDIV               1
  #define HDL_APB1_PREDIV              1
  #define HDL_APB2_PREDIV              1
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_prescaler   /* Can be clocked by: mod_clock_pll_prescaler, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll_mul         /* Can be clocked by: mod_clock_pll_mul, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

  hdl_core_t mod_sys_core = {
    .module.init = &hdl_core,
    .module.dependencies = NULL,
    .module.reg = (void *)SCB_BASE,
    .flash_latency = WS_WSCNT_2 /* WS_WSCNT_0: sys_clock <= 24MHz, WS_WSCNT_1: sys_clock <= 48MHz, WS_WSCNT_2: sys_clock <= 72MHz */
    /* TODO: ... */
  };

  hdl_nvic_interrupt_t mod_systick_irq = {
    .irq_type = SysTick_IRQn,
    .priority = 0,
    .priority_group = 0,
  };

  hdl_nvic_interrupt_t mod_exti_0_1_irq = {
    .irq_type = EXTI0_1_IRQn,
    .priority = 0,
    .priority_group = 1,
  };

  hdl_nvic_interrupt_t mod_exti_2_3_irq = {
    .irq_type = EXTI2_3_IRQn,
    .priority = 0,
    .priority_group = 2,
  };

  hdl_nvic_interrupt_t mod_unexisting_8_irq = {
    .irq_type = 8,
    .priority = 0,
    .priority_group = 2,
  };

  hdl_nvic_t mod_nvic = {
    .module.init = &hdl_nvic,
    //.module.dependencies = hdl_module_dependencies(&mod_sys_core.module),
    .module.reg = NVIC,
    .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
    .interrupts = hdl_interrupts(&mod_systick_irq, &mod_exti_0_1_irq, &mod_exti_2_3_irq, &mod_unexisting_8_irq)
  };

  /**************************************************************
   *  Oscillator
   *************************************************************/
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

  hdl_clock_t mod_clock_irc28m = {
    .module.init = &hdl_clock_irc28m,
    .module.dependencies = NULL,
    .module.reg = (void *)RCU,
    .freq = 28000000,
    .div = 1
  };

  hdl_clock_t mod_clock_irc40k = {
    .module.init = &hdl_clock_irc40k,
    .module.dependencies = NULL,
    .freq = 40000,
    .div = 1
  };

  /**************************************************************
   *  PLL prescaler
   *************************************************************/
  hdl_clock_prescaler_t mod_clock_pll_prescaler = {
    .module.init = &hdl_clock_pll_prescaler,
    .module.dependencies = hdl_module_dependencies(&mod_clock_hxtal.module),
    .module.reg = (void *)RCU,
    .muldiv_factor = HDL_HXTAL_2_PLLSEL_PREDIV,
  };

  /**************************************************************
   *  Selector PLL source second rang (HXTAL or IRC8M)
   **************************************************************/
  hdl_clock_prescaler_t mod_clock_pll_selector = {
    .module.init = &hdl_clock_selector_pll,
    /* If source IRC8M before oscillator there is prescaler 2, this logic realized inside driver */
    .module.dependencies = hdl_module_dependencies(&HDL_PLL_MUL_CLOCK.module),
    .module.reg = (void *)RCU,
    .muldiv_factor = 1,
  };

  /**************************************************************
   *  PLL multiply factor
   *************************************************************/
  hdl_clock_prescaler_t mod_clock_pll_mul = {
    .module.init = &hdl_clock_pll_mul,
    .module.dependencies = hdl_module_dependencies(&mod_clock_pll_selector.module),
    .module.reg = (void *)RCU,
    .muldiv_factor = HDL_PLLMUL,
  };

  /**************************************************************
   *  Selector system clock source (IRC8M, CK_PLL, HXTAL)
   **************************************************************/
  hdl_clock_prescaler_t mod_clock_system_clock_source = {
    .module.init = &hdl_clock_system,
    .module.dependencies = hdl_module_dependencies(&mod_sys_core.module, &HDL_SYS_CLOCK.module),
    .module.reg = (void *)RCU,
    .muldiv_factor = 1,
  };

  /**************************************************************
   *  Selector RTC(HXTAL, LXTAL, IRC40K)
   *************************************************************/
  hdl_clock_prescaler_t mod_clock_selector_rtc = {
    .module.init = &hdl_clock_selector_rtc,
    .module.dependencies = hdl_module_dependencies(&HDL_RTC_CLOCK.module),
    .module.reg = (void *)RCU,
    .muldiv_factor = 1,
  };

  /**************************************************************
   *  AHB Prescaler
   *************************************************************/
  hdl_clock_prescaler_t mod_clock_ahb = {
    .module.init = &hdl_clock_ahb,
    .module.dependencies = hdl_module_dependencies(&mod_clock_system_clock_source.module),
    .module.reg = (void *)RCU,
    .muldiv_factor = HDL_AHB_PREDIV,
  };
  /**************************************************************
   *  APB1 Prescaler
   *************************************************************/
  hdl_clock_prescaler_t mod_clock_apb1 = {
    .module.init = &hdl_clock_apb1,
    .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .module.reg = (void *)RCU,
    .muldiv_factor = HDL_APB1_PREDIV,
  };
  /**************************************************************
   *  APB2 Prescaler
   *************************************************************/
  hdl_clock_prescaler_t mod_clock_apb2 = {
    .module.init = &hdl_clock_apb2,
    .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .module.reg = (void *)RCU,
    .muldiv_factor = HDL_APB2_PREDIV,
  };
  /**************************************************************
   *  DMA
   *************************************************************/
  hdl_dma_t mod_dma = {
    .module.init = &hdl_dma,
    .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .module.reg = (void*)DMA_BASE,
  };

  hdl_clock_prescaler_t mod_clock_timer0 = {
    .module.init = NULL,
    .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .module.reg = (void *)RCU,
    .muldiv_factor = 72,
  };

  hdl_clock_counter_t mod_timer0_counter = {
    .module.init = &hdl_clock_counter,
    .module.dependencies = hdl_module_dependencies(&mod_clock_timer0.module),
    .module.reg = (void *)TIMER0,
    .diction = HDL_UP_COUNTER,
    .counter_reload = 1000 - 1
  };

  hdl_clock_counter_t mod_systick_counter = {
    .module.init = &hdl_clock_counter,
    .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .module.reg = (void *)SysTick,
    .diction = HDL_DOWN_COUNTER,
    .counter_reload = 72000 - 1
  };

  hdl_sys_timer_t mod_sys_timer_ms = {
    .module.init = hdl_sys_timer,
    .module.dependencies = hdl_module_dependencies(&mod_systick_counter.module, &mod_nvic.module),
    .module.reg = NULL,
    .reload_iterrupt = &mod_systick_irq,
    .val = 0
  };

  /**************************************************************
   *  ADC
   *************************************************************/
  hdl_adc_channel_source_t mod_adc_channel_0 = {
    .channel_number = HDL_ADC_CHANNEL_0,
    .channel_sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
  };
  hdl_adc_channel_source_t mod_adc_channel_1 = {
    .channel_number = HDL_ADC_CHANNEL_1,
    .channel_sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
  };
  hdl_adc_channel_source_t mod_adc_channel_7 = {
    .channel_number = HDL_ADC_CHANNEL_7,
    .channel_sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
  };
  hdl_adc_channel_source_t mod_adc_channel_8 = {
    .channel_number = HDL_ADC_CHANNEL_8,
    .channel_sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
  };
  hdl_adc_t mod_adc = {
    .module.init = &hdl_adc,
    .module.dependencies = hdl_module_dependencies(&mod_clock_irc28m.module, &mod_sys_timer_ms.module, &mod_dma.module),
    .module.reg = (void*)ADC_BASE,
    .start_triger = HDL_ADC_TRIGER_SOFTWARE,
    .mode = ADC_OPERATION_MODE_SINGLE_SCAN,
    .resolution = HDL_ADC_RESOLUTION_12BIT,
    .channel_array = hdl_adc_channel_sequence(&mod_adc_channel_0, &mod_adc_channel_1,
                                                &mod_adc_channel_7, &mod_adc_channel_8)
  };

  hdl_gpio_port_t hdl_gpio_port_a = {
    .init = &hdl_gpio_port,
    .dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .reg = (void *)GPIOA,
  };

  hdl_gpio_port_t hdl_gpio_port_b = {
    .init = &hdl_gpio_port,
    .dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .reg = (void *)GPIOB,
  };

  hdl_gpio_port_t hdl_gpio_port_c = {
    .init = &hdl_gpio_port,
    .dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .reg = (void *)GPIOC,
  };

  hdl_gpio_port_t hdl_gpio_port_f = {
    .init = &hdl_gpio_port,
    .dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .reg = (void *)GPIOF,
  };

  hdl_gpio_mode_t mod_gpio_output_pp_mode = {
    .type = GPIO_MODE_OUTPUT,
    .otype = GPIO_OTYPE_PP,
    .ospeed = GPIO_OSPEED_2MHZ,
  };

  hdl_gpio_mode_t mod_gpio_input_pullup_mode = {
    .type = GPIO_MODE_INPUT,
    .pull = GPIO_PUPD_PULLUP,
  };

  hdl_gpio_mode_t mod_gpio_alternate_swd_mode = {
    .af = GPIO_AF_0,
    .type = GPIO_MODE_AF,
    .otype = GPIO_OTYPE_PP,
    .ospeed = GPIO_OSPEED_50MHZ,
  };

  hdl_gpio_mode_t mod_gpio_i2c_mode = {
    .af = GPIO_AF_4,
    .type = GPIO_MODE_AF,
    .otype = GPIO_OTYPE_OD,
    .ospeed = GPIO_OSPEED_2MHZ
  };

  // const hdl_gpio_t mod_gpi_pmic_sleep_1v8 = {
  //   .port = &hdl_gpio_port_a,
  //   .pin = GPIO_PIN_15,
  //   .mode = &mod_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t mod_gpi_carrier_lid = {
  //   .port = &hdl_gpio_port_b,
  //   .pin = GPIO_PIN_2,
  //   .mode = &mod_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t mod_gpi_carrier_batlow = {
  //   .port = &hdl_gpio_port_b,
  //   .pin = GPIO_PIN_6,
  //   .mode = &mod_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t mod_gpi_carrier_sleep = {
  //   .port = &hdl_gpio_port_b,
  //   .pin = GPIO_PIN_7,
  //   .mode = &mod_gpio_input_pullup_mode
  // };

  hdl_gpio_pin_t mod_gpi_carrier_wdt_time_out = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
    .pin = GPIO_PIN_8,
    .mode = &mod_gpio_output_pp_mode
  };

  // const hdl_gpio_t mod_gpi_carrier_charging = {
  //   .port = &hdl_gpio_port_f,
  //   .pin = GPIO_PIN_0,
  //   .mode = &mod_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t mod_gpi_carrier_charger_prstn = {
  //   .port = &hdl_gpio_port_f,
  //   .pin = GPIO_PIN_1,
  //   .mode = &mod_gpio_input_pullup_mode
  // };

  hdl_gpio_pin_t mod_gpo_emmc_lock = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
    .pin = GPIO_PIN_0,
    .mode = &mod_gpio_output_pp_mode
  };

  hdl_gpio_pin_t mod_gpo_qspi_lock = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
    .pin = GPIO_PIN_1,
    .mode = &mod_gpio_output_pp_mode
  };

  hdl_gpio_pin_t mod_gpo_sd_lock = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
    .pin = GPIO_PIN_2,
    .mode = &mod_gpio_output_pp_mode
  };

  hdl_gpio_pin_t mod_gpi_carrier_force_recovery = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
    .pin = GPIO_PIN_13,
    #if defined(DEBUG)
    .mode = &mod_gpio_alternate_swd_mode
    #else
    .mode = &mod_gpio_input_pullup_mode
    #endif
  };

  hdl_gpio_pin_t mod_gpi_carrier_boot_sel0 = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
    .pin = GPIO_PIN_4,
    .mode = &mod_gpio_input_pullup_mode
  };

  hdl_gpio_pin_t mod_gpi_carrier_boot_sel1 = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
    .pin = GPIO_PIN_5,
    .mode = &mod_gpio_input_pullup_mode
  };

  hdl_gpio_pin_t mod_gpi_carrier_boot_sel2 = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
    .pin = GPIO_PIN_6,
    .mode = &mod_gpio_input_pullup_mode
  };

  hdl_gpio_pin_t mod_gpi_carrier_power_good = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
    .pin = GPIO_PIN_0,
    .mode = &mod_gpio_input_pullup_mode
  };

  hdl_gpio_pin_t mod_gpi_carrier_reset_in = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
    .pin = GPIO_PIN_7,
    .mode = &mod_gpio_input_pullup_mode
  };

  hdl_gpio_pin_t mod_gpo_carrier_reset_out = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
    .pin = GPIO_PIN_8,
    .mode = &mod_gpio_output_pp_mode
  };

  hdl_gpio_pin_t mod_gpo_carrier_pwr_on = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
    .pin = GPIO_PIN_1,
    .mode = &mod_gpio_output_pp_mode
  };

  hdl_gpio_pin_t mod_gpo_carrier_stby = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
    .pin = GPIO_PIN_3,
    .mode = &mod_gpio_output_pp_mode
  };

  hdl_gpio_pin_t mod_gpi_carrier_power_btn = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
    .pin = GPIO_PIN_4,
    .mode = &mod_gpio_input_pullup_mode
  };

  hdl_gpio_pin_t mod_gpo_pmic_soc_rst = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
    .pin = GPIO_PIN_3,
    .mode = &mod_gpio_output_pp_mode
  };

  hdl_gpio_pin_t mod_gpo_pmic_power_on = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b),
    .pin = GPIO_PIN_5,
    .mode = &mod_gpio_output_pp_mode
  };

  hdl_gpio_pin_t mod_gpio_soc_scl = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
    .pin = GPIO_PIN_9,
    .mode = &mod_gpio_i2c_mode
  };

  hdl_gpio_pin_t mod_gpio_soc_sda = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a),
    .pin = GPIO_PIN_10,
    .mode = &mod_gpio_i2c_mode
  };

  hdl_i2c_hw_t mod_i2c_0 = {
    .module.init = &hdl_i2c,
    .module.dependencies = hdl_module_dependencies(&mod_clock_apb1.module, &mod_gpio_soc_scl.module, &mod_gpio_soc_sda.module),
    //.clock = (hdl_clock_t *)&,
    //.rx_dma_ch = &mod_i2c_rx_dma
    //.tx_dma_ch = &mod_i2c_tx_dma
    //.interrupt = &mod_i2c_int
    .reg = I2C0,
    .speed = 100000,
    .dtcy = I2C_DTCY_2,
    .mode = 3,
    .stretching = 1,
    .general_call = 1,
    .addr_size = I2C_ADDFORMAT_7BITS,
    .addr = 0x3F,
  };

#endif

hdl_button_hw_t mod_btn_power = {
  .active_state = HDL_GPIO_LOW,
  .btn_gpio = &mod_gpi_carrier_power_btn,
  .debounce_delay = 50,
  .hold_delay = 5000
};

hdl_button_hw_t mod_btn_reset = {
  .active_state = HDL_GPIO_LOW,
  .btn_gpio = & mod_gpi_carrier_reset_in,
  .debounce_delay = 5,
  .hold_delay = 0 /* never */
};

bldl_som_boot_lock_t mod_boot_lock = {
  .active_state_lock_emmc = HDL_GPIO_HIGH,
  .active_state_lock_qspi = HDL_GPIO_HIGH,
  .active_state_lock_sd = HDL_GPIO_HIGH,
  .lock_pin_emmc = &mod_gpo_emmc_lock,
  .lock_pin_qspi = &mod_gpo_qspi_lock,
  .lock_pin_sd = &mod_gpo_sd_lock
};

bldl_som_boot_sel_t mod_boot_select = {
  .active_state_bs0 = HDL_GPIO_LOW,
  .active_state_bs1 = HDL_GPIO_LOW,
  .active_state_bs2 = HDL_GPIO_LOW,
  .active_state_fr = HDL_GPIO_LOW,
  .gpio_bs0 = &mod_gpi_carrier_boot_sel0,
  .gpio_bs1 = &mod_gpi_carrier_boot_sel1,
  .gpio_bs2 = &mod_gpi_carrier_boot_sel2,
  .gpio_fr = &mod_gpi_carrier_force_recovery
};

bldl_som_power_state_hw_t mod_som_state_ctrl = {
  .active_state_carrier_pwr_on = HDL_GPIO_HIGH,
  .active_state_carrier_stby = HDL_GPIO_LOW,
  .active_state_reset_out = HDL_GPIO_LOW,
  .active_state_power_good = HDL_GPIO_HIGH,
  .active_state_pmic_power_on = HDL_GPIO_HIGH,
  .active_state_pmic_soc_rst = HDL_GPIO_HIGH,
  .active_state_carrier_wdt = HDL_GPIO_LOW,
  .carrier_wdt = &mod_gpi_carrier_wdt_time_out,
  .carrier_pwr_on = &mod_gpo_carrier_pwr_on,
  .carrier_stby = &mod_gpo_carrier_stby,
  .reset_out = &mod_gpo_carrier_reset_out,
  .power_good = &mod_gpi_carrier_power_good,
  .pmic_power_on = &mod_gpo_pmic_power_on,
  .pmic_soc_rst = &mod_gpo_pmic_soc_rst,
  .reset_in = &mod_btn_reset,
  .boot_lock = &mod_boot_lock,
  .bootsel = &mod_boot_select
};
