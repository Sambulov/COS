#include "CodeLib.h"

uint32_t ulBitReflect(uint32_t ulVal, uint8_t ucBits) {
  ulVal = ((ulVal & 0x55555555) <<  1) | ((ulVal >>  1) & 0x55555555);
  ulVal = ((ulVal & 0x33333333) <<  2) | ((ulVal >>  2) & 0x33333333);
  ulVal = ((ulVal & 0x0f0f0f0f) <<  4) | ((ulVal >>  4) & 0x0f0f0f0f);
  if(ucBits <= 8) return (ulVal & 0xff);
  ulVal = ((ulVal & 0x00ff00ff) <<  8) | ((ulVal >>  8) & 0x00ff00ff);
  if(ucBits <= 16) return (ulVal & 0xffff);
  ulVal = ((ulVal & 0x0000ffff) << 16) | (ulVal >> 16);
  return ulVal;
}

uint32_t bit_reflect(uint32_t val, uint8_t bits) __attribute__ ((alias ("ulBitReflect")));
