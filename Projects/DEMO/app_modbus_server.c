#include "app.h"

static uint8_t coils[2] = { 0x00, 0x00 };

static uint16_t registers[10] = {
  0x0123, 0x4567, 0x89AB, 0xCDEF, 0xFEDC,
  0xBA98, 0x7654, 0x3210, 0x01FE, 0x23DC
};

void test_mbserver_f01_02(modbus_t *mb, void *context, modbus_frame_t *frame) {
  (void)mb; (void)context;
  if((frame->usRegAddr >= 16) || 
     (frame->usRegValueCount > 16) || 
     ((frame->usRegAddr + frame->usRegValueCount) > 16)) {
    modbus_init_frame_error(frame, frame->ucAddr, frame->ucFunc, ModbusIllegalDataAddress);
    return;
  }
  if(frame->ucBufferSize < ((frame->usRegValueCount + 7) / 8)) {
    modbus_init_frame_error(frame, frame->ucAddr, frame->ucFunc, ModbusSlaveDeviceFailure);
    return;
  }
  uint8_t coils_bit = 1 << (frame->usRegAddr & 7);
  uint8_t coils_reg = (frame->usRegAddr >> 3);
  uint8_t bit_mask = 1;
  uint8_t byte = 0;
  uint8_t res = 0;
  frame->ucLengthCode = (frame->usRegValueCount + 7) / 8;
  while (frame->usRegValueCount--) {
    if(coils[coils_reg] & coils_bit) res |= bit_mask;
    coils_bit <<= 1;
    bit_mask <<= 1;
    if(!bit_mask) {
      bit_mask = 1;
      frame->pucData[byte] = res;
      byte++;
      res = 0;
    }
    if(!coils_bit) {
      coils_reg++;
      coils_bit = 1;
    }
  }
  uint16_t *tmp = (uint16_t *)coils;
  *tmp = *tmp + 1;
  if(bit_mask != 0x01) frame->pucData[byte] = res;
}

void test_mbserver_f03_04(modbus_t *mb, void *context, modbus_frame_t *frame) {
  (void)mb; (void)context;
  if((frame->usRegAddr >= 10) || 
     (frame->usRegValueCount > 10) || 
     ((frame->usRegAddr + frame->usRegValueCount) > 10)) {
    modbus_init_frame_error(frame, frame->ucAddr, frame->ucFunc, ModbusIllegalDataAddress);
    return;
  }
  if(frame->ucBufferSize < (frame->usRegValueCount * 2)) {
    modbus_init_frame_error(frame, frame->ucAddr, frame->ucFunc, ModbusSlaveDeviceFailure);
    return;
  }
  for(uint32_t i = 0; i < frame->usRegValueCount; i++) {
    *((uint16_t *)&frame->pucData[i*2]) = swap_bytes(registers[i + frame->usRegAddr]++);
  }
  frame->ucLengthCode = (frame->usRegValueCount * 2);
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
  modbus_server_link_endpoints(mb, &mb_ep);
}
