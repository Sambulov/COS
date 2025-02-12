#ifndef PORT_I2C_H_
#define PORT_I2C_H_

#include "hdl_i2c.h"

#define HDL_I2C_VAR_SIZE                    104

typedef struct {
  const uint32_t dtcy;         /* I2C_DTCY_2 or I2C_DTCY_16_9 */
  const uint32_t speed;        /* Max 1000000 */
  const uint8_t general_call_enable : 1,
                addr_10_bits        : 1,
                dual_address        : 1,
                stretch_enable      : 1;
  hdl_interrupt_t *err_interrupt;
  hdl_interrupt_t *ev_interrupt;
  const rcu_periph_enum rcu;
  uint32_t phy;
} hdl_i2c_config_hw_t;

hdl_module_new_t(hdl_i2c_t, HDL_I2C_VAR_SIZE, hdl_i2c_config_t, hdl_i2c_iface_t);

extern const hdl_i2c_iface_t hdl_i2c_iface;

#endif // PORT_I2C_H_
