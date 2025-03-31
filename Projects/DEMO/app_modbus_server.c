#include "app.h"

static uint16_t coils[2] = { 0x01, 0x23 };

static uint16_t registers[10] = {
  0x0123, 0x4567, 0x89AB, 0xCDEF, 0xFEDC,
  0xBA98, 0x7654, 0x3210, 0x01FE, 0x23DC
};

modbus_result_t test_mbserver_f01_02(modbus_request_t *req, modbus_frame_t *resp) {
  if((req->xFrame.usRegAddr < 10) && 
     (req->xFrame.usRegValueCount <= 10) && 
     ((req->xFrame.usRegAddr + req->xFrame.usRegValueCount) <= 10)) {
    if(resp->ucLengthCode < ((req->xFrame.usRegValueCount + 7) / 8)) return ModbusSlaveDeviceFailure;
    uint8_t coils_bit = 1 << (req->xFrame.usRegAddr & 7);
    uint8_t coils_reg = (req->xFrame.usRegAddr >> 3);
    uint8_t bit_mask = 1;
    uint8_t byte = 0;
    uint8_t res = 0;
    resp->ucLengthCode = (req->xFrame.usRegValueCount + 7) / 8;
    while (req->xFrame.usRegValueCount--) {
      if(coils[coils_reg] & coils_bit) res |= bit_mask;
      coils[coils_reg] ^= coils_bit;
      coils_bit <<= 1;
      bit_mask <<= 1;
      if(!bit_mask) {
        bit_mask = 1;
        resp->pucData[byte] = res;
        byte++;
        res = 0;
      }
      if(!coils_bit) {
        coils_reg++;
        coils_bit = 1;
      }
    }
    if(bit_mask != 0x01) resp->pucData[byte] = res;
    return ModbusOk;
  }
  return ModbusIllegalDataAddress;
}

modbus_result_t test_mbserver_f03_04(modbus_request_t *req, modbus_frame_t *resp) {
  if((req->xFrame.usRegAddr < 10) && 
     (req->xFrame.usRegValueCount <= 10) && 
     ((req->xFrame.usRegAddr + req->xFrame.usRegValueCount) <= 10)) {
    if(resp->ucLengthCode < (req->xFrame.usRegValueCount * 2)) return ModbusSlaveDeviceFailure;
    for(uint32_t i = req->xFrame.usRegAddr; i < req->xFrame.usRegValueCount; i++) {
      *((uint16_t *)&resp->pucData[i*2]) = swap_bytes(registers[i]++);
    }
    resp->ucLengthCode = (req->xFrame.usRegValueCount * 2);
    return ModbusOk;
  }
  return ModbusIllegalDataAddress;
}

modbus_handler_t mbh01 = {
  .ucFunctionNo = MB_FUNC_READ_OUTPUTS,
  .pxContext = NULL,
  .pfOnRequest = &test_mbserver_f01_02
};

modbus_handler_t mbh02 = {
  .ucFunctionNo = MB_FUNC_READ_INPUTS,
  .pxContext = NULL,
  .pfOnRequest = &test_mbserver_f01_02
};

modbus_handler_t mbh03 = {
  .ucFunctionNo = MB_FUNC_READ_HOLDINGS,
  .pxContext = NULL,
  .pfOnRequest = &test_mbserver_f03_04
};

modbus_handler_t mbh04 = {
  .ucFunctionNo = MB_FUNC_READ_INPUT_REGISTERS,
  .pxContext = NULL,
  .pfOnRequest = &test_mbserver_f03_04
};

modbus_endpoint_t mb_ep = {
  .ucAddress = 0x01,
  .paxHandlers = (const modbus_handler_t *const []){&mbh01, &mbh02, &mbh03, &mbh04, NULL},
  .pxNext = NULL
};

void modbus_server_test(modbus_t *mb) {
  bModbusServerLinkEndpoints(mb, &mb_ep);
}
