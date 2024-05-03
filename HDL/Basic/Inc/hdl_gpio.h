#ifndef HDL_GPIO_H_
#define HDL_GPIO_H_

#include "port_gpio.h"

typedef enum {
  HDL_GPIO_LOW = 0,
  HDL_GPIO_HIGH = !HDL_GPIO_LOW
} hdl_gpio_state;

_Static_assert(HDL_GPIO_LOW == !HDL_GPIO_HIGH, "Expression (HDL_GPIO_LOW == !HDL_GPIO_HIGH) must be always true");

typedef struct {
  hdl_module_t module;
  const hdl_gpio_mode_t *mode;
  hdl_gpio_state inactive_default;
} hdl_gpio_pin_t;

hdl_module_state_t hdl_gpio_pin(void *desc, const uint8_t enable);
hdl_module_state_t hdl_gpio_port(void *desc, const uint8_t enable);

hdl_gpio_state hdl_gpio_read(const hdl_gpio_pin_t *gpio);
void hdl_gpio_write(const hdl_gpio_pin_t *gpio, const hdl_gpio_state state);
void hdl_gpio_toggle(const hdl_gpio_pin_t *gpio);

#endif // HDL_GPIO_H_
