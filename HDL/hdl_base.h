/*
    Harware Driver Layer
    
*/

#ifndef HDL_BASE_H_
#define HDL_BASE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define HDL_REG32(addr) ((volatile uint32_t *)(uint32_t)(addr))
#define HDL_REG16(addr) ((volatile uint16_t *)(uint32_t)(addr))

#define BITS_COUNT_U16(x)               ((x >> 0 ) & 1) + ((x >> 1 ) & 1) + ((x >> 2 ) & 1) + ((x >> 3 ) & 1) + \
                                        ((x >> 4 ) & 1) + ((x >> 5 ) & 1) + ((x >> 6 ) & 1) + ((x >> 7 ) & 1) + \
                                        ((x >> 8 ) & 1) + ((x >> 9 ) & 1) + ((x >> 10) & 1) + ((x >> 11) & 1) + \
                                        ((x >> 12) & 1) + ((x >> 13) & 1) + ((x >> 14) & 1) + ((x >> 15) & 1)

#define HDL_FALSE         0
#define HDL_TRUE          !HDL_FALSE

#define PRIVATE(prefix, size)                          uint32_t __##prefix##_private[(size + sizeof(void *) - 1) >> 2]

#define sizeof_member(type, member)        (sizeof(  ((type *)0)->member ))
#define sizeof_member_by_ptr(type, member) (sizeof( *((type *)0)->member ))

#define HDL_ASSERRT_STRUCTURE_CAST(private_type, public_type, prv_size_def, def_file)   _Static_assert(sizeof(private_type) == sizeof(public_type), "In "#def_file" data structure size of "#public_type" doesn't match, check "#prv_size_def)

#include <stddef.h>
#include __MCU_HAL_HDR__

#include "CodeLib.h"

#include "hdl_module.h"
#include "hdl_transceiver.h"
#include "hdl_buffer.h"
#include "hdl_event.h"
#include "hdl_heap.h"


#ifdef __cplusplus
}
#endif

#endif // HDL_BASE_H_
