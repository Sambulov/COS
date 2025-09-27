#ifndef PORT_GPIO_H_
#define PORT_GPIO_H_

#include "hdl_gpio.h"

typedef struct {
  const uint32_t rcc; /* RCC_APB2ENR_IOPAEN, RCC_APB2ENR_IOPBEN,RCC_APB2ENR_IOPCEN ... */
  GPIO_TypeDef *phy;
} hdl_gpio_port_config_t;

typedef struct {
  const uint32_t mode;      /* GPIO_MODE(_AF)_INPUT, GPIO_MODE_ANALOG, GPIO_MODE_OUTPUT_PP, GPIO_MODE_OUTPUT_OD, GPIO_MODE_AF_PP, GPIO_MODE_AF_OD */ 
  const uint32_t pull;      /* GPIO_NOPULL, GPIO_PULLUP, GPIO_PULLDOWN */
  const uint32_t ospeed;    /* GPIO_SPEED_FREQ_LOW, GPIO_SPEED_FREQ_MEDIUM, GPIO_SPEED_FREQ_HIGH */ 
} hdl_gpio_pin_hw_config_t;

hdl_module_new_t(hdl_gpio_port_mcu_t, 0, hdl_gpio_port_config_t*, hdl_module_base_iface_t);

extern const hdl_module_base_iface_t hdl_gpio_port_iface;
extern const hdl_gpio_pin_iface_t hdl_gpio_pin_iface;

#endif // PORT_GPIO_H_
