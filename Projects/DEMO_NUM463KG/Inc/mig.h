#ifndef MIG_ATB_RK3568J_SMC_H_
#define MIG_ATB_RK3568J_SMC_H_

extern hdl_time_counter_t mod_timer_ms;

extern hdl_gpio_pin_t mod_gpio_pin_btn_0;
extern hdl_gpio_pin_t mod_gpio_pin_btn_1;

extern hdl_gpio_pin_t mod_gpio_pin_led_r;
extern hdl_gpio_pin_t mod_gpio_pin_led_y;
extern hdl_gpio_pin_t mod_gpio_pin_led_g;

extern hdl_interrupt_controller_t mod_interrupt_controller;
extern hdl_interrupt_t mod_irq_gpio_btn_port;

extern hdl_i2c_t mod_i2c0;
extern hdl_i2c_t mod_i2c1;

extern hdl_nvm_t mod_flash;

#endif // MIG_ATB_RK3568J_SMC_H_