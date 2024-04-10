#include "hdl_portable.h"


hdl_init_state_t hdl_gpio_port(void *desc, const uint8_t enable) {
  /* Casting desc to hdl_gpio_port_t* type */
  hdl_gpio_port_t *port = (hdl_gpio_port_t *)desc;
  if(port->periph == NULL)
    return HDL_HW_INIT_FAILED;

  rcu_periph_enum rcu;
  switch ((uint32_t)port->periph){
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
      return HDL_HW_INIT_FAILED;
  }

  if(enable)
    rcu_periph_clock_enable(rcu);
  else
    rcu_periph_clock_disable(rcu);

  return HDL_HW_INIT_OK;
}

hdl_init_state_t hdl_gpio_pin(void *desc, const uint8_t enable){
  /* Casting desc to hdl_gpio_pin_t* type */
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;

  if (gpio->mode == NULL || gpio->hw.dependencies[0] == NULL || gpio->hw.dependencies[0]->periph == NULL)
    return HDL_HW_INIT_FAILED;
  /* gpio_port it`s GPIOx(x = A,B,C) */
  uint32_t gpio_port = (uint32_t)gpio->hw.dependencies[0]->periph;
  if(enable){
    gpio_af_set(gpio_port, gpio->mode->af, gpio->pin);
    gpio_mode_set(gpio_port, gpio->mode->type, gpio->mode->pull, gpio->pin);
    
    if(gpio->mode->otype == GPIO_OTYPE_PP || gpio->mode->otype == GPIO_OTYPE_OD)
      gpio_output_options_set(gpio_port, gpio->mode->otype, gpio->mode->ospeed, gpio->pin);
  }
  else{
     gpio_af_set(gpio_port, 0, gpio->pin);
     gpio_mode_set(gpio_port, GPIO_MODE_INPUT, GPIO_PUPD_NONE, gpio->pin);
  }

  return HDL_HW_INIT_OK;
}

hdl_gpio_state hdl_gpio_read(const hdl_gpio_pin_t *gpio){
  if (gpio->hw.dependencies[0] == NULL || gpio->hw.dependencies[0]->periph == NULL)
    return HDL_GPIO_LOW;
  /* gpio_port it`s GPIOx(x = A,B,C) */
  uint32_t gpio_port = (uint32_t)gpio->hw.dependencies[0]->periph;
  return (gpio_input_bit_get(gpio_port, gpio->pin) == RESET) ? HDL_GPIO_LOW : HDL_GPIO_HIGH;
}

void hdl_gpio_write(const hdl_gpio_pin_t *gpio, const hdl_gpio_state state){
  if (gpio->hw.dependencies[0] == NULL || gpio->hw.dependencies[0]->periph == NULL)
    return;
  /* gpio_port it`s GPIOx(x = A,B,C) */
  uint32_t gpio_port = (uint32_t)gpio->hw.dependencies[0]->periph;
  gpio_bit_write(gpio_port, gpio->pin, (state == HDL_GPIO_LOW) ? RESET : SET);
}

void hdl_gpio_toggle(const hdl_gpio_pin_t *gpio){
  if (gpio->hw.dependencies[0] == NULL || gpio->hw.dependencies[0]->periph == NULL)
    return;
  /* gpio_port it`s GPIOx(x = A,B,C) */
  uint32_t gpio_port = (uint32_t)gpio->hw.dependencies[0]->periph;
  gpio_bit_toggle(gpio_port, gpio->pin);
}
