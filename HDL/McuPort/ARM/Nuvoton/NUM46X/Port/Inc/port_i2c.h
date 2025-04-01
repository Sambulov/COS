#ifndef PORT_I2C_H_
#define PORT_I2C_H_

#include "hdl_i2c.h"

#define HDL_I2C_VAR_SIZE                    68

typedef struct {
  const uint32_t speed;        /* Max 1000000 */
  const uint8_t general_call_enable : 1,
                swap_scl_sda        : 1,
                addr_10_bits        : 1;
  hdl_interrupt_t *interrupt;
  hdl_i2c_channel_t **channels;
  void *phy;
} hdl_i2c_config_hw_t;

/* module depends on 
  hdl_clock_t HDL_CLOCK_TYPE_APB0 for I2C0, I2C2, I2C4 or HDL_CLOCK_TYPE_APB0 for I2C1, I2C3
  hdl_gpio_pin_t scl pin, 
  hdl_gpio_pin_t sda pin,
  hdl_interrupt_controller_t
  hdl_time_counter_t

  max 4 channels
*/ 
hdl_module_new_t(hdl_i2c_mcu_t, HDL_I2C_VAR_SIZE, hdl_i2c_config_t, hdl_i2c_iface_t);

extern const hdl_i2c_iface_t hdl_i2c_iface;

#endif // PORT_I2C_H_
