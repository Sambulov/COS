/* 
    HAL dependent methods

*/

#ifndef HDL_PORTABLE_H_
#define HDL_PORTABLE_H_

#if defined ( NUM46X )
  #include "../MCU/NUM46X/Sys/inc/m460.h"
  #define __MCU_HAL_OK__
#endif

#if defined ( GD32E23X )
  #include <stddef.h>
  #include "../MCU/GD32E23X/Sys/inc/gd32e23x.h"
  #define __MCU_HAL_OK__
#endif

#if defined ( STM32L0XX )
  #include <stddef.h>
  #include "stm32l0xx_hal.h"
  #define __MCU_HAL_OK__
#endif

#ifndef __MCU_HAL_OK__
  #error "MCU unsupported!"
#endif

#include "hdl_sys.h"
#include "hdl_gpio.h"

#endif // HDL_PORTABLE_H_
