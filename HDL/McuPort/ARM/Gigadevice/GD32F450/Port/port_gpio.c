#include "hdl_iface.h"

static hdl_module_state_t _hdl_gpio_port(const void *desc, const uint8_t enable) {
  /* Casting desc to hdl_gpio_port_mcu_t* type */
  hdl_gpio_port_mcu_t *port = (hdl_gpio_port_mcu_t *)desc;
  if(!port->config->phy)
    return HDL_MODULE_FAULT;
  if(enable)
    rcu_periph_clock_enable(port->config->rcu);
  else{
    rcu_periph_clock_disable(port->config->rcu);
    return HDL_MODULE_UNLOADED;
  }
    
  return HDL_MODULE_ACTIVE;
}

hdl_module_state_t __hdl_gpio_pin(const void *desc, const uint8_t enable){
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  if ((gpio->config->hwc == NULL) || (gpio->dependencies == NULL) || (gpio->dependencies[0] == NULL))
    return HDL_MODULE_FAULT;

  uint32_t gpio_port = ((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  hdl_gpio_pin_hw_config_t *gpio_hwc = (hdl_gpio_pin_hw_config_t *)gpio->config->hwc;
  
  gpio_bit_write(gpio_port, gpio->config->pin, (gpio->config->inactive_default == HDL_GPIO_LOW) ? RESET : SET);
  if(enable) {
    gpio_af_set(gpio_port, gpio_hwc->af, (uint32_t)gpio->config->pin);
    gpio_mode_set(gpio_port, gpio_hwc->type, gpio_hwc->pull, gpio->config->pin);
    gpio_output_options_set(gpio_port, gpio_hwc->otype, gpio_hwc->ospeed, gpio->config->pin);
  }
  else{
     gpio_af_set(gpio_port, 0, (uint32_t)gpio->config->pin);
     gpio_mode_set(gpio_port, GPIO_MODE_INPUT, GPIO_PUPD_NONE, (uint32_t)gpio->config->pin);
     return HDL_MODULE_UNLOADED;
  }

  return HDL_MODULE_ACTIVE;
}

const hdl_module_base_iface_t hdl_gpio_port_iface = {
  .init = &_hdl_gpio_port
};
