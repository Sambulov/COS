#ifndef CHARSET_ENCODING_H_INCLUDED
#define CHARSET_ENCODING_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

uint32_t ulAsciiToUtf8Mesure(const uint8_t *pucAsciiStr, uint32_t ulLen);
uint32_t ulAsciiToUtf8(const uint8_t *pucAsciiStr, uint32_t lLen, uint8_t *pucUtf8Str);
  
/*!
  Snake notation
*/
  
uint32_t ascii_to_utf8(const uint8_t *ascii_str, uint32_t len, uint8_t *utf8_str);
uint32_t ascii_to_utf8_mesure(const uint8_t *ascii_str, uint32_t len);
  
#ifdef __cplusplus
}
#endif

#endif /* CHARSET_ENCODING_H_INCLUDED */
