#ifndef CODE_LIB_MACROS_H_INCLUDED
#define CODE_LIB_MACROS_H_INCLUDED

#define SIZE_ALIGN2(x)         (((x) + (x & 0x01)))
#define SIZE_ALIGN4(x)         ((SIZE_ALIGN2(x) + (SIZE_ALIGN2(x) & 0x02)))


#define HDL_REG_MODIFY(reg, mask, bits)         (reg = (((reg) & ~(mask)) | ((bits) & (mask))))
#define HDL_REG_CLEAR(reg, mask)                (reg = ((reg) & ~(mask)))
#define HDL_REG_SET(reg, mask)                  (reg |= (mask))
#define HDL_REG_GET(reg, mask)                  ((reg) & (mask))
#define HDL_REG_DIF(reg, mask, val)             (((reg) ^ (val)) & (mask))
#define HDL_REG_CHECK(reg, mask, bits)          (HDL_REG_DIF(reg, mask, bits) == 0)

#define MIN(A, B)                               ((A < B)? (A): (B))
#define MAX(A, B)                               ((A > B)? (A): (B))

#define ABS(A)                                  (((A) >= 0)? (A) : -(A))

#define TIME_ELAPSED(timestamp, delay, now) ((uint32_t)(now - timestamp) >= (uint32_t)(delay))

#endif //CODE_LIB_MACROS_H_INCLUDED
