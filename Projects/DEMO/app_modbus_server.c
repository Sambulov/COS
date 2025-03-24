#include "app.h"

static uint16_t coils[2] = { 0x01, 0x23 };

static uint16_t registers[10] = {
  0x0123, 0x4567, 0x89AB, 0xCDEF, 0xFEDC,
  0xBA98, 0x7654, 0x3210, 0x01FE, 0x23DC
};

ModbusResult_t test_mbserver_f01_02(ModbusRequest_t *pxRreq, ModbusFrame_t *pxResp) {
  if((pxRreq->xFrame.usRegAddr < 10) && 
     (pxRreq->xFrame.usRegValueCount <= 10) && 
     ((pxRreq->xFrame.usRegAddr + pxRreq->xFrame.usRegValueCount) <= 10)) {
    if(pxResp->ucLengthError < ((pxRreq->xFrame.usRegValueCount + 7) / 8)) return ModbusSlaveDeviceFailure;
    uint8_t coils_bit = 1 << (pxRreq->xFrame.usRegAddr & 7);
    uint8_t coils_reg = (pxRreq->xFrame.usRegAddr >> 3);
    uint8_t bit_mask = 1;
    uint8_t byte = 0;
    uint8_t res = 0;
    pxResp->ucLengthError = (pxRreq->xFrame.usRegValueCount + 7) / 8;
    while (pxRreq->xFrame.usRegValueCount--) {
      if(coils[coils_reg] & coils_bit) res |= bit_mask;
      coils[coils_reg] ^= coils_bit;
      coils_bit <<= 1;
      bit_mask <<= 1;
      if(!bit_mask) {
        bit_mask = 1;
        pxResp->pucData[byte] = res;
        byte++;
        res = 0;
      }
      if(!coils_bit) {
        coils_reg++;
        coils_bit = 1;
      }
    }
    if(bit_mask != 0x01) pxResp->pucData[byte] = res;
    return ModbusOk;
  }
  return ModbusIllegalDataAddress;
}

ModbusResult_t test_mbserver_f03_04(ModbusRequest_t *pxRreq, ModbusFrame_t *pxResp) {
  if((pxRreq->xFrame.usRegAddr < 10) && 
     (pxRreq->xFrame.usRegValueCount <= 10) && 
     ((pxRreq->xFrame.usRegAddr + pxRreq->xFrame.usRegValueCount) <= 10)) {
    if(pxResp->ucLengthError < (pxRreq->xFrame.usRegValueCount * 2)) return ModbusSlaveDeviceFailure;
    for(uint32_t i = pxRreq->xFrame.usRegAddr; i < pxRreq->xFrame.usRegValueCount; i++) {
      *((uint16_t *)&pxResp->pucData[i*2]) = swap_bytes(registers[i]++);
    }
    pxResp->ucLengthError = (pxRreq->xFrame.usRegValueCount * 2);
    return ModbusOk;
  }
  return ModbusIllegalDataAddress;
}

ModbusHandler_t mbh01 = {
  .ucFunctionNo = MB_FUNC_READ_OUTPUTS,
  .pxContext = NULL,
  .pfOnRequest = &test_mbserver_f01_02
};

ModbusHandler_t mbh02 = {
  .ucFunctionNo = MB_FUNC_READ_INPUTS,
  .pxContext = NULL,
  .pfOnRequest = &test_mbserver_f01_02
};

ModbusHandler_t mbh03 = {
  .ucFunctionNo = MB_FUNC_READ_HOLDINGS,
  .pxContext = NULL,
  .pfOnRequest = &test_mbserver_f03_04
};

ModbusHandler_t mbh04 = {
  .ucFunctionNo = MB_FUNC_READ_INPUT_REGISTERS,
  .pxContext = NULL,
  .pfOnRequest = &test_mbserver_f03_04
};

ModbusEndpoint_t mb_ep = {
  .ucAddress = 0x01,
  .paxHandlers = (const ModbusHandler_t *const []){&mbh01, &mbh02, &mbh03, &mbh04, NULL},
  .pxNext = NULL
};

void modbus_server_test(Modbus_t *pxMb) {
  bModbusServerLinkEndpoints(pxMb, &mb_ep);
}
