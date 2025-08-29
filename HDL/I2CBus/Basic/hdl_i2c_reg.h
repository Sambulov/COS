#ifndef HDL_I2C_REG_H_
#define HDL_I2C_REG_H_

#define HDL_I2C_REG_IO_PRV_SIZE     44

typedef struct {
  PRIVATE(hdl, HDL_I2C_REG_IO_PRV_SIZE);
} hdl_i2c_reg_io_t;

typedef struct {
  const uint8_t *cmd;
  uint8_t *reg_buf;
  uint32_t cmd_size;
  uint32_t xfer_size;
  const void *mod_i2c;
  const void *bus_lock_obj;
  uint8_t dev_addr;
  uint8_t retry;
} hdl_i2c_reg_cnf_t;

void hdl_i2c_reg_io_init(hdl_i2c_reg_io_t *reg_io, const hdl_i2c_reg_cnf_t *reg_cnf);

int8_t hdl_i2c_reg_read(hdl_i2c_reg_io_t *reg_io);
int8_t hdl_i2c_reg_write(hdl_i2c_reg_io_t *reg_io);


#endif /* HDL_I2C_REG_H_ */
