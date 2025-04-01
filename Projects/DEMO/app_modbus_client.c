#include "app.h"

modbus_result_t mb_test_registers_cb(modbus_request_t *req, modbus_frame_t *resp) {
  (void)req;
  if(resp->ucFunc & 0x80) 
    return ModbusOk;
  volatile uint16_t x = swap_bytes(*(uint16_t*)&resp->pucData[0]);
  (void)x;
  return ModbusOk;
}

modbus_result_t mb_test_coils_cb(modbus_request_t *req, modbus_frame_t *resp) {
  (void)req;
  if(resp->ucFunc & 0x80) 
    return ModbusOk;
  volatile uint8_t x[2] = {resp->pucData[0], resp->pucData[1]} ;
  (void)x;
  return ModbusOk;
}

static uint8_t _modbus_client_handler(coroutine_t *this, uint8_t cancel, void *arg) {  
  (void)this; (void)arg;
  modbus_t *mb = (modbus_t *)arg;
  static modbus_request_t mb_request;
  static uint8_t curr_req = 0;
  switch (curr_req) {
    case 0: 
      modbus_init_request_read_outputs(&mb_request, NULL, &mb_test_coils_cb, 1, 0, 10);
      if(modbus_request(mb, &mb_request)) curr_req++; 
      break;
    case 1: 
      modbus_init_request_read_inputs(&mb_request, NULL, &mb_test_coils_cb, 1, 0, 10);
      if(modbus_request(mb, &mb_request)) curr_req++; 
      break;
    case 2:
      modbus_init_request_read_holdings(&mb_request, NULL, &mb_test_registers_cb, 1, 0, 10);
      if(modbus_request(mb, &mb_request)) curr_req++; 
      break;
    case 3:
      modbus_init_request_read_input_registers(&mb_request, NULL, &mb_test_registers_cb, 1, 0, 10);
      if(modbus_request(mb, &mb_request)) curr_req++;
      break;
    default: curr_req = 0; break;
  }
  return cancel;
}

void modbus_client_test(modbus_t *mb) {
  static coroutine_t client_worker;
  coroutine_add(&client_worker, &_modbus_client_handler, mb);
}
