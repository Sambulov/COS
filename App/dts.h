#ifndef DTS_H_
#define DTS_H_

  extern const hdl_sys_t dts_sys_cnf;

  // extern const hdl_gpio_t dts_gpi_pmic_sleep_1v8;
  // extern const hdl_gpio_t dts_gpi_carrier_lid;
  // extern const hdl_gpio_t dts_gpi_carrier_batlow;
  // extern const hdl_gpio_t dts_gpi_carrier_sleep;
  // extern const hdl_gpio_t dts_gpi_carrier_wdt_time_out;
  // extern const hdl_gpio_t dts_gpi_carrier_charging;
  // extern const hdl_gpio_t dts_gpi_carrier_charger_prstn;

  extern const hdl_gpio_t dts_gpo_emmc_lock;
  extern const hdl_gpio_t dts_gpo_qspi_lock;
  extern const hdl_gpio_t dts_gpo_sd_lock;
  extern const hdl_gpio_t dts_gpi_carrier_force_recovery;
  extern const hdl_gpio_t dts_gpi_carrier_boot_sel0;
  extern const hdl_gpio_t dts_gpi_carrier_boot_sel1;
  extern const hdl_gpio_t dts_gpi_carrier_boot_sel2;
  extern const hdl_gpio_t dts_gpi_carrier_reset_in;
  extern const hdl_gpio_t dts_gpo_carrier_reset_out;
  extern const hdl_gpio_t dts_gpi_carrier_power_good;
  extern const hdl_gpio_t dts_gpo_carrier_pwr_on;
  extern const hdl_gpio_t dts_gpo_carrier_stby;
  extern const hdl_gpio_t dts_gpi_carrier_power_btn;
  extern const hdl_gpio_t dts_gpo_pmic_soc_rst;
  extern const hdl_gpio_t dts_gpo_pmic_power_on;
  extern const hdl_gpio_t dts_gpio_soc_scl;
  extern const hdl_gpio_t dts_gpio_soc_sda;

  extern const hdl_button_hw_t dts_btn_power;
  extern const hdl_button_hw_t dts_btn_reset;

  extern const hdl_i2c_hw_t dts_i2c_0;

  extern const bldl_som_boot_lock_t dts_boot_lock;

  extern const bldl_som_boot_sel_t dts_boot_select;

  extern const bldl_som_power_state_hw_t dts_som_state_ctrl;

#endif // DTS_H_
