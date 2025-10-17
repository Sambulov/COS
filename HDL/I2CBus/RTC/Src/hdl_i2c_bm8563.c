#include "hdl_iface.h"

typedef struct {
  coroutine_t worker;
  hdl_i2c_bm8563_data_t *data;
  uint8_t awaiting : 1,
          mode     : 1;
} hdl_i2c_bm8563_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_i2c_bm8563_var_t, *((hdl_i2c_bm8563_t *)0)->obj_var, HDL_I2C_BM8563_VAR_SIZE, hdl_i2c_bm8563.h);

#define RTC_REG_SECONDS    0x02
#define RTC_REG_MONTH      0x07

#define RTC_REG_SECONDS_VL 0x80  /* power reset flag */

#define RTC_REG_MONTH_C    0x80 /* year: C=0 20xx, C=1 19xx*/

#define RTC_MODE_READ   0
#define RTC_MODE_WRITE  1


static uint8_t _bm8563_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_i2c_bm8563_t *bm8563 = (hdl_i2c_bm8563_t *)arg;
  hdl_i2c_mem_t *mem = (hdl_i2c_mem_t *)bm8563->dependencies[0];
  hdl_i2c_bm8563_var_t *bm8563_var = (hdl_i2c_bm8563_var_t *)bm8563->obj_var;
  if(bm8563_var->data != NULL) {
    if(bm8563_var->awaiting) {
      hdl_i2c_mem_state_t state = hdl_i2c_mem_state(mem);
      if(!(state & HDL_I2C_MEM_BUSY)) {
        if(state & HDL_I2C_MEM_XFER_FAIL) 
          bm8563_var->data->state = HDL_I2C_BM8563_XFER_STATE_FAIL;
        else {
          bm8563_var->data->sec  &= ~RTC_REG_SECONDS_VL;
          bm8563_var->data->month &= ~RTC_REG_MONTH_C;
          bm8563_var->data->state = HDL_I2C_BM8563_XFER_STATE_READY;
        }
        bm8563_var->data = NULL;
        hdl_give(mem, bm8563);
      }
    }
    else {
      if(hdl_take(mem, bm8563)) {
        if(bm8563_var->mode == RTC_MODE_READ) {
          if(hdl_i2c_mem_read_r1(mem, bm8563->config, RTC_REG_SECONDS, (uint8_t *)bm8563_var->data, 7))
            bm8563_var->awaiting = HDL_TRUE;
        }
        else {
          if(hdl_i2c_mem_write_r1(mem, bm8563->config, RTC_REG_SECONDS, (uint8_t *)bm8563_var->data, 7))
            bm8563_var->awaiting = HDL_TRUE;
        }
      }
    }
  }
  return cancel;
}

static hdl_module_state_t _hdl_i2c_bm8563(const void *desc, uint8_t enable) {
  hdl_i2c_bm8563_t *bm8563 = (hdl_i2c_bm8563_t *)desc;
  hdl_i2c_bm8563_var_t *bm8563_var = (hdl_i2c_bm8563_var_t *)bm8563->obj_var;
  if(enable) {
    bm8563_var->data = NULL;
    coroutine_add(&bm8563_var->worker, &_bm8563_worker, (void*)bm8563);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&bm8563_var->worker);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_i2c_bm8563_get_set(const void *desc, hdl_i2c_bm8563_data_t *data, uint8_t mode) {
  hdl_i2c_bm8563_t *bm8563 = (hdl_i2c_bm8563_t *)desc;
  hdl_i2c_bm8563_var_t *bm8563_var = (hdl_i2c_bm8563_var_t *)bm8563->obj_var;
  if(bm8563_var->data != NULL) return HDL_FALSE;
  data->state = HDL_I2C_BM8563_XFER_STATE_BUSY;
  bm8563_var->data = data;
  bm8563_var->awaiting = HDL_FALSE;
  bm8563_var->mode = mode;
  return HDL_TRUE;
}

static uint8_t _hdl_i2c_bm8563_get(const void *desc, hdl_i2c_bm8563_data_t *out_data) {
  return _hdl_i2c_bm8563_get_set(desc, out_data, RTC_MODE_READ);
}

static uint8_t _hdl_i2c_bm8563_set(const void *desc, hdl_i2c_bm8563_data_t *data) {
  return _hdl_i2c_bm8563_get_set(desc, data, RTC_MODE_WRITE);
}

const hdl_i2c_bm8563_iface_t hdl_i2c_bm8563_iface = {
  .init = &_hdl_i2c_bm8563,
  .get = &_hdl_i2c_bm8563_get,
  .set = &_hdl_i2c_bm8563_set
};
