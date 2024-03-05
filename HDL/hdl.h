/*
    Harware Driver Layer
    
*/

#ifndef HDL_H_
#define HDL_H_

#define HDL_FALSE   0
#define HDL_TRUE    !HDL_FALSE

#define HDL_REG32(addr) ((volatile uint32_t *)(uint32_t)(addr))
#define HDL_REG16(addr) ((volatile uint16_t *)(uint32_t)(addr))

#include "hdl_callback.h"
#include "hdl_portable.h"
#include "hdl_button.h"
#include "hdl_sw_wdt.h"

#endif // HDL_H_
