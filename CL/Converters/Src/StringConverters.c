#include "CodeLib.h"

uint16_t usConvertByteToAsciiHex(uint8_t ucVal, uint8_t bToUpperCase) {
	uint16_t ucHigh = (ucVal >> 4);	
	uint16_t ucLow = (ucVal & 0x0F);
  uint8_t base = (bToUpperCase? 'A': 'a');
	ucHigh = (ucHigh > 9) ? (ucHigh - 10 + base) : ucHigh + '0';
	ucLow = (ucLow > 9) ? (ucLow - 10 + base) : ucLow + '0';
	return ((ucHigh << 8) | ucLow);
}

uint8_t bConvertAsciiHexToByte(uint16_t usTwoAscii, uint8_t *ucOutByte) {
	uint8_t ucHigh = usTwoAscii;
	uint8_t ucLow = usTwoAscii >> 8;
	if (!IS_HEX(ucHigh) || !IS_HEX(ucLow)) return 0;
  if (ucOutByte != libNULL) {
    ucLow = CHAR_TO_HEX(ucLow);
    ucHigh = CHAR_TO_HEX(ucHigh);
    *ucOutByte = (ucLow | (ucHigh << 4));
  }
  return 1;
}

/*!
  Snake notation
*/

uint16_t convert_byte_to_ascii_hex(uint8_t val, uint8_t to_upper_case) __attribute__ ((alias ("usConvertByteToAsciiHex")));
uint8_t convert_ascii_hex_to_byte(uint16_t two_ascii, uint8_t *out_byte) __attribute__ ((alias ("bConvertAsciiHexToByte")));