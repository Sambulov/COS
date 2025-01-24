#ifndef HDL_XL9535_H_
#define HDL_XL9535_H_

#define HDL_XL9535_PORT_PRV_SIZE  60

typedef struct {
} hdl_xl9535_port_config_t;

/* depends on
  hdl_i2c
  hdl_time_counter
  hdl_gpio int
*/
typedef struct {
  hdl_module_t module;
  const hdl_xl9535_port_config_t *config;
  PRIVATE(hdl, HDL_XL9535_PORT_PRV_SIZE);
} hdl_xl9535_port_t;

#define HDL_XL9535_CNF_OUTPUT_LOW           0
#define HDL_XL9535_CNF_OUTPUT_HIGH          1

#define HDL_XL9535_CNF_POLARITY_RETAINED    0
#define HDL_XL9535_CNF_POLARITY_INVERTED    1

#define HDL_XL9535_CNF_MODE_OUTPUT          0
#define HDL_XL9535_CNF_MODE_INPUT           1

typedef struct {
  hdl_gpio_state inactive_default;
  uint8_t output   :1,
          polarity :1,
          mode     :1;
} hdl_xl9535_pin_config_t;

typedef struct {
  hdl_module_t module;
  const hdl_xl9535_pin_config_t *config;
} hdl_xl9535_pin_t;

hdl_module_state_t hdl_xl9535_port(void *desc, const uint8_t enable);
hdl_module_state_t hdl_xl9535_pin(void *desc, const uint8_t enable);

hdl_gpio_state hdl_xl9535_read(const hdl_xl9535_pin_t *gpio);
hdl_gpio_state hdl_xl9535_read_output(const hdl_xl9535_pin_t *gpio);
void hdl_xl9535_write(const hdl_xl9535_pin_t *gpio, const hdl_gpio_state state);
void hdl_xl9535_toggle(const hdl_xl9535_pin_t *gpio);

#endif /* HDL_XL9535_H_ */
