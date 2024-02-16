#include "bldl.h"

#if defined ( NUM46X )
  const hdl_sys_t sys_cnf = {
    .x = NULL
  };

  const hdl_gpio_t emmc_lock = {
    .port = PH,
    .u32Pin = GPIO_PIN_PIN4_Msk,
    .u32IOMode = GPIO_MODE_OUTPUT,
  };

  const hdl_gpio_t qspi_lock = {
    .port = PH,
    .u32Pin = GPIO_PIN_PIN5_Msk,
    .u32IOMode = GPIO_MODE_OUTPUT,
  };

  const hdl_gpio_t sd_lock = {
    .port = PH,
    .u32Pin = GPIO_PIN_PIN6_Msk,
    .u32IOMode = GPIO_MODE_OUTPUT,
  };

  const hdl_gpio_t x1_boot_sel0 = {
    .port = PE,
    .u32Pin = GPIO_PIN_PIN6_Msk,
    .u32IOMode = GPIO_MODE_INPUT,
  };

  const hdl_gpio_t x1_boot_sel1 = {
    .port = PE,
    .u32Pin = GPIO_PIN_PIN4_Msk,
    .u32IOMode = GPIO_MODE_INPUT,
  };

  const hdl_gpio_t x1_boot_sel2 = {
    .port = PE,
    .u32Pin = GPIO_PIN_PIN2_Msk,
    .u32IOMode = GPIO_MODE_INPUT,
  };

  const hdl_gpio_t x1_force_recovery = {
    .port = PE,
    .u32Pin = GPIO_PIN_PIN0_Msk,
    .u32IOMode = GPIO_MODE_INPUT,
  };


  const hdl_gpio_t x1_reset_in = {
    .port = PE,
    .u32Pin = GPIO_PIN_PIN9_Msk,
    .u32IOMode = GPIO_MODE_INPUT,
  };

  const hdl_gpio_t x1_power_btn = {
    .port = PE,
    .u32Pin = GPIO_PIN_PIN8_Msk,
    .u32IOMode = GPIO_MODE_INPUT,
  };

#endif

#if defined ( GD32E23X )
  const hdl_sys_t dts_sys_cnf = {
    .ticks = 72000000 / 1000,
  };

  const hdl_gpio_mode_t dts_gpio_output_pp_mode = {
    .af = GPIO_AF_0,
    .pull = GPIO_PUPD_NONE,
    .type = GPIO_MODE_OUTPUT,
    .otype = GPIO_OTYPE_PP,
    .ospeed = GPIO_OSPEED_2MHZ,
  };

  const hdl_gpio_mode_t dts_gpio_input_pullup_mode = {
    .af = GPIO_AF_0,
    .pull = GPIO_PUPD_PULLUP,
    .type = GPIO_MODE_INPUT,
    .otype = GPIO_OTYPE_PP,
    .ospeed = GPIO_OSPEED_2MHZ,
  };

  const hdl_gpio_mode_t dts_gpio_input_nopull_mode = {
    .af = GPIO_AF_0,
    .pull = GPIO_PUPD_NONE,
    .type = GPIO_MODE_INPUT,
    .otype = GPIO_OTYPE_PP,
    .ospeed = GPIO_OSPEED_2MHZ,
  };

  hdl_gpio_mode_t dts_gpio_swd_mode = {
    .af = GPIO_AF_0,
    .pull = GPIO_PUPD_NONE,
    .type = GPIO_MODE_AF,
    .otype = GPIO_OTYPE_PP,
    .ospeed = GPIO_OSPEED_50MHZ,
  };

  hdl_gpio_mode_t dts_gpio_i2c_mode = {
    .af = GPIO_AF_4,
    .pull = GPIO_PUPD_NONE,
    .type = GPIO_MODE_AF,
    .otype = GPIO_OTYPE_PP,
    .ospeed = GPIO_OSPEED_2MHZ
  };

  // const hdl_gpio_t dts_gpi_pmic_sleep_1v8 = {
  //   .rcu = RCU_GPIOA,
  //   .port = GPIOA,
  //   .pin = GPIO_PIN_15,
  //   .mode = &dts_gpio_input_nopull_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_lid = {
  //   .rcu = RCU_GPIOB,
  //   .port = GPIOB,
  //   .pin = GPIO_PIN_2,
  //   .mode = &dts_gpio_input_nopull_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_batlow = {
  //   .rcu = RCU_GPIOB,
  //   .port = GPIOB,
  //   .pin = GPIO_PIN_6,
  //   .mode = &dts_gpio_input_nopull_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_sleep = {
  //   .rcu = RCU_GPIOB,
  //   .port = GPIOB,
  //   .pin = GPIO_PIN_7,
  //   .mode = &dts_gpio_input_nopull_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_wdt_time_out = {
  //   .rcu = RCU_GPIOB,
  //   .port = GPIOB,
  //   .pin = GPIO_PIN_8,
  //   .mode = &dts_gpio_input_nopull_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_charging = {
  //   .rcu = RCU_GPIOF,
  //   .port = GPIOF,
  //   .pin = GPIO_PIN_0,
  //   .mode = &dts_gpio_input_nopull_mode
  // };

  // const hdl_gpio_t dts_gpi_carrier_charger_prstn = {
  //   .rcu = RCU_GPIOF,
  //   .port = GPIOF,
  //   .pin = GPIO_PIN_1,
  //   .mode = &dts_gpio_input_nopull_mode
  // };


  const hdl_gpio_t dts_gpo_emmc_lock = {
    .rcu = RCU_GPIOA,
    .port = GPIOA,
    .pin = GPIO_PIN_0,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_qspi_lock = {
    .rcu = RCU_GPIOA,
    .port = GPIOA,
    .pin = GPIO_PIN_1,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_sd_lock = {
    .rcu = RCU_GPIOA,
    .port = GPIOA,
    .pin = GPIO_PIN_2,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpi_carrier_force_recovery = {
    .rcu = RCU_GPIOA,
    .port = GPIOA,
    .pin = GPIO_PIN_13,
    .mode = &dts_gpio_input_pullup_mode
  };

  const hdl_gpio_t dts_gpi_carrier_boot_sel0 = {
    .rcu = RCU_GPIOA,
    .port = GPIOA,
    .pin = GPIO_PIN_4,
    .mode = &dts_gpio_input_nopull_mode
  };

  const hdl_gpio_t dts_gpi_carrier_boot_sel1 = {
    .rcu = RCU_GPIOA,
    .port = GPIOA,
    .pin = GPIO_PIN_5,
    .mode = &dts_gpio_input_nopull_mode
  };

  const hdl_gpio_t dts_gpi_carrier_boot_sel2 = {
    .rcu = RCU_GPIOA,
    .port = GPIOA,
    .pin = GPIO_PIN_6,
    .mode = &dts_gpio_input_nopull_mode
  };

  const hdl_gpio_t dts_gpi_carrier_power_good = {
    .rcu = RCU_GPIOB,
    .port = GPIOB,
    .pin = GPIO_PIN_0,
    .mode = &dts_gpio_input_nopull_mode
  };

  const hdl_gpio_t dts_gpi_carrier_reset_in = {
    .rcu = RCU_GPIOA,
    .port = GPIOA,
    .pin = GPIO_PIN_7,
    .mode = &dts_gpio_input_nopull_mode
  };

  const hdl_gpio_t dts_gpo_carrier_reset_out = {
    .rcu = RCU_GPIOA,
    .port = GPIOA,
    .pin = GPIO_PIN_8,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_carrier_pwr_on = {
    .rcu = RCU_GPIOB,
    .port = GPIOB,
    .pin = GPIO_PIN_1,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_carrier_stby = {
    .rcu = RCU_GPIOB,
    .port = GPIOB,
    .pin = GPIO_PIN_3,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpi_carrier_power_btn = {
    .rcu = RCU_GPIOB,
    .port = GPIOB,
    .pin = GPIO_PIN_4,
    .mode = &dts_gpio_input_nopull_mode
  };

  const hdl_gpio_t dts_gpo_pmic_soc_rst = {
    .rcu = RCU_GPIOA,
    .port = GPIOA,
    .pin = GPIO_PIN_3,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpo_pmic_power_on = {
    .rcu = RCU_GPIOB,
    .port = GPIOB,
    .pin = GPIO_PIN_5,
    .mode = &dts_gpio_output_pp_mode
  };

  const hdl_gpio_t dts_gpio_bmc_swdio = {
    .rcu = RCU_GPIOA,
    .port = GPIOA,
    .pin = GPIO_PIN_13,
    .mode = &dts_gpio_swd_mode
  };

  const hdl_gpio_t dts_gpio_soc_scl = {
    .rcu = RCU_GPIOA,
    .port = GPIOA,
    .pin = GPIO_PIN_9,
    .mode = &dts_gpio_i2c_mode
  };

  const hdl_gpio_t dts_gpio_soc_sda = {
    .rcu = RCU_GPIOA,
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

  // const bldl_som_power_cycle_t dts_som_power_cycle = {
  // };

  const bldl_som_boot_lock_t dts_boot_lock = {
    .lock_active_state_emmc = HDL_GPIO_HIGH,
    .lock_active_state_qspi = HDL_GPIO_HIGH,
    .lock_active_state_sd = HDL_GPIO_HIGH,
    .lock_pin_emmc = &dts_gpo_emmc_lock,
    .lock_pin_qspi = &dts_gpo_qspi_lock,
    .lock_pin_sd = &dts_gpo_sd_lock
  };

  const bldl_som_boot_sel_t dts_boot_select = {
    .gpio_bs0 = &dts_gpi_carrier_boot_sel0,
    .gpio_bs1 = &dts_gpi_carrier_boot_sel1,
    .gpio_bs2 = &dts_gpi_carrier_boot_sel2,
    .gpio_fr = &dts_gpi_carrier_force_recovery
  };

  const bldl_som_state_hw_t dts_som_state_ctrl = {
    .carrier_pwr_on = &dts_gpo_carrier_pwr_on,
    .carrier_stby_n = &dts_gpo_carrier_stby,
    .reset_in_n = &dts_gpi_carrier_reset_in,
    .reset_out_n = &dts_gpo_carrier_reset_out,
    .power_good = &dts_gpi_carrier_power_good,
    .btn_pwron = &dts_gpo_pmic_power_on,
    .pmic_rst = &dts_gpo_pmic_soc_rst,
    .boot_lock = &dts_boot_lock,
    .bootsel = &dts_boot_select
  };
