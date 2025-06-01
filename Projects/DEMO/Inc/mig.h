#ifndef ATB_MIG_H_
#define ATB_MIG_H_

extern const hdl_interrupt_controller_t mod_ic;

extern const hdl_time_counter_t mod_timer_ms;

extern const hdl_tick_counter_t mod_tick_counter;

extern const hdl_gpio_pin_t mod_output1_pin;
extern const hdl_gpio_pin_t mod_output2_pin;

extern const hdl_gpio_pin_t mod_led1_pin;
extern const hdl_gpio_pin_t mod_led2_pin;
extern const hdl_gpio_pin_t mod_led3_pin;
extern const hdl_gpio_pin_t mod_led4_pin;
extern const hdl_gpio_pin_t mod_button_pin;

extern const hdl_uart_t mod_uart;
extern const hdl_uart_t mod_rs485;

extern const hdl_spi_client_ch_t mod_spi_client;

extern const hdl_i2c_t mod_i2c;

#endif // ATB_MIG_H_
