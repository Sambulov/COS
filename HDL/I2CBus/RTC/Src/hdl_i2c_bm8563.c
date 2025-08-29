#include "hdl_iface.h"

typedef struct {
  coroutine_t worker;
  hdl_i2c_reg_io_t reg_io;
  hdl_i2c_bm8563_data_t *data;
  uint8_t state;
  uint8_t reg_addr;
} hdl_i2c_bm8563_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_i2c_bm8563_var_t, *((hdl_i2c_bm8563_t *)0)->obj_var, HDL_I2C_BM8563_VAR_SIZE, hdl_i2c_bm8563.h);

#define RTC_REG_SECONDS    0x02
#define RTC_REG_MONTH      0x07

#define RTC_REG_SECONDS_VL 0x80  /* power reset flag */

#define RTC_REG_MONTH_C    0x80 /* year: C=0 20xx, C=1 19xx*/

static uint8_t _bm8563_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_i2c_bm8563_t *bm8563 = (hdl_i2c_bm8563_t *)arg;
  hdl_i2c_bm8563_var_t *bm8563_var = (hdl_i2c_bm8563_var_t *)bm8563->obj_var;
  if(bm8563_var->data != NULL) {
    if((bm8563_var->state == 0) || (bm8563_var->state == 2)) {
      bm8563_var->reg_addr = RTC_REG_SECONDS;
      hdl_i2c_reg_cnf_t reg_cnf = {
        .cmd = &bm8563_var->reg_addr,
        .cmd_size = 1,
        .dev_addr = bm8563->config->chip_address,
        .bus_lock_obj = bm8563,
        .mod_i2c = bm8563->dependencies[0],
        .reg_buf = (uint8_t *)bm8563_var->data,
        .xfer_size = 7,
        .retry = 3
      };
      hdl_i2c_reg_io_init(&bm8563_var->reg_io, &reg_cnf);
      bm8563_var->state++;
    }
    int8_t res;
    if(((bm8563_var->state == 1) && (res = hdl_i2c_reg_read(&bm8563_var->reg_io))) ||
       ((bm8563_var->state == 3) && (res = hdl_i2c_reg_write(&bm8563_var->reg_io)))) {
      bm8563_var->data->sec  &= ~RTC_REG_SECONDS_VL;
      bm8563_var->data->month &= ~RTC_REG_MONTH_C;
      bm8563_var->data->state = res;
      bm8563_var->data = NULL;
    }
  }
  return cancel;
}

static hdl_module_state_t _hdl_i2c_bm8563(const void *desc, uint8_t enable) {
  hdl_i2c_bm8563_t *bm8563 = (hdl_i2c_bm8563_t *)desc;
  hdl_i2c_bm8563_var_t *bm8563_var = (hdl_i2c_bm8563_var_t *)bm8563->obj_var;
  if(enable) {
    bm8563_var->data = NULL;
    bm8563_var->state = 0;
    coroutine_add(&bm8563_var->worker, &_bm8563_worker, (void*)bm8563);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&bm8563_var->worker);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_i2c_bm8563_get(const void *desc, hdl_i2c_bm8563_data_t *out_data) {
  hdl_i2c_bm8563_t *bm8563 = (hdl_i2c_bm8563_t *)desc;
  hdl_i2c_bm8563_var_t *bm8563_var = (hdl_i2c_bm8563_var_t *)bm8563->obj_var;
  if(bm8563_var->data != NULL) return HDL_FALSE;
  out_data->state = 0;
  bm8563_var->data = out_data;
  bm8563_var->state = 0;
  return HDL_TRUE;
}

static uint8_t _hdl_i2c_bm8563_set(const void *desc, hdl_i2c_bm8563_data_t *data) {
  hdl_i2c_bm8563_t *bm8563 = (hdl_i2c_bm8563_t *)desc;
  hdl_i2c_bm8563_var_t *bm8563_var = (hdl_i2c_bm8563_var_t *)bm8563->obj_var;
  if(bm8563_var->data != NULL) return HDL_FALSE;
  data->state = 0;
  bm8563_var->data = data;
  bm8563_var->state = 2;
  return HDL_TRUE;
}

const hdl_i2c_bm8563_iface_t hdl_i2c_bm8563_iface = {
  .init = &_hdl_i2c_bm8563,
  .get = &_hdl_i2c_bm8563_get,
  .set = &_hdl_i2c_bm8563_set
};
