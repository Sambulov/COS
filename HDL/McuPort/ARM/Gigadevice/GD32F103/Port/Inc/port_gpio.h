#ifndef PORT_GPIO_H_
#define PORT_GPIO_H_

#include "hdl_gpio.h"

typedef struct {
  const rcu_periph_enum rcu;
  uint32_t phy;
} hdl_gpio_port_config_t;

typedef struct {
  const uint32_t type;      // GPIO_MODE_AIN, GPIO_MODE_IN_FLOATING, GPIO_MODE_IPD, GPIO_MODE_IPU, GPIO_MODE_OUT_OD, GPIO_MODE_OUT_PP, GPIO_MODE_AF_OD, GPIO_MODE_AF_PP
  const uint32_t ospeed;    // GPIO_OSPEED_2MHZ, GPIO_OSPEED_10MHZ, GPIO_OSPEED_50MHZ
} hdl_gpio_pin_hw_config_t;

hdl_module_new_t(hdl_gpio_port_mcu_t, 0, hdl_gpio_port_config_t, hdl_module_base_iface_t);

extern const hdl_module_base_iface_t hdl_gpio_port_iface;
extern const hdl_gpio_pin_iface_t hdl_gpio_pin_iface;

#endif // PORT_GPIO_H_
