#include "hdl_portable.h"

static hdl_gpio_state _hdl_gpio_read(const hdl_module_base_t *gpio){
  if (gpio->dependencies == NULL || gpio->dependencies[0] == NULL)
    return HDL_GPIO_LOW;
  uint32_t gpio_port = ((hdl_gpio_port_t *)gpio->dependencies[0])->config->reg;
  uint32_t gpio_pin = ((hdl_gpio_pin_t *)gpio)->config->pin;  
  return (gpio_input_bit_get(gpio_port, gpio_pin) == RESET) ? HDL_GPIO_LOW : HDL_GPIO_HIGH;
}

static hdl_gpio_state _hdl_gpio_read_output(const hdl_module_base_t *gpio) {
  if (gpio->dependencies == NULL || gpio->dependencies[0] == NULL)
    return HDL_GPIO_LOW;
  uint32_t gpio_port = ((hdl_gpio_port_t *)gpio->dependencies[0])->config->reg;
  uint32_t gpio_pin = ((hdl_gpio_pin_t *)gpio)->config->pin;  
  return (gpio_output_bit_get(gpio_port, gpio_pin) == RESET) ? HDL_GPIO_LOW : HDL_GPIO_HIGH;
}

static void _hdl_gpio_write(const hdl_module_base_t *gpio, const hdl_gpio_state state){
  if (gpio->dependencies == NULL || gpio->dependencies[0] == NULL)
    return;
  uint32_t gpio_port = ((hdl_gpio_port_t *)gpio->dependencies[0])->config->reg;
  uint32_t gpio_pin = ((hdl_gpio_pin_t *)gpio)->config->pin;  
  gpio_bit_write(gpio_port, gpio_pin, (state == HDL_GPIO_LOW) ? RESET : SET);
}

static void _hdl_gpio_toggle(const hdl_module_base_t *gpio){
  if (gpio->dependencies == NULL || gpio->dependencies[0] == NULL)
    return;
  uint32_t gpio_port = ((hdl_gpio_port_t *)gpio->dependencies[0])->config->reg;
  uint32_t gpio_pin = ((hdl_gpio_pin_t *)gpio)->config->pin;  
  gpio_bit_toggle(gpio_port, gpio_pin);
}

extern hdl_module_state_t hdl_gpio_pin(const void *desc, const uint8_t enable);

hdl_gpio_pin_iface_t hdl_gpio_pin_iface = {
  .init = &hdl_gpio_pin,
  .read = &_hdl_gpio_read,
  .read_ouput = &_hdl_gpio_read_output,
  .toggle = &_hdl_gpio_toggle,
  .write = &_hdl_gpio_write
};
