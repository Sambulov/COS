#ifndef BLDL_SOM_STATE_H_
#define BLDL_SOM_STATE_H_

typedef enum {
    BLDL_SOM_STATE_OFF,
    BLDL_SOM_STATE_ON,
    BLDL_SOM_STATE_RESET
} bldl_som_state_t;

typedef struct {
  const hdl_gpio_state active_state_carrier_pwr_on;
  const hdl_gpio_state active_state_carrier_stby;
  const hdl_gpio_state active_state_reset_out;
  const hdl_gpio_state active_state_power_good;
  const hdl_gpio_state active_state_pmic_power_on;
  const hdl_gpio_state active_state_pmic_soc_rst;

  const hdl_gpio_t *carrier_pwr_on;
  const hdl_gpio_t *carrier_stby;
  const hdl_gpio_t *reset_out;
  const hdl_gpio_t *pmic_power_on;
  const hdl_gpio_t *power_good;
  const hdl_gpio_t *pmic_soc_rst;
  const hdl_button_hw_t *reset_in;
  const bldl_som_boot_sel_t *bootsel;
  const bldl_som_boot_lock_t *boot_lock;
} bldl_som_power_state_hw_t;

typedef struct {
  const bldl_som_power_state_hw_t *hw;
  bldl_som_state_t current_state;
} bldl_som_power_state_t;

void bldl_som_exec_state(bldl_som_power_state_t *desc, bldl_som_state_t new_state);

#endif // BLDL_SOM_STATE_H_
