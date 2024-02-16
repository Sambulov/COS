#include "app.h"

static void _exec_start(const bldl_som_state_hw_t *desc) {
  bldl_boot_select_t sel = bldl_som_boot_sel_get(desc->bootsel);
  uint8_t fr = (sel & BLDL_BOOT_FR) != 0;
  hdl_gpio_write(desc->pmic_rst, HDL_GPIO_LOW);
  hdl_gpio_write(desc->reset_out_n, HDL_GPIO_HIGH);
  bldl_som_boot_dev_en(desc->boot_lock, fr? BLDL_BOOT_DEV_SD: BLDL_BOOT_DEV_ALL);
  if (fr) {
    hdl_delay(5000);
  }
  bldl_som_boot_dev_en(desc->boot_lock, BLDL_BOOT_DEV_ALL);
}

static void _exec_cold_start(const bldl_som_state_hw_t *desc) {
    hdl_gpio_init(desc->pmic_rst);
    hdl_gpio_init(desc->carrier_pwr_on);
    hdl_gpio_init(desc->carrier_stby_n);
    hdl_gpio_init(desc->reset_out_n);
    hdl_gpio_init(desc->power_good);
    hdl_gpio_init(desc->reset_in_n);
    hdl_gpio_init(desc->btn_pwron);

    hdl_gpio_write(desc->carrier_pwr_on, HDL_GPIO_LOW);
    hdl_gpio_write(desc->carrier_stby_n, HDL_GPIO_LOW);
    hdl_gpio_write(desc->reset_out_n, HDL_GPIO_LOW);

    while (hdl_gpio_read(desc->power_good) == HDL_GPIO_LOW) ;
    hdl_gpio_write(desc->carrier_pwr_on, HDL_GPIO_HIGH);

    hdl_gpio_write(desc->btn_pwron, HDL_GPIO_HIGH);
    hdl_delay(500);
    hdl_gpio_write(desc->btn_pwron, HDL_GPIO_LOW);
    hdl_delay(500);

    hdl_gpio_write(desc->carrier_stby_n, HDL_GPIO_HIGH);

    while (hdl_gpio_read(desc->reset_in_n) == HDL_GPIO_LOW) ;

    hdl_gpio_write(desc->reset_out_n, HDL_GPIO_HIGH);

   _exec_start(desc);
}

static void _exec_reset(const bldl_som_state_hw_t *desc) {
  hdl_gpio_write(desc->pmic_rst, HDL_GPIO_HIGH);
  bldl_som_boot_dev_en(desc->boot_lock, BLDL_BOOT_DEV_NONE);
  hdl_gpio_write(desc->reset_out_n, HDL_GPIO_LOW);
}

void bldl_som_exec_state(const bldl_som_state_hw_t *desc, bldl_som_state_t state) {
  switch (state) {
    case BLDL_SOM_STATE_COLD_START:
      _exec_cold_start(desc);
      break;
    case BLDL_SOM_STATE_START:
      _exec_start(desc);
      break;
    case BLDL_SOM_STATE_OFF:
    case BLDL_SOM_STATE_RESET:
      _exec_reset(desc);
      break;
    default:
      break;
  }
}
