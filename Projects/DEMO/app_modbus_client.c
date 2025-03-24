#include "app.h"

ModbusResult_t mb_test_registers_cb(ModbusRequest_t *pxRreq, ModbusFrame_t *pxResp) {
  (void)pxRreq; (void)pxResp;
  if(pxResp->ucFunc & 0x80) 
    return ModbusOk;
  volatile uint16_t x0 = swap_bytes(*(uint16_t*)&pxResp->pucData[0]);
  volatile uint16_t x1 = swap_bytes(*(uint16_t*)&pxResp->pucData[2]);
  volatile uint16_t x2 = swap_bytes(*(uint16_t*)&pxResp->pucData[4]);
  volatile uint16_t x3 = swap_bytes(*(uint16_t*)&pxResp->pucData[6]);
  volatile uint16_t x4 = swap_bytes(*(uint16_t*)&pxResp->pucData[8]);
  volatile uint16_t x5 = swap_bytes(*(uint16_t*)&pxResp->pucData[10]);
  volatile uint16_t x6 = swap_bytes(*(uint16_t*)&pxResp->pucData[12]);
  volatile uint16_t x7 = swap_bytes(*(uint16_t*)&pxResp->pucData[14]);
  volatile uint16_t x8 = swap_bytes(*(uint16_t*)&pxResp->pucData[16]);
  volatile uint16_t x9 = swap_bytes(*(uint16_t*)&pxResp->pucData[18]);
  return ModbusOk;
}

ModbusResult_t mb_test_coils_cb(ModbusRequest_t *pxRreq, ModbusFrame_t *pxResp) {
  (void)pxRreq; (void)pxResp;
  if(pxResp->ucFunc & 0x80) 
    return ModbusOk;
  volatile uint8_t x[2] = {pxResp->pucData[0], pxResp->pucData[1]} ;
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
