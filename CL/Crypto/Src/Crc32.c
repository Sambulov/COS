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

uint32_t crc32(const void *data, uint32_t len, uint32_t init, const uint32_t poly, crc32_options_t opt)\
                                                  __attribute__ ((alias ("ulCrc32")));
