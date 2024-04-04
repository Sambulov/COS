#include "bldl.h"

#if defined ( ATB_RK3568J_SMC_R1 )
  const hdl_sys_t dts_sys_cnf = {
    .ticks_per_ms = 72000000 / 1000,
  };

  #define HDL_GD_HXTAL_CLOCK              8000000
  #define HDL_GD_LXTAL_CLOCK              32768
  #define HDL_GD_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_GD_PLLMUL                   9
  #define HDL_GD_AHB_PREDIV               1
  #define HDL_GD_APB1_PREDIV              1
  #define HDL_GD_APB2_PREDIV              1
  #define HDL_GD_RTC_CLOCK                dts_clock_irc40k /* Can be clocked by: dts_clock_lxtal, dts_clock_irc40k, dts_clock_hxtal. For dts_clock_hxtal applied prediv 32 */
  #define HDL_GD_PLL_MUL_CLOCK            dts_clock_hxtal2pllsel /* Can be clocked by: dts_clock_hxtal2pllsel, dts_clock_irc8m. For dts_clock_irc8m applied prediv 2 */
  #define HDL_GD_SYS_CLOCK                dts_clock_pll /* Can be clocked by: dts_clock_pll, dts_clock_irc8m, dts_clock_hxtal */

  _Static_assert((HDL_GD_HXTAL_CLOCK >= 4000000) && (HDL_GD_HXTAL_CLOCK <= 32000000), "HDL_GD_HXTAL_CLOCK mast be from 4 to 32 MHz");
  _Static_assert((HDL_GD_LXTAL_CLOCK >= 32000) && (HDL_GD_LXTAL_CLOCK <= 1000000), "HDL_GD_LXTAL_CLOCK mast be from 32KHz to 1MHz");
  _Static_assert((HDL_GD_HXTAL_2_PLLSEL_PREDIV >= 1) && (HDL_GD_HXTAL_2_PLLSEL_PREDIV <= 16), "HDL_GD_HXTAL_2_PLLSEL_PREDIV mast be within 1-16");
  _Static_assert((HDL_GD_PLLMUL >= 2) && (HDL_GD_PLLMUL <= 32), "HDL_GD_PLLMUL mast be within 2-32");
  _Static_assert((BITS_COUNT_U16(HDL_GD_AHB_PREDIV) == 1) && (HDL_GD_PLLMUL <= 512), "HDL_GD_AHB_PREDIV mast be 1, 2, 4, 8, 16, 128, 256 or 512");
  _Static_assert((BITS_COUNT_U16(HDL_GD_APB1_PREDIV) == 1) && (HDL_GD_APB1_PREDIV <= 16), "HDL_GD_APB1_PREDIV mast be 1, 2, 4, 8 or 16");
  _Static_assert((BITS_COUNT_U16(HDL_GD_APB2_PREDIV) == 1) && (HDL_GD_APB2_PREDIV <= 16), "HDL_GD_APB2_PREDIV mast be 1, 2, 4, 8 or 16");

  const hdl_clock_t dts_clock_hxtal = {
    .hw.init = &hdl_gd_clock_hxtal,
    .freq =  HDL_GD_HXTAL_CLOCK
  };

  const hdl_clock_t dts_clock_lxtal = {
    .hw.init = &hdl_gd_clock_lxtal,
    .freq = HDL_GD_LXTAL_CLOCK
  };

  const hdl_clock_t dts_clock_irc40k = {
    .hw.init = &hdl_gd_clock_irc40k,
    .freq = 40000
  };

  const hdl_clock_t dts_clock_irc8m = {
    .hw.init = &hdl_gd_clock_irc8m,
    .freq = 8000000
  };

  hdl_clock_selector_t dts_clock_rtc = {
    .source = (hdl_clock_t *)&HDL_GD_RTC_CLOCK,
    .clock.hw.init = &hdl_gd_clock_rtc,
    .clock.freq = 0,                     /* calculated while initialization */
  };

  const hdl_clock_t dts_clock_irc28m = {
    .hw.init = &hdl_gd_clock_irc28m,
    .freq = 28000000
  };

  hdl_clock_prescaler_t dts_clock_hxtal2pllsel = {
    .source = (hdl_clock_t *)&dts_clock_hxtal,
    .clock.hw.init = &hdl_gd_clock_hxtal_pll,
    .muldiv_factor = HDL_GD_HXTAL_2_PLLSEL_PREDIV,
    .clock.freq = 0,                     /* calculated while initialization */
  };

  hdl_clock_selector_t dts_clock_pllsel2mul = {
    .source = (hdl_clock_t *)&HDL_GD_PLL_MUL_CLOCK,
    .clock.hw.init = &hdl_gd_clock_pll_mf,
    .clock.freq = 0,                     /* calculated while initialization */
  };

  hdl_clock_prescaler_t dts_clock_pll = {
    .source = (hdl_clock_t *)&dts_clock_pllsel2mul,
    .clock.hw.init = hdl_gd_clock_pll,
    .muldiv_factor = HDL_GD_PLLMUL,
    .clock.freq = 0,                     /* calculated while initialization */
  };

  hdl_sys_clock_t dts_clock_sys = {
    .source = (hdl_clock_t *)&HDL_GD_SYS_CLOCK,
    .clock.hw.init = &hdl_gd_clock_sys,
    .clock_monitor_enable = 0,
    .clock.freq = 0,
  };

  hdl_clock_prescaler_t dts_clock_ahb = {
    .source = (hdl_clock_t *)&dts_clock_sys,
    .clock.hw.init = hdl_gd_clock_ahb,
    .clock.freq = 0,                     /* calculated while initialization */
    .muldiv_factor = HDL_GD_AHB_PREDIV
  };

  hdl_clock_prescaler_t dts_clock_apb1 = {
    .source = (hdl_clock_t *)&dts_clock_ahb,
    .clock.hw.init = hdl_gd_clock_apb1,
    .clock.freq = 0,                     /* calculated while initialization */
    .muldiv_factor = HDL_GD_APB1_PREDIV
  };

  hdl_clock_prescaler_t dts_clock_apb2 = {
    .source = (hdl_clock_t *)&dts_clock_ahb,
    .clock.hw.init = hdl_gd_clock_apb2,
    .clock.freq = 0,                     /* calculated while initialization */
    .muldiv_factor = HDL_GD_APB2_PREDIV
  };

  const hdl_gpio_port_t hdl_gpio_port_a = {
    .periph = GPIOA,
    .clock = (hdl_clock_t *)&dts_clock_ahb
  };

  const hdl_gpio_port_t hdl_gpio_port_b = {
    .periph = GPIOB,
    .clock = (hdl_clock_t *)&dts_clock_ahb
  };

  const hdl_gpio_port_t hdl_gpio_port_c = {
    .periph = GPIOC,
    .clock = (hdl_clock_t *)&dts_clock_ahb
  };

  const hdl_gpio_port_t hdl_gpio_port_f = {
    .periph = GPIOF,
    .clock = (hdl_clock_t *)&dts_clock_ahb
  };

  const hdl_gpio_mode_t dts_gpio_output_pp_mode = {
    .type = GPIO_MODE_OUTPUT,
    .otype = GPIO_OTYPE_PP,
    .ospeed = GPIO_OSPEED_2MHZ,
  };

  const hdl_gpio_mode_t dts_gpio_input_pullup_mode = {
    .type = GPIO_MODE_INPUT,
    .pull = GPIO_PUPD_PULLUP,
  };

  const hdl_gpio_mode_t dts_gpio_swd_mode = {
    .af = GPIO_AF_0,
    .type = GPIO_MODE_AF,
    .otype = GPIO_OTYPE_PP,
    .ospeed = GPIO_OSPEED_50MHZ,
  };

  const hdl_gpio_mode_t dts_gpio_i2c_mode = {
    .af = GPIO_AF_4,
    .type = GPIO_MODE_AF,
    .otype = GPIO_OTYPE_OD,
    .ospeed = GPIO_OSPEED_2MHZ
  };

  // const hdl_gpio_t dts_gpi_pmic_sleep_1v8 = {
  //   .port = &hdl_gpio_port_a,
  //   .pin = GPIO_PIN_15,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_lid = {
  //   .port = &hdl_gpio_port_b,
  //   .pin = GPIO_PIN_2,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_batlow = {
  //   .port = &hdl_gpio_port_b,
  //   .pin = GPIO_PIN_6,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_sleep = {
  //   .port = &hdl_gpio_port_b,
  //   .pin = GPIO_PIN_7,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  const hdl_gpio_t dts_gpi_carrier_wdt_time_out = {
    .port = &hdl_gpio_port_b,
    .pin = GPIO_PIN_8,
    .mode = &dts_gpio_output_pp_mode
  };

  // const hdl_gpio_t dts_gpi_carrier_charging = {
  //   .port = &hdl_gpio_port_f,
  //   .pin = GPIO_PIN_0,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_charger_prstn = {
  //   .port = &hdl_gpio_port_f,
  //   .pin = GPIO_PIN_1,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  const hdl_gpio_t dts_gpo_emmc_lock = {
    .port = &hdl_gpio_port_a,
    .pin = GPIO_PIN_0,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_qspi_lock = {
    .port = &hdl_gpio_port_a,
    .pin = GPIO_PIN_1,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_sd_lock = {
    .port = &hdl_gpio_port_a,
    .pin = GPIO_PIN_2,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpi_carrier_force_recovery = {
    .port = &hdl_gpio_port_a,
    .pin = GPIO_PIN_13,
    #if defined(DEBUG)
    .mode = &dts_gpio_swd_mode
    #else
    .mode = &dts_gpio_input_pullup_mode
    #endif
  };

  const hdl_gpio_t dts_gpi_carrier_boot_sel0 = {
    .port = &hdl_gpio_port_a,
    .pin = GPIO_PIN_4,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpi_carrier_boot_sel1 = {
    .port = &hdl_gpio_port_a,
    .pin = GPIO_PIN_5,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpi_carrier_boot_sel2 = {
    .port = &hdl_gpio_port_a,
    .pin = GPIO_PIN_6,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpi_carrier_power_good = {
    .port = &hdl_gpio_port_b,
    .pin = GPIO_PIN_0,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpi_carrier_reset_in = {
    .port = &hdl_gpio_port_a,
    .pin = GPIO_PIN_7,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpo_carrier_reset_out = {
    .port = &hdl_gpio_port_a,
    .pin = GPIO_PIN_8,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_carrier_pwr_on = {
    .port = &hdl_gpio_port_b,
    .pin = GPIO_PIN_1,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_carrier_stby = {
    .port = &hdl_gpio_port_b,
    .pin = GPIO_PIN_3,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpi_carrier_power_btn = {
    .port = &hdl_gpio_port_b,
    .pin = GPIO_PIN_4,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpo_pmic_soc_rst = {
    .port = &hdl_gpio_port_a,
    .pin = GPIO_PIN_3,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_pmic_power_on = {
    .port = &hdl_gpio_port_b,
    .pin = GPIO_PIN_5,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpio_soc_scl = {
    .port = &hdl_gpio_port_a,
    .pin = GPIO_PIN_9,
    .mode = &dts_gpio_i2c_mode
  };

  const hdl_gpio_t dts_gpio_soc_sda = {
    .port = &hdl_gpio_port_a,
    .pin = GPIO_PIN_10,
    .mode = &dts_gpio_i2c_mode
  };

  const hdl_i2c_hw_t dts_i2c_0 = {
    
    //.clock = (hdl_clock_t *)&dts_clock_apb1,
    .scl = &dts_gpio_soc_scl,
    .sda = &dts_gpio_soc_sda,
    //.rx_dma_ch = &dts_i2c_rx_dma
    //.tx_dma_ch = &dts_i2c_tx_dma
    //.interrupt = &dts_i2c_int
    //.
    .periph = I2C0,
    .speed = 100000,
    .dtcy = I2C_DTCY_2,
    .mode = 3,
    .stretching = 1,
    .general_call = 1,
    .addr_size = I2C_ADDFORMAT_7BITS,
    .addr = 0x3F,
  };

#include "dts_app.c"

#endif

const hdl_button_hw_t dts_btn_power = {
  .active_state = HDL_GPIO_LOW,
  .btn_gpio = &dts_gpi_carrier_power_btn,
  .debounce_delay = 50,
  .hold_delay = 5000
};

const hdl_button_hw_t dts_btn_reset = {
  .active_state = HDL_GPIO_LOW,
  .btn_gpio = & dts_gpi_carrier_reset_in,
  .debounce_delay = 5,
  .hold_delay = 0 /* never */
};

const bldl_som_boot_lock_t dts_boot_lock = {
  .active_state_lock_emmc = HDL_GPIO_HIGH,
  .active_state_lock_qspi = HDL_GPIO_HIGH,
  .active_state_lock_sd = HDL_GPIO_HIGH,
  .lock_pin_emmc = &dts_gpo_emmc_lock,
  .lock_pin_qspi = &dts_gpo_qspi_lock,
  .lock_pin_sd = &dts_gpo_sd_lock
};

const bldl_som_boot_sel_t dts_boot_select = {
  .active_state_bs0 = HDL_GPIO_LOW,
  .active_state_bs1 = HDL_GPIO_LOW,
  .active_state_bs2 = HDL_GPIO_LOW,
  .active_state_fr = HDL_GPIO_LOW,
  .gpio_bs0 = &dts_gpi_carrier_boot_sel0,
  .gpio_bs1 = &dts_gpi_carrier_boot_sel1,
  .gpio_bs2 = &dts_gpi_carrier_boot_sel2,
  .gpio_fr = &dts_gpi_carrier_force_recovery
};

const bldl_som_power_state_hw_t dts_som_state_ctrl = {
  .active_state_carrier_pwr_on = HDL_GPIO_HIGH,
  .active_state_carrier_stby = HDL_GPIO_LOW,
  .active_state_reset_out = HDL_GPIO_LOW,
  .active_state_power_good = HDL_GPIO_HIGH,
  .active_state_pmic_power_on = HDL_GPIO_HIGH,
  .active_state_pmic_soc_rst = HDL_GPIO_HIGH,
  .active_state_carrier_wdt = HDL_GPIO_LOW,
  .carrier_wdt = &dts_gpi_carrier_wdt_time_out,
  .carrier_pwr_on = &dts_gpo_carrier_pwr_on,
  .carrier_stby = &dts_gpo_carrier_stby,
  .reset_out = &dts_gpo_carrier_reset_out,
  .power_good = &dts_gpi_carrier_power_good,
  .pmic_power_on = &dts_gpo_pmic_power_on,
  .pmic_soc_rst = &dts_gpo_pmic_soc_rst,
  .reset_in = &dts_btn_reset,
  .boot_lock = &dts_boot_lock,
  .bootsel = &dts_boot_select
};
