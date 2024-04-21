#ifndef PORT_GPIO_H_
#define PORT_GPIO_H_

typedef struct {
  const uint32_t af;        // see stm32l0xx_hal_gpio_ex.h, line 538
  const uint32_t pull;      // GPIO_NOPULL, GPIO_PULLUP, GPIO_PULLDOWN
  const uint32_t type;      // GPIO_MODE_INPUT, GPIO_MODE_OUTPUT_PP, GPIO_MODE_OUTPUT_OD, GPIO_MODE_AF_PP, GPIO_MODE_AF_OD
  const uint32_t ospeed;    // GPIO_SPEED_FREQ_LOW, GPIO_SPEED_FREQ_MEDIUM, GPIO_SPEED_FREQ_HIGH, GPIO_SPEED_FREQ_VERY_HIGH
} hdl_gpio_mode_t;

typedef struct {
  GPIO_TypeDef *port;       // GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOH
  const uint16_t pin;             // GPIO_PIN_0, GPIO_PIN_1, ... GPIO_PIN_15, GPIO_PIN_All
  const hdl_gpio_mode_t *mode;
} hdl_gpio_t;

#endif // PORT_GPIO_H_
