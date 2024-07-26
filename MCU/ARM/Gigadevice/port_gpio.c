#include "hdl_portable.h"

hdl_gpio_state hdl_gpio_read(const hdl_gpio_pin_t *gpio){
  if (gpio->module.dependencies[0] == NULL || gpio->module.dependencies[0]->reg == NULL)
    return HDL_GPIO_LOW;
  uint32_t gpio_port = (uint32_t)gpio->module.dependencies[0]->reg;
  return (gpio_input_bit_get(gpio_port, (uint32_t)gpio->module.reg) == RESET) ? HDL_GPIO_LOW : HDL_GPIO_HIGH;
}

hdl_gpio_state hdl_gpio_read_output(const hdl_gpio_pin_t *gpio) {
  if (gpio->module.dependencies[0] == NULL || gpio->module.dependencies[0]->reg == NULL)
    return HDL_GPIO_LOW;
  uint32_t gpio_port = (uint32_t)gpio->module.dependencies[0]->reg;
  return (gpio_output_bit_get(gpio_port, (uint32_t)gpio->module.reg) == RESET) ? HDL_GPIO_LOW : HDL_GPIO_HIGH;
}

void hdl_gpio_write(const hdl_gpio_pin_t *gpio, const hdl_gpio_state state){
  if (gpio->module.dependencies[0] == NULL || gpio->module.dependencies[0]->reg == NULL)
    return;
  uint32_t gpio_port = (uint32_t)gpio->module.dependencies[0]->reg;
  gpio_bit_write(gpio_port, (uint32_t)gpio->module.reg, (state == HDL_GPIO_LOW) ? RESET : SET);
}

void hdl_gpio_toggle(const hdl_gpio_pin_t *gpio){
  if (gpio->module.dependencies[0] == NULL || gpio->module.dependencies[0]->reg == NULL)
    return;
  uint32_t gpio_port = (uint32_t)gpio->module.dependencies[0]->reg;
  gpio_bit_toggle(gpio_port, (uint32_t)gpio->module.reg);
}
