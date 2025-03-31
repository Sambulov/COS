#include "hdl_iface.h"

static hdl_module_state_t _hdl_gpio_port(const void *desc, const uint8_t enable) {
  hdl_gpio_port_mcu_t *port = (hdl_gpio_port_mcu_t *)desc;
  if(!port->config->phy) return HDL_MODULE_FAULT;
  if(enable) {
    CL_REG_SET(RCC->AHB1ENR, port->config->rcu);
    return HDL_MODULE_ACTIVE;
  }
	else CL_REG_CLEAR(RCC->AHB1ENR, port->config->rcu);
  return HDL_MODULE_UNLOADED;
}

static hdl_gpio_state _hdl_gpio_read(const void *desc){
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  GPIO_TypeDef *port = (GPIO_TypeDef *)((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;

  return (CL_REG_GET(port->IDR, gpio->config->pin)? HDL_GPIO_HIGH: HDL_GPIO_LOW);
}

static void _hdl_gpio_write(const void *desc, const hdl_gpio_state state){
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  GPIO_TypeDef *port = (GPIO_TypeDef *)((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  if(state == HDL_GPIO_LOW) CL_REG_SET(port->BSRR, (gpio->config->pin << 16));
  else CL_REG_SET(port->BSRR, gpio->config->pin);
}

static hdl_gpio_state _hdl_gpio_read_output(const void *desc) {
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  GPIO_TypeDef *port = (GPIO_TypeDef *)((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  return (CL_REG_GET(port->ODR, gpio->config->pin)? HDL_GPIO_HIGH: HDL_GPIO_LOW);
}

static void _hdl_gpio_toggle(const void *desc){
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  GPIO_TypeDef *port = (GPIO_TypeDef *)((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  port->ODR ^= gpio->config->pin;
}

static void _set_gpio_af(hdl_gpio_pin_t *gpio, uint32_t af) {
  GPIO_TypeDef *port = (GPIO_TypeDef *)((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  volatile uint32_t *afr_reg = &port->AFR[0];
  uint32_t afr = (gpio->config->pin * gpio->config->pin);
  if(gpio->config->pin >= GPIO_PIN_8) {
    afr_reg = &port->AFR[1];
    afr >>= 16;
  }
  uint32_t afr_mask = 0xf * (afr * afr);
  afr = af * (afr * afr);
  CL_REG_MODIFY(*afr_reg, afr_mask, afr);
}

static hdl_module_state_t _hdl_gpio_pin(const void *desc, const uint8_t enable){
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  if ((gpio->dependencies == NULL) || (gpio->dependencies[0] == NULL))
    return HDL_MODULE_FAULT;
  GPIO_TypeDef *port = (GPIO_TypeDef *)((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  hdl_gpio_pin_hw_config_t *hwc = (hdl_gpio_pin_hw_config_t *)gpio->config->hwc;
  if(enable) {
    if(gpio->config->inactive_default == HDL_GPIO_HIGH) CL_REG_SET(port->ODR, gpio->config->pin);
    else CL_REG_CLEAR(port->ODR, gpio->config->pin);
    uint32_t offset_2bits = (gpio->config->pin * gpio->config->pin);
    uint32_t mask_2bits = 0x3 * offset_2bits;
    CL_REG_MODIFY(port->OSPEEDR, mask_2bits, hwc->ospeed * offset_2bits);
    CL_REG_MODIFY(port->PUPDR, mask_2bits, hwc->pull * offset_2bits);
    CL_REG_MODIFY(port->OTYPER, gpio->config->pin, hwc->otype);
    _set_gpio_af(gpio, hwc->af);
    CL_REG_MODIFY(port->MODER, mask_2bits, hwc->mode * offset_2bits);
  }
  else {
    _set_gpio_af(gpio, 0);
    CL_REG_CLEAR(port->MODER, 0x03 << (gpio->config->pin * gpio->config->pin));
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
