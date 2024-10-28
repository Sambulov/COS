#include "hdl_portable.h"
#include "Macros.h"

hdl_module_state_t hdl_gpio_port(void *desc, const uint8_t enable) {
  hdl_gpio_port_t *port = (hdl_gpio_port_t *)desc;
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

hdl_module_state_t hdl_gpio_pin(void *desc, const uint8_t enable) {
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  if (gpio->config->hwc == NULL || gpio->module.dependencies[0] == NULL || gpio->module.dependencies[0]->reg == NULL)
    return HDL_MODULE_FAULT;
  uint32_t gpio_port = (uint32_t)gpio->module.dependencies[0]->reg;
  gpio_bit_write(gpio_port, (uint32_t)gpio->module.reg, (gpio->config->inactive_default == HDL_GPIO_LOW) ? RESET : SET);
  if(enable) {
    volatile uint32_t *afio_pcf0 = (uint32_t *)(AFIO + 0x4U);
    if((gpio_port == GPIOB) && ((uint32_t)gpio->module.reg == GPIO_PIN_4)) {
      HDL_REG_MODIFY(*afio_pcf0, 0b111 << 24, 0b001 << 24);
    }
    if(((gpio_port == GPIOA) && ((uint32_t)gpio->module.reg == GPIO_PIN_15)) || 
       ((gpio_port == GPIOB) && ((uint32_t)gpio->module.reg == GPIO_PIN_3))) {
      HDL_REG_MODIFY(*afio_pcf0, 0b111 << 24, 0b010 << 24);
    }
    if((gpio_port == GPIOA) && (((uint32_t)gpio->module.reg == GPIO_PIN_13) || ((uint32_t)gpio->module.reg == GPIO_PIN_14))) {
      HDL_REG_MODIFY(*afio_pcf0, 0b111 << 24, 0b100 << 24);
    }
    gpio_init(gpio_port, gpio->config->hwc->type, gpio->config->hwc->ospeed, (uint32_t)gpio->module.reg);
  }
  else {
    gpio_init(gpio_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, (uint32_t)gpio->module.reg);
    return HDL_MODULE_UNLOADED;
  }
  return HDL_MODULE_ACTIVE;
}
