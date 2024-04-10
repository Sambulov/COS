#include "hdl_portable.h"


hdl_init_state_t hdl_gpio_port(void *desc, const uint8_t enable) {
  if(desc == NULL)
    return HDL_HW_INIT_FAILED;

  hdl_gpio_port_t *port = (hdl_gpio_port_t *)desc;
  if(port->periph == NULL)
    return HDL_HW_INIT_FAILED;

  rcu_periph_enum rcu;
  switch (*(uint32_t*)port->periph)
  {
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
  rcu_periph_clock_enable(rcu);

  return HDL_HW_INIT_OK;
}

hdl_init_state_t hdl_gpio_pin(void *desc, const uint8_t enable){
  if (desc == NULL)
    return HDL_HW_INIT_FAILED;

  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  if (gpio->mode == NULL)
    return HDL_HW_INIT_FAILED;

  if (gpio->hw.dependencies[0] == NULL)
    return HDL_HW_INIT_FAILED;

  if(enable){
    gpio_af_set(*(uint32_t *)gpio->hw.dependencies[0]->periph, gpio->mode->af, gpio->pin);
    gpio_mode_set(*(uint32_t *)gpio->hw.dependencies[0]->periph, gpio->mode->type, gpio->mode->pull, gpio->pin);
    
    if(gpio->mode->otype == GPIO_OTYPE_PP || gpio->mode->otype == GPIO_OTYPE_OD)
      gpio_output_options_set(*(uint32_t *)gpio->hw.dependencies[0]->periph, gpio->mode->otype, gpio->mode->ospeed, gpio->pin);
  }
  else{
     gpio_af_set(*(uint32_t *)gpio->hw.dependencies[0]->periph, 0, gpio->pin);
     gpio_mode_set(*(uint32_t *)gpio->hw.dependencies[0]->periph, GPIO_MODE_INPUT, GPIO_PUPD_NONE, gpio->pin);
  }

  return HDL_HW_INIT_OK;
}

hdl_gpio_state hdl_gpio_read(const hdl_gpio_pin_t *gpio) {
  if (gpio->hw.dependencies[0] == NULL)
    return HDL_GPIO_LOW;

  return (gpio_input_bit_get(*(uint32_t *)gpio->hw.dependencies[0]->periph, gpio->pin) == RESET)? HDL_GPIO_LOW: HDL_GPIO_HIGH;
}

void hdl_gpio_write(const hdl_gpio_pin_t *gpio, const hdl_gpio_state state) {
  if (gpio->hw.dependencies[0] == NULL)
    return;

  gpio_bit_write(*(uint32_t *)gpio->hw.dependencies[0]->periph, gpio->pin, (state == HDL_GPIO_LOW)? RESET: SET);
}

void hdl_gpio_toggle(const hdl_gpio_pin_t *gpio) {
    if (gpio->hw.dependencies[0] == NULL)
      return;
    
    gpio_bit_toggle(*(uint32_t *)gpio->hw.dependencies[0]->periph, gpio->pin);
}
