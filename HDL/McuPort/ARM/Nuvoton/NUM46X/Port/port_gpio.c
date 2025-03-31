#include "hdl_iface.h"

static hdl_module_state_t _hdl_gpio_port(const void *desc, const uint8_t enable) {
  /* Casting desc to hdl_gpio_port_mcu_t* type */
  hdl_gpio_port_mcu_t *port = (hdl_gpio_port_mcu_t *)desc;
  if(!port->config->phy) return HDL_MODULE_FAULT;
  if(enable) CLK->AHBCLK0 |= port->config->rcu;
  else {
    CLK->AHBCLK0 &= ~(port->config->rcu);
    return HDL_MODULE_UNLOADED;
  }
  return HDL_MODULE_ACTIVE;
}

static hdl_gpio_state _hdl_gpio_read(const void *desc){
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  GPIO_T *gpio_port = (GPIO_T *)((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  return (gpio_port->PIN & (1 << (uint32_t)gpio->config->pin))? HDL_GPIO_HIGH: HDL_GPIO_LOW;
}

static void _hdl_gpio_write(const void *desc, const hdl_gpio_state state){
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  GPIO_T *gpio_port = (GPIO_T *)((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  if(state == HDL_GPIO_LOW) {
    gpio_port->DOUT &= ~((1 << (uint32_t)gpio->config->pin));
  }
  else {
    gpio_port->DOUT |= (1 << (uint32_t)gpio->config->pin);
  }
}

static hdl_gpio_state _hdl_gpio_read_output(const void *desc) {
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  GPIO_T *gpio_port = (GPIO_T *)((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  return (gpio_port->DOUT & (1 << (uint32_t)gpio->config->pin))? HDL_GPIO_LOW : HDL_GPIO_HIGH;
}

static void _hdl_gpio_toggle(const void *desc){
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  GPIO_T *gpio_port = (GPIO_T *)((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  gpio_port->DOUT ^= (1 << (uint32_t)gpio->config->pin);
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

static hdl_module_state_t _hdl_gpio_pin(const void *desc, const uint8_t enable){
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  hdl_gpio_pin_hw_config_t *hwc = (hdl_gpio_pin_hw_config_t *)gpio->obj_var;
  if ((hwc == NULL) || (gpio->dependencies == NULL) || (gpio->dependencies[0] == NULL))
    return HDL_MODULE_FAULT;
  GPIO_T *gpio_port = (GPIO_T *)((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  uint32_t pin_no = (uint32_t)gpio->config->pin;
  uint32_t port_no = _hdl_gpio_port_to_num(gpio_port); 

  if(gpio->config->inactive_default == HDL_GPIO_LOW) {
    gpio_port->DOUT &= ~(1 << pin_no);
  }
  else {
    gpio_port->DOUT |= (1 << pin_no);
  }

  if(enable) {
    GPIO_SetMode(gpio_port, (1 << pin_no), hwc->func);
    GPIO_SetPullCtl(gpio_port, (1 << pin_no), hwc->pull_mode);
    GPIO_SetSlewCtl(gpio_port, (1 << pin_no), hwc->slew_mode);
    GPIO_EnableInt(gpio_port, pin_no, hwc->int_mode);
    __IO uint32_t *MFP = (__IO uint32_t *)&((&(SYS->GPA_MFP0))[port_no * 4 + (pin_no >> 2)]);
    if(hwc->func_alternate != 0) {
      __IO uint32_t *MFOS = (__IO uint32_t *)&((&SYS->GPA_MFOS)[port_no]);
      if(hwc->func == GPIO_MODE_OPEN_DRAIN) CL_REG_SET(*MFOS, (0x1UL << pin_no));
      else CL_REG_CLEAR(*MFOS, (0x1UL << pin_no));
    }
    uint32_t mfp_cnf_offset = 8 * (pin_no & 0x3);
    CL_REG_MODIFY(*MFP, 0x1f << mfp_cnf_offset, (hwc->func_alternate & 0x1f) << mfp_cnf_offset);
  }
  else {
    GPIO_SetMode(gpio_port, (1 << pin_no), GPIO_MODE_QUASI);
    GPIO_SetPullCtl(gpio_port, (1 << pin_no), GPIO_PUSEL_DISABLE);
    GPIO_SetSlewCtl(gpio_port, (1 << pin_no), GPIO_SLEWCTL_NORMAL);
    GPIO_DisableInt(gpio_port, pin_no);
    return HDL_MODULE_UNLOADED;
  }

  return HDL_MODULE_ACTIVE;
}

const hdl_module_base_iface_t hdl_gpio_port_iface = {
  .init = &_hdl_gpio_port
};

const hdl_gpio_pin_iface_t hdl_gpio_pin_iface = {
  .init = &_hdl_gpio_pin,
  .read = &_hdl_gpio_read,
  .read_ouput = &_hdl_gpio_read_output,
  .toggle = &_hdl_gpio_toggle,
  .write = &_hdl_gpio_write
};
