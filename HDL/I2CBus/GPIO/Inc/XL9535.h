#ifndef HDL_XL9535_H_
#define HDL_XL9535_H_

#define HDL_XL9535_PORT_VAR_SIZE  60

typedef struct {
  uint16_t address;
} hdl_xl9535_port_config_t;

/* depends on
  hdl_i2c
  hdl_time_counter
  hdl_gpio int
*/
hdl_module_new_t(hdl_xl9535_port_t, HDL_XL9535_PORT_VAR_SIZE, hdl_xl9535_port_config_t, hdl_module_base_iface_t);

extern const hdl_module_base_iface_t hdl_xl9535_port_iface;

#define HDL_XL9535_CNF_OUTPUT_LOW           0
#define HDL_XL9535_CNF_OUTPUT_HIGH          1

#define HDL_XL9535_CNF_POLARITY_RETAINED    0
#define HDL_XL9535_CNF_POLARITY_INVERTED    1

#define HDL_XL9535_CNF_MODE_OUTPUT          0
#define HDL_XL9535_CNF_MODE_INPUT           1

typedef struct {
  hdl_gpio_state inactive_default;
  uint16_t pin_no;  /* 0 - 15*/
  uint8_t output   :1,
          polarity :1,
          mode     :1;
} hdl_xl9535_pin_config_t;

/* depends on
  hdl_xl9535_port_t
*/
hdl_module_new_t(hdl_xl9535_pin_t, 0, hdl_xl9535_pin_config_t, hdl_gpio_pin_iface_t);

extern const hdl_gpio_pin_iface_t hdl_xl9535_pin_iface;

#endif /* HDL_XL9535_H_ */
