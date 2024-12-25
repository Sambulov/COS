#include "app.h"
#include "CodeLib.h"

hdl_timer_t led_y_timer = {
  .module.dependencies = hdl_module_dependencies(&mod_timer_ms.module),
  .module.init = &hdl_timer,
};

hdl_timer_t led_g_timer = {
  .module.dependencies = hdl_module_dependencies(&mod_timer_ms.module),
  .module.init = &hdl_timer,
};

const hdl_button_config_t btn1_cnf = {
  .debounce_delay = 50,
  .hold_delay = 3000
};

hdl_button_t btn1 = {
  .module.init = &hdl_button,
  .module.dependencies = hdl_module_dependencies(&mod_gpio_pin_btn_1.module, &mod_timer_ms.module),
  .config = &btn1_cnf,
};

void led_y_timer_handler(uint32_t event_trigger, void *sender, void *context) {
  hdl_gpio_toggle(&mod_gpio_pin_led_y);
}

void led_g_timer_handler(uint32_t event_trigger, void *sender, void *context) {
  hdl_gpio_toggle(&mod_gpio_pin_led_g);
}

void btn0_int_handler(uint32_t event_trigger, void *sender, void *context) {
  if(event_trigger & 0x100)
    hdl_gpio_toggle(&mod_gpio_pin_led_r);
}

void btn1_handler(uint32_t event_trigger, void *sender, void *context) {
  if(event_trigger == HDL_BTN_EVENT_CLICK)
    hdl_gpio_toggle(&mod_gpio_pin_led_r);
  if(event_trigger == HDL_BTN_EVENT_HOLD)
    hdl_gpio_set_active(&mod_gpio_pin_led_r);
}

hdl_delegate_t led_y_timer_delegate = {
  .context = NULL,
  .handler = &led_y_timer_handler
};

hdl_delegate_t led_g_timer_delegate = {
  .context = NULL,
  .handler = &led_g_timer_handler
};

hdl_delegate_t btn0_int_delegate = {
  .context = NULL,
  .handler = &btn0_int_handler
};

hdl_delegate_t btn1_delegate = {
  .context = NULL,
  .handler = &btn1_handler
};

typedef struct {
uint16_t ac_x,
         ac_y,
         ac_z,
         temp,
         gy_x,
         gy_y,
         gy_z;
} mpu_6050_data_t;

volatile mpu_6050_data_t gy_ax_data;

void i2c_master_test() {
  static uint8_t mess_buff[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  static hdl_i2c_message_t test_mess = {.address = 0x68, .buffer = mess_buff, .length = sizeof(mess_buff)};
  static uint8_t test_state = 0;
  switch (test_state) {
  case 0: // init mpu-6050
    test_mess.options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR | HDL_I2C_MESSAGE_STOP; 
    test_mess.buffer[0] = 0x6b;
    test_mess.buffer[1] = 0x00;
    test_mess.length = 2;
    if(hdl_i2c_transfer_message(&mod_i2c0, &test_mess)) test_state++;
    break;
  case 2: // mpu-6050 select 
    test_mess.options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR; 
    test_mess.buffer[0] = 0x3b;
    test_mess.length = 1;
    if(hdl_i2c_transfer_message(&mod_i2c0, &test_mess)) test_state++;
    break;
  case 4: // mpu-6050 get
    test_mess.options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR | HDL_I2C_MESSAGE_MRSW | HDL_I2C_MESSAGE_NACK_LAST | HDL_I2C_MESSAGE_STOP;    
    test_mess.length = 14;
    if(hdl_i2c_transfer_message(&mod_i2c0, &test_mess)) test_state++;
    break;
  case 1:
  case 3:
    if(test_mess.status & HDL_I2C_MESSAGE_STATUS_COMPLETE) test_state++;
    break;
  case 5:
    if(test_mess.status & HDL_I2C_MESSAGE_STATUS_COMPLETE) {
      gy_ax_data.ac_x = (((uint16_t)test_mess.buffer[0]) << 8) | test_mess.buffer[1];
      gy_ax_data.ac_y = (((uint16_t)test_mess.buffer[2]) << 8) | test_mess.buffer[3];
      gy_ax_data.ac_z = (((uint16_t)test_mess.buffer[4]) << 8) | test_mess.buffer[5];
      gy_ax_data.temp = (((uint16_t)test_mess.buffer[6]) << 8) | test_mess.buffer[7];
      gy_ax_data.gy_x = (((uint16_t)test_mess.buffer[8]) << 8) | test_mess.buffer[9];
      gy_ax_data.gy_y = (((uint16_t)test_mess.buffer[10]) << 8) | test_mess.buffer[11];
      gy_ax_data.gy_z = (((uint16_t)test_mess.buffer[12]) << 8) | test_mess.buffer[13];
      test_state = 2;
    } 
  default:
    break;
  }
}

void main() {
  hdl_enable(&mod_timer_ms.module);
  hdl_enable(&led_y_timer.module);
  hdl_enable(&led_g_timer.module);
  hdl_enable(&mod_gpio_pin_btn_0.module);
  hdl_enable(&mod_gpio_pin_btn_1.module);
  hdl_enable(&mod_gpio_pin_led_r.module);
  hdl_enable(&mod_gpio_pin_led_y.module);
  hdl_enable(&mod_gpio_pin_led_g.module);
  hdl_enable(&btn1.module);
  hdl_enable(&mod_i2c0.module);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  hdl_event_subscribe(&btn1.event, &btn1_delegate);
  hdl_event_subscribe(&led_y_timer.event, &led_y_timer_delegate);
  hdl_event_subscribe(&led_g_timer.event, &led_g_timer_delegate);
  hdl_timer_set(&led_y_timer, 100, HDL_TIMER_EVENT_LOOP);
  hdl_timer_set(&led_g_timer, 100, HDL_TIMER_EVENT_LOOP);

  hdl_event_subscribe(&mod_irq_gpio_btn_port.event, &btn0_int_delegate);
  hdl_interrupt_request(&mod_interrupt_controller, &mod_irq_gpio_btn_port);

  hdl_gpio_set_active(&mod_gpio_pin_led_y);

  while (1) {
    cooperative_scheduler(false);
    i2c_master_test();
  }
}
