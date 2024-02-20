#include "app.h"

static void _exec_start(const bldl_som_power_state_hw_t *desc) {
  bldl_boot_select_t sel = bldl_som_boot_sel_get(desc->bootsel);
  uint8_t fr = (sel & BLDL_BOOT_FR) != 0;
  hdl_gpio_write(desc->pmic_soc_rst, !desc->active_state_pmic_soc_rst);
  hdl_gpio_write(desc->reset_out, !desc->active_state_reset_out);
  if (fr) {
    bldl_som_boot_dev_en(desc->boot_lock, BLDL_BOOT_DEV_SD);
    hdl_delay(5000);
  }
  bldl_som_boot_dev_en(desc->boot_lock, BLDL_BOOT_DEV_ALL);
}

static void _exec_reset(const bldl_som_power_state_hw_t *desc) {
  hdl_gpio_write(desc->pmic_soc_rst, desc->active_state_pmic_soc_rst);
  bldl_som_boot_dev_en(desc->boot_lock, BLDL_BOOT_DEV_NONE);
  hdl_gpio_write(desc->reset_out, desc->active_state_reset_out);
}

static void _exec_power_cycle(const bldl_som_power_state_hw_t *desc) {
  hdl_gpio_init(desc->pmic_soc_rst);
  hdl_gpio_init(desc->carrier_pwr_on);
  hdl_gpio_init(desc->carrier_stby);
  hdl_gpio_init(desc->reset_out);
  hdl_gpio_init(desc->power_good);
  hdl_gpio_init(desc->reset_in->btn_gpio);
  hdl_gpio_init(desc->pmic_power_on);
  hdl_gpio_write(desc->carrier_pwr_on, !desc->active_state_carrier_pwr_on);
  hdl_gpio_write(desc->carrier_stby, desc->active_state_carrier_stby);

  _exec_reset(desc);

  while (hdl_gpio_read(desc->power_good) == !desc->active_state_power_good) ;

  hdl_gpio_write(desc->pmic_power_on, desc->active_state_pmic_power_on);
  hdl_delay(500);
  //hdl_gpio_write(desc->pmic_power_on, !desc->active_state_pmic_power_on);
  //hdl_delay(500); /* NOTE: specific for customer */

  hdl_gpio_write(desc->carrier_pwr_on, desc->active_state_carrier_pwr_on);
  hdl_gpio_write(desc->carrier_stby, !desc->active_state_carrier_stby);
  while (hdl_gpio_read(desc->reset_in->btn_gpio) == desc->reset_in->active_state) ;
}

void bldl_som_exec_state(bldl_som_power_state_t *desc, bldl_som_state_t new_state) {
  static uint8_t init = 1;
  if(desc == NULL) 
    return;
  if(init) {
    _exec_power_cycle(desc->hw);
    desc->current_state = BLDL_SOM_STATE_OFF;
    init = 0;
  }
  if(desc->current_state != new_state) {
    if(desc->current_state == BLDL_SOM_STATE_ON) {
      if((new_state == BLDL_SOM_STATE_RESET) || (new_state == BLDL_SOM_STATE_OFF)) {
        _exec_reset(desc->hw);
        desc->current_state = new_state;
      }
    }  
    else { /* ((current_state == BLDL_SOM_STATE_RESET) || (current_state == BLDL_SOM_STATE_OFF))*/
      if(new_state == BLDL_SOM_STATE_ON) {
          _exec_start(desc->hw);
        desc->current_state = new_state;
      }
    }
  }
}
