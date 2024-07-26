#include "hdl_portable.h"

hdl_module_state_t hdl_gpio_port(void *desc, const uint8_t enable) {
  /* Casting desc to hdl_gpio_port_t* type */
  hdl_gpio_port_t *port = (hdl_gpio_port_t *)desc;
  if(enable)
    SET_BIT(RCC->IOPENR, port->config->hwc->rcu);
  else{
    CLEAR_BIT(RCC->IOPENR, port->config->hwc->rcu);
    return HDL_MODULE_DEINIT_OK;
  }
  return HDL_MODULE_INIT_OK;
}

hdl_gpio_state hdl_gpio_read(const hdl_gpio_pin_t *gpio) {
  if (hdl_state(&gpio->module) != HDL_MODULE_INIT_OK)
    return HDL_GPIO_LOW;
  GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)gpio->module.dependencies[0]->reg;
  return (HAL_GPIO_ReadPin(gpio_port, (uint32_t)gpio->module.reg) == GPIO_PIN_SET)? HDL_GPIO_HIGH: HDL_GPIO_LOW;
}

void hdl_gpio_write(const hdl_gpio_pin_t *gpio, const hdl_gpio_state state){
  if (hdl_state(&gpio->module) != HDL_MODULE_INIT_OK)
    return;
  GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)gpio->module.dependencies[0]->reg;
  HAL_GPIO_WritePin(gpio_port, (uint32_t)gpio->module.reg, (state == HDL_GPIO_HIGH)? GPIO_PIN_SET: GPIO_PIN_RESET);
}

hdl_gpio_state hdl_gpio_read_output(const hdl_gpio_pin_t *gpio) {
  if (hdl_state(&gpio->module) != HDL_MODULE_INIT_OK)
    return HDL_GPIO_LOW;
  GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)gpio->module.dependencies[0]->reg;
  return (gpio_port->ODR & (uint32_t)gpio->module.reg)? HDL_GPIO_HIGH: HDL_GPIO_LOW;
}

void hdl_gpio_toggle(const hdl_gpio_pin_t *gpio){
  if (hdl_state(&gpio->module) != HDL_MODULE_INIT_OK)
    return;
  GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)gpio->module.dependencies[0]->reg;
  HAL_GPIO_TogglePin(gpio_port, (uint32_t)gpio->module.reg);
}


hdl_module_state_t hdl_gpio_pin(void *desc, const uint8_t enable){
  hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  if (gpio->config->hwc == NULL || gpio->module.dependencies[0] == NULL || gpio->module.dependencies[0]->reg == NULL)
    return HDL_MODULE_INIT_FAILED;
  GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)gpio->module.dependencies[0]->reg;
  HAL_GPIO_WritePin(gpio_port, (uint32_t)gpio->module.reg, (gpio->config->inactive_default == HDL_GPIO_HIGH)? GPIO_PIN_SET: GPIO_PIN_RESET);
  GPIO_InitTypeDef GPIO_InitStructure;
  if(enable) {
    GPIO_InitStructure.Pin = (uint32_t)gpio->module.reg;
    GPIO_InitStructure.Mode = gpio->config->hwc->type;
    GPIO_InitStructure.Speed = gpio->config->hwc->ospeed;
    GPIO_InitStructure.Pull = gpio->config->hwc->pull;
    GPIO_InitStructure.Alternate = gpio->config->hwc->af;
    HAL_GPIO_Init(gpio_port, &GPIO_InitStructure);
  }
  else{
    GPIO_InitStructure.Pin = (uint32_t)gpio->module.reg;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Alternate = GPIO_AF0_EVENTOUT;
    HAL_GPIO_Init(gpio_port, &GPIO_InitStructure);
    return HDL_MODULE_DEINIT_OK;
  }
  return HDL_MODULE_INIT_OK;
}
