#include "app.h"

typedef struct {
  hdl_module_t module;
  const hdl_uspd_dio_port_config_t *config;
  struct {
    hdl_gpio_state input_hold_state;
  } private;
} bldl_uspd_dio_port_prv_t;

HDL_ASSERRT_STRUCTURE_CAST(bldl_uspd_dio_port_prv_t, bldl_uspd_dio_port_t, HDL_USPD_DIO_PORT_PRV_SIZE, uspd_dio.h);

hdl_module_state_t bldl_uspd_dio_port(void *desc, uint8_t enable) {
  bldl_uspd_dio_port_prv_t *dio = (bldl_uspd_dio_port_prv_t*)desc;
  if(enable) {
    return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_UNLOADED;
}

hdl_gpio_state bldl_uspd_dio_read(const bldl_uspd_dio_port_t *dio) {
  bldl_uspd_dio_port_prv_t *_dio = (bldl_uspd_dio_port_prv_t*)dio;
  hdl_gpio_pin_t *di = (hdl_gpio_pin_t *)dio->module.dependencies[0];
  hdl_gpio_pin_t *di_dis = (hdl_gpio_pin_t *)dio->module.dependencies[2];
  if(hdl_gpio_is_active(di_dis))
    return _dio->private.input_hold_state;
  return !hdl_gpio_read(di);
}

hdl_gpio_state bldl_uspd_dio_read_output(const bldl_uspd_dio_port_t *dio) {
  hdl_gpio_pin_t *_do = (hdl_gpio_pin_t *)dio->module.dependencies[1];
  return hdl_gpio_is_active(_do)? HDL_GPIO_LOW: HDL_GPIO_HIGH;
}

void bldl_uspd_dio_write(const bldl_uspd_dio_port_t *dio, const hdl_gpio_state state) {
  hdl_gpio_pin_t *_do = (hdl_gpio_pin_t *)dio->module.dependencies[1];
  if(state == HDL_GPIO_LOW) {
    hdl_gpio_set_active(_do);
  }
  else {
    hdl_gpio_set_inactive(_do);
  }
}

void bldl_uspd_dio_input_hold(const bldl_uspd_dio_port_t *dio, const hdl_gpio_state state) {
  bldl_uspd_dio_port_prv_t *_dio = (bldl_uspd_dio_port_prv_t*)dio;
  hdl_gpio_pin_t *di_dis = (hdl_gpio_pin_t *)dio->module.dependencies[2];
  _dio->private.input_hold_state = state;
  hdl_gpio_set_active(di_dis);
}

void bldl_uspd_dio_input_enable(const bldl_uspd_dio_port_t *dio) {
  hdl_gpio_pin_t *di_dis = (hdl_gpio_pin_t *)dio->module.dependencies[2];
  hdl_gpio_set_inactive(di_dis);
}
