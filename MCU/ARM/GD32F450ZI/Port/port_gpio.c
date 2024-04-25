#include "hdl_portable.h"
#include "Macros.h"

hdl_module_state_t hdl_gpio_port(void *desc, const uint8_t enable) {
  // hdl_gpio_port_t *port = (hdl_gpio_port_t *)desc;
  // if(port->reg == NULL)
  //   return HDL_MODULE_INIT_FAILED;
  // rcu_periph_enum rcu;
  // switch ((uint32_t)port->reg){
  //   case GPIOA:
  //     rcu = RCU_GPIOA;
  //     break;
  //   case GPIOB:
  //     rcu = RCU_GPIOB;
  //     break;
  //   case GPIOC:
  //     rcu = RCU_GPIOC;
  //     break;
  //   case GPIOD:
  //     rcu = RCU_GPIOD;
  //     break;
  //   case GPIOE:
  //     rcu = RCU_GPIOE;
  //     break;
  //   case GPIOF:
  //     rcu = RCU_GPIOF;
  //     break;    
  //   case GPIOG:
  //     rcu = RCU_GPIOG;
  //     break;
  //   default:
  //     return HDL_MODULE_INIT_FAILED;
  // }
  // if(enable) {
  //   rcu_periph_clock_enable(RCU_AF);
  //   rcu_periph_clock_enable(rcu);
  // }
  // else {
  //   rcu_periph_clock_disable(rcu);
  //   return HDL_MODULE_DEINIT_OK;
  // }
  return HDL_MODULE_INIT_OK;
}

hdl_module_state_t hdl_gpio_pin(void *desc, const uint8_t enable) {
  // hdl_gpio_pin_t *gpio = (hdl_gpio_pin_t *)desc;
  // if (gpio->mode == NULL || gpio->module.dependencies[0] == NULL || gpio->module.dependencies[0]->reg == NULL)
  //   return HDL_MODULE_INIT_FAILED;
  // uint32_t gpio_port = (uint32_t)gpio->module.dependencies[0]->reg;
  // if(enable) {
  //   volatile uint32_t *afio_pcf0 = (uint32_t *)(AFIO + 0x4U);
  //   if((gpio_port == GPIOB) && ((uint32_t)gpio->module.reg == GPIO_PIN_4)) {
  //     HDL_REG_MODIFY(*afio_pcf0, 0b111 << 24, 0b001 << 24);
  //   }
  //   if(((gpio_port == GPIOA) && ((uint32_t)gpio->module.reg == GPIO_PIN_15)) || 
  //      ((gpio_port == GPIOB) && ((uint32_t)gpio->module.reg == GPIO_PIN_3))) {
  //     HDL_REG_MODIFY(*afio_pcf0, 0b111 << 24, 0b010 << 24);
  //   }
  //   if((gpio_port == GPIOA) && (((uint32_t)gpio->module.reg == GPIO_PIN_13) || ((uint32_t)gpio->module.reg == GPIO_PIN_14))) {
  //     HDL_REG_MODIFY(*afio_pcf0, 0b111 << 24, 0b100 << 24);
  //   }
  //   gpio_init(gpio_port, gpio->mode->type, gpio->mode->ospeed, (uint32_t)gpio->module.reg);
  // }
  // else {
  //   gpio_init(gpio_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, (uint32_t)gpio->module.reg);
  //   return HDL_MODULE_DEINIT_OK;
  // }
  return HDL_MODULE_INIT_OK;
}

hdl_gpio_state hdl_gpio_read(const hdl_gpio_pin_t *gpio){
  if (gpio->module.dependencies[0] == NULL || gpio->module.dependencies[0]->reg == NULL)
    return HDL_GPIO_LOW;
  uint32_t gpio_port = (uint32_t)gpio->module.dependencies[0]->reg;
  return (gpio_input_bit_get(gpio_port, (uint32_t)gpio->module.reg) == RESET) ? HDL_GPIO_LOW : HDL_GPIO_HIGH;
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
  FlagStatus state = gpio_output_bit_get((uint32_t)gpio->module.dependencies[0]->reg, (uint32_t)gpio->module.reg);
  hdl_gpio_write(gpio, !state);
}
