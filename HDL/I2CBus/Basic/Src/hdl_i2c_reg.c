#include "hdl_iface.h"

typedef struct {
  hdl_i2c_message_t msg;
  const uint8_t *cmd;
  uint8_t *reg_buf;
  uint32_t cmd_size;
  uint32_t xfer_size;
  const void *mod_i2c;
  const void *lock_obj;
  uint8_t dev_addr;
  uint8_t retry;
  uint8_t state;
} hdl_i2c_reg_io_prv_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_i2c_reg_io_prv_t, hdl_i2c_reg_io_t, HDL_I2C_REG_IO_PRV_SIZE, hdl_i2c_reg.h);

void hdl_i2c_reg_io_init(hdl_i2c_reg_io_t *reg_io, const hdl_i2c_reg_cnf_t *reg_cnf) {
  hdl_i2c_reg_io_prv_t *reg = (hdl_i2c_reg_io_prv_t *)reg_io;
  reg->cmd = reg_cnf->cmd;
  reg->reg_buf = reg_cnf->reg_buf;
  reg->cmd_size = reg_cnf->cmd_size;
  reg->xfer_size = reg_cnf->xfer_size;
  reg->mod_i2c = reg_cnf->mod_i2c;
  reg->lock_obj = reg_cnf->bus_lock_obj;
  reg->dev_addr = reg_cnf->dev_addr;
  reg->retry = reg_cnf->retry;
  reg->state = 0;
}

static int8_t hdl_i2c_reg_xfer(hdl_i2c_reg_io_prv_t *reg_io, uint8_t wr) {
  if(reg_io == NULL) return -1;
  switch (reg_io->state) {
    case 0:
      reg_io->msg.address = reg_io->dev_addr;
      reg_io->msg.buffer = (uint8_t *)reg_io->cmd;
      reg_io->msg.length = reg_io->cmd_size;
      reg_io->msg.options = HDL_I2C_MESSAGE_WRITE_HALT;
      reg_io->state++;
      /* fall through */
    case 1:
    case 4:
      if(!hdl_take(reg_io->mod_i2c, reg_io->lock_obj) || !hdl_i2c_transfer(reg_io->mod_i2c, &reg_io->msg)) break;
      reg_io->state++;
      /* fall through */
    case 5:
    case 2:
      if(!(reg_io->msg.status & HDL_I2C_MESSAGE_STATUS_COMPLETE)) break;
      if(reg_io->msg.status & HDL_I2C_MESSAGE_FAULT_MASK) {
        if(reg_io->retry--) reg_io->state = 0;
        else reg_io->state = 7;
        hdl_give(reg_io->mod_i2c, reg_io->lock_obj);
        break;
      }
      reg_io->state++;
      break;
    case 3:
      reg_io->msg.address = reg_io->dev_addr;
      reg_io->msg.buffer = reg_io->reg_buf;
      reg_io->msg.length = reg_io->xfer_size;
      if(wr) reg_io->msg.options = HDL_I2C_MESSAGE_STOP;
      else reg_io->msg.options = HDL_I2C_MESSAGE_READ_STOP;
      reg_io->state++;
      break;
    case 6:
    case 7:
    default:
      hdl_give(reg_io->mod_i2c, reg_io->lock_obj);
      return (reg_io->state == 6)? 1: -1;
  }
  return 0;
}

inline int8_t hdl_i2c_reg_read(hdl_i2c_reg_io_t *reg_io) {
  return hdl_i2c_reg_xfer((hdl_i2c_reg_io_prv_t *)reg_io, 0);
}

inline int8_t hdl_i2c_reg_write(hdl_i2c_reg_io_t *reg_io) {
  return hdl_i2c_reg_xfer((hdl_i2c_reg_io_prv_t *)reg_io, 1);
}
