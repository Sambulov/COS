#include "app.h"
#include "CodeLib.h"

extern hdl_time_counter_t mod_timer_ms;
extern hdl_gpio_pin_t mod_gpo_led;
extern hdl_gpio_pin_t mod_gpi_button;
extern hdl_exti_controller_t mod_exti;
extern hdl_interrupt_controller_t mod_nvic;
extern hdl_i2c_t mod_i2c1;
extern hdl_tick_counter_t mod_systick_counter;

hdl_timer_t sw_timer = {
  .module.init = hdl_timer,
  .module.dependencies = hdl_module_dependencies(&mod_timer_ms.module)
};

const hdl_button_config_t button_cnf = {  
  .debounce_delay = 30,
  .hold_delay = 3000
};

hdl_button_t button = {
  .module.dependencies = hdl_module_dependencies(&mod_gpi_button.module, &mod_timer_ms.module),
  .module.init = &hdl_button,
  .config = &button_cnf
};

typedef struct {
  uint32_t msg_no;
  uint32_t msg_type;  
} ping_pong_data_t;

#define MSG_PING_TYPE    10
#define MSG_PONG_TYPE    20

ping_pong_data_t i2c_msg_ping_data = {.msg_type = MSG_PING_TYPE, .msg_no = 0};
ping_pong_data_t i2c_msg_pong_data = {.msg_type = MSG_PONG_TYPE, .msg_no = 0};
ping_pong_data_t i2c_msg_rx_data;
hdl_i2c_message_t *pong_msg = NULL;

hdl_i2c_message_t i2c_msg_ping = {
  .address = 0x22,
  .buffer = (uint8_t*)&i2c_msg_ping_data,
  .length = sizeof(i2c_msg_ping_data),
  .options =  HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR | HDL_I2C_MESSAGE_STOP,
};

hdl_i2c_message_t i2c_msg_pong = {
  .address = 0x22,
  .buffer = (uint8_t*)&i2c_msg_pong_data,
  .length = sizeof(i2c_msg_pong_data),
  .options =  HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR | HDL_I2C_MESSAGE_STOP,
};

uint32_t castom_rand(uint32_t max) {
  static uint32_t state = 0;
  static uint8_t init = 0;
  if(!init) {
    state = (hdl_tick_counter_get(&mod_systick_counter)) ^ 0xff15ea13;
    init = 1;
  }
  state = 214013 * state + 2531011;
  state ^= state >> 15;
  return (uint32_t) (state % max);
}

void sw_timer_handler(uint32_t event_trigger, void *sender, void *context) {
  hdl_timer_t *timer = (hdl_timer_t *)sender;
  if(!hdl_i2c_transfer_message(&mod_i2c1, &i2c_msg_ping)) {
    hdl_timer_set(timer, 1, HDL_TIMER_EVENT_SINGLE);
  }
  else {
    hdl_timer_set(timer, castom_rand(1000), HDL_TIMER_EVENT_SINGLE);
    hdl_gpio_toggle(&mod_gpo_led);
  }
}

hdl_delegate_t sw_timer_deleagate = {
  .handler = sw_timer_handler,
  .context = NULL,
};

/****************** Transiver ******************* */
uint8_t global_index = 0;

uint32_t transceiver_rx_callback(void *context, uint8_t *data, uint32_t count) {
  ((uint8_t*)&i2c_msg_rx_data)[global_index] = *data;
  global_index++;
}

uint32_t transceiver_rx_available_callback(void *context) {
  return sizeof(ping_pong_data_t) - global_index;
}

void transceiver_end_of_transmission(void *context) {
  if(i2c_msg_rx_data.msg_type == MSG_PING_TYPE) {
    i2c_msg_pong_data.msg_no = i2c_msg_rx_data.msg_no;
    pong_msg = &i2c_msg_pong;
  }
  else if(i2c_msg_rx_data.msg_type == MSG_PONG_TYPE) {
    if(i2c_msg_rx_data.msg_no == i2c_msg_ping_data.msg_no) {
      i2c_msg_ping_data.msg_no++;
    }
  }
  global_index = 0;
}

hdl_transceiver_t i2c_transiver = {
  .end_of_transmission = transceiver_end_of_transmission,
  .rx_available = transceiver_rx_available_callback,
  .rx_data = transceiver_rx_callback,
  .tx_empty = NULL,
  .proto_context = NULL,
  .tx_available = NULL,
};

void i2c_pingpong() {
  hdl_enable(&mod_timer_ms.module);
  hdl_enable(&mod_i2c1.module);
  hdl_enable(&mod_gpo_led.module);
  hdl_enable(&sw_timer.module);
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  hdl_event_subscribe(&sw_timer.event, &sw_timer_deleagate);
  hdl_timer_set(&sw_timer, 10000, HDL_TIMER_EVENT_SINGLE);

  hdl_i2c_set_transceiver(&mod_i2c1, &i2c_transiver);

  while (1) {
    cooperative_scheduler(false);
    if((pong_msg != NULL) && hdl_i2c_transfer_message(&mod_i2c1, pong_msg)) {
      pong_msg = NULL;
    }
  }
}

void led_timer_handler(uint32_t event_trigger, void *sender, void *context) {
  static uint8_t short_blink = 0;
  short_blink = !short_blink;
  hdl_gpio_toggle(&mod_gpo_led);
  if(short_blink) {
    hdl_timer_set(&sw_timer, 50, HDL_TIMER_EVENT_SINGLE);
  }
  else {
    hdl_timer_set(&sw_timer, 950, HDL_TIMER_EVENT_SINGLE);
  }
}

void button_handler(uint32_t event_trigger, void *sender, void *context) {
  if((event_trigger == HDL_BTN_EVENT_CLICK) || (event_trigger == HDL_BTN_EVENT_HOLD)) {
    hdl_gpio_toggle(&mod_gpo_led);
  }
}

void test_svc(uint32_t event_trigger, void *sender, void *context) {
  hdl_gpio_toggle(&mod_gpo_led);
}

void button_int() {
  hdl_delegate_t sw_timer_deleagate = {
    .handler = led_timer_handler,
    .context = NULL,
  };
  hdl_delegate_t button_deleagate = {
    .handler = button_handler,
    .context = NULL,
  };
  hdl_delegate_t svc_deleagate = {
    .handler = test_svc,
    .context = NULL,
  };
  hdl_enable(&mod_timer_ms.module);
  hdl_enable(&mod_gpo_led.module);
  hdl_enable(&sw_timer.module);
  hdl_enable(&button.module);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  extern hdl_interrupt_t mod_irq_svc;
  hdl_event_subscribe(&mod_irq_svc.event, &svc_deleagate);
  hdl_interrupt_request(&mod_nvic, &mod_irq_svc);
  asm("SVC 44");
  hdl_event_subscribe(&sw_timer.event, &sw_timer_deleagate);
  hdl_event_subscribe(&button.event, &button_deleagate);
  hdl_timer_set(&sw_timer, 1000, HDL_TIMER_EVENT_SINGLE);
  while (1) {
    cooperative_scheduler(false);
  }
}

void main() {
  //i2c_pingpong();
  button_int();
}
