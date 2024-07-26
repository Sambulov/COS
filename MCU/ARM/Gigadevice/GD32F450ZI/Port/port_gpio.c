#include "hdl_portable.h"

hdl_module_state_t hdl_gpio_port(void *desc, const uint8_t enable) {
  /* Casting desc to hdl_gpio_port_t* type */
  hdl_gpio_port_t *port = (hdl_gpio_port_t *)desc;
  if(port->module.reg == NULL)
    return HDL_MODULE_INIT_FAILED;
  if(enable)
    rcu_periph_clock_enable(port->config->hwc->rcu);
  else{
    rcu_periph_clock_disable(port->config->hwc->rcu);
    return HDL_MODULE_DEINIT_OK;
  }
    
  return HDL_MODULE_INIT_OK;
}

hdl_module_state_t hdl_gpio_pin(void *desc, const uint8_t enable){
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  if (gpio->config->hwc == NULL || gpio->module.dependencies[0] == NULL || gpio->module.dependencies[0]->reg == NULL)
    return HDL_MODULE_INIT_FAILED;
  /* gpio_port it`s GPIOx(x = A,B,C) */
  uint32_t gpio_port = (uint32_t)gpio->module.dependencies[0]->reg;
  gpio_bit_write(gpio_port, (uint32_t)gpio->module.reg, (gpio->config->inactive_default == HDL_GPIO_LOW) ? RESET : SET);
  if(enable) {
    gpio_af_set(gpio_port, gpio->config->hwc->af, (uint32_t)gpio->module.reg);
    gpio_mode_set(gpio_port, gpio->config->hwc->type, gpio->config->hwc->pull, (uint32_t)gpio->module.reg);
    
    if(gpio->config->hwc->otype == GPIO_OTYPE_PP || gpio->config->hwc->otype == GPIO_OTYPE_OD)
      gpio_output_options_set(gpio_port, gpio->config->hwc->otype, gpio->config->hwc->ospeed, (uint32_t)gpio->module.reg);
  }
  else{
     gpio_af_set(gpio_port, 0, (uint32_t)gpio->module.reg);
     gpio_mode_set(gpio_port, GPIO_MODE_INPUT, GPIO_PUPD_NONE, (uint32_t)gpio->module.reg);
     return HDL_MODULE_DEINIT_OK;
  }

  return HDL_MODULE_INIT_OK;
}
