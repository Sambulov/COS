/**************************************************************************//**
 * @file     main.c
 * @version  
 * @brief    
 *
 ******************************************************************************/

#include "app.h"

static void power_btn_pass_throw(void *sender, void *context) {
  hdl_btn_state_t btn = hdl_btn_state_get((hdl_button_t *)sender);
  bldl_som_power_state_t *som_state = (bldl_som_power_state_t *)context;
  //hdl_gpio_write(&dts_gpo_pmic_power_on, (btn == HDL_BTN_PRESSED)? HDL_GPIO_HIGH: HDL_GPIO_LOW); //?????
}

static void power_btn_hold(void *sender, void *context) {
  bldl_som_power_state_t *som_state = (bldl_som_power_state_t *)context;
  if(som_state->current_state == BLDL_SOM_STATE_ON)
    bldl_som_exec_state(som_state, BLDL_SOM_STATE_OFF);
}

static void power_btn_click(void *sender, void *context) {
  bldl_som_power_state_t *som_state = (bldl_som_power_state_t *)context;
  if(som_state->current_state == BLDL_SOM_STATE_OFF)
    bldl_som_exec_state(som_state, BLDL_SOM_STATE_ON);
}

static void reset_btn_down(void *sender, void *context) {
  bldl_som_power_state_t *som_state = (bldl_som_power_state_t *)context;
  if(som_state->current_state == BLDL_SOM_STATE_ON)
    bldl_som_exec_state(som_state, BLDL_SOM_STATE_RESET);
}

static void reset_btn_up(void *sender, void *context) {
  bldl_som_power_state_t *som_state = (bldl_som_power_state_t *)context;
  if(som_state->current_state == BLDL_SOM_STATE_RESET)
    bldl_som_exec_state(som_state, BLDL_SOM_STATE_ON);
}

void main() {
  static bldl_som_power_state_t som_state;
  som_state.hw = &dts_som_state_ctrl;

  hdl_system_init(&dts_sys_cnf);

  hdl_callback_t power_btn_down_up_cb = {
    .context = &som_state,
    .handler = &power_btn_pass_throw
  };

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
    .on_button_down_cb = &power_btn_down_up_cb,
    .on_button_click_cb = &power_btn_click_cb,
    .on_button_hold_cb = &power_btn_hold_cb,
    .on_button_up_cb = &power_btn_down_up_cb
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

  //bldl_som_exec_state(&som_state, BLDL_SOM_STATE_OFF);
  bldl_som_exec_state(&som_state, BLDL_SOM_STATE_ON);
  while(1) {
    hdl_btn_process(&power_button);
    hdl_btn_process(&reset_button);
  }
}
