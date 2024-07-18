#ifndef HDL_I2C_SERVER_H_
#define HDL_I2C_SERVER_H_

#include "port_i2c.h"

typedef struct {
  hdl_module_t module;
  hdl_i2c_server_config_t *config;
  //PRIVATE(hdl, HDL_I2C_CLIENT_PRV_SIZE);
} hdl_i2c_server_t;

#endif