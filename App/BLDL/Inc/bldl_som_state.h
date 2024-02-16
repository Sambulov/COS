#ifndef BLDL_SM_POWER_H_
#define BLDL_SM_POWER_H_

typedef enum {
    BLDL_SOM_STATE_OFF,
    BLDL_SOM_STATE_COLD_START,
    BLDL_SOM_STATE_ON,
    BLDL_SOM_STATE_START,
    BLDL_SOM_STATE_RESET
} bldl_som_state_t;

typedef struct {
  const hdl_gpio_t *carrier_pwr_on;
  const hdl_gpio_t *carrier_stby_n;
  const hdl_gpio_t *reset_out_n;

  const hdl_gpio_t *btn_pwron;

  const hdl_gpio_t *power_good;
  const hdl_gpio_t *reset_in_n;


  const hdl_gpio_t *pmic_rst;
  //const hdl_gpio_t *reset_out;
  
  //const hdl_gpio_t *btn_pwron;

  const bldl_som_boot_sel_t *bootsel;
  const bldl_som_boot_lock_t *boot_lock;
} bldl_som_state_hw_t;

void bldl_som_exec_state(const bldl_som_state_hw_t *desc, bldl_som_state_t state);

#endif // BLDL_SM_POWER_H_
