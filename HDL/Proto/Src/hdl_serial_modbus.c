#include "bldl.h"

#define MB_TX_BUFER   8

typedef struct {
  coroutine_t worker;
  hdl_event_t event;
  uint32_t timer;
  hdl_transceiver_t transceiver;
  simple_circular_buffer_t scb_tx;
  uint8_t mb_tx[MB_TX_BUFER];
  modbus_t modbus;
} hdl_serial_modbus_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_serial_modbus_var_t, *((hdl_serial_modbus_t *)0)->obj_var, HDL_SERIAL_MODBUS_VAR_SIZE, "hdl_serial_modbus.h");

static const modbus_iface_t mb_iface = {
  .pfWrite = (modbus_iface_write_t)&scb_write,
  .pfTimer = (modbus_iface_timer_t)&hdl_time_counter_get
};

static uint8_t _serial_modbus_work(coroutine_t *this, uint8_t cancel, void *context) {
  (void)this;
  hdl_serial_modbus_t *serial_modbus = (hdl_serial_modbus_t *)context;
  hdl_serial_modbus_var_t *obj_var = (hdl_serial_modbus_var_t *)serial_modbus->obj_var;
  modbus_work(&obj_var->modbus);
  return cancel;
}

static void _serial_modbus_subscribe(const void *desc, hdl_delegate_t *delegate) {
  hdl_serial_modbus_t *serial_modbus = (hdl_serial_modbus_t *)desc;
  hdl_serial_modbus_var_t *obj_var = (hdl_serial_modbus_var_t *)serial_modbus->obj_var;
  hdl_event_subscribe(&obj_var->event, delegate);
}

static int32_t _mb_receive(hdl_serial_modbus_t *mb, const uint8_t *data, uint16_t size) {
  hdl_serial_modbus_var_t *obj_var = (hdl_serial_modbus_var_t *)mb->obj_var;
  modbus_receive_data(&obj_var->modbus, data, size, 0);
  return size;
}

static void mb_eot_cb(hdl_serial_modbus_t *mb) {
  hdl_serial_modbus_var_t *obj_var = (hdl_serial_modbus_var_t *)mb->obj_var;
  modbus_receive_data(&obj_var->modbus, NULL, 0, 1);
}

static int32_t mb_available_free(void *) {
  return 0x7fffffff;
}

static void _modbus_callback(modbus_frame_t *frame, modbus_t *mb, void *context) {
  (void)mb;
  hdl_serial_modbus_t *this = (hdl_serial_modbus_t *)context;
  hdl_serial_modbus_var_t *obj_var = (hdl_serial_modbus_var_t *)this->obj_var;
  hdl_event_raise(&obj_var->event, this, frame);
}

static hdl_module_state_t _hdl_serial_modbus(const void *desc, uint8_t enable) {
  hdl_serial_modbus_t *this = (hdl_serial_modbus_t *)desc;
  hdl_serial_modbus_var_t *obj_var = (hdl_serial_modbus_var_t *)this->obj_var;
  if(enable) {
    scb_init(&obj_var->scb_tx, obj_var->mb_tx, MB_TX_BUFER);

    obj_var->transceiver.receiver_context = this;
    obj_var->transceiver.rx_data = (hdl_transceiver_xfer_cb_t)&_mb_receive;
    obj_var->transceiver.rx_available = (hdl_transceiver_size_cb_t)&mb_available_free;
    obj_var->transceiver.end_of_transmission = (hdl_transceiver_eot_cb_t)&mb_eot_cb;

    obj_var->transceiver.tx_empty = (hdl_transceiver_xfer_cb_t)&scb_read;
    obj_var->transceiver.transmitter_context = &obj_var->scb_tx;
    obj_var->transceiver.tx_available = (hdl_transceiver_size_cb_t)&scb_available;

    modbus_config_t mb_cnf = {
      .eMode = this->config->mode,
      .bIsServer = this->config->server_mode,
      .pxIface = &mb_iface,
      .usTxTimeout = this->config->tx_timeout,
      .usRxTimeout = this->config->rx_timeout,
      .pxTimerContext = (void*) this->dependencies[SERIAL_MODBUS_DEPENDENCY_ID_TIMER],
      .pucPayLoadBuffer = this->config->mb_buffer,
      .ucPayLoadBufferSize = this->config->mb_buffer_size,
      .pxTxContext = obj_var->transceiver.transmitter_context,
      .pxRequestContext = this,
      .pfOnRequest = &_modbus_callback
    };
    hdl_uart_set_transceiver(this->dependencies[SERIAL_MODBUS_DEPENDENCY_ID_UART], &obj_var->transceiver);
    modbus_init(&obj_var->modbus, &mb_cnf);
    obj_var->timer = hdl_time_counter_get(this->dependencies[SERIAL_MODBUS_DEPENDENCY_ID_TIMER]);
    coroutine_add(&obj_var->worker, &_serial_modbus_work, this);
    return  HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&obj_var->worker);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_serial_modbus_request(const void *desc, hdl_modbus_frame_t *frame, uint32_t timeout) {
  hdl_serial_modbus_t *this = (hdl_serial_modbus_t *)desc;
  hdl_serial_modbus_var_t *obj_var = (hdl_serial_modbus_var_t *)this->obj_var;
  return modbus_request(&obj_var->modbus, frame, _modbus_callback, this, timeout) > 0;
}

static uint8_t _hdl_serial_modbus_response(const void *desc, hdl_modbus_frame_t *frame) {
  hdl_serial_modbus_t *this = (hdl_serial_modbus_t *)desc;
  hdl_serial_modbus_var_t *obj_var = (hdl_serial_modbus_var_t *)this->obj_var;
  return modbus_response(&obj_var->modbus, frame) > 0;
}

static uint8_t _hdl_serial_modbus_reset(const void *desc) {
  hdl_serial_modbus_t *this = (hdl_serial_modbus_t *)desc;
  hdl_serial_modbus_var_t *obj_var = (hdl_serial_modbus_var_t *)this->obj_var;
  return modbus_reset(&obj_var->modbus);
}

static hdl_modbus_state_t _hdl_serial_modbus_state(const void *desc) {
  hdl_serial_modbus_t *this = (hdl_serial_modbus_t *)desc;
  hdl_serial_modbus_var_t *obj_var = (hdl_serial_modbus_var_t *)this->obj_var;
  return modbus_get_state(&obj_var->modbus);
}

const hdl_modbus_iface_t hdl_serial_modbus_iface = {
  .init = &_hdl_serial_modbus,
  .request = &_hdl_serial_modbus_request,
  .response = &_hdl_serial_modbus_response,
  .reset = &_hdl_serial_modbus_reset,
  .state = &_hdl_serial_modbus_state,
  .on_receive = &_serial_modbus_subscribe,
};
