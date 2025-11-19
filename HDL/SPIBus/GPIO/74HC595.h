#ifndef HDL_74HC595_H_
#define HDL_74HC595_H_

#define HDL_74HC595_PORT_VAR_SIZE  48

typedef struct {
    uint32_t shift_reg_length;
    void *port_regs;            /* static_malloc(shift_reg_length / 4) */
} hdl_74hc595_port_config_t;

/* depends on
  hdl_spi_client_ch
  hdl_gpio STCP
  hdl_gpio MR
  hdl_gpio OE
*/
hdl_module_new_t(hdl_74hc595_port_t, HDL_74HC595_PORT_VAR_SIZE, hdl_74hc595_port_config_t*, hdl_module_base_iface_t);

extern const hdl_module_base_iface_t hdl_74hc595_port_iface;

/* depends on
  hdl_74hc595_port_t
*/
hdl_module_new_t(hdl_74hc595_pin_t, 0, hdl_gpio_pin_config_t*, hdl_gpio_pin_iface_t);

extern const hdl_gpio_pin_iface_t hdl_74hc595_pin_iface;

#endif /* HDL_74HC595_H_ */
