#include "app.h"
#include "CodeLib.h"

typedef struct {
    hdl_module_t module;
  struct {
    atb3500_i2c_gateway_proto_tx_t tx_data;
    hdl_i2c_message_t msg;
    coroutine_t worker;
  } private;
} atb3500_i2c_gateway_private_t;

static uint8_t _i2c_gateway_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  atb3500_i2c_gateway_private_t *i2c_gateway = (atb3500_i2c_gateway_private_t *)arg;
  i2c_gateway->private.tx_data.address = i2c_gateway->private.msg.address;
  i2c_gateway->private.tx_data.length = i2c_gateway->private.msg.length;
  i2c_gateway->private.tx_data.options = i2c_gateway->private.msg.options;
  i2c_gateway->private.tx_data.status = i2c_gateway->private.msg.status;
  i2c_gateway->private.tx_data.transferred = i2c_gateway->private.msg.transferred;
  return cancel;
}

hdl_module_state_t atb3500_i2c_gateway(void *desc, uint8_t enable) {
  atb3500_i2c_gateway_private_t *i2c_gateway = (atb3500_i2c_gateway_private_t *)desc;
  if(enable) {
    i2c_gateway->private.tx_data.ovn = ATB3500_I2C_GATEWAY_OVN;
    i2c_gateway->private.msg.buffer = i2c_gateway->private.tx_data.buff;
    coroutine_add(&i2c_gateway->private.worker, &_i2c_gateway_worker, desc);
    return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_UNLOADED;
}

atb3500_i2c_gateway_proto_tx_t *atb3500_i2c_gateway_update(atb3500_i2c_gateway_t *desc, uint32_t recieved, atb3500_i2c_gateway_proto_rx_t *rx_data) {
  atb3500_i2c_gateway_private_t *i2c_gateway = (atb3500_i2c_gateway_private_t *)desc;
  hdl_i2c_t *i2c = (hdl_i2c_t *)i2c_gateway->module.dependencies[0];
  if(rx_data->config_magic == ATB3500_I2C_GATEWAY_MAGIC) {
    i2c_gateway->private.msg.address = rx_data->address;
    i2c_gateway->private.msg.length = MIN(ATB3500_MAX_BUFFER_SIZE, rx_data->length);
    i2c_gateway->private.msg.options = rx_data->options;
    uint32_t data_count = offsetof(atb3500_i2c_gateway_proto_rx_t, buff);
    if(data_count < recieved) {
      data_count = recieved - data_count;
      data_count = MIN(ATB3500_MAX_BUFFER_SIZE, data_count);
      mem_cpy(i2c_gateway->private.tx_data.buff, rx_data->buff, data_count);
    }
    hdl_i2c_transfer_message(i2c, &i2c_gateway->private.msg);
  }
  return &i2c_gateway->private.tx_data;
}
