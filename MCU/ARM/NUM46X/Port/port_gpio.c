#include "hdl_portable.h"

hdl_module_state_t hdl_gpio_port(void *desc, const uint8_t enable) {
  /* Casting desc to hdl_gpio_port_t* type */
  hdl_gpio_port_t *port = (hdl_gpio_port_t *)desc;
  if(port->reg == NULL)
    return HDL_MODULE_INIT_FAILED;
  uint32_t rcu;
  switch((uint32_t)port->reg) {
    case (uint32_t)PA:
      rcu = CLK_AHBCLK0_GPACKEN_Msk;
    break;
    case (uint32_t)PB:
      rcu = CLK_AHBCLK0_GPBCKEN_Msk;
    break;
    case (uint32_t)PC:
      rcu = CLK_AHBCLK0_GPCCKEN_Msk;
    break;
    case (uint32_t)PD:
      rcu = CLK_AHBCLK0_GPDCKEN_Msk;
    break;
    case (uint32_t)PE:
      rcu = CLK_AHBCLK0_GPECKEN_Msk;
    break;
    case (uint32_t)PF:
      rcu = CLK_AHBCLK0_GPFCKEN_Msk;
    break;
    case (uint32_t)PG:
      rcu = CLK_AHBCLK0_GPGCKEN_Msk;
    break;
    case (uint32_t)PH:
      rcu = CLK_AHBCLK0_GPHCKEN_Msk;
    break;
    default:
      return HDL_MODULE_INIT_FAILED;
  }

  if(enable)
    CLK->AHBCLK0 |= rcu;
  else{
    CLK->AHBCLK0 &= ~rcu;
    return HDL_MODULE_DEINIT_OK;
  }
  return HDL_MODULE_INIT_OK;
}

hdl_gpio_state hdl_gpio_read(const hdl_gpio_pin_t *gpio){
  if (hdl_state(&gpio->module) != HDL_MODULE_INIT_OK)
    return HDL_GPIO_LOW;
  GPIO_T *gpio_port = (GPIO_T *)gpio->module.dependencies[0]->reg;
  return (gpio_port->PIN & (uint32_t)gpio->module.reg)? HDL_GPIO_HIGH: HDL_GPIO_LOW;
}

void hdl_gpio_write(const hdl_gpio_pin_t *gpio, const hdl_gpio_state state){
  if (hdl_state(&gpio->module) != HDL_MODULE_INIT_OK)
    return;
  GPIO_T *gpio_port = (GPIO_T *)gpio->module.dependencies[0]->reg;
  if(state == HDL_GPIO_LOW) {
    gpio_port->DOUT &= ~((uint32_t)gpio->module.reg);
  }
  else {
    gpio_port->DOUT |= (uint32_t)gpio->module.reg;
  }
}

hdl_gpio_state hdl_gpio_read_output(const hdl_gpio_pin_t *gpio) {
  if (hdl_state(&gpio->module) != HDL_MODULE_INIT_OK)
    return HDL_GPIO_LOW;
  GPIO_T *gpio_port = (GPIO_T *)gpio->module.dependencies[0]->reg;
  return (gpio_port->DOUT & (uint32_t)gpio->module.reg)? HDL_GPIO_LOW : HDL_GPIO_HIGH;
}

void hdl_gpio_toggle(const hdl_gpio_pin_t *gpio){
  if (hdl_state(&gpio->module) != HDL_MODULE_INIT_OK)
    return;
  GPIO_T *gpio_port = (GPIO_T *)gpio->module.dependencies[0]->reg;
  gpio_port->DOUT ^= (uint32_t)gpio->module.reg;
}


hdl_module_state_t hdl_gpio_pin(void *desc, const uint8_t enable){
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  if (gpio->mode == NULL || gpio->module.dependencies[0] == NULL || gpio->module.dependencies[0]->reg == NULL)
    return HDL_MODULE_INIT_FAILED;
  GPIO_T *gpio_port = (GPIO_T *)gpio->module.dependencies[0]->reg;

  if(gpio->inactive_default == HDL_GPIO_LOW) {
    gpio_port->DOUT &= ~((uint32_t)gpio->module.reg);
  }
  else {
    gpio_port->DOUT |= (uint32_t)gpio->module.reg;
  }

  if(enable) {
    GPIO_SetMode(gpio_port, (uint32_t)gpio->module.reg, gpio->mode->func);
    GPIO_SetPullCtl(gpio_port, (uint32_t)gpio->module.reg, gpio->mode->pull_mode);
    GPIO_SetSlewCtl(gpio_port, (uint32_t)gpio->module.reg, gpio->mode->slew_mode);
  }
  else{
    GPIO_SetMode(gpio_port, (uint32_t)gpio->module.reg, GPIO_MODE_QUASI);
    GPIO_SetPullCtl(gpio_port, (uint32_t)gpio->module.reg, GPIO_PUSEL_DISABLE);
    GPIO_SetSlewCtl(gpio_port, (uint32_t)gpio->module.reg, GPIO_SLEWCTL_NORMAL);
    return HDL_MODULE_DEINIT_OK;
  }

  return HDL_MODULE_INIT_OK;
}
