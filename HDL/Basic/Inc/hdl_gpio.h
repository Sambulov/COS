#ifndef HDL_GPIO_H_
#define HDL_GPIO_H_

#include "port_gpio.h"

typedef enum {
  HDL_GPIO_LOW = 0,
  HDL_GPIO_HIGH = !HDL_GPIO_LOW
} hdl_gpio_state;

_Static_assert(HDL_GPIO_LOW == !HDL_GPIO_HIGH, "Expression (HDL_GPIO_LOW == !HDL_GPIO_HIGH) must always be true");

hdl_module_new_t(hdl_gpio_port_t, 0, hdl_gpio_port_config_t, hdl_module_base_iface_t);

extern hdl_module_base_iface_t hdl_gpio_port_iface;

typedef struct {
  hdl_gpio_state inactive_default;
  uint32_t pin;
  const hdl_gpio_pin_hw_config_t *hwc;
} hdl_gpio_pin_config_t;

typedef hdl_gpio_state (*hdl_gpio_read_t)(const void *gpio);
typedef void (*hdl_gpio_write_t)(const void *gpio, const hdl_gpio_state);
typedef void (*hdl_gpio_toggle_t)(const void *gpio);

typedef struct {
  hdl_module_initializer_t init;
  hdl_gpio_read_t read;
  hdl_gpio_read_t read_ouput;
  hdl_gpio_write_t write;
  hdl_gpio_toggle_t toggle;
} hdl_gpio_pin_iface_t;

hdl_module_new_t(hdl_gpio_pin_t, 0, hdl_gpio_pin_config_t, hdl_gpio_pin_iface_t);

extern hdl_gpio_pin_iface_t hdl_gpio_pin_iface;

#define hdl_gpio_set_inactive(gpio)            (hdl_gpio_write((gpio), (gpio)->config->inactive_default))
#define hdl_gpio_set_active(gpio)              (hdl_gpio_write((gpio), !((gpio)->config->inactive_default)))
#define hdl_gpio_is_inactive(gpio)             (hdl_gpio_read((gpio)) == (gpio)->config->inactive_default)
#define hdl_gpio_is_active(gpio)               (!hdl_gpio_is_inactive(gpio))
#define hdl_gpio_is_set_as_inactive(gpio)      (hdl_gpio_read_output(gpio) == gpio->config->inactive_default)
#define hdl_gpio_is_set_as_active(gpio)        (!hdl_gpio_is_set_as_inactive(gpio))

__STATIC_INLINE hdl_gpio_state hdl_gpio_read(const void *gpio) {
  return ((hdl_gpio_pin_iface_t *)((hdl_module_base_t *)gpio)->iface)->read(gpio);
}

__STATIC_INLINE hdl_gpio_state hdl_gpio_read_output(const void *gpio) {
  return ((hdl_gpio_pin_iface_t *)((hdl_module_base_t *)gpio)->iface)->read_ouput(gpio);
}

__STATIC_INLINE void hdl_gpio_write(const void *gpio, const hdl_gpio_state state) {
  ((hdl_gpio_pin_iface_t *)((hdl_module_base_t *)gpio)->iface)->write(gpio, state);
}


__STATIC_INLINE void hdl_gpio_toggle(const void *gpio) {
  ((hdl_gpio_pin_iface_t *)((hdl_module_base_t *)gpio)->iface)->toggle(gpio);
}

#endif // HDL_GPIO_H_
