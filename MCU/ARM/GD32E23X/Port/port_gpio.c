#include "hdl_portable.h"

hdl_module_state_t hdl_gpio_port(void *desc, const uint8_t enable) {
  /* Casting desc to hdl_gpio_port_t* type */
  hdl_gpio_port_t *port = (hdl_gpio_port_t *)desc;
  if(port->reg == NULL)
    return HDL_MODULE_INIT_FAILED;

  rcu_periph_enum rcu;
  switch ((uint32_t)port->reg){
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
      return HDL_MODULE_INIT_FAILED;
  }

  if(enable)
    rcu_periph_clock_enable(rcu);
  else{
    rcu_periph_clock_disable(rcu);
    return HDL_MODULE_DEINIT_OK;
  }
    
  return HDL_MODULE_INIT_OK;
}

hdl_module_state_t hdl_gpio_pin(void *desc, const uint8_t enable){
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  if (gpio->mode == NULL || gpio->module.dependencies[0] == NULL || gpio->module.dependencies[0]->reg == NULL)
    return HDL_MODULE_INIT_FAILED;
  /* gpio_port it`s GPIOx(x = A,B,C) */
  uint32_t gpio_port = (uint32_t)gpio->module.dependencies[0]->reg;
  if(enable) {
    gpio_af_set(gpio_port, gpio->mode->af, (uint32_t)gpio->module.reg);
    gpio_mode_set(gpio_port, gpio->mode->type, gpio->mode->pull, (uint32_t)gpio->module.reg);
    
    if(gpio->mode->otype == GPIO_OTYPE_PP || gpio->mode->otype == GPIO_OTYPE_OD)
      gpio_output_options_set(gpio_port, gpio->mode->otype, gpio->mode->ospeed, (uint32_t)gpio->module.reg);
  }
  else{
     gpio_af_set(gpio_port, 0, (uint32_t)gpio->module.reg);
     gpio_mode_set(gpio_port, GPIO_MODE_INPUT, GPIO_PUPD_NONE, (uint32_t)gpio->module.reg);
     return HDL_MODULE_DEINIT_OK;
  }

  return HDL_MODULE_INIT_OK;
}

hdl_gpio_state hdl_gpio_read(const hdl_gpio_pin_t *gpio){
  if (gpio->module.dependencies[0] == NULL || gpio->module.dependencies[0]->reg == NULL)
    return HDL_GPIO_LOW;
  /* gpio_port it`s GPIOx(x = A,B,C) */
  uint32_t gpio_port = (uint32_t)gpio->module.dependencies[0]->reg;
  return (gpio_input_bit_get(gpio_port, (uint32_t)gpio->module.reg) == RESET) ? HDL_GPIO_LOW : HDL_GPIO_HIGH;
}

void hdl_gpio_write(const hdl_gpio_pin_t *gpio, const hdl_gpio_state state){
  if (gpio->module.dependencies[0] == NULL || gpio->module.dependencies[0]->reg == NULL)
    return;
  /* gpio_port it`s GPIOx(x = A,B,C) */
  uint32_t gpio_port = (uint32_t)gpio->module.dependencies[0]->reg;
  gpio_bit_write(gpio_port, (uint32_t)gpio->module.reg, (state == HDL_GPIO_LOW) ? RESET : SET);
}

void hdl_gpio_toggle(const hdl_gpio_pin_t *gpio){
  if (gpio->module.dependencies[0] == NULL || gpio->module.dependencies[0]->reg == NULL)
    return;
  /* gpio_port it`s GPIOx(x = A,B,C) */
  uint32_t gpio_port = (uint32_t)gpio->module.dependencies[0]->reg;
  gpio_bit_toggle(gpio_port, (uint32_t)gpio->module.reg);
}
