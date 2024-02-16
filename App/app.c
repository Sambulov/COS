/**************************************************************************//**
 * @file     main.c
 * @version  
 * @brief    
 *
 ******************************************************************************/

#include "app.h"


#if defined ( NUM46X )
// static void power_btn_pass_throw(void *sender, void *context) {
//   hdl_btn_state_t btn = hdl_btn_state_get((hdl_button_t *)sender);
//   hdl_gpio_write(&btn_pwron, (btn == HDL_BTN_PRESSED)? HDL_GPIO_HIGH: HDL_GPIO_LOW);
// }

static void power_btn_down(void *sender, void *context) {
  hdl_gpio_write(&qspi_lock, HDL_GPIO_LOW);
}

static void power_btn_up(void *sender, void *context) {
  hdl_gpio_write(&qspi_lock, HDL_GPIO_HIGH);
}

static void power_btn_hold(void *sender, void *context) {
  bldl_som_state_t *som_state = (bldl_som_state_t *)context;
  *som_state = BLDL_SOM_STATE_OFF;  
  hdl_gpio_write(&sd_lock, HDL_GPIO_LOW);
}

static void power_btn_click(void *sender, void *context) {
  bldl_som_state_t *som_state = (bldl_som_state_t *)context;
  if (*som_state == BLDL_SOM_STATE_OFF) {
      *som_state = BLDL_SOM_STATE_START;
  }
  hdl_gpio_write(&emmc_lock, HDL_GPIO_LOW);
}

static bldl_som_state_t reset_in_process(bldl_som_state_t som_state) {
  static hdl_gpio_state state_prev = HDL_GPIO_HIGH;
  hdl_gpio_state state_current = hdl_gpio_read(&x1_reset_in);
  if(state_current != state_prev) {
    if((state_current == HDL_GPIO_LOW) && (som_state == BLDL_SOM_STATE_ON)) {
      som_state = BLDL_SOM_STATE_RESET;
    } 
    else if((state_current == HDL_GPIO_HIGH) && (som_state == BLDL_SOM_STATE_RESET)) {
      som_state = BLDL_SOM_STATE_START;
    }
    state_prev = state_current;
  }
  return som_state;
}

void main() {
  static bldl_som_state_t som_state = BLDL_SOM_STATE_COLD_START;
  hdl_system_init(&sys_cnf);

  hdl_gpio_init(&sd_lock);
  hdl_gpio_init(&emmc_lock);
  hdl_gpio_init(&qspi_lock);

  hdl_callback_t power_btn_hold_cb = {
    .context = &som_state,
    .handler = &power_btn_hold
  };

  hdl_callback_t power_btn_click_cb = {
    .context = &som_state,
    .handler = &power_btn_click
  };

  hdl_callback_t power_btn_down_cb = {
    .context = &som_state,
    .handler = &power_btn_down
  };

  hdl_callback_t power_btn_up_cb = {
    .context = &som_state,
    .handler = &power_btn_up
  };

  hdl_button_t power_button = {
    .hw = &power_btn_hw,
    .on_button_down_cb = &power_btn_down_cb,
    .on_button_click_cb = &power_btn_click_cb,
    .on_button_hold_cb = &power_btn_hold_cb,
    .on_button_up_cb = &power_btn_up_cb
  };

  hdl_btn_init(&power_button);

  //bldl_power_cycle(&som_pc);
  while(1) {
    //bldl_som_exec_state(&som_power, som_state);
    //hdl_gpio_write(&btn_pwron, hdl_gpio_read(&x1_power_btn));
    //hdl_btn_process(&power_button);
    //som_state = reset_in_process(som_state);


    bldl_boot_dev_t dev = bldl_bootsel2bootdev(bldl_som_boot_sel_get(&boot_select));

    hdl_gpio_write(&sd_lock, (dev & BLDL_BOOT_DEV_SD)? HDL_GPIO_LOW: HDL_GPIO_HIGH);
    hdl_gpio_write(&emmc_lock, (dev & BLDL_BOOT_DEV_EMMC)? HDL_GPIO_LOW: HDL_GPIO_HIGH);
    hdl_gpio_write(&qspi_lock, (dev & BLDL_BOOT_DEV_SPI)? HDL_GPIO_LOW: HDL_GPIO_HIGH);
  }

}

#else




// static void power_btn_pass_throw(void *sender, void *context) {
//   hdl_btn_state_t btn = hdl_btn_state_get((hdl_button_t *)sender);
//   hdl_gpio_write(&btn_pwron, (btn == HDL_BTN_PRESSED)? HDL_GPIO_HIGH: HDL_GPIO_LOW);
// }

static void power_btn_hold(void *sender, void *context) {
  bldl_som_state_t *som_state = (bldl_som_state_t *)context;
  *som_state = BLDL_SOM_STATE_OFF;
  bldl_som_exec_state(&dts_som_state_ctrl, *som_state);
}

static void power_btn_click(void *sender, void *context) {
  bldl_som_state_t *som_state = (bldl_som_state_t *)context;
  if (*som_state == BLDL_SOM_STATE_OFF) {
    *som_state = BLDL_SOM_STATE_START;
    bldl_som_exec_state(&dts_som_state_ctrl, *som_state);
    *som_state = BLDL_SOM_STATE_ON;
  }
}

static void reset_btn_down(void *sender, void *context) {
  bldl_som_state_t *som_state = (bldl_som_state_t *)context;
  if (*som_state == BLDL_SOM_STATE_ON) {
    *som_state = BLDL_SOM_STATE_RESET;
    bldl_som_exec_state(&dts_som_state_ctrl, *som_state);
  }
}

static void reset_btn_up(void *sender, void *context) {
  bldl_som_state_t *som_state = (bldl_som_state_t *)context;
  if (*som_state == BLDL_SOM_STATE_RESET) {
    *som_state = BLDL_SOM_STATE_START;
    bldl_som_exec_state(&dts_som_state_ctrl, *som_state);
    *som_state = BLDL_SOM_STATE_ON;
  }
}

void main() {
  static bldl_som_state_t som_state = BLDL_SOM_STATE_COLD_START;
  hdl_system_init(&dts_sys_cnf);
  //hdl_delay(1000);

  hdl_callback_t power_btn_hold_cb = {
    .context = &som_state,
    .handler = &power_btn_hold
  };

  hdl_callback_t power_btn_click_cb = {
    .context = &som_state,
    .handler = &power_btn_click
  };

  hdl_callback_t reset_btn_down_cb = {
    .context = &som_state,
    .handler = &reset_btn_down
  };

  hdl_callback_t reset_btn_up_cb = {
    .context = &som_state,
    .handler = &reset_btn_up
  };

  hdl_button_t power_button = {
    .hw = &dts_btn_power,
    .on_button_down_cb = NULL,
    .on_button_click_cb = &power_btn_click_cb,
    .on_button_hold_cb = &power_btn_hold_cb,
    .on_button_up_cb = NULL
  };

  hdl_button_t reset_button = {
    .hw = &dts_btn_reset,
    .on_button_down_cb = &reset_btn_down_cb,
    .on_button_click_cb = NULL,
    .on_button_hold_cb = NULL,
    .on_button_up_cb = &reset_btn_up_cb
  };

  hdl_btn_init(&power_button);
  hdl_btn_init(&reset_button);

  bldl_som_exec_state(&dts_som_state_ctrl, som_state);
  som_state = BLDL_SOM_STATE_ON;
  while(1) {
    hdl_btn_process(&power_button);
    hdl_btn_process(&reset_button);
    //hdl_gpio_write(&dts_gpo_pmic_power_on, (hdl_gpio_read(&dts_gpi_carrier_power_btn)==HDL_GPIO_LOW)? HDL_GPIO_HIGH: HDL_GPIO_LOW); //?????
  }

}
#endif
