#include "hdl_iface.h"

typedef struct {
  coroutine_t worker;
  hdl_i2c_reg_io_t reg_io;
  int32_t temp;
  uint8_t sns_reg_val[6];
  uint8_t sns_reg_adr;
  uint8_t sns_reg_size;
  uint8_t state     : 4,
          ready     : 1,
          reg_prep  : 1,
          reg_rsync : 1,
          reg_wsync : 1;
  hdl_i2c_lis2dh12_data_t *data;
} hdl_i2c_lis2dh12_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_i2c_lis2dh12_var_t, *((hdl_i2c_lis2dh12_t *)0)->obj_var, HDL_I2C_LIS2DH12_VAR_SIZE, hdl_i2c_lis2dh12.h);

#define READ_MULTIPLE    0x80
//#define READ_MULTIPLE    0x00

#define STATUS_REG_AUX   0x06
#define OUT_TEMP_L       0x0c
#define OUT_TEMP_H       0x0d
#define WHO_AM_I         0x0f
#define CTRL_REG0        0x1e
#define TEMP_CFG_REG     0x1f
#define CTRL_REG1        0x20
#define CTRL_REG2        0x21
#define CTRL_REG3        0x22
#define CTRL_REG4        0x23
#define CTRL_REG5        0x24
#define CTRL_REG6        0x25
#define REFERENCE        0x26
#define STATUS_REG       0x27
#define OUT_X_L          0x28
#define OUT_X_H          0x29
#define OUT_Y_L          0x2a
#define OUT_Y_H          0x2b
#define OUT_Z_L          0x2c
#define OUT_Z_H          0x2d
#define FIFO_CTRL_REG    0x2e
#define FIFO_SRC_REG     0x2f
#define INT1_CFG         0x30
#define INT1_SRC         0x31
#define INT1_THS         0x32
#define INT1_DURATION    0x33
#define INT2_CFG         0x34
#define INT2_SRC         0x35
#define INT2_THS         0x36
#define INT2_DURATION    0x37
#define CLICK_CFG        0x38
#define CLICK_SRC        0x39
#define CLICK_THS        0x3a
#define TIME_LIMIT       0x3b
#define TIME_LATENCY     0x3c
#define TIME_WINDOW      0x3d
#define ACT_THS          0x3e
#define ACT_DUR          0x3f

#define REG_FIX_WHO_AM_I  0b00110011

#define REG_FIX_CTRL0     0b00010000
#define REG_MASK_CTRL0    0b10000000

#define REG_FIX_CTRL1     0b00000000
#define REG_MASK_CTRL1    0b11111111

#define REG_FIX_CTRL2     0b00000000
#define REG_MASK_CTRL2    0b11111111

#define REG_FIX_CTRL3     0b00000000
#define REG_MASK_CTRL3    0b11110110

#define REG_FIX_CTRL4     0b00000000
#define REG_MASK_CTRL4    0b11111111

#define REG_FIX_CTRL5     0b00000000
#define REG_MASK_CTRL5    0b11001111

#define REG_FIX_CTRL6     0b00000000
#define REG_MASK_CTRL6    0b11111010

#define LIS2DH12_REG_CNF0_SYNC       0
#define LIS2DH12_REG_CNF1_SYNC       1
#define LIS2DH12_REG_CNF2_SYNC       2
#define LIS2DH12_REG_CNF3_SYNC       3
#define LIS2DH12_REG_CNF4_SYNC       4
#define LIS2DH12_REG_CNF5_SYNC       5
#define LIS2DH12_REG_CNF6_SYNC       6
#define LIS2DH12_REG_AXIS_SYNC       7
#define LIS2DH12_REG_AXIS_COMPLETE   8

static uint8_t _lis2dh12_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_i2c_lis2dh12_t *lis2dh12 = (hdl_i2c_lis2dh12_t *)arg;
  hdl_i2c_lis2dh12_var_t *lis2dh12_var = (hdl_i2c_lis2dh12_var_t *)lis2dh12->obj_var;

  if(lis2dh12_var->reg_prep) {
    hdl_i2c_reg_cnf_t reg_cnf = {
      .cmd = &lis2dh12_var->sns_reg_adr,
      .cmd_size = 1,
      .dev_addr = lis2dh12->config->chip_address,
      .bus_lock_obj = lis2dh12,
      .mod_i2c = lis2dh12->dependencies[0],
      .reg_buf = (uint8_t *)&lis2dh12_var->sns_reg_val,
      .xfer_size = lis2dh12_var->sns_reg_size,
      .retry = 3
    };
    hdl_i2c_reg_io_init(&lis2dh12_var->reg_io, &reg_cnf);
    lis2dh12_var->reg_prep = 0;
  }
  if(lis2dh12_var->reg_wsync) {
    int8_t res = hdl_i2c_reg_write(&lis2dh12_var->reg_io);
    if(!res) return cancel;
    if(res < 0) {
      lis2dh12_var->state = LIS2DH12_REG_CNF0_SYNC;
      lis2dh12_var->ready = 0;
      if(lis2dh12_var->data != NULL) {
        lis2dh12_var->data->state = -1;
        lis2dh12_var->data = NULL;
      }
    }
    lis2dh12_var->reg_wsync = 0;
  }
  if(lis2dh12_var->reg_rsync) {
    int8_t res = hdl_i2c_reg_read(&lis2dh12_var->reg_io);
    if(!res) return cancel;
    if(res < 0) {
      lis2dh12_var->state = LIS2DH12_REG_CNF0_SYNC;
      lis2dh12_var->ready = 0;
      if(lis2dh12_var->data != NULL) {
        lis2dh12_var->data->state = -1;
        lis2dh12_var->data = NULL;
      }
    }
    lis2dh12_var->reg_rsync = 0;
  }

  switch (lis2dh12_var->state) {
    case LIS2DH12_REG_CNF0_SYNC:
      lis2dh12_var->sns_reg_adr = CTRL_REG0;
      lis2dh12_var->sns_reg_val[0] = ((lis2dh12->config->cnf_reg0 & REG_MASK_CTRL0) | REG_FIX_CTRL0);
      lis2dh12_var->sns_reg_size = 1;
      lis2dh12_var->reg_prep = 1;
      lis2dh12_var->reg_wsync = 1;
      lis2dh12_var->state = LIS2DH12_REG_CNF1_SYNC;
      break;
    case LIS2DH12_REG_CNF1_SYNC:
      lis2dh12_var->sns_reg_adr = CTRL_REG1;
      lis2dh12_var->sns_reg_val[0] = ((lis2dh12->config->cnf_reg1 & REG_MASK_CTRL1) | REG_FIX_CTRL1);
      lis2dh12_var->sns_reg_size = 1;
      lis2dh12_var->reg_prep = 1;
      lis2dh12_var->reg_wsync = 1;
      lis2dh12_var->state = LIS2DH12_REG_CNF2_SYNC;
      break;
    case LIS2DH12_REG_CNF2_SYNC:
      lis2dh12_var->sns_reg_adr = CTRL_REG2;
      lis2dh12_var->sns_reg_val[0] = ((lis2dh12->config->cnf_reg1 & REG_MASK_CTRL2) | REG_FIX_CTRL2);
      lis2dh12_var->sns_reg_size = 1;
      lis2dh12_var->reg_prep = 1;
      lis2dh12_var->reg_wsync = 1;
      lis2dh12_var->state = LIS2DH12_REG_CNF3_SYNC;
      break;
    case LIS2DH12_REG_CNF3_SYNC:
      lis2dh12_var->sns_reg_adr = CTRL_REG3;
      lis2dh12_var->sns_reg_val[0] = ((lis2dh12->config->cnf_reg1 & REG_MASK_CTRL3) | REG_FIX_CTRL3);
      lis2dh12_var->sns_reg_size = 1;
      lis2dh12_var->reg_prep = 1;
      lis2dh12_var->reg_wsync = 1;
      lis2dh12_var->state = LIS2DH12_REG_CNF4_SYNC;
      break;
    case LIS2DH12_REG_CNF4_SYNC:
      lis2dh12_var->sns_reg_adr = CTRL_REG4;
      lis2dh12_var->sns_reg_val[0] = ((lis2dh12->config->cnf_reg1 & REG_MASK_CTRL4) | REG_FIX_CTRL4);
      lis2dh12_var->sns_reg_size = 1;
      lis2dh12_var->reg_prep = 1;
      lis2dh12_var->reg_wsync = 1;
      lis2dh12_var->state = LIS2DH12_REG_CNF5_SYNC;
      break;
    case LIS2DH12_REG_CNF5_SYNC:
      lis2dh12_var->sns_reg_adr = CTRL_REG5;
      lis2dh12_var->sns_reg_val[0] = ((lis2dh12->config->cnf_reg1 & REG_MASK_CTRL5) | REG_FIX_CTRL5);
      lis2dh12_var->sns_reg_size = 1;
      lis2dh12_var->reg_prep = 1;
      lis2dh12_var->reg_wsync = 1;
      lis2dh12_var->state = LIS2DH12_REG_CNF6_SYNC;
      break;
    case LIS2DH12_REG_CNF6_SYNC:
      lis2dh12_var->sns_reg_adr = CTRL_REG6;
      lis2dh12_var->sns_reg_val[0] = ((lis2dh12->config->cnf_reg1 & REG_MASK_CTRL6) | REG_FIX_CTRL6);
      lis2dh12_var->sns_reg_size = 1;
      lis2dh12_var->reg_prep = 1;
      lis2dh12_var->reg_wsync = 1;
      lis2dh12_var->state = LIS2DH12_REG_AXIS_SYNC;
      break;
    case LIS2DH12_REG_AXIS_SYNC:
      lis2dh12_var->ready = 1;
      if(lis2dh12_var->data != NULL) {
        lis2dh12_var->sns_reg_adr = OUT_X_L | READ_MULTIPLE;
        lis2dh12_var->sns_reg_size = 6;
        lis2dh12_var->reg_prep = 1;
        lis2dh12_var->reg_rsync = 1;
        lis2dh12_var->state = LIS2DH12_REG_AXIS_COMPLETE;
      }
      break;

    case LIS2DH12_REG_AXIS_COMPLETE:
      lis2dh12_var->data->gx = swap_bytes(*(int16_t *)(&lis2dh12_var->sns_reg_val[0]));
      lis2dh12_var->data->gy = swap_bytes(*(int16_t *)(&lis2dh12_var->sns_reg_val[2]));
      lis2dh12_var->data->gz = swap_bytes(*(int16_t *)(&lis2dh12_var->sns_reg_val[4]));
      lis2dh12_var->data->state = 1;
      lis2dh12_var->data = NULL;
      lis2dh12_var->state = LIS2DH12_REG_AXIS_SYNC;
    default:
      break;
  }
  return cancel;
}

static hdl_module_state_t _hdl_i2c_lis2dh12(const void *desc, uint8_t enable) {
  hdl_i2c_lis2dh12_t *lis2dh12 = (hdl_i2c_lis2dh12_t *)desc;
  hdl_i2c_lis2dh12_var_t *lis2dh12_var = (hdl_i2c_lis2dh12_var_t *)lis2dh12->obj_var;
  if(enable) {
    lis2dh12_var->reg_rsync = 0;
    lis2dh12_var->reg_wsync = 0;
    lis2dh12_var->data = NULL;
    lis2dh12_var->state = LIS2DH12_REG_CNF0_SYNC;
    coroutine_add(&lis2dh12_var->worker, &_lis2dh12_worker, (void*)lis2dh12);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&lis2dh12_var->worker);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_i2c_lis2dh12_get(const void *desc, hdl_i2c_lis2dh12_data_t *out_data) {
  hdl_i2c_lis2dh12_t *lis2dh12 = (hdl_i2c_lis2dh12_t *)desc;
  hdl_i2c_lis2dh12_var_t *lis2dh12_var = (hdl_i2c_lis2dh12_var_t *)lis2dh12->obj_var;
  if(!lis2dh12_var->ready) return HDL_FALSE;
  lis2dh12_var->data = out_data;
  return HDL_TRUE;
}

const hdl_i2c_lis2dh12_iface_t hdl_i2c_lis2dh12_iface = {
  .init = &_hdl_i2c_lis2dh12,
  .get = &_hdl_i2c_lis2dh12_get
};
