#ifndef CRC32_H_INCLUDED
#define CRC32_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  CRC32_REFLECT_IN  = 1,
  CRC32_REFLECT_OUT = 2,
  CRC32_INVERT_OUT = 4
} Crc32Options_t;

uint32_t ulCrc32(const void *pvData, uint32_t ulLen, uint32_t ulInit, const uint32_t ulPoly, Crc32Options_t eOpt);

#define CRC_32_AIXM(data, len) ulCrc32(data, len, 0x00000000, 0x814141AB, !CRC32_REFLECT_IN | !CRC32_REFLECT_OUT | !CRC32_INVERT_OUT);
#define CRC_32_AUTOSAR(data, len) ulCrc32_n(data, len, 0xFFFFFFFF, 0xF4ACFB13, CRC32_REFLECT_IN | CRC32_REFLECT_OUT | CRC32_INVERT_OUT);
#define CRC_32_BASE91_D(data, len) ulCrc32(data, len, 0xFFFFFFFF, 0xA833982B, CRC32_REFLECT_IN | CRC32_REFLECT_OUT | CRC32_INVERT_OUT);
#define CRC_32_BZIP2(data, len) ulCrc32(data, len, 0xFFFFFFFF, 0x04C11DB7, !CRC32_REFLECT_IN | !CRC32_REFLECT_OUT | CRC32_INVERT_OUT);
#define CRC_32_CD_ROM_EDC(data, len) ulCrc32(data, len, 0x00000000, 0x8001801B, CRC32_REFLECT_IN | CRC32_REFLECT_OUT | !CRC32_INVERT_OUT);
#define CRC_32_CKSUM(data, len) ulCrc32(data, len, 0x00000000, 0x04C11DB7, !CRC32_REFLECT_IN | !CRC32_REFLECT_OUT | CRC32_INVERT_OUT);
#define CRC_32_ISCSI(data, len) ulCrc32(data, len, 0xFFFFFFFF, 0x1EDC6F41, CRC32_REFLECT_IN | CRC32_REFLECT_OUT | CRC32_INVERT_OUT);
#define CRC_32_ISO_HDLC(data, len) ulCrc32(data, len, 0xFFFFFFFF, 0x04C11DB7, CRC32_REFLECT_IN | CRC32_REFLECT_OUT | CRC32_INVERT_OUT);
#define CRC_32_JAMCRC(data, len) ulCrc32(data, len, 0xFFFFFFFF, 0x04C11DB7, CRC32_REFLECT_IN | CRC32_REFLECT_OUT | !CRC32_INVERT_OUT);
#define CRC_32_MEF(data, len) ulCrc32(data, len, 0xFFFFFFFF, 0x741B8CD7, CRC32_REFLECT_IN | CRC32_REFLECT_OUT | !CRC32_INVERT_OUT);
#define CRC_32_MPEG_2(data, len) ulCrc32(data, len, 0xFFFFFFFF, 0x04C11DB7, !CRC32_REFLECT_IN | !CRC32_REFLECT_OUT | !CRC32_INVERT_OUT);
#define CRC_32_XFER(data, len) ulCrc32(data, len, 0x00000000, 0x000000AF, !CRC32_REFLECT_IN | !CRC32_REFLECT_OUT | !CRC32_INVERT_OUT);

/*!
  Snake notation
*/

typedef Crc32Options_t crc32_options_t;

uint32_t crc32(const void *data, uint32_t len, uint32_t init, const uint32_t poly, crc32_options_t opt);

#ifdef __cplusplus
}
#endif

#endif /* CRC32_H_INCLUDED */
