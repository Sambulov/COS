#include "hdl_portable.h"

static volatile uint32_t ticks = 0;

void SysTick_Handler() {
  ticks++;
}

void hdl_gpio_init(const hdl_gpio_t *gpio) {
  if ((gpio == NULL) || (gpio->mode == NULL))
    return;
  rcu_periph_clock_enable(gpio->rcu);
  gpio_af_set(gpio->port, gpio->mode->af, gpio->pin);
  gpio_mode_set(gpio->port, gpio->mode->type, gpio->mode->pull, gpio->pin);
  gpio_output_options_set(gpio->port, gpio->mode->otype, gpio->mode->ospeed, gpio->pin);
}

hdl_gpio_state hdl_gpio_read(const hdl_gpio_t *gpio) {
  return (gpio_input_bit_get(gpio->port, gpio->pin) == RESET)? HDL_GPIO_LOW: HDL_GPIO_HIGH;
}

void hdl_gpio_write(const hdl_gpio_t *gpio, const hdl_gpio_state state) {
  gpio_bit_write(gpio->port, gpio->pin, (state == HDL_GPIO_LOW)? RESET: SET);
}

void hdl_gpio_toggle(const hdl_gpio_t *gpio) {
    gpio_bit_toggle(gpio->port, gpio->pin);
}

uint32_t hdl_millis() {
  return ticks;
}

void hdl_system_init(const hdl_sys_t *desc) {
  SysTick_Config(desc->ticks);
}
