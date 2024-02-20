#include "bldl.h"

#if defined ( NUM46X )
  const hdl_sys_t dts_sys_cnf = {

  };

  // const hdl_gpio_t dts_gpi_pmic_sleep_1v8 = {
  // };

  // const hdl_gpio_t dts_gpi_carrier_lid = {
  // };

  // const hdl_gpio_t dts_gpi_carrier_batlow = {
  // };

  // const hdl_gpio_t dts_gpi_carrier_sleep = {
  // };

  // const hdl_gpio_t dts_gpi_carrier_wdt_time_out = {
  // };

  // const hdl_gpio_t dts_gpi_carrier_charging = {
  // };

  // const hdl_gpio_t dts_gpi_carrier_charger_prstn = {
  // };


  const hdl_gpio_t dts_gpo_emmc_lock = {
    .port = PH,
    .pin = GPIO_PIN_PIN4_Msk,
    .mode = GPIO_MODE_OUTPUT,
  };

  const hdl_gpio_t dts_gpo_qspi_lock = {
    .port = PH,
    .pin = GPIO_PIN_PIN5_Msk,
    .mode = GPIO_MODE_OUTPUT,
  };

  const hdl_gpio_t dts_gpo_sd_lock = {
    .port = PH,
    .pin = GPIO_PIN_PIN6_Msk,
    .mode = GPIO_MODE_OUTPUT,
  };

  const hdl_gpio_t dts_gpi_carrier_boot_sel0 = {
    .port = PE,
    .pin = GPIO_PIN_PIN6_Msk,
    .mode = GPIO_MODE_INPUT,
    .pull_mode = GPIO_PUSEL_DISABLE
  };

  const hdl_gpio_t dts_gpi_carrier_boot_sel1 = {
    .port = PE,
    .pin = GPIO_PIN_PIN4_Msk,
    .mode = GPIO_MODE_INPUT,
    .pull_mode = GPIO_PUSEL_DISABLE
  };

  const hdl_gpio_t dts_gpi_carrier_boot_sel2 = {
    .port = PE,
    .pin = GPIO_PIN_PIN2_Msk,
    .mode = GPIO_MODE_INPUT,
    .pull_mode = GPIO_PUSEL_DISABLE
  };

  const hdl_gpio_t dts_gpi_carrier_force_recovery = {
    .port = PE,
    .pin = GPIO_PIN_PIN0_Msk,
    .mode = GPIO_MODE_INPUT,
    .pull_mode = GPIO_PUSEL_PULL_UP
  };

  const hdl_gpio_t dts_gpi_carrier_reset_in = {
    .port = PE,
    .pin = GPIO_PIN_PIN9_Msk,
    .mode = GPIO_MODE_INPUT,
    .pull_mode = GPIO_PUSEL_DISABLE
  };

  const hdl_gpio_t dts_gpi_carrier_power_btn = {
    .port = PE,
    .pin = GPIO_PIN_PIN8_Msk,
    .mode = GPIO_MODE_INPUT,
    .pull_mode = GPIO_PUSEL_DISABLE
  };

  const hdl_gpio_t dts_gpi_carrier_power_good = {
    .port = PE,
    .pin = GPIO_PIN_PIN10_Msk,
    .mode = GPIO_MODE_INPUT,
    .pull_mode = GPIO_PUSEL_DISABLE
  };





  const hdl_gpio_t dts_gpo_carrier_pwr_on = {
    .port = PA,
    .pin = GPIO_PIN_PIN0_Msk,
    .mode = GPIO_MODE_OUTPUT,
    .slew_mode = GPIO_SLEWCTL_NORMAL,
  };
  const hdl_gpio_t dts_gpo_carrier_stby = {
    .port = PA,
    .pin = GPIO_PIN_PIN0_Msk,
    .mode = GPIO_MODE_OUTPUT,
    .slew_mode = GPIO_SLEWCTL_NORMAL,
  };
  const hdl_gpio_t dts_gpo_carrier_reset_out = {
    .port = PA,
    .pin = GPIO_PIN_PIN0_Msk,
    .mode = GPIO_MODE_OUTPUT,
    .slew_mode = GPIO_SLEWCTL_NORMAL,
  };
  const hdl_gpio_t dts_gpo_pmic_power_on = {
    .port = PA,
    .pin = GPIO_PIN_PIN0_Msk,
    .mode = GPIO_MODE_OUTPUT,
    .slew_mode = GPIO_SLEWCTL_NORMAL,
  };
  const hdl_gpio_t dts_gpo_pmic_soc_rst = {
    .port = PA,
    .pin = GPIO_PIN_PIN0_Msk,
    .mode = GPIO_MODE_OUTPUT,
    .slew_mode = GPIO_SLEWCTL_NORMAL,
  };
#endif

#if defined ( GD32E23X )
  const hdl_sys_t dts_sys_cnf = {
    .ticks_per_ms = 72000000 / 1000,
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

  hdl_gpio_mode_t dts_gpio_swd_mode = {
    .af = GPIO_AF_0,
    .type = GPIO_MODE_AF,
    .otype = GPIO_OTYPE_PP,
    .ospeed = GPIO_OSPEED_50MHZ,
  };

  hdl_gpio_mode_t dts_gpio_i2c_mode = {
    .af = GPIO_AF_4,
    .type = GPIO_MODE_AF,
    .otype = GPIO_OTYPE_OD,
    .ospeed = GPIO_OSPEED_2MHZ
  };

  // const hdl_gpio_t dts_gpi_pmic_sleep_1v8 = {
  //   .port = GPIOA,
  //   .pin = GPIO_PIN_15,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_lid = {
  //   .port = GPIOB,
  //   .pin = GPIO_PIN_2,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_batlow = {
  //   .port = GPIOB,
  //   .pin = GPIO_PIN_6,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_sleep = {
  //   .port = GPIOB,
  //   .pin = GPIO_PIN_7,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_wdt_time_out = {
  //   .port = GPIOB,
  //   .pin = GPIO_PIN_8,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_charging = {
  //   .port = GPIOF,
  //   .pin = GPIO_PIN_0,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_charger_prstn = {
  //   .port = GPIOF,
  //   .pin = GPIO_PIN_1,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  const hdl_gpio_t dts_gpo_emmc_lock = {
    .port = GPIOA,
    .pin = GPIO_PIN_0,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_qspi_lock = {
    .port = GPIOA,
    .pin = GPIO_PIN_1,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_sd_lock = {
    .port = GPIOA,
    .pin = GPIO_PIN_2,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpi_carrier_force_recovery = {
    .port = GPIOA,
    .pin = GPIO_PIN_13,
    #if defined(DEBUG)
    .mode = &dts_gpio_swd_mode
    #else
    .mode = &dts_gpio_input_pullup_mode
    #endif
  };

  const hdl_gpio_t dts_gpi_carrier_boot_sel0 = {
    .port = GPIOA,
    .pin = GPIO_PIN_4,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpi_carrier_boot_sel1 = {
    .port = GPIOA,
    .pin = GPIO_PIN_5,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpi_carrier_boot_sel2 = {
    .port = GPIOA,
    .pin = GPIO_PIN_6,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpi_carrier_power_good = {
    .port = GPIOB,
    .pin = GPIO_PIN_0,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpi_carrier_reset_in = {
    .port = GPIOA,
    .pin = GPIO_PIN_7,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpo_carrier_reset_out = {
    .port = GPIOA,
    .pin = GPIO_PIN_8,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_carrier_pwr_on = {
    .port = GPIOB,
    .pin = GPIO_PIN_1,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_carrier_stby = {
    .port = GPIOB,
    .pin = GPIO_PIN_3,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpi_carrier_power_btn = {
    .port = GPIOB,
    .pin = GPIO_PIN_4,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpo_pmic_soc_rst = {
    .port = GPIOA,
    .pin = GPIO_PIN_3,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_pmic_power_on = {
    .port = GPIOB,
    .pin = GPIO_PIN_5,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpio_soc_scl = {
    .port = GPIOA,
    .pin = GPIO_PIN_9,
    .mode = &dts_gpio_i2c_mode
  };

  const hdl_gpio_t dts_gpio_soc_sda = {
    .port = GPIOA,
    .pin = GPIO_PIN_10,
    .mode = &dts_gpio_i2c_mode
  };

  // const hdl_i2c_t dts_i2c_0 = {
  //   .scl = &scl,
  //   .sda = &sda,
  //   .RCU_I2CX = RCU_I2C0,
  //   .I2CX = I2C0,
  //   .speed = 100000,
  //   .dtcy = I2C_DTCY_2,
  //   .is_slave = 1,
  //   .slave_addr = 0x3F,
  // };

#endif

#if defined(STM32L0XX)
  const hdl_sys_t dts_sys_cnf = {

  };

  const hdl_gpio_mode_t dts_gpio_output_pp_mode = {
    .type = GPIO_MODE_OUTPUT_PP,
    .ospeed = GPIO_SPEED_FREQ_MEDIUM,
  };

  const hdl_gpio_mode_t dts_gpio_input_pullup_mode = {
    .pull = GPIO_PULLUP,
    .type = GPIO_MODE_INPUT,
  };

  hdl_gpio_mode_t dts_gpio_swd_mode = {
    .af = GPIO_AF0_SWDIO,
    .type = GPIO_MODE_AF_PP,
    .ospeed = GPIO_SPEED_FREQ_VERY_HIGH,
  };

  hdl_gpio_mode_t dts_gpio_i2c_mode = {
    .af = GPIO_AF6_I2C1,
    .type = GPIO_MODE_AF_OD,
    .ospeed = GPIO_SPEED_FREQ_MEDIUM
  };

  // const hdl_gpio_t dts_gpi_pmic_sleep_1v8 = {
  //   .port = GPIOA,
  //   .pin = GPIO_PIN_15,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_lid = {
  //   .port = GPIOB,
  //   .pin = GPIO_PIN_2,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_batlow = {
  //   .port = GPIOB,
  //   .pin = GPIO_PIN_6,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_sleep = {
  //   .port = GPIOB,
  //   .pin = GPIO_PIN_7,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_wdt_time_out = {
  //   .port = GPIOB,
  //   .pin = GPIO_PIN_8,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_charging = {
  //   .port = GPIOF,
  //   .pin = GPIO_PIN_0,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_charger_prstn = {
  //   .port = GPIOF,
  //   .pin = GPIO_PIN_1,
  //   .mode = &dts_gpio_input_pullup_mode
  // };

  const hdl_gpio_t dts_gpo_emmc_lock = {
    .port = GPIOA,
    .pin = GPIO_PIN_0,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_qspi_lock = {
    .port = GPIOA,
    .pin = GPIO_PIN_1,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_sd_lock = {
    .port = GPIOA,
    .pin = GPIO_PIN_2,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpi_carrier_force_recovery = {
    .port = GPIOA,
    .pin = GPIO_PIN_13,
    #if defined(DEBUG)
    .mode = &dts_gpio_swd_mode
    #else
    .mode = &dts_gpio_input_pullup_mode
    #endif
  };

  const hdl_gpio_t dts_gpi_carrier_boot_sel0 = {
    .port = GPIOA,
    .pin = GPIO_PIN_4,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpi_carrier_boot_sel1 = {
    .port = GPIOA,
    .pin = GPIO_PIN_5,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpi_carrier_boot_sel2 = {
    .port = GPIOA,
    .pin = GPIO_PIN_6,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpi_carrier_power_good = {
    .port = GPIOB,
    .pin = GPIO_PIN_0,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpi_carrier_reset_in = {
    .port = GPIOA,
    .pin = GPIO_PIN_7,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpo_carrier_reset_out = {
    .port = GPIOA,
    .pin = GPIO_PIN_8,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_carrier_pwr_on = {
    .port = GPIOB,
    .pin = GPIO_PIN_1,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_carrier_stby = {
    .port = GPIOB,
    .pin = GPIO_PIN_4,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpi_carrier_power_btn = {
    .port = GPIOB,
    .pin = GPIO_PIN_5,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpo_pmic_soc_rst = {
    .port = GPIOA,
    .pin = GPIO_PIN_3,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_pmic_power_on = {
    .port = GPIOB,
    .pin = GPIO_PIN_6,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpio_soc_scl = {
    .port = GPIOA,
    .pin = GPIO_PIN_9,
    .mode = &dts_gpio_i2c_mode
  };

  const hdl_gpio_t dts_gpio_soc_sda = {
    .port = GPIOA,
    .pin = GPIO_PIN_10,
    .mode = &dts_gpio_i2c_mode
  };

  // const hdl_i2c_t dts_i2c_0 = {
  //   .scl = &scl,
  //   .sda = &sda,
  //   .RCU_I2CX = RCU_I2C0,
  //   .I2CX = I2C0,
  //   .speed = 100000,
  //   .dtcy = I2C_DTCY_2,
  //   .is_slave = 1,
  //   .slave_addr = 0x3F,
  // };

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
