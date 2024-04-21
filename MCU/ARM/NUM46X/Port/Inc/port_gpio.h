#ifndef PORT_GPIO_H_
#define PORT_GPIO_H_

typedef struct {
  GPIO_T *port;
  const uint32_t pin; 
  const uint32_t mode;   /* GPIO_MODE_INPUT - GPIO_MODE_OUTPUT - GPIO_MODE_OPEN_DRAIN - GPIO_MODE_QUASI */
  const uint32_t pull_mode; /* GPIO_PUSEL_DISABLE - GPIO_PUSEL_PULL_UP - GPIO_PUSEL_PULL_DOWN */
  //const uint32_t int_mode;  /* GPIO_INT_RISING - GPIO_INT_FALLING - GPIO_INT_BOTH_EDGE - GPIO_INT_HIGH - GPIO_INT_LOW */
  const uint32_t slew_mode; /* GPIO_SLEWCTL_NORMAL - GPIO_SLEWCTL_HIGH - GPIO_SLEWCTL_FAST */
} hdl_gpio_t;

#endif // PORT_GPIO_H_
