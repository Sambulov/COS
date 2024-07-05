#ifndef PORT_GPIO_H_
#define PORT_GPIO_H_

// pin.module.reg = {GPIO_PIN_0, GPIO_PIN_1, ..., GPIO_PIN_15, GPIO_PIN_ALL}
// module.reg = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF}
typedef hdl_module_t hdl_gpio_port_t;

typedef struct {
  const uint32_t func;   /* GPIO_MODE_INPUT - GPIO_MODE_OUTPUT - GPIO_MODE_OPEN_DRAIN - GPIO_MODE_QUASI */
  const uint32_t pull_mode; /* GPIO_PUSEL_DISABLE - GPIO_PUSEL_PULL_UP - GPIO_PUSEL_PULL_DOWN */
  const uint32_t slew_mode; /* GPIO_SLEWCTL_NORMAL - GPIO_SLEWCTL_HIGH - GPIO_SLEWCTL_FAST */
} hdl_gpio_mode_t;

#endif // PORT_GPIO_H_
