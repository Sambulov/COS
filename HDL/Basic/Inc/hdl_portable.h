/* 
    HAL dependent methods

*/

#ifndef HDL_PORTABLE_H_
#define HDL_PORTABLE_H_

#define HDL_FALSE         0
#define HDL_TRUE          !HDL_FALSE

#include <stddef.h>
#include __MCU_HAL_HDR__

#include "hdl_callback.h"
#include "hdl_hw.h"
#include "hdl_core.h"
#include "hdl_clock.h"
//#include "Interrupt.h"
#include "hdl_clock.h"
#include "hdl_clock_counter.h"
#include "hdl_sys_timer.h"
#include "hdl_sys.h"
#include "hdl_gpio.h"
//#include "hdl_i2c.h"

#endif // HDL_PORTABLE_H_
