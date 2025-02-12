/*
    Harware Driver Layer
    
*/

#ifndef HDL_IFACE_H_
#define HDL_IFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hdl_base.h"

#include "hdl_portable.h"

#include "hdl_core.h"

#include "hdl_clock.h"

#include "hdl_tick_counter.h"

#include "hdl_time_counter.h"

#include "hdl_gpio.h"

#include "hdl_nvm.h"
#include "hdl_dma.h"
#include "hdl_adc.h"

#include "hdl_uart.h"
#include "hdl_i2c.h"
#include "hdl_spi.h"

#include "hdl_i2c_sw.h"

#include "hdl_button.h"
#include "hdl_timer.h"
#include "hdl_eeprom_i2c.h"
#include "xl9535.h"
#include "ms5194t.h"


//#include "smarc_carrier.h"
//#include "smarc_module.h"

#ifdef __cplusplus
}
#endif

#endif // HDL_IFACE_H_
