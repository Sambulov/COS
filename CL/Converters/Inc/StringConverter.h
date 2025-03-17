#ifndef STING_CONVERTERS_H_INCLUDED
#define STING_CONVERTERS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

uint16_t usConvertByteToAsciiHex(uint8_t ucVal, uint8_t bToUpperCase);
uint8_t bConvertAsciiHexToByte(uint16_t usTwoAscii, uint8_t *ucOutByte);

/*!
  Snake notation
*/

uint16_t convert_byte_to_ascii_hex(uint8_t val, uint8_t to_upper_case);
uint8_t convert_ascii_hex_to_byte(uint16_t two_ascii, uint8_t *out_byte);

#ifdef __cplusplus
}
#endif

#endif /* STING_CONVERTERS_H_INCLUDED */