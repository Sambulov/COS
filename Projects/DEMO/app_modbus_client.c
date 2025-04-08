#include "app.h"

static void mb_test_registers_cb(modbus_t *mb, void *context, modbus_frame_t *frame) {
  (void)mb; (void)context;
  if(frame->ucFunc & 0x80) return;
  volatile uint16_t x = swap_bytes(*(uint16_t*)&frame->pucData[0]);
  (void)x;
}

static void mb_test_coils_cb(modbus_t *mb, void *context, modbus_frame_t *frame) {
  (void)mb; (void)context;
  if(frame->ucFunc & 0x80) return;
  volatile uint8_t x[2] = {frame->pucData[0], frame->pucData[1]} ;
  (void)x;
}

static uint8_t _modbus_client_handler(coroutine_t *this, uint8_t cancel, void *arg) {  
  (void)this; (void)arg;
  static uint8_t curr_req = 0;
  modbus_t *mb = (modbus_t *)arg;
  modbus_frame_t mb_request;
  modbus_cb_t cb;
  switch (curr_req) {
    case 0:
      cb = &mb_test_coils_cb;
      modbus_init_frame_read_outputs(&mb_request, 1, 0, 10);
      break;
    case 1: 
      cb = &mb_test_coils_cb;
      modbus_init_frame_read_inputs(&mb_request, 1, 0, 10);
      break;
    case 2:
      cb = &mb_test_registers_cb;
      modbus_init_frame_read_holdings(&mb_request, 1, 0, 10);
      break;
    case 3:
      cb = &mb_test_registers_cb;
      modbus_init_frame_read_input_registers(&mb_request, 1, 0, 10);
      break;
    default: curr_req = 0; return cancel;
  }
  if(modbus_request(mb, &mb_request, cb, NULL)) curr_req++;
  return cancel;
}

void modbus_client_test(modbus_t *mb) {
  static coroutine_t client_worker;
  coroutine_add(&client_worker, &_modbus_client_handler, mb);
}
