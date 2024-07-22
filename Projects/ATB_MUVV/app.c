#include "app.h"
#include "CodeLib.h"

extern hdl_timer_t mod_timer_ms;
extern hdl_gpio_pin_t mod_gpo_led;
extern hdl_gpio_pin_t mod_gpi_button;
extern hdl_exti_controller_t mod_exti;
extern hdl_nvic_t mod_nvic;
extern hdl_i2c_t mod_i2c1;

uint32_t i2c_msg_ping_data = 0;
uint32_t i2c_msg_pong_data;
hdl_i2c_message_t *p_pong_msg = NULL;

hdl_i2c_message_t i2c_msg_ping = {
  .address = 0x23,
  .buffer = (uint8_t*)&i2c_msg_ping_data,
  .length = sizeof(i2c_msg_ping_data),
  .options =  HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR | HDL_I2C_MESSAGE_STOP,
};

hdl_i2c_message_t i2c_msg_pong = {
  .address = 0x23,
  .buffer = (uint8_t*)&i2c_msg_pong_data,
  .length = sizeof(i2c_msg_pong_data),
  .options =  HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR | HDL_I2C_MESSAGE_STOP,
};

uint32_t castom_rand(void) {
  const long randMax = 1000;
  static uint32_t state = 0xff15ea13;
  state = 214013 * state + 2531011;
  state ^= state >> 15;
  return (uint32_t) (state % randMax);
}

void event_1_handler(uint32_t event_trigger, void *sender, void *context) {
  hdl_timer_event_t *timer = (hdl_timer_event_t *)sender;
  hdl_gpio_toggle(&mod_gpo_led);
  if((hdl_i2c_can_transfer(&mod_i2c1) != HDL_TRUE) || !(hdl_i2c_transfer_message(&mod_i2c1, &i2c_msg_ping))) {
    timer->delay = 1;
  }
  else {
    i2c_msg_ping_data++;
    timer->delay = castom_rand();
  }
  hdl_timer_event_reset(timer);
}

hdl_timer_event_t event_1 = {
  .module.init = hdl_timer_event,
  .module.dependencies = hdl_module_dependencies(&mod_timer_ms.module),
  .delay = 15000,
};

hdl_delegate_t event_1_deleagate = {
  .handler = event_1_handler,
  .context = NULL,
};


/****************** Transiver ******************* */
uint8_t global_index = 0;


int32_t transceiver_rx_callback(void *context, uint8_t *data, uint16_t count) {
  ((uint8_t*)&i2c_msg_pong_data)[global_index] = *data;
  global_index++;
}

int32_t transceiver_rx_available_callback(void *context) {
  return 4 - global_index;
}
void transceiver_end_of_transmission(void *context) {
  i2c_msg_pong_data++;
  global_index = 0;
  p_pong_msg = &i2c_msg_pong;
}

hdl_transceiver_t i2c_transiver = {
  .end_of_transmission = transceiver_end_of_transmission,
  .rx_available = transceiver_rx_available_callback,
  .rx_data = transceiver_rx_callback,
  .tx_empty = NULL,
  .proto_context = NULL,
  .tx_available = NULL,
};

void main() {
  hdl_enable(&mod_timer_ms.module);
  hdl_enable(&mod_i2c1.module);
  hdl_enable(&mod_gpo_led.module);
  hdl_enable(&event_1.module);
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  hdl_event_subscribe(&event_1.event, &event_1_deleagate);
  hdl_timer_event_run(&event_1);

  // uint8_t i2c_isr_rx_buff[4] = {1, 1, 1, 1};
  // uint8_t i2c_isr_tx_buff[4] = {0x11, 0x12, 0x13, 0x14};

  // hdl_isr_buffer_t i2s_isr_buffer = {0};

  // hdl_isr_buffer_congig_t i2s_isr_buffer_config = {
  // .rx_buffer = i2c_isr_rx_buff,
  // .tx_buffer = i2c_isr_tx_buff,
  // .tx_buffer_size = sizeof(i2c_isr_tx_buff),
  // .rx_buffer_size = sizeof(i2c_isr_rx_buff),
  // };

  hdl_i2c_set_transceiver(&mod_i2c1, &i2c_transiver);
  // hdl_isr_buffer_write(&i2s_isr_buffer, i2c_isr_tx_buff, sizeof(i2c_isr_tx_buff));

  while (1)
  {
    cooperative_scheduler(false);
    if(p_pong_msg != NULL) {
      if((hdl_i2c_can_transfer(&mod_i2c1) == HDL_TRUE) && (hdl_i2c_transfer_message(&mod_i2c1, p_pong_msg))) {
        p_pong_msg = NULL;
      }
    }
  }
}

