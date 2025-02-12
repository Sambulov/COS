/* 
    HAL dependent methods

*/

#ifndef HDL_PORTABLE_H_
#define HDL_PORTABLE_H_

#define HDL_FALSE         0
#define HDL_TRUE          !HDL_FALSE

#define PRIVATE(prefix, size)                          uint32_t __##prefix##_private[(size + sizeof(void *) - 1) >> 2]

#define sizeof_member(type, member)        (sizeof(  ((type *)0)->member ))
#define sizeof_member_by_ptr(type, member) (sizeof( *((type *)0)->member ))

#define static_malloc(size)             (&((size_t [(size + sizeof(size_t *) - 1) >> 2]){ 0 }))

#define HDL_ASSERRT_STRUCTURE_CAST(private_type, public_type, prv_size_def, def_file)   _Static_assert(sizeof(private_type) == sizeof(public_type), "In "#def_file" data structure size of "#public_type" doesn't match, check "#prv_size_def)

#include <stddef.h>
#include __MCU_HAL_HDR__

#include "hdl_module.h"
#include "hdl_transceiver.h"
#include "hdl_buffer.h"
#include "hdl_event.h"
#include "hdl_heap.h"
#include "hdl_core.h"
#include "hdl_clock.h"

#include "port_nvm.h"
#include "port_dma.h"
#include "port_adc.h"

#include "hdl_tick_counter.h"
#include "hdl_time_counter.h"
#include "hdl_gpio.h"
#include "hdl_spi.h"
#include "hdl_i2c.h"
#include "hdl_uart.h"

#endif // HDL_PORTABLE_H_
