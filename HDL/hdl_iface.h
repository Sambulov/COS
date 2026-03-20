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
#include "port_systick_counter.h"
#include "hdl_tick_counter.h"
#include "hdl_time_counter.h"
#include "hdl_entropy.h"
#include "hdl_gpio.h"
#include "hdl_pwm.h"
#include "hdl_nvm.h"
#include "hdl_dma.h"
#include "hdl_adc.h"
#include "hdl_uart.h"
#include "hdl_i2c.h"
#include "hdl_i2c_sw.h"
#include "hdl_spi.h"
#include "hdl_spi_client_sw.h"
#include "hdl_sdio.h"
#include "hdl_mdio.h"
#include "hdl_eth_phy.h"
#include "hdl_dummy_phy.h"
#include "hdl_mac.h"
#include "hdl_can.h"
#include "hdl_button.h"
#include "hdl_buzzer.h"
#include "hdl_timer.h"
#include "hdl_meter.h"
#include "hdl_rtc.h"
#include "hdl_rtc_bm8563.h"
#include "hdl_rtc_ds3231.h"
#include "hdl_i2c_mem.h"
#include "hdl_i2c_tmp112.h"
#include "hdl_i2c_lis2dh12.h"
#include "hdl_sd.h"
#include "hdl_eeprom_i2c.h"
#include "w25xx.h"
#include "xl9535.h"
#include "ms5194t.h"
#include "74HC595.h"
#include "lan8742.h"

//#include "smarc_carrier.h"
//#include "smarc_module.h"

#ifdef __cplusplus
}
#endif

#endif // HDL_IFACE_H_
