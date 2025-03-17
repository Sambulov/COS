#include "CodeLib.h"

uint32_t ulCrc32(const void *pvData, uint32_t ulLen, uint32_t ulInit, const uint32_t ulPoly, Crc32Options_t eOpt) {
  const uint8_t *buf = (const uint8_t *)pvData;
  uint32_t crc = ulInit;
  while(ulLen--) {
    uint8_t b = *buf++;
    if(eOpt & CRC32_REFLECT_IN) b = ulBitReflect(b, 8);
    crc = crc ^ (b << 24);
    for( int bit = 0; bit < 8; bit++ ) {
      if( crc & (1L << 31)) crc = (crc << 1) ^ ulPoly;
      else                  crc = (crc << 1);
    }
  }
  if(eOpt & CRC32_INVERT_OUT) crc = ~crc;
  if(eOpt & CRC32_REFLECT_OUT) crc = ulBitReflect(crc, 32);
  return crc;
}

static uint16_t _usCrc16ModbusRtuNext(uint16_t pucCrc, uint8_t ucData) {
	pucCrc ^= (uint16_t)ucData; /* XOR byte into least sig. byte of crc */
	for (int i = 8; i != 0; i--) { /* Loop over each bit */
		if ((pucCrc & 0x0001) != 0) { /*If the LSB is set */
			pucCrc >>= 1; /* Shift right and XOR 0xA001 */
			pucCrc ^= 0xA001;
		}
		else { /* Else LSB is not set */
			pucCrc >>= 1; /* Just shift right */
		}
	}
	return pucCrc;
}

static uint8_t _ucCrc8DallasNext(uint8_t ucData) {
	const uint8_t pucCrcBitComp[] = { 0x5e, 0xbc, 0x61, 0xc2, 0x9d, 0x23, 0x46, 0x8c };
	uint8_t crc = 0;
	uint8_t i;
	for (i = 0; i < 8; i++) {
		crc ^= (ucData & (0x01 << i)) ? pucCrcBitComp[i] : 0;
	}
	return ((crc<<4)|(crc>>4));
}

uint8_t ucCrc8Dallas(const uint8_t *pucData, uint32_t ulLen, const uint8_t *pucCrc) {
	uint8_t crc = 0;
	if (pucCrc != libNULL) {
		crc = *pucCrc;
	}
	for (uint32_t i = 0; i < ulLen; ++i) {
		crc = _ucCrc8DallasNext(pucData[i] ^ crc);
	}
	return crc;
}

uint16_t usCrc16ModbusRtu(const uint8_t *pucData, int32_t ulLen, const uint16_t *pusCrc) {
	uint16_t crc = 0xFFFF;
	if (pusCrc != libNULL) {
		crc = *pusCrc;
	}
	if (pucData != libNULL) {
		for (int32_t i = 0; i < ulLen; i++) {
			crc = _usCrc16ModbusRtuNext(crc, pucData[i]);
		}
	}
	return crc;
}


uint32_t crc32(const void *data, uint32_t len, uint32_t init, const uint32_t poly, crc32_options_t opt)\
                                                  __attribute__ ((alias ("ulCrc32")));
uint16_t crc16_modbus_rtu(const uint8_t *data, int32_t len, const uint16_t *crc)\
                                                  __attribute__ ((alias ("usCrc16ModbusRtu")));
uint8_t crc8_dallas(const uint8_t *data, uint32_t len, const uint8_t *crc)\
                                                  __attribute__ ((alias ("ucCrc8Dallas")));
