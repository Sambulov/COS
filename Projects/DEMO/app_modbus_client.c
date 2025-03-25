#include "app.h"

ModbusResult_t mb_test_registers_cb(ModbusRequest_t *pxRreq, ModbusFrame_t *pxResp) {
  (void)pxRreq; (void)pxResp;
  if(pxResp->ucFunc & 0x80) 
    return ModbusOk;
  volatile uint16_t x = swap_bytes(*(uint16_t*)&pxResp->pucData[0]);
  (void)x;
  return ModbusOk;
}

ModbusResult_t mb_test_coils_cb(ModbusRequest_t *pxRreq, ModbusFrame_t *pxResp) {
  (void)pxRreq; (void)pxResp;
  if(pxResp->ucFunc & 0x80) 
    return ModbusOk;
  volatile uint8_t x[2] = {pxResp->pucData[0], pxResp->pucData[1]} ;
  (void)x;
  return ModbusOk;
}

static uint8_t _modbus_client_handler(coroutine_t *this, uint8_t cancel, void *arg) {  
  (void)this; (void)arg;
  Modbus_t *mb = (Modbus_t *)arg;
  static ModbusRequest_t mb_request;
  static uint8_t curr_req = 0;
  switch (curr_req) {
    case 0: 
      vModbusInitRequestReadOutputs(&mb_request, NULL, &mb_test_coils_cb, 1, 0, 10);
      if(bModbusRequest(mb, &mb_request)) curr_req++; 
      break;
    case 1: 
      vModbusInitRequestReadInputs(&mb_request, NULL, &mb_test_coils_cb, 1, 0, 10);
      if(bModbusRequest(mb, &mb_request)) curr_req++; 
      break;
    case 2:
      vModbusInitRequestReadHoldings(&mb_request, NULL, &mb_test_registers_cb, 1, 0, 10);
      if(bModbusRequest(mb, &mb_request)) curr_req++; 
      break;
    case 3:
      vModbusInitRequestReadInputRegisters(&mb_request, NULL, &mb_test_registers_cb, 1, 0, 10);
      if(bModbusRequest(mb, &mb_request)) curr_req++;
      break;
    default: curr_req = 0; break;
  }
  return cancel;
}

void modbus_client_test(Modbus_t *pxMb) {
  static coroutine_t client_worker;
  coroutine_add(&client_worker, &_modbus_client_handler, pxMb);
}
