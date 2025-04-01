#ifndef PORT_GPIO_H_
#define PORT_GPIO_H_

#include "hdl_gpio.h"

#define HDL_GPIO_MODE_INPUT       ( (uint32_t) 0x00 )
#define HDL_GPIO_MODE_OUTPUT      ( (uint32_t) 0x01 )
#define HDL_GPIO_MODE_AF          ( (uint32_t) 0x02 )
#define HDL_GPIO_MODE_ANALOG      ( (uint32_t) 0x03 )

#define HDL_GPIO_SPEED_LOW        ( (uint32_t) 0x00 )
#define HDL_GPIO_SPEED_MEDIUM     ( (uint32_t) 0x01 )
#define HDL_GPIO_SPEED_HIGH       ( (uint32_t) 0x02 )
#define HDL_GPIO_SPEED_VERY_HIGH  ( (uint32_t) 0x03 )

#define HDL_GPIO_PUPD_NONE        ( (uint32_t) 0x00 )
#define HDL_GPIO_PUPD_PULL_UP     ( (uint32_t) 0x01 )
#define HDL_GPIO_PUPD_PULL_DOWN   ( (uint32_t) 0x02 )

#define HDL_GPIO_OTYPE_PUSH_PULL  ( (uint32_t) 0x00 )
#define HDL_GPIO_OTYPE_OPEN_DRAIN ( (uint32_t) 0x01 )

typedef struct {
  uint32_t rcu;     /* RCC_AHB1ENR_GPIOAEN ... */
  uint32_t phy;     /* GPIOA ... */
} hdl_gpio_port_config_t;

typedef struct {
  const uint32_t af;        // see stm32f4xx_hal_gpio_ex.h line GPIO_AF_ ...
  const uint32_t pull;      // HDL_GPIO_PUPD_NONE, HDL_GPIO_PUPD_PULL_UP, HDL_GPIO_PUPD_PULL_DOWN.
  const uint32_t mode;      // HDL_GPIO_MODE_INPUT, HDL_GPIO_MODE_OUTPUT, HDL_GPIO_MODE_AF, HDL_GPIO_MODE_ANALOG.
  const uint32_t otype;     // HDL_GPIO_OTYPE_PUSH_PULL, HDL_GPIO_OTYPE_OPEN_DRAIN.
  const uint32_t ospeed;    // HDL_GPIO_SPEED_LOW, HDL_GPIO_SPEED_MEDIUM, HDL_GPIO_SPEED_HIGH, HDL_GPIO_SPEED_VERY_HIGH
} hdl_gpio_pin_hw_config_t;

hdl_module_new_t(hdl_gpio_port_mcu_t, 0, hdl_gpio_port_config_t, hdl_module_base_iface_t);

extern const hdl_module_base_iface_t hdl_gpio_port_iface;
extern const hdl_gpio_pin_iface_t hdl_gpio_pin_iface;

#endif // PORT_GPIO_H_
