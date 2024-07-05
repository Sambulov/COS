/*!
   LibPortable.h

    Created on: 10.02.2017
        Author: Sambulov Dmitry
        e-mail: dmitry.sambulov@gmail.com
 */

#ifndef CODE_LIB_H_INCLUDED
#define CODE_LIB_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

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
#define CL_PRIVATE(size)                        uint32_t __private[(size + sizeof(void *) - 1) >> 2]

#define libNULL                ((void *)0)

#define libUNUSED(x)           ((void)(x))

#define offsetof(TYPE, MEMBER) __builtin_offsetof (TYPE, MEMBER)

#define __weak                 __attribute__((weak))
#define __packed               __attribute__((__packed__))


#define va_start(v,l)          __builtin_va_start(v,l)
#define va_end(v)              __builtin_va_end(v)
#define va_arg(v,l)            __builtin_va_arg(v,l)

#include "Macros.h"
#include "ClMath.h"
#include "LinkedList.h"
#include "CooperativeMultitasking.h"
#include "MachineState.h"
#include "SimpleCircularBuffer.h"

#ifdef __cplusplus
}
#endif
	
#endif /* CODE_LIB_H_INCLUDED */
