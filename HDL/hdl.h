/*
    Harware Driver Layer
    
*/

#ifndef HDL_H_
#define HDL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define HDL_FALSE   0
#define HDL_TRUE    !HDL_FALSE

#define HDL_REG32(addr) ((volatile uint32_t *)(uint32_t)(addr))
#define HDL_REG16(addr) ((volatile uint16_t *)(uint32_t)(addr))

typedef enum {
  HDL_OK = 0,
  

} hdl_errno_t;

#include "hdl_portable.h"
#include "hdl_i2c.h"
#include "hdl_button.h"
#include "hdl_sw_wdt.h"

#include "RK809.h"

#ifdef __cplusplus
}
#endif

#endif // HDL_H_
