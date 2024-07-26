#ifndef PORT_GPIO_H_
#define PORT_GPIO_H_

// pin.module.reg = {GPIO_PIN_0, GPIO_PIN_1, ..., GPIO_PIN_15, GPIO_PIN_ALL}
// module.reg = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF}
typedef hdl_module_t hdl_gpio_port_t;

typedef struct {
  const uint32_t type;      // GPIO_MODE_AIN, GPIO_MODE_IN_FLOATING, GPIO_MODE_IPD, GPIO_MODE_IPU, GPIO_MODE_OUT_OD, GPIO_MODE_OUT_PP, GPIO_MODE_AF_OD, GPIO_MODE_AF_PP
  const uint32_t ospeed;    // GPIO_OSPEED_2MHZ, GPIO_OSPEED_10MHZ, GPIO_OSPEED_50MHZ
} hdl_gpio_mode_t;

#endif // PORT_GPIO_H_
