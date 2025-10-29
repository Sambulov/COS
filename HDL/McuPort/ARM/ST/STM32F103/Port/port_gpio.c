#include "hdl_iface.h"

static hdl_module_state_t _hdl_gpio_port(const void *desc, const uint8_t enable) {
  hdl_gpio_port_mcu_t *port = (hdl_gpio_port_mcu_t *)desc;
  if(port->config == NULL)
    return HDL_MODULE_FAULT;
  if(enable)
    CL_REG_SET(RCC->APB2ENR, port->config->rcc);
  else {
    CL_REG_CLEAR(RCC->APB2ENR, port->config->rcc);
    return HDL_MODULE_UNLOADED;
  }
  return HDL_MODULE_ACTIVE;
}

hdl_module_state_t _hdl_gpio_pin(const void *desc, const uint8_t enable) {
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  if (gpio->config->hwc == NULL || gpio->dependencies == NULL || gpio->dependencies[0] == NULL)
    return HDL_MODULE_FAULT;
  GPIO_TypeDef *gpio_port = ((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  hdl_gpio_pin_hw_config_t *gpio_cnf_hw = (hdl_gpio_pin_hw_config_t *)gpio->config->hwc;
  if(gpio->config->inactive_default == HDL_GPIO_LOW)
    gpio_port->ODR &= ~gpio->config->pin;
  else
    gpio_port->ODR |= gpio->config->pin;
  if(enable) {
    CL_REG_SET(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);
    volatile uint32_t *afio_pcf0 = (uint32_t *)(AFIO + 0x4U);
    if((gpio_port == GPIOB) && (gpio->config->pin == GPIO_PIN_4)) {
      CL_REG_MODIFY(*afio_pcf0, 0b111 << 24, 0b001 << 24);
    }
    if(((gpio_port == GPIOA) && (gpio->config->pin == GPIO_PIN_15)) || 
       ((gpio_port == GPIOB) && (gpio->config->pin == GPIO_PIN_3))) {
      CL_REG_MODIFY(*afio_pcf0, 0b111 << 24, 0b010 << 24);
    }
    if((gpio_port == GPIOA) && ((gpio->config->pin == GPIO_PIN_13) || (gpio->config->pin == GPIO_PIN_14))) {
      CL_REG_MODIFY(*afio_pcf0, 0b111 << 24, 0b100 << 24);
    }
    GPIO_InitTypeDef def = {
      .Mode = gpio_cnf_hw->mode,
      .Speed = gpio_cnf_hw->ospeed,
      .Pull = gpio_cnf_hw->pull,
      .Pin = gpio->config->pin
    };
    HAL_GPIO_Init(gpio_port, &def);
  }
  else {
    HAL_GPIO_DeInit(gpio_port, gpio->config->pin);
    return HDL_MODULE_UNLOADED;
  }
  return HDL_MODULE_ACTIVE;
}

static hdl_gpio_state _hdl_gpio_read(const void *desc) {
  hdl_module_base_t *gpio = (hdl_module_base_t *)desc;
  if (gpio->dependencies == NULL || gpio->dependencies[0] == NULL)
    return HDL_GPIO_LOW;
  GPIO_TypeDef *gpio_port = ((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  uint32_t gpio_pin = ((hdl_gpio_pin_t *)gpio)->config->pin;  
  return (gpio_port->IDR & gpio_pin)? HDL_GPIO_HIGH: HDL_GPIO_LOW;
}

static hdl_gpio_state _hdl_gpio_read_output(const void *desc) {
  hdl_module_base_t *gpio = (hdl_module_base_t *)desc;
  if (gpio->dependencies == NULL || gpio->dependencies[0] == NULL)
    return HDL_GPIO_LOW;
  GPIO_TypeDef *gpio_port = ((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  uint32_t gpio_pin = ((hdl_gpio_pin_t *)gpio)->config->pin;  
  return (gpio_port->ODR & gpio_pin)? HDL_GPIO_HIGH: HDL_GPIO_LOW;
}

static void _hdl_gpio_write(const void *desc, const hdl_gpio_state state) {
  hdl_module_base_t *gpio = (hdl_module_base_t *)desc;
  if (gpio->dependencies == NULL || gpio->dependencies[0] == NULL)
    return;
  GPIO_TypeDef *gpio_port = ((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  uint32_t gpio_pin = ((hdl_gpio_pin_t *)gpio)->config->pin;
  if(state == HDL_GPIO_LOW)
    gpio_port->ODR &= ~gpio_pin;
  else
    gpio_port->ODR |= gpio_pin;
}

static void _hdl_gpio_toggle(const void *desc) {
  hdl_module_base_t *gpio = (hdl_module_base_t *)desc;
  if (gpio->dependencies == NULL || gpio->dependencies[0] == NULL)
    return;
  GPIO_TypeDef *gpio_port = ((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  uint32_t gpio_pin = ((hdl_gpio_pin_t *)gpio)->config->pin;
  CL_REG_MODIFY(gpio_port->ODR, gpio_pin, ~gpio_port->ODR);
}

const hdl_gpio_pin_iface_t hdl_gpio_pin_iface = {
  .init = &_hdl_gpio_pin,
  .read = &_hdl_gpio_read,
  .read_ouput = &_hdl_gpio_read_output,
  .toggle = &_hdl_gpio_toggle,
  .write = &_hdl_gpio_write
};

const hdl_module_base_iface_t hdl_gpio_port_iface = {
  .init = &_hdl_gpio_port
};
