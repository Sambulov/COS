#ifndef MODULES_H_
#define MODULES_H_

  extern hdl_timer_t mod_timer_ms;
  extern hdl_clock_counter_t mod_timer0_counter;

  // extern const hdl_gpio_pin_t mod_gpi_pmic_sleep_1v8;
  // extern const hdl_gpio_pin_t mod_gpi_carrier_lid;
  // extern const hdl_gpio_pin_t mod_gpi_carrier_batlow;
  // extern const hdl_gpio_pin_t mod_gpi_carrier_sleep;
  // extern const hdl_gpio_pin_t mod_gpi_carrier_wdt_time_out;
  // extern const hdl_gpio_pin_t mod_gpi_carrier_charging;
  // extern const hdl_gpio_pin_t mod_gpi_carrier_charger_prstn;

  extern hdl_gpio_pin_t mod_gpo_emmc_lock;
  extern hdl_gpio_pin_t mod_gpo_qspi_lock;
  extern hdl_gpio_pin_t mod_gpo_sd_lock;
  extern hdl_gpio_pin_t mod_gpi_carrier_force_recovery;
  extern hdl_gpio_pin_t mod_gpi_carrier_boot_sel0;
  extern hdl_gpio_pin_t mod_gpi_carrier_boot_sel1;
  extern hdl_gpio_pin_t mod_gpi_carrier_boot_sel2;
  extern hdl_gpio_pin_t mod_gpi_carrier_reset_in;
  extern hdl_gpio_pin_t mod_gpo_carrier_reset_out;
  extern hdl_gpio_pin_t mod_gpi_carrier_power_good;
  extern hdl_gpio_pin_t mod_gpo_carrier_pwr_on;
  extern hdl_gpio_pin_t mod_gpo_carrier_stby;
  extern hdl_gpio_pin_t mod_gpi_carrier_power_btn;
  extern hdl_gpio_pin_t mod_gpo_pmic_soc_rst;
  extern hdl_gpio_pin_t mod_gpo_pmic_power_on;
  extern hdl_gpio_pin_t mod_gpio_soc_scl;
  extern hdl_gpio_pin_t mod_gpio_soc_sda;

  extern hdl_gpio_pin_t mod_gpio_adc_channel_1v5;
  extern hdl_gpio_pin_t mod_gpio_adc_channel_3v3;

  extern hdl_i2c_hw_t mod_i2c_0;

  extern bldl_som_boot_lock_t mod_boot_lock;

  extern bldl_som_boot_sel_t mod_boot_select;

  extern bldl_som_power_state_hw_t mod_som_state_ctrl;

#endif // MODULES_H_
