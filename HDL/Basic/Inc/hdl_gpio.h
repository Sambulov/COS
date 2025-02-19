#ifndef HDL_GPIO_H_
#define HDL_GPIO_H_

typedef enum {
  HDL_GPIO_LOW = 0,
  HDL_GPIO_HIGH = !HDL_GPIO_LOW
} hdl_gpio_state;

_Static_assert(HDL_GPIO_LOW == !HDL_GPIO_HIGH, "Expression (HDL_GPIO_LOW == !HDL_GPIO_HIGH) must always be true");

typedef struct {
  hdl_gpio_state inactive_default;
  uint32_t pin;
  const void *hwc;
} hdl_gpio_pin_config_t;

typedef hdl_gpio_state (*hdl_gpio_read_t)(const void *desc);
typedef void (*hdl_gpio_write_t)(const void *desc, const hdl_gpio_state);
typedef void (*hdl_gpio_toggle_t)(const void *desc);

typedef struct {
  hdl_module_initializer_t init;
  hdl_gpio_read_t read;
  hdl_gpio_read_t read_ouput;
  hdl_gpio_write_t write;
  hdl_gpio_toggle_t toggle;
} hdl_gpio_pin_iface_t;

hdl_module_new_t(hdl_gpio_pin_t, 0, hdl_gpio_pin_config_t, hdl_gpio_pin_iface_t);

__STATIC_INLINE hdl_gpio_state hdl_gpio_read(const void *desc) {
  MODULE_ASSERT(desc, HDL_GPIO_LOW);
  return ((hdl_gpio_pin_iface_t *)((hdl_module_base_t *)desc)->iface)->read(desc);
}

__STATIC_INLINE hdl_gpio_state hdl_gpio_read_output(const void *desc) {
  MODULE_ASSERT(desc, HDL_GPIO_LOW);
  return ((hdl_gpio_pin_iface_t *)((hdl_module_base_t *)desc)->iface)->read_ouput(desc);
}

__STATIC_INLINE void hdl_gpio_write(const void *desc, const hdl_gpio_state state) {
  MODULE_ASSERT(desc, );
  ((hdl_gpio_pin_iface_t *)((hdl_module_base_t *)desc)->iface)->write(desc, state);
}

__STATIC_INLINE void hdl_gpio_toggle(const void *desc) {
  MODULE_ASSERT(desc, );
  ((hdl_gpio_pin_iface_t *)((hdl_module_base_t *)desc)->iface)->toggle(desc);
}

__STATIC_INLINE void hdl_gpio_set_inactive(const void *desc) {
  MODULE_ASSERT(desc, );
  ((hdl_gpio_pin_iface_t *)((hdl_module_base_t *)desc)->iface)->write(
    (desc), ((hdl_gpio_pin_config_t *)((hdl_module_base_t *)(desc))->config)->inactive_default
  );
}

__STATIC_INLINE void hdl_gpio_set_active(const void *desc) {
  MODULE_ASSERT(desc, );
  ((hdl_gpio_pin_iface_t *)((hdl_module_base_t *)desc)->iface)->write(
    (desc), !((hdl_gpio_pin_config_t *)((hdl_module_base_t *)(desc))->config)->inactive_default
  );
}

__STATIC_INLINE uint8_t hdl_gpio_is_inactive(const void *desc) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return (((hdl_gpio_pin_iface_t *)((hdl_module_base_t *)desc)->iface)->read(desc) == 
    ((hdl_gpio_pin_config_t *)((hdl_module_base_t *)(desc))->config)->inactive_default);
}

__STATIC_INLINE uint8_t hdl_gpio_is_active(const void *desc) {               
  MODULE_ASSERT(desc, HDL_FALSE);
  return (((hdl_gpio_pin_iface_t *)((hdl_module_base_t *)desc)->iface)->read(desc) == 
    !((hdl_gpio_pin_config_t *)((hdl_module_base_t *)(desc))->config)->inactive_default);
}

__STATIC_INLINE uint8_t hdl_gpio_is_set_as_inactive(const void *desc) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return (((hdl_gpio_pin_iface_t *)((hdl_module_base_t *)desc)->iface)->read_ouput(desc) == 
    ((hdl_gpio_pin_config_t *)((hdl_module_base_t *)(desc))->config)->inactive_default);
}

__STATIC_INLINE uint8_t hdl_gpio_is_set_as_active(const void *desc) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return (((hdl_gpio_pin_iface_t *)((hdl_module_base_t *)desc)->iface)->read_ouput(desc) == 
    !((hdl_gpio_pin_config_t *)((hdl_module_base_t *)(desc))->config)->inactive_default);
}

#endif // HDL_GPIO_H_
