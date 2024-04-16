
#if defined ( ATB_RK3568J_SMC_R3 )
  const hdl_sys_t mod_sys_cnf = {

  };

  // const hdl_gpio_t mod_gpi_pmic_sleep_1v8 = {
  // };

  // const hdl_gpio_t mod_gpi_carrier_lid = {
  // };

  // const hdl_gpio_t mod_gpi_carrier_batlow = {
  // };

  // const hdl_gpio_t mod_gpi_carrier_sleep = {
  // };

  // const hdl_gpio_t mod_gpi_carrier_wdt_time_out = {
  // };

  // const hdl_gpio_t mod_gpi_carrier_charging = {
  // };

  // const hdl_gpio_t mod_gpi_carrier_charger_prstn = {
  // };


  const hdl_gpio_t mod_gpo_emmc_lock = {
    .port = PH,
    .pin = GPIO_PIN_PIN4_Msk,
    .mode = GPIO_MODE_OUTPUT,
  };

  const hdl_gpio_t mod_gpo_qspi_lock = {
    .port = PH,
    .pin = GPIO_PIN_PIN5_Msk,
    .mode = GPIO_MODE_OUTPUT,
  };

  const hdl_gpio_t mod_gpo_sd_lock = {
    .port = PH,
    .pin = GPIO_PIN_PIN6_Msk,
    .mode = GPIO_MODE_OUTPUT,
  };

  const hdl_gpio_t mod_gpi_carrier_boot_sel0 = {
    .port = PE,
    .pin = GPIO_PIN_PIN6_Msk,
    .mode = GPIO_MODE_INPUT,
    .pull_mode = GPIO_PUSEL_DISABLE
  };

  const hdl_gpio_t mod_gpi_carrier_boot_sel1 = {
    .port = PE,
    .pin = GPIO_PIN_PIN4_Msk,
    .mode = GPIO_MODE_INPUT,
    .pull_mode = GPIO_PUSEL_DISABLE
  };

  const hdl_gpio_t mod_gpi_carrier_boot_sel2 = {
    .port = PE,
    .pin = GPIO_PIN_PIN2_Msk,
    .mode = GPIO_MODE_INPUT,
    .pull_mode = GPIO_PUSEL_DISABLE
  };

  const hdl_gpio_t mod_gpi_carrier_force_recovery = {
    .port = PE,
    .pin = GPIO_PIN_PIN0_Msk,
    .mode = GPIO_MODE_INPUT,
    .pull_mode = GPIO_PUSEL_PULL_UP
  };

  const hdl_gpio_t mod_gpi_carrier_reset_in = {
    .port = PE,
    .pin = GPIO_PIN_PIN9_Msk,
    .mode = GPIO_MODE_INPUT,
    .pull_mode = GPIO_PUSEL_DISABLE
  };

  const hdl_gpio_t mod_gpi_carrier_power_btn = {
    .port = PE,
    .pin = GPIO_PIN_PIN8_Msk,
    .mode = GPIO_MODE_INPUT,
    .pull_mode = GPIO_PUSEL_DISABLE
  };

  const hdl_gpio_t mod_gpi_carrier_power_good = {
    .port = PE,
    .pin = GPIO_PIN_PIN10_Msk,
    .mode = GPIO_MODE_INPUT,
    .pull_mode = GPIO_PUSEL_DISABLE
  };





  const hdl_gpio_t mod_gpo_carrier_pwr_on = {
    .port = PA,
    .pin = GPIO_PIN_PIN0_Msk,
    .mode = GPIO_MODE_OUTPUT,
    .slew_mode = GPIO_SLEWCTL_NORMAL,
  };
  const hdl_gpio_t mod_gpo_carrier_stby = {
    .port = PA,
    .pin = GPIO_PIN_PIN0_Msk,
    .mode = GPIO_MODE_OUTPUT,
    .slew_mode = GPIO_SLEWCTL_NORMAL,
  };
  const hdl_gpio_t mod_gpo_carrier_reset_out = {
    .port = PA,
    .pin = GPIO_PIN_PIN0_Msk,
    .mode = GPIO_MODE_OUTPUT,
    .slew_mode = GPIO_SLEWCTL_NORMAL,
  };
  const hdl_gpio_t mod_gpo_pmic_power_on = {
    .port = PA,
    .pin = GPIO_PIN_PIN0_Msk,
    .mode = GPIO_MODE_OUTPUT,
    .slew_mode = GPIO_SLEWCTL_NORMAL,
  };
  const hdl_gpio_t mod_gpo_pmic_soc_rst = {
    .port = PA,
    .pin = GPIO_PIN_PIN0_Msk,
    .mode = GPIO_MODE_OUTPUT,
    .slew_mode = GPIO_SLEWCTL_NORMAL,
  };
#endif
