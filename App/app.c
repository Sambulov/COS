/**************************************************************************//**
 * @file     main.c
 * @version  
 * @brief    
 *
 ******************************************************************************/

#include "app.h"

static void power_btn_pass_throw(void *sender, void *context) {
  hdl_btn_state_t btn_state = hdl_btn_state_get((hdl_button_t *)sender);
  //bldl_som_power_state_t *som_state = (bldl_som_power_state_t *)context;
  hdl_gpio_write(&dts_gpo_pmic_power_on, (btn_state == HDL_BTN_PRESSED)? HDL_GPIO_HIGH: HDL_GPIO_LOW); //?????
}

static void power_btn_hold(void *sender, void *context) {
  //hdl_button_t *btn = (hdl_button_t *)sender;
  bldl_som_power_state_t *som_state = (bldl_som_power_state_t *)context;
  if(som_state->current_state == BLDL_SOM_STATE_ON)
    bldl_som_exec_state(som_state, BLDL_SOM_STATE_OFF);
}

static void power_btn_click(void *sender, void *context) {
  //hdl_button_t *btn = (hdl_button_t *)sender;
  bldl_som_power_state_t *som_state = (bldl_som_power_state_t *)context;
  if(som_state->current_state == BLDL_SOM_STATE_OFF)
    bldl_som_exec_state(som_state, BLDL_SOM_STATE_ON);
}

static void reset_btn_down(void *sender, void *context) {
  //hdl_button_t *btn = (hdl_button_t *)sender;
  bldl_som_power_state_t *som_state = (bldl_som_power_state_t *)context;
  if(som_state->current_state == BLDL_SOM_STATE_ON)
    bldl_som_exec_state(som_state, BLDL_SOM_STATE_RESET);
}

static void reset_btn_up(void *sender, void *context) {
  //hdl_button_t *btn = (hdl_button_t *)sender;
  bldl_som_power_state_t *som_state = (bldl_som_power_state_t *)context;
  if(som_state->current_state == BLDL_SOM_STATE_RESET)
    bldl_som_exec_state(som_state, BLDL_SOM_STATE_ON);
}

#include "RK809.h"

void test_read_pmic_regs() {
  static hdl_i2c_t i2c;
  hdl_i2c_init(&i2c, &dts_i2c_0);



  #define U16_MSB(val)   ((uint8_t)(val >> 8))
  #define U16_LSB(val)   ((uint8_t)(val))

  uint16_t reg_arr[] = {
    RK809_REG_PMIC_CHIP_NAME,
    RK809_REG_PMIC_CHIP_VER,
    RK809_REG_PMIC_SYS_CFG3,
  };
  uint8_t reg_addr[2];
  uint8_t reg_data[1];

  hdl_i2c_transaction_t transaction;
  hdl_i2c_client_new_transaction(&transaction);

  hdl_i2c_message_t start_mess = {
    .address = RK809_PMIC_I2C_ADDR,
    .data = reg_addr,
    .length = sizeof(reg_addr),
    .options = HDL_I2C_MESSAGE_SEND_START 
  };

  hdl_i2c_message_t data_mess = {
    .address = RK809_PMIC_I2C_ADDR,
    .data = reg_data,
    .length = sizeof(reg_data),
    .options = HDL_I2C_MESSAGE_SEND_START | HDL_I2C_MESSAGE_READ | HDL_I2C_MESSAGE_SEND_READ_NACK 
  };

  hdl_i2c_client_transaction_add_message(&transaction, &start_mess);
  hdl_i2c_client_transaction_add_message(&transaction, &data_mess);

  for(int32_t i =0; i<(sizeof(reg_arr)/sizeof(uint16_t)); i++) {
    reg_addr[0] = U16_MSB(reg_arr[i]);
    reg_addr[1] = U16_LSB(reg_arr[i]);
    hdl_i2c_enqueue_transaction(&i2c, &transaction);
    volatile uint8_t reg = reg_data[0];
    hdl_i2c_work(&i2c);
  }

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
    .on_button_down_cb = &power_btn_down_up_cb,
    .on_button_click_cb = &power_btn_click_cb,
    .on_button_hold_cb = &power_btn_hold_cb,
    .on_button_up_cb = &power_btn_down_up_cb
  };

  hdl_button_t reset_button = {
    .on_button_down_cb = &reset_btn_down_cb,
    .on_button_click_cb = NULL,
    .on_button_hold_cb = NULL,
    .on_button_up_cb = &reset_btn_up_cb
  };

  hdl_btn_init(&power_button, &dts_btn_power);
  hdl_btn_init(&reset_button, &dts_btn_reset);
  bldl_som_exec_state(&som_state, BLDL_SOM_STATE_OFF);
  //bldl_som_exec_state(&som_state, BLDL_SOM_STATE_ON);
  test_read_pmic_regs();
  while(1) {
    hdl_btn_process(&power_button);
    hdl_btn_process(&reset_button);
  }
}
