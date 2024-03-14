#include "hdl_portable.h"

void hdl_gpio_deinit(const hdl_gpio_t *gpio) {
  gpio_af_set(gpio->port, GPIO_AF_0, gpio->pin);
  gpio_mode_set(gpio->port, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, gpio->pin);
}

void hdl_gpio_set_mode(const hdl_gpio_t *gpio, const hdl_gpio_mode_t *mode) {
  if ((gpio == NULL) || (mode == NULL))
    return;
  gpio_af_set(gpio->port, gpio->mode->af, gpio->pin);
  gpio_mode_set(gpio->port, gpio->mode->type, gpio->mode->pull, gpio->pin);
  gpio_output_options_set(gpio->port, gpio->mode->otype, gpio->mode->ospeed, gpio->pin);
}

void hdl_gpio_init(const hdl_gpio_t *gpio) {
  if (gpio == NULL)
    return;
  rcu_periph_enum rcu;
  switch (gpio->port) {
    case GPIOA:
      rcu = RCU_GPIOA;
      break;
    case GPIOB:
      rcu = RCU_GPIOB;
      break;
    case GPIOC:
      rcu = RCU_GPIOC;
      break;
    case GPIOF:
      rcu = RCU_GPIOF;
      break;    
    default:
      return;
  }
  rcu_periph_clock_enable(rcu);
  hdl_gpio_set_mode(gpio, gpio->mode);
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

