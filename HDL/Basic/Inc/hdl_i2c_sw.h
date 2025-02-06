#ifndef HDL_I2C_SW_H_
#define HDL_I2C_SW_H_

#include "hdl_i2c.h"

#define HDL_I2C_SW_VAR_SIZE     36

/* depends on
  gpio scl
  gpio sda
  tick_counter
*/

typedef struct {
  uint32_t switch_delay;
} hdl_i2c_sw_hw_config_t;

hdl_module_new_t(hdl_i2c_sw_t, HDL_I2C_SW_VAR_SIZE, hdl_i2c_config_t, hdl_i2c_iface_t);

extern const hdl_i2c_iface_t hdl_i2c_sw_iface;

#endif /* HDL_I2C_SW_H_ */
