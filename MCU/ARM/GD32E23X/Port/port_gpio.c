#include "hdl_portable.h"

hdl_init_state_t hdl_gpio(void *desc, const uint8_t enable) {
  hdl_gpio_t *gpio = (hdl_gpio_t *)desc;
  if ((gpio == NULL) || (gpio->mode == NULL) || (gpio->port == NULL))
    return HDL_FALSE;
  if(!enable) {
    gpio_af_set(gpio->port->periph, GPIO_AF_0, gpio->pin);
    gpio_mode_set(gpio->port->periph, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, gpio->pin);
    return HDL_TRUE;
  }
  rcu_periph_enum rcu;
  switch (gpio->port->periph) {
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
      return -1;
  }
  rcu_periph_clock_enable(rcu);
  gpio_af_set(gpio->port->periph, gpio->mode->af, gpio->pin);
  gpio_mode_set(gpio->port->periph, gpio->mode->type, gpio->mode->pull, gpio->pin);
  gpio_output_options_set(gpio->port->periph, gpio->mode->otype, gpio->mode->ospeed, gpio->pin);
  return HDL_TRUE;
}

hdl_gpio_state hdl_gpio_read(const hdl_gpio_t *gpio) {
  return (gpio_input_bit_get(gpio->port->periph, gpio->pin) == RESET)? HDL_GPIO_LOW: HDL_GPIO_HIGH;
}

void hdl_gpio_write(const hdl_gpio_t *gpio, const hdl_gpio_state state) {
  gpio_bit_write(gpio->port->periph, gpio->pin, (state == HDL_GPIO_LOW)? RESET: SET);
}

void hdl_gpio_toggle(const hdl_gpio_t *gpio) {
    gpio_bit_toggle(gpio->port->periph, gpio->pin);
}
