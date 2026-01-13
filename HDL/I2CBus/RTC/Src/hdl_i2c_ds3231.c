#include "hdl_iface.h"

typedef struct {
  coroutine_t worker;
  hdl_i2c_ds3231_data_t *data;
  uint8_t awaiting : 1,
          mode     : 1;
} hdl_i2c_ds3231_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_i2c_ds3231_var_t, *((hdl_i2c_ds3231_t *)0)->obj_var, HDL_I2C_DS3231_VAR_SIZE, hdl_i2c_ds3231.h);

#define RTC_REG_SECONDS    0x00
#define RTC_REG_AMOUNT     0x07

#define RTC_REG_MONTH_C    0x80 /* year: C=0 20xx, C=1 19xx*/

#define RTC_MODE_READ   0
#define RTC_MODE_WRITE  1


static uint8_t _ds3231_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_i2c_ds3231_t *ds3231 = (hdl_i2c_ds3231_t *)arg;
  hdl_i2c_mem_t *mem = (hdl_i2c_mem_t *)ds3231->dependencies[0];
  hdl_i2c_ds3231_var_t *ds3231_var = (hdl_i2c_ds3231_var_t *)ds3231->obj_var;
  if(ds3231_var->data != NULL) {
    if(ds3231_var->awaiting) {
      hdl_i2c_mem_state_t state = hdl_i2c_mem_state(mem);
      if(!(state & HDL_I2C_MEM_BUSY)) {
        if(state & HDL_I2C_MEM_XFER_FAIL) 
          ds3231_var->data->state = HDL_I2C_DS3231_XFER_STATE_FAIL;
        else {
          ds3231_var->data->month &= ~RTC_REG_MONTH_C;
          ds3231_var->data->state = HDL_I2C_DS3231_XFER_STATE_READY;
        }
        ds3231_var->data = NULL;
        hdl_give(mem, ds3231);
      }
    }
    else {
      if(hdl_take(mem, ds3231)) {
        if(ds3231_var->mode == RTC_MODE_READ) {
          if(hdl_i2c_mem_read_r1(mem, ds3231->config, RTC_REG_SECONDS, (uint8_t *)ds3231_var->data, RTC_REG_AMOUNT))
            ds3231_var->awaiting = HDL_TRUE;
        }
        else {
          if(hdl_i2c_mem_write_r1(mem, ds3231->config, RTC_REG_SECONDS, (uint8_t *)ds3231_var->data, RTC_REG_AMOUNT))
            ds3231_var->awaiting = HDL_TRUE;
        }
      }
    }
  }
  return cancel;
}

static hdl_module_state_t _hdl_i2c_ds3231(const void *desc, uint8_t enable) {
  hdl_i2c_ds3231_t *ds3231 = (hdl_i2c_ds3231_t *)desc;
  hdl_i2c_ds3231_var_t *ds3231_var = (hdl_i2c_ds3231_var_t *)ds3231->obj_var;
  if(enable) {
    ds3231_var->data = NULL;
    coroutine_add(&ds3231_var->worker, &_ds3231_worker, (void*)ds3231);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&ds3231_var->worker);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_i2c_ds3231_get_set(const void *desc, hdl_i2c_ds3231_data_t *data, uint8_t mode) {
  hdl_i2c_ds3231_t *ds3231 = (hdl_i2c_ds3231_t *)desc;
  hdl_i2c_ds3231_var_t *ds3231_var = (hdl_i2c_ds3231_var_t *)ds3231->obj_var;
  if(ds3231_var->data != NULL) return HDL_FALSE;
  data->state = HDL_I2C_DS3231_XFER_STATE_BUSY;
  ds3231_var->data = data;
  ds3231_var->awaiting = HDL_FALSE;
  ds3231_var->mode = mode;
  return HDL_TRUE;
}

static uint8_t _hdl_i2c_ds3231_get(const void *desc, hdl_i2c_ds3231_data_t *out_data) {
  return _hdl_i2c_ds3231_get_set(desc, out_data, RTC_MODE_READ);
}

static uint8_t _hdl_i2c_ds3231_set(const void *desc, hdl_i2c_ds3231_data_t *data) {
  return _hdl_i2c_ds3231_get_set(desc, data, RTC_MODE_WRITE);
}

const hdl_i2c_ds3231_iface_t hdl_i2c_ds3231_iface = {
  .init = &_hdl_i2c_ds3231,
  .get = &_hdl_i2c_ds3231_get,
  .set = &_hdl_i2c_ds3231_set
};
