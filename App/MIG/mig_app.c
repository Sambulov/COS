#include "bldl.h"

// hdl_button_hw_t mod_btn_power = {
//   .active_state = HDL_GPIO_LOW,
//   .btn_gpio = &mod_gpi_carrier_power_btn,
//   .debounce_delay = 50,
//   .hold_delay = 5000
// };

// hdl_button_hw_t mod_btn_reset = {
//   .active_state = HDL_GPIO_LOW,
//   .btn_gpio = & mod_gpi_carrier_reset_in,
//   .debounce_delay = 5,
//   .hold_delay = 0 /* never */
// };

// bldl_som_boot_lock_t mod_boot_lock = {
//   .active_state_lock_emmc = HDL_GPIO_HIGH,
//   .active_state_lock_qspi = HDL_GPIO_HIGH,
//   .active_state_lock_sd = HDL_GPIO_HIGH,
//   .lock_pin_emmc = &mod_gpo_emmc_lock,
//   .lock_pin_qspi = &mod_gpo_qspi_lock,
//   .lock_pin_sd = &mod_gpo_sd_lock
// };

// bldl_som_boot_sel_t mod_boot_select = {
//   .active_state_bs0 = HDL_GPIO_LOW,
//   .active_state_bs1 = HDL_GPIO_LOW,
//   .active_state_bs2 = HDL_GPIO_LOW,
//   .active_state_fr = HDL_GPIO_LOW,
//   .gpio_bs0 = &mod_gpi_carrier_boot_sel0,
//   .gpio_bs1 = &mod_gpi_carrier_boot_sel1,
//   .gpio_bs2 = &mod_gpi_carrier_boot_sel2,
//   .gpio_fr = &mod_gpi_carrier_force_recovery
// };

// bldl_som_power_state_hw_t mod_som_state_ctrl = {
//   .active_state_carrier_pwr_on = HDL_GPIO_HIGH,
//   .active_state_carrier_stby = HDL_GPIO_LOW,
//   .active_state_reset_out = HDL_GPIO_LOW,
//   .active_state_power_good = HDL_GPIO_HIGH,
//   .active_state_pmic_power_on = HDL_GPIO_HIGH,
//   .active_state_pmic_soc_rst = HDL_GPIO_HIGH,
//   .active_state_carrier_wdt = HDL_GPIO_LOW,
//   .carrier_wdt = &mod_gpi_carrier_wdt_time_out,
//   .carrier_pwr_on = &mod_gpo_carrier_pwr_on,
//   .carrier_stby = &mod_gpo_carrier_stby,
//   .reset_out = &mod_gpo_carrier_reset_out,
//   .power_good = &mod_gpi_carrier_power_good,
//   .pmic_power_on = &mod_gpo_pmic_power_on,
//   .pmic_soc_rst = &mod_gpo_pmic_soc_rst,
//   .reset_in = &mod_btn_reset,
//   .boot_lock = &mod_boot_lock,
//   .bootsel = &mod_boot_select
// };
