#include "app.h"

ModbusIface_t mb_iface = {
  .lAvailableToRead = &fifo_available_to_read,
  .lAvailableToWrite = &fifo_available_to_write,
  .lRead = &fifo_read,
  .lWrite = &fifo_write,
  .vFlush = &fifo_flush,
  .pxTimerPhy = (void *)&mod_timer_ms,
  .ulTimerMs = &hdl_time_counter_get
};

static uint8_t mb_payload[256];

const ModbusConfig_t mb_cnf = {
  .bAsciiMode = 1,
  .pxIface = &mb_iface,
  .pucPayLoadBuffer = mb_payload,
  .ucPayLoadBufferSize = 255,
  .rx_timeout = 500,
  .tx_timeout = 500
};

static uint8_t _modbus_work_handler(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  vModbusWork((Modbus_t *)arg);
  return cancel;
}

Modbus_t *modbus_init(hdl_stream_buffer_t *hdl_stream) {
  static Modbus_t mb;
  static coroutine_t modbus_worker;
  mb_iface.pxRxPhy = hdl_stream->stream.pxIFifo,
  mb_iface.pxTxPhy = hdl_stream->stream.pxOFifo,
  bModbusInit(&mb, &mb_cnf);
  coroutine_add(&modbus_worker, &_modbus_work_handler, (void *)&mb);
  return &mb;
}
