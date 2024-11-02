#include "hdl_portable.h"

hdl_module_state_t hdl_gpio_port(void *desc, const uint8_t enable) {
  /* Casting desc to hdl_gpio_port_t* type */
  hdl_gpio_port_t *port = (hdl_gpio_port_t *)desc;
  if(port->module.reg == NULL)
    return HDL_MODULE_FAULT;
  if(enable)
    CLK->AHBCLK0 |= port->config->rcu;
  else{
    CLK->AHBCLK0 &= ~(port->config->rcu);
    return HDL_MODULE_UNLOADED;
  }
  return HDL_MODULE_ACTIVE;
}

hdl_gpio_state hdl_gpio_read(const hdl_gpio_pin_t *gpio){
  if (hdl_state(&gpio->module) != HDL_MODULE_ACTIVE)
    return HDL_GPIO_LOW;
  GPIO_T *gpio_port = (GPIO_T *)gpio->module.dependencies[0]->reg;
  return (gpio_port->PIN & (1 << (uint32_t)gpio->module.reg))? HDL_GPIO_HIGH: HDL_GPIO_LOW;
}

void hdl_gpio_write(const hdl_gpio_pin_t *gpio, const hdl_gpio_state state){
  if (hdl_state(&gpio->module) != HDL_MODULE_ACTIVE)
    return;
  GPIO_T *gpio_port = (GPIO_T *)gpio->module.dependencies[0]->reg;
  if(state == HDL_GPIO_LOW) {
    gpio_port->DOUT &= ~((1 << (uint32_t)gpio->module.reg));
  }
  else {
    gpio_port->DOUT |= (1 << (uint32_t)gpio->module.reg);
  }
}

hdl_gpio_state hdl_gpio_read_output(const hdl_gpio_pin_t *gpio) {
  if (hdl_state(&gpio->module) != HDL_MODULE_ACTIVE)
    return HDL_GPIO_LOW;
  GPIO_T *gpio_port = (GPIO_T *)gpio->module.dependencies[0]->reg;
  return (gpio_port->DOUT & (1 << (uint32_t)gpio->module.reg))? HDL_GPIO_LOW : HDL_GPIO_HIGH;
}

void hdl_gpio_toggle(const hdl_gpio_pin_t *gpio){
  if (hdl_state(&gpio->module) != HDL_MODULE_ACTIVE)
    return;
  GPIO_T *gpio_port = (GPIO_T *)gpio->module.dependencies[0]->reg;
  gpio_port->DOUT ^= (1 << (uint32_t)gpio->module.reg);
}


hdl_module_state_t hdl_gpio_pin(void *desc, const uint8_t enable){
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  if (gpio->config->hwc == NULL || gpio->module.dependencies[0] == NULL || gpio->module.dependencies[0]->reg == NULL)
    return HDL_MODULE_FAULT;
  GPIO_T *gpio_port = (GPIO_T *)gpio->module.dependencies[0]->reg;

  if(gpio->config->inactive_default == HDL_GPIO_LOW) {
    gpio_port->DOUT &= ~((1 << (uint32_t)gpio->module.reg));
  }
  else {
    gpio_port->DOUT |= (1 << (uint32_t)gpio->module.reg);
  }

  if(enable) {
    GPIO_SetMode(gpio_port, (1 << (uint32_t)gpio->module.reg), gpio->config->hwc->func);
    GPIO_SetPullCtl(gpio_port, (1 << (uint32_t)gpio->module.reg), gpio->config->hwc->pull_mode);
    GPIO_SetSlewCtl(gpio_port, (1 << (uint32_t)gpio->module.reg), gpio->config->hwc->slew_mode);
    if(gpio->config->hwc->int_mode) {
      gpio_port->INTTYPE = (gpio_port->INTTYPE & ~(1 << (uint32_t)gpio->module.reg)) | (((gpio->config->hwc->int_mode >> 24) & 0xFFUL) << (uint32_t)gpio->module.reg);
      gpio_port->INTEN = (gpio_port->INTEN & ~(0x00010001ul << (uint32_t)gpio->module.reg)) | ((gpio->config->hwc->int_mode & 0xFFFFFFUL) << (uint32_t)gpio->module.reg);
    }
      GPIO_EnableInt(gpio_port, (1 << (uint32_t)gpio->module.reg), gpio->config->hwc->int_mode);
  }
  else {
    GPIO_SetMode(gpio_port, (1 << (uint32_t)gpio->module.reg), GPIO_MODE_QUASI);
    GPIO_SetPullCtl(gpio_port, (1 << (uint32_t)gpio->module.reg), GPIO_PUSEL_DISABLE);
    GPIO_SetSlewCtl(gpio_port, (1 << (uint32_t)gpio->module.reg), GPIO_SLEWCTL_NORMAL);
    GPIO_DisableInt(gpio_port, (1 << (uint32_t)gpio->module.reg));
    return HDL_MODULE_UNLOADED;
  }

  return HDL_MODULE_ACTIVE;
}
