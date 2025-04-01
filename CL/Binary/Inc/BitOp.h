#ifndef BIT_OP_H_INCLUDED
#define BIT_OP_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

uint32_t ulBitReflect(uint32_t ulVal, uint8_t ucBits);

static inline uint64_t ulSwapDWords(uint64_t ulVal) {
  return (ulVal >> 32) | (ulVal << 32);
}

static inline uint32_t ulSwapWords(uint32_t ulVal) {
  return (ulVal >> 16) | (ulVal << 16);
}

static inline uint16_t ulSwapBytes(uint16_t usVal) {
  return (usVal >> 8) | (usVal << 8);
}

/*!
  Snake notation
*/

uint32_t bit_reflect(uint32_t val, uint8_t bits);

static inline uint64_t swap_dwords(uint64_t ulVal)  __attribute__ ((alias ("ulSwapDWords")));
static inline uint32_t swap_words(uint32_t ulVal)  __attribute__ ((alias ("ulSwapWords")));
static inline uint16_t swap_bytes(uint16_t ulVal)  __attribute__ ((alias ("ulSwapBytes")));


#ifdef __cplusplus
}
#endif

#endif /* BIT_OP_H_INCLUDED */