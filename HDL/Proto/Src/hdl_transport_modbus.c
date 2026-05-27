#include "bldl.h"

typedef struct {
  coroutine_t worker;
  hdl_event_t event;
  modbus_t modbus;
  hdl_delegate_t transport_cb;
  const hdl_transport_modbus_t *origin_ptr;
} hdl_transport_modbus_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_transport_modbus_var_t, *((hdl_transport_modbus_t *)0)->obj_var, HDL_TRANSPORT_MODBUS_VAR_SIZE, "hdl_transport_modbus.h");

static int32_t modbus_write(void *desc, const uint8_t *buf, uint16_t size) {
  hdl_transport_result_t res = hdl_transport_send_zero_copy(desc, buf, size);
  if(res == HDL_TRANSPORT_OK)
    return size;
  if(res == HDL_TRANSPORT_BUSY)
    return 0;
  return -1;
}

static void _modbus_callback(modbus_frame_t *frame, modbus_t *mb, void *context) {
  (void)mb;
  hdl_transport_modbus_t *this = (hdl_transport_modbus_t *)context;
  hdl_transport_modbus_var_t *obj_var = (hdl_transport_modbus_var_t *)this->obj_var;
  if(obj_var->origin_ptr)
    obj_var = (hdl_transport_modbus_var_t *)obj_var->origin_ptr->obj_var;
  hdl_event_raise(&obj_var->event, this, frame);
}

static const modbus_iface_t mb_iface = {
  .pfWrite = modbus_write,
  .pfTimer = (modbus_iface_timer_t)&hdl_time_counter_get
};

static uint8_t _transport_modbus_work(coroutine_t *this, uint8_t cancel, void *context) {
  (void)this;
  hdl_transport_modbus_t *mb = (hdl_transport_modbus_t *)context;
  hdl_transport_modbus_var_t *obj_var = (hdl_transport_modbus_var_t *)mb->obj_var;
  cancel = cancel && !modbus_busy(&obj_var->modbus);
  if(cancel) {
    if(obj_var->origin_ptr) {
      hdl_event_unsubscribe(&obj_var->transport_cb);
      coroutine_terminate(&obj_var->worker);
      hdl_free(mb);
    }
  }
  else
    modbus_work(&obj_var->modbus);
  return cancel;
}

static void _transport_modbus_subscribe(const void *desc, hdl_delegate_t *delegate) {
  hdl_transport_modbus_t *transport_modbus = (hdl_transport_modbus_t *)desc;
  hdl_transport_modbus_var_t *obj_var = (hdl_transport_modbus_var_t *)transport_modbus->obj_var;
  if(obj_var->origin_ptr)
    obj_var = (hdl_transport_modbus_var_t *)obj_var->origin_ptr->obj_var;
  hdl_event_subscribe(&obj_var->event, delegate);
}

static hdl_transport_modbus_t *_hdl_mb_clone(hdl_transport_modbus_t *mb, void **mb_buf) {
  uint8_t *mem = hdl_malloc(
    sizeof(hdl_transport_modbus_t) + 
    hdl_module_sizeof_obj_var(hdl_transport_modbus_t) +
    mb->config->mb_buffer_size
  );
  hdl_transport_modbus_t *new_mb = (hdl_transport_modbus_t *)mem;
  if(new_mb) {
    mem_cpy(new_mb, mb, sizeof(hdl_transport_modbus_t));
    mem += sizeof(hdl_transport_modbus_t);
    new_mb->obj_var = (void *)mem;
    mem_cpy(new_mb->obj_var, mb->obj_var, hdl_module_sizeof_obj_var(hdl_transport_modbus_t));
    mem += hdl_module_sizeof_obj_var(hdl_transport_modbus_t);
    *mb_buf = mem;
  }
  return new_mb;
}

static void transport_handler(void *event_trigger, void *sender, void *context) {
  (void)sender;
  hdl_transport_event_trigger_t *et = (hdl_transport_event_trigger_t *) event_trigger;
  hdl_transport_modbus_t *mb = (hdl_transport_modbus_t *)context;
  hdl_transport_modbus_var_t *mb_var = (hdl_transport_modbus_var_t *)mb->obj_var;
  if((et->event & HDL_TRANSPORT_CLOSED) && mb_var->origin_ptr)
    coroutine_cancel(&mb_var->worker);
  if(et->event & HDL_TRANSPORT_RECEIVED)
    modbus_receive_data(&mb_var->modbus, et->data.ptr, et->data.length, (et->data.left == 0));
}

static void listener_handler(void *event_trigger, void *sender, void *context) {
  hdl_transport_event_trigger_t *et = (hdl_transport_event_trigger_t *) event_trigger;
  hdl_transport_modbus_t *mb = (hdl_transport_modbus_t *)context;
  hdl_transport_modbus_var_t *mb_var = (hdl_transport_modbus_var_t *)mb->obj_var;
  if(et->event & HDL_TRANSPORT_ESTABLISHED) {
    if(mb->config->server_mode) {
      void *new_mb_buf;
      hdl_transport_modbus_t *new_mb = _hdl_mb_clone(mb, &new_mb_buf);
      if(!new_mb)
        hdl_transport_close(sender);
      else {
        mb_var = (hdl_transport_modbus_var_t *)new_mb->obj_var;
        mb_var->transport_cb.context = new_mb;
        mb_var->transport_cb.handler = &transport_handler;
        hdl_transport_subscribe(sender, &mb_var->transport_cb);
        modbus_config_t mb_cnf = {
          .eMode = mb->config->mode,
          .bIsServer = mb->config->server_mode,
          .pxIface = &mb_iface,
          .usTxTimeout = mb->config->tx_timeout,
          .usRxTimeout = mb->config->rx_timeout,
          .pucPayLoadBuffer = new_mb_buf,
          .ucPayLoadBufferSize = mb->config->mb_buffer_size,
          .pxTimerContext = (void*)mb->dependencies[TRANSPORT_MODBUS_DEPENDENCY_ID_TIMER],
          .pxTxContext = sender,
          .pxRequestContext = new_mb,
          .pfOnRequest = &_modbus_callback
        };
        modbus_init(&mb_var->modbus, &mb_cnf);
        coroutine_add(&mb_var->worker, &_transport_modbus_work, new_mb);
      }
      mb_var->origin_ptr = mb;
    }
  }
}

static hdl_module_state_t _hdl_transport_modbus(const void *desc, uint8_t enable) {
  hdl_transport_modbus_t *this = (hdl_transport_modbus_t *)desc;
  hdl_transport_modbus_var_t *obj_var = (hdl_transport_modbus_var_t *)this->obj_var;
  if(enable) {
    obj_var->transport_cb.context = this;
    if(this->config->server_mode) {
      obj_var->origin_ptr = NULL;
      obj_var->transport_cb.handler = &listener_handler;
    }
    else {
      obj_var->transport_cb.handler = &transport_handler;
      modbus_config_t mb_cnf = {
        .eMode = this->config->mode,
        .pxIface = &mb_iface,
        .usTxTimeout = this->config->tx_timeout,
        .pxTimerContext = (void*)this->dependencies[TRANSPORT_MODBUS_DEPENDENCY_ID_TIMER],
        .pucPayLoadBuffer = this->config->mb_buffer,
        .ucPayLoadBufferSize = this->config->mb_buffer_size,
        .pxTxContext = (void*)this->dependencies[TRANSPORT_MODBUS_DEPENDENCY_ID_TRANSPORT],
        .bIsServer = HDL_FALSE,
        .usRxTimeout = 0,
        .pxRequestContext = NULL,
        .pfOnRequest = NULL
      };
      modbus_init(&obj_var->modbus, &mb_cnf);
      coroutine_add(&obj_var->worker, &_transport_modbus_work, this);
    }
    hdl_transport_subscribe(this->dependencies[TRANSPORT_MODBUS_DEPENDENCY_ID_TRANSPORT], &obj_var->transport_cb);
    return  HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&obj_var->worker);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_transport_modbus_request(const void *desc, hdl_modbus_frame_t *frame, uint32_t timeout) {
  hdl_transport_modbus_t *this = (hdl_transport_modbus_t *)desc;
  hdl_transport_modbus_var_t *obj_var = (hdl_transport_modbus_var_t *)this->obj_var;
  return modbus_request(&obj_var->modbus, frame, _modbus_callback, this, timeout) > 0;
}

static uint8_t _hdl_transport_modbus_response(const void *desc, hdl_modbus_frame_t *frame) {
  hdl_transport_modbus_t *this = (hdl_transport_modbus_t *)desc;
  hdl_transport_modbus_var_t *obj_var = (hdl_transport_modbus_var_t *)this->obj_var;
  return modbus_response(&obj_var->modbus, frame) > 0;
}

static uint8_t _hdl_transport_modbus_abort(const void *desc) {
  hdl_transport_modbus_t *this = (hdl_transport_modbus_t *)desc;
  hdl_transport_modbus_var_t *obj_var = (hdl_transport_modbus_var_t *)this->obj_var;
  return modbus_reset(&obj_var->modbus);
}

static hdl_modbus_state_t _hdl_transport_get_state(const void *desc) {
  hdl_transport_modbus_t *this = (hdl_transport_modbus_t *)desc;
  hdl_transport_modbus_var_t *obj_var = (hdl_transport_modbus_var_t *)this->obj_var;
  return modbus_get_state(&obj_var->modbus);
}

const hdl_modbus_iface_t hdl_transport_modbus_iface = {
  .init = &_hdl_transport_modbus,
  .request = &_hdl_transport_modbus_request,
  .response = &_hdl_transport_modbus_response,
  .reset = &_hdl_transport_modbus_abort,
  .state = &_hdl_transport_get_state,
  .on_receive = &_transport_modbus_subscribe,
};
