#include "hdl_portable.h"
#include "Macros.h"

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

static uint32_t _hdl_gpio_port_to_num(GPIO_T *gpio_port) {
  switch ((uint32_t)gpio_port) {
    case GPIOA_BASE: return 0;
    case GPIOB_BASE: return 1;
    case GPIOC_BASE: return 2;
    case GPIOD_BASE: return 3;
    case GPIOE_BASE: return 4;
    case GPIOF_BASE: return 5;
    case GPIOG_BASE: return 6;
    case GPIOH_BASE: return 7;
    case GPIOI_BASE: return 8;
    case GPIOJ_BASE: return 9;
    default: return -1;
  }
}

hdl_module_state_t hdl_gpio_pin(void *desc, const uint8_t enable){
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  if (gpio->config->hwc == NULL || gpio->module.dependencies[0] == NULL || gpio->module.dependencies[0]->reg == NULL)
    return HDL_MODULE_FAULT;
  GPIO_T *gpio_port = (GPIO_T *)gpio->module.dependencies[0]->reg;
  uint32_t pin_no = (uint32_t)gpio->module.reg;
  uint32_t port_no = _hdl_gpio_port_to_num(gpio_port); 
  __IO uint32_t *MFP = (__IO uint32_t *)((&SYS->GPA_MFP0)[port_no * 4 + (pin_no >> 2)]);

  if(gpio->config->inactive_default == HDL_GPIO_LOW) {
    gpio_port->DOUT &= ~(1 << pin_no);
  }
  else {
    gpio_port->DOUT |= (1 << pin_no);
  }

  if(enable) {
    GPIO_SetMode(gpio_port, (1 << pin_no), gpio->config->hwc->func);
    GPIO_SetPullCtl(gpio_port, (1 << pin_no), gpio->config->hwc->pull_mode);
    GPIO_SetSlewCtl(gpio_port, (1 << pin_no), gpio->config->hwc->slew_mode);

    if(gpio->config->hwc->int_mode) {
      gpio_port->INTTYPE = (gpio_port->INTTYPE & ~(1 << pin_no)) | (((gpio->config->hwc->int_mode >> 24) & 0xFFUL) << pin_no);
      gpio_port->INTEN = (gpio_port->INTEN & ~(0x00010001ul << pin_no)) | ((gpio->config->hwc->int_mode & 0xFFFFFFUL) << pin_no);
    }
    GPIO_EnableInt(gpio_port, (1 << pin_no), gpio->config->hwc->int_mode);
    if(gpio->config->hwc->func_alternate != 0) {
      __IO uint32_t *MFOS = (__IO uint32_t *)((&SYS->GPA_MFOS)[port_no]);
      if(gpio->config->hwc->func == GPIO_MODE_OPEN_DRAIN) HDL_REG_SET(*MFOS, (0x1UL << pin_no));
      else HDL_REG_CLEAR(*MFOS, (0x1UL << pin_no));
    }
    HDL_REG_MODIFY(*MFP, 0x1f << (pin_no & 0x3), (gpio->config->hwc->func_alternate & 0x1f) << (pin_no & 0x3));
  }
  else {
    HDL_REG_CLEAR(*MFP, 0x1f << (pin_no & 0x3));
    GPIO_SetMode(gpio_port, (1 << pin_no), GPIO_MODE_QUASI);
    GPIO_SetPullCtl(gpio_port, (1 << pin_no), GPIO_PUSEL_DISABLE);
    GPIO_SetSlewCtl(gpio_port, (1 << pin_no), GPIO_SLEWCTL_NORMAL);
    GPIO_DisableInt(gpio_port, (1 << pin_no));
    return HDL_MODULE_UNLOADED;
  }

  return HDL_MODULE_ACTIVE;
}
