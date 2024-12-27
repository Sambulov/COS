#include "app.h"
#include "CodeLib.h"

void led_y_timer_handler(uint32_t event_trigger, void *sender, void *context) {
  __NOP();
  //hdl_gpio_toggle(&mod_gpio_pin_led_y);
}

void led_g_timer_handler(uint32_t event_trigger, void *sender, void *context) {
  __NOP();
  //hdl_gpio_toggle(&mod_gpio_pin_led_g);
}

void btn0_int_handler(uint32_t event_trigger, void *sender, void *context) {
  __NOP();
  //if(event_trigger & 0x100)
  //  hdl_gpio_toggle(&mod_gpio_pin_led_r);
}

uint8_t slave_test_do = 0;
uint8_t slave_test_no = 4;

void set_leds(uint8_t mask) {
  hdl_gpio_set_inactive(&mod_gpio_pin_led_y);
  hdl_gpio_set_inactive(&mod_gpio_pin_led_r);
  hdl_gpio_set_inactive(&mod_gpio_pin_led_g);
  if(mask & 0x01) hdl_gpio_set_active(&mod_gpio_pin_led_y);
  if(mask & 0x02) hdl_gpio_set_active(&mod_gpio_pin_led_r);
  if(mask & 0x04) hdl_gpio_set_active(&mod_gpio_pin_led_g);
}

void btn0_handler(uint32_t event_trigger, void *sender, void *context) {
  if(event_trigger == HDL_BTN_EVENT_CLICK) {
    slave_test_no++;
    if(slave_test_no > 4) slave_test_no = 0;
    set_leds(slave_test_no);
  }
}

void btn1_handler(uint32_t event_trigger, void *sender, void *context) {
  if(event_trigger == HDL_BTN_EVENT_CLICK) {
    slave_test_do = 1;
  }
  if(event_trigger == HDL_BTN_EVENT_HOLD) {
    slave_test_no = 10;
    slave_test_do = 1;
  }
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

hdl_delegate_t btn0_delegate = {
  .context = NULL,
  .handler = &btn0_handler
};

hdl_delegate_t btn1_delegate = {
  .context = NULL,
  .handler = &btn1_handler
};

typedef struct {
int16_t ac_x,
        ac_y,
        ac_z,
        temp,
        gy_x,
        gy_y,
        gy_z;
} mpu_6050_data_t;

typedef struct {
double gyro_x,
       gyro_y,
       gyro_z,
       acel_x,
       acel_y,
       acel_z,
       pitch_ang,
       roll_ang, 
       temp_cels;
} gyro_axel_data_t;

mpu_6050_data_t gy_ax_data;
gyro_axel_data_t data;

#include "math.h"

void mpu6050_converter(mpu_6050_data_t *ga_data, gyro_axel_data_t *out) {
  out->temp_cels = ((double)ga_data->temp - 1600) / 340.0 + 36.53;
  double x = ga_data->ac_x;
  double y = ga_data->ac_y;
  double z = ga_data->ac_z;
  out->pitch_ang = atan(x / sqrt((y * y) + (z * z))) * (180.0 / 3.14);
  out->roll_ang = atan(y / sqrt((x * x) + (z * z))) * (180.0 / 3.14);
  out->acel_x = ga_data->ac_x + -950;
  out->acel_y = ga_data->ac_y + -300;
  out->acel_z = ga_data->ac_z + 0;
  out->gyro_x = ga_data->gy_x + 480;
  out->gyro_y = ga_data->gy_y + 170;
  out->gyro_z = ga_data->gy_z + 210;
}

void i2c_master_test(hdl_i2c_t *i2c) {
  static uint8_t mess_buff[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  static hdl_i2c_message_t test_mess = {.address = 0x68, .buffer = mess_buff, .length = sizeof(mess_buff)};
  static uint8_t test_state = 0;
  switch (test_state) {
  case 0: // init mpu-6050
    test_mess.options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR | HDL_I2C_MESSAGE_STOP; 
    test_mess.buffer[0] = 0x6b;
    test_mess.buffer[1] = 0x00;
    test_mess.length = 2;
    if(hdl_i2c_transfer_message(i2c, &test_mess)) test_state++;
    break;
  case 2: // mpu-6050 select 
    test_mess.options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR; 
    test_mess.buffer[0] = 0x3b;
    test_mess.length = 1;
    if(hdl_i2c_transfer_message(i2c, &test_mess)) test_state++;
    break;
  case 4: // mpu-6050 get
    test_mess.options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR | HDL_I2C_MESSAGE_MRSW | HDL_I2C_MESSAGE_NACK_LAST | HDL_I2C_MESSAGE_STOP;    
    test_mess.length = 14;
    if(hdl_i2c_transfer_message(i2c, &test_mess)) test_state++;
    break;
  case 1:
  case 3:
    if(test_mess.status & HDL_I2C_MESSAGE_STATUS_COMPLETE) test_state++;
    break;
  case 5:
    if(test_mess.status & HDL_I2C_MESSAGE_STATUS_COMPLETE) {
      if(!(test_mess.status & HDL_I2C_MESSAGE_FAULT_MASK)) {
        gy_ax_data.ac_x = (((uint16_t)test_mess.buffer[0]) << 8) | test_mess.buffer[1];
        gy_ax_data.ac_y = (((uint16_t)test_mess.buffer[2]) << 8) | test_mess.buffer[3];
        gy_ax_data.ac_z = (((uint16_t)test_mess.buffer[4]) << 8) | test_mess.buffer[5];
        gy_ax_data.temp = (((uint16_t)test_mess.buffer[6]) << 8) | test_mess.buffer[7];
        gy_ax_data.gy_x = (((uint16_t)test_mess.buffer[8]) << 8) | test_mess.buffer[9];
        gy_ax_data.gy_y = (((uint16_t)test_mess.buffer[10]) << 8) | test_mess.buffer[11];
        gy_ax_data.gy_z = (((uint16_t)test_mess.buffer[12]) << 8) | test_mess.buffer[13];
        mpu6050_converter(&gy_ax_data, &data);
      }
      test_state = 2;
    } 
  default:
    break;
  }
}

void i2c_slave_test(hdl_i2c_t *i2c) {
  static uint8_t mess_buff[2];
  static hdl_i2c_message_t test_mess = {.address = 0x68, .buffer = mess_buff, .length = sizeof(mess_buff)};
  static uint8_t test_state = 4;
  switch (test_state)
  {         // leds      gry
    case 0: // start     000
      test_mess.options = HDL_I2C_MESSAGE_START; 
      test_mess.length = 0;
      test_state = 5;
      break;
    case 1: // addr_mt   001
      test_mess.options = HDL_I2C_MESSAGE_ADDR; 
      test_mess.address = 0x68;
      test_mess.length = 0;
      test_state = 5;
      break;
    case 2: // addr_mr   010
      test_mess.options = HDL_I2C_MESSAGE_ADDR | HDL_I2C_MESSAGE_MRSW; 
      test_mess.address = 0x68;
      test_mess.length = 0;
      test_state = 5;
      break;
    case 3: // data      011
      test_mess.options = HDL_I2C_MESSAGE_NACK_LAST;
      mess_buff[0] = 0x10;
      mess_buff[1] = 0x00;
      test_mess.length = 2;
      test_state = 5;
      break;
    case 4: // stop      100
      test_mess.options = HDL_I2C_MESSAGE_STOP;
      test_mess.length = 0;
      test_state = 5;
      break;
    case 5: // transfer
      set_leds(5);
      if(hdl_i2c_transfer_message(i2c, &test_mess)) test_state = 9;
      break;
    case 9: // awaite
      set_leds(6);
      if(test_mess.status & HDL_I2C_MESSAGE_STATUS_COMPLETE) {
        set_leds(7);
        test_state = -1;
      }
      break;
    case 10:
      i2c_master_test(&mod_i2c1);
      if(slave_test_do) test_state = -1;
      break;
    default:
      if(slave_test_do) {
        slave_test_do = 0; 
        test_state = slave_test_no;
      }
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
  hdl_enable(&btn0.module);
  hdl_enable(&btn1.module);
  hdl_enable(&mod_i2c0.module);
  hdl_enable(&mod_i2c1.module);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  hdl_gpio_set_inactive(&mod_gpio_pin_led_g);
  hdl_gpio_set_inactive(&mod_gpio_pin_led_r);
  hdl_gpio_set_inactive(&mod_gpio_pin_led_y);

  hdl_event_subscribe(&btn0.event, &btn0_delegate);
  hdl_event_subscribe(&btn1.event, &btn1_delegate);
  hdl_event_subscribe(&led_y_timer.event, &led_y_timer_delegate);
  hdl_event_subscribe(&led_g_timer.event, &led_g_timer_delegate);
  hdl_timer_set(&led_y_timer, 100, HDL_TIMER_EVENT_LOOP);
  hdl_timer_set(&led_g_timer, 100, HDL_TIMER_EVENT_LOOP);

  hdl_event_subscribe(&mod_irq_gpio_btn_port.event, &btn0_int_delegate);
  hdl_interrupt_request(&mod_interrupt_controller, &mod_irq_gpio_btn_port);

  hdl_i2c_set_transceiver(&mod_i2c0, bldl_som_link_init());

  while (1) {
    cooperative_scheduler(false);
    i2c_slave_test(&mod_i2c1);
  }
}
