#include "app.h"

const modbus_iface_t mb_iface = {
  .pfRead = (modbus_iface_read_t)&fifo_read,
  .pfWrite = (modbus_iface_write_t)&fifo_write,
  .pfTimer = (modbus_iface_timer_t)&hdl_time_counter_get
};

static uint8_t _modbus_work_handler(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  modbus_work((modbus_t *)arg);
  return cancel;
}

modbus_t *app_modbus_init(hdl_stream_buffer_t *hdl_stream) {
  static modbus_t mb;
  static coroutine_t modbus_worker;
  static uint8_t mb_payload[256];
  modbus_config_t mb_cnf = {
    .bAsciiMode = 0,
    .pxIface = &mb_iface,
    .pucPayLoadBuffer = mb_payload,
    .ucPayLoadBufferSize = 255,
    .rx_timeout = 500,
    .tx_timeout = 500,
    .pxTimerContext = (void *)&mod_timer_ms,
    .pxRxContext = hdl_stream->stream.pxIFifo,
    .pxTxContext = hdl_stream->stream.pxOFifo,
  };
  modbus_init(&mb, &mb_cnf);
  coroutine_add(&modbus_worker, &_modbus_work_handler, (void *)&mb);
  return &mb;
}
