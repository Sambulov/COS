#ifndef CODE_LIB_MACROS_H_INCLUDED
#define CODE_LIB_MACROS_H_INCLUDED

#define SIZE_ALIGN2(x)         (((x) + (x & 0x01)))
#define SIZE_ALIGN4(x)         ((SIZE_ALIGN2(x) + (SIZE_ALIGN2(x) & 0x02)))

#define TIME_ELAPSED(timestamp, delay, now) ((uint32_t)(now - timestamp) >= (uint32_t)(delay))

#endif //CODE_LIB_MACROS_H_INCLUDED
