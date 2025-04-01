#include "app.h"

#define UART_BUFFER_SIZE          512


/* workaround for line echo function, fifo doesn't support any end of transmission handlers */
static uint8_t eot = 0;
void hdl_transceiver_eot_cb(void *context) {
  (void)context;
  eot = 1;
}

hdl_stream_buffer_t *uart_stream_init() {
  static uint8_t uart_buf_rx[UART_BUFFER_SIZE], uart_buf_tx[UART_BUFFER_SIZE];
  static hdl_stream_buffer_t uart_stream = {
    .buffer_in = uart_buf_rx,
    .buffer_out = uart_buf_tx,
    .size_in = UART_BUFFER_SIZE,
    .size_out = UART_BUFFER_SIZE,
  };
  static hdl_transceiver_t uart_trx;
  hdl_stream_buffer_init(&uart_stream, HDL_TRUE);
  hdl_transceiver_from_stream(&uart_stream.stream, &uart_trx, HDL_FALSE);
  uart_trx.end_of_transmission = &hdl_transceiver_eot_cb;
  hdl_uart_set_transceiver(&mod_uart, &uart_trx, 0);
  return &uart_stream;
}

#define STREAM_PRINT_DELAY  1000

void stream_printer(uint32_t event_trigger, void *sender, void *context) {
  (void)event_trigger; (void)sender;
  hdl_stream_buffer_t *uart_stream = (hdl_stream_buffer_t *)context;
  uint32_t now = hdl_time_counter_get(&mod_timer_ms);
  stream_printf(&uart_stream->stream, (uint8_t *)"Hello world %%, %u\r\n", now);
}

const hdl_timer_t printer_timer = {
  .iface = &hdl_timer_iface,
  .dependencies= hdl_module_dependencies(&mod_timer_ms),
  .config = hdl_module_config(hdl_timer_config_t,
    .mode = HDL_TIMER_MODE_LOOP,
    .delay = STREAM_PRINT_DELAY
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_TIMER_VAR_SIZE)
};

static uint8_t _stream_work_handler(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_stream_buffer_t *uart_stream = (hdl_stream_buffer_t *)arg;
  static uint8_t state = 0;
  if(stream_read_available(&uart_stream->stream)) {
    uint8_t tmp_buf[128];
    int32_t len = stream_read(&uart_stream->stream, tmp_buf, 128);
    if(len > 0) {
      if(state == 0) {
        stream_write_string(&uart_stream->stream, (uint8_t *)"Echo: ");
        state = 1;
      }
      stream_write(&uart_stream->stream, tmp_buf, len);
      
    }
  }
  else if(eot){
    eot = 0;
    state = 0;
    stream_write_string(&uart_stream->stream, (uint8_t *)"\r\n");
  }
  return cancel;
}

void stream_test(hdl_stream_buffer_t *hdl_stream) {
  static coroutine_t stream_worker;
  static hdl_delegate_t stream_delegate;
  stream_delegate.context = (void *)hdl_stream;
  stream_delegate.handler = &stream_printer;
  hdl_enable(&printer_timer);
  hdl_timer_subscribe(&printer_timer, &stream_delegate);
  coroutine_add(&stream_worker, &_stream_work_handler, (void *)hdl_stream);
}
