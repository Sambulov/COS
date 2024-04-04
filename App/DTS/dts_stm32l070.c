
#if defined(ATB_RK3568J_SMC_R0)
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


  const hdl_gpio_t dts_gpi_carrier_wdt_time_out = {
    .port = GPIOA,
    .pin = GPIO_PIN_12,
    .mode = &dts_gpio_output_pp_mode
  };

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
