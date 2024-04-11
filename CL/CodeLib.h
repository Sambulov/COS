/*!
   LibPortable.h

    Created on: 10.02.2017
        Author: Sambulov Dmitry
        e-mail: dmitry.sambulov@gmail.com
 */

#ifndef LIB_PORTABLE_H_INCLUDED
#define LIB_PORTABLE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define SIZE_ALIGN2(x)         (((x) + (x & 0x01)))
#define SIZE_ALIGN4(x)         ((SIZE_ALIGN2(x) + (SIZE_ALIGN2(x) & 0x02)))

typedef __builtin_va_list      va_list;
typedef __INT8_TYPE__          int8_t;
typedef __UINT8_TYPE__         uint8_t;
typedef __INT16_TYPE__         int16_t;
typedef __UINT16_TYPE__        uint16_t;
typedef __INT32_TYPE__         int32_t;
typedef __UINT32_TYPE__        uint32_t;
typedef __INT64_TYPE__         int64_t;
typedef __UINT64_TYPE__        uint64_t;
typedef __SIZE_TYPE__          size_t;
typedef __PTRDIFF_TYPE__       ptrdiff_t;

#define false	               0
#define true	               !false

#define libNULL                ((void *)0)

#define offsetof(TYPE, MEMBER) __builtin_offsetof (TYPE, MEMBER)

#define __weak                 __attribute__((weak))
#define __packed               __attribute__((__packed__))


#define va_start(v,l)          __builtin_va_start(v,l)
#define va_end(v)              __builtin_va_end(v)
#define va_arg(v,l)            __builtin_va_arg(v,l)

#define UNUSED(x)             ((void)(x))

#include "LinkedList.h"
#include "CooperativeMultitasking.h"
#include "MachineState.h"

#ifdef __cplusplus
}
#endif
	
#endif /* LIB_PORTABLE_H_INCLUDED */
