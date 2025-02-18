#ifndef PORT_GPIO_H_
#define PORT_GPIO_H_

#include "hdl_gpio.h"

typedef struct {
  const rcu_periph_enum rcu;
  uint32_t phy;
} hdl_gpio_port_config_t;

typedef struct {
  const uint32_t af;        // GPIO_AF_0, GPIO_AF_1, ..., GPIO_AF_15.
  const uint32_t pull;      // GPIO_PUPD_NONE, GPIO_PUPD_PULLUP, GPIO_PUPD_PULLDOWN.
  const uint32_t type;      // GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG.
  const uint32_t otype;     // GPIO_OTYPE_PP, GPIO_OTYPE_OD.
  const uint32_t ospeed;    // GPIO_OSPEED_2MHZ, GPIO_OSPEED_10MHZ, GPIO_OSPEED_50MHZ
} hdl_gpio_pin_hw_config_t;

hdl_module_new_t(hdl_gpio_port_t, 0, hdl_gpio_port_config_t, hdl_module_base_iface_t);

extern const hdl_module_base_iface_t hdl_gpio_port_iface;

hdl_module_new_t(hdl_gpio_pin_t, 0, hdl_gpio_pin_config_t, hdl_gpio_pin_iface_t);

extern const hdl_gpio_pin_iface_t hdl_gpio_pin_iface;

#endif // PORT_GPIO_H_
