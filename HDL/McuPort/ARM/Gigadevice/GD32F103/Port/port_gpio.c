#include "hdl_iface.h"

static hdl_module_state_t _hdl_gpio_port(const void *desc, const uint8_t enable) {
  hdl_gpio_port_mcu_t *port = (hdl_gpio_port_mcu_t *)desc;
  if(port->config == NULL)
    return HDL_MODULE_FAULT;
  if(enable) {
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(port->config->rcu);
  }
  else {
    rcu_periph_clock_disable(port->config->rcu);
    return HDL_MODULE_UNLOADED;
  }
  return HDL_MODULE_ACTIVE;
}

hdl_module_state_t __hdl_gpio_pin(const void *desc, const uint8_t enable) {
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  if (gpio->config->hwc == NULL || gpio->dependencies == NULL || gpio->dependencies[0] == NULL)
    return HDL_MODULE_FAULT;
  uint32_t gpio_port = ((hdl_gpio_port_mcu_t *)gpio->dependencies[0])->config->phy;
  hdl_gpio_pin_hw_config_t *gpio_cnf_hw = (hdl_gpio_pin_hw_config_t *)gpio->config->hwc;
  gpio_bit_write(gpio_port, gpio->config->pin, (gpio->config->inactive_default == HDL_GPIO_LOW) ? RESET : SET);
  if(enable) {
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
    gpio_init(gpio_port, gpio_cnf_hw->type, gpio_cnf_hw->ospeed, gpio->config->pin);
  }
  else {
    gpio_init(gpio_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, gpio->config->pin);
    return HDL_MODULE_UNLOADED;
  }
  return HDL_MODULE_ACTIVE;
}

const hdl_module_base_iface_t hdl_gpio_port_iface = {
  .init = &_hdl_gpio_port
};
