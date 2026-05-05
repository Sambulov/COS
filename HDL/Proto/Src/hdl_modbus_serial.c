#include "hdl_iface.h"

// typedef struct {
//   coroutine_t worker;
//   hdl_event_t event;

// } hdl_modbus_rtu_var_t;

// HDL_ASSERRT_STRUCTURE_CAST(hdl_modbus_var_t, *((hdl_modbus_rtu_t *)0)->obj_var, HDL_MODBUS_RTU_VAR_SIZE, "hdl_modbus.h");

// static uint8_t _modbus_worker(coroutine_t *this, uint8_t cancel, void *arg) {
//   hdl_modbus_rtu_t *modbus = (hdl_modbus_rtu_t *)arg;
//   return cancel;
// }

// static hdl_module_state_t _hdl_modbus_rtu(const void *desc, uint8_t enable) {
//   hdl_modbus_rtu_t *modbus = (hdl_modbus_rtu_t *)desc;
//   hdl_modbus_rtu_var_t *modbus_var = (hdl_modbus_rtu_var_t *)modbus->obj_var;
//   if(enable) {

//     coroutine_add(&modbus_var->worker, &_modbus_worker, (void*)modbus);
//     return HDL_MODULE_ACTIVE;
//   }
//   coroutine_cancel(&modbus_var->worker);
//   return HDL_MODULE_UNLOADED;
// }

// static uint8_t _modbus_request(const void *desc, hdl_modbus_frame_t *frame, event_handler_t *cb, void *cb_context) {

// }

// static uint8_t _modbus_response(const void *desc, hdl_modbus_frame_t *frame) {

// }

// static uint8_t _modbus_tx_cancel(const void *desc, hdl_modbus_frame_t *frame) {
//   // uint8_t test[128];

//   // frame->dev_addr                = 0x11;
//   // frame->func                    = 0x11;
//   // frame->payload.var_data.length = sizeof(test);
//   // frame->payload.var_data.u8     = test;
//   // frame->payload.regs.addr       = 0x1010;
//   // frame->payload.regs.amount     = 5;
  
  
// }

// static void _hdl_event_subscribtion(const void *desc, hdl_delegate_t *delegate) {
//   hdl_modbus_rtu_t *modbus = (hdl_modbus_rtu_var_t *)desc;
//   hdl_modbus_rtu_var_t *modbus_var = (hdl_modbus_rtu_var_t *)modbus->obj_var;
//   hdl_event_subscribe(&modbus_var->event, delegate);
// }

// const hdl_modbus_iface_t hdl_modbus_rtu_iface = {
//   .init = &_hdl_modbus_rtu,
//   .request = &_modbus_request,
//   .response = &_modbus_response,
//   .cancel = &_modbus_tx_cancel,
//   .subscribe = &_hdl_event_subscribtion
// };
