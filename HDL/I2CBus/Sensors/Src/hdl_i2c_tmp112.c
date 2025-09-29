#include "hdl_iface.h"

typedef struct {
  coroutine_t worker;
  hdl_i2c_tmp112_data_t *data;
  uint16_t temp_sns_reg_val;
  uint8_t temp_sns_reg;
  uint8_t state;
} hdl_i2c_tmp112_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_i2c_tmp112_var_t, *((hdl_i2c_tmp112_t *)0)->obj_var, HDL_I2C_TMP112_VAR_SIZE, hdl_i2c_tmp112.h);

static uint8_t _tmp112_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_i2c_tmp112_t *tmp112 = (hdl_i2c_tmp112_t *)arg;
  hdl_i2c_mem_t *mem = (hdl_i2c_mem_t *)tmp112->dependencies[0];
  hdl_i2c_tmp112_var_t *tmp112_var = (hdl_i2c_tmp112_var_t *)tmp112->obj_var;
  if(tmp112_var->data != NULL) {
    if(tmp112_var->state == 0) {
      if(hdl_i2c_mem_read(mem, tmp112->config, tmp112_var->temp_sns_reg, (uint8_t *)&tmp112_var->temp_sns_reg_val, 2))
        tmp112_var->state++;
    }
    if(tmp112_var->state == 1) {
      hdl_i2c_mem_state_t res = hdl_i2c_mem_state(mem);
      if(!(res & HDL_I2C_MEM_BUSY)) {
        tmp112_var->data->state = -1;
        if(!(res & HDL_I2C_MEM_XFER_FAIL)) {
          int32_t temp = (int32_t)(swap_bytes(tmp112_var->temp_sns_reg_val) >> 4);
          temp = (625L * (temp + 880)) - 550000L;
          tmp112_var->data->temp = temp;
          tmp112_var->data->state = 1;
        }
        tmp112_var->data = NULL;
      }
    }
  }
  return cancel;
}

static hdl_module_state_t _hdl_i2c_tmp112(const void *desc, uint8_t enable) {
  hdl_i2c_tmp112_t *tmp112 = (hdl_i2c_tmp112_t *)desc;
  hdl_i2c_tmp112_var_t *tmp112_var = (hdl_i2c_tmp112_var_t *)tmp112->obj_var;
  if(enable) {
    tmp112_var->temp_sns_reg_val = 0x0000;
    tmp112_var->temp_sns_reg = 0x00;
    tmp112_var->state = 0;
    coroutine_add(&tmp112_var->worker, &_tmp112_worker, (void*)tmp112);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&tmp112_var->worker);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_i2c_tmp112_get(const void *desc, hdl_i2c_tmp112_data_t *out_data) {
  hdl_i2c_tmp112_t *tmp112 = (hdl_i2c_tmp112_t *)desc;
  hdl_i2c_tmp112_var_t *tmp112_var = (hdl_i2c_tmp112_var_t *)tmp112->obj_var;
  if(tmp112_var->data != NULL) return HDL_FALSE;
  tmp112_var->data = out_data;
  tmp112_var->data->state = 0;
  tmp112_var->state = 0;
  return HDL_TRUE;
}

const hdl_i2c_tmp112_iface_t hdl_i2c_tmp112_iface = {
  .init = &_hdl_i2c_tmp112,
  .get = &_hdl_i2c_tmp112_get
};
