#ifndef HDL_RTC_BM8563_H_
#define HDL_RTC_BM8563_H_

#define HDL_RTC_BM8563_VAR_SIZE          36
#define HDL_RTC_BM8563_CHIP_ADDRESS    0x51

/* depends on:
  hdl_i2c_mem
  config: 
  chip adress
*/
hdl_module_new_t(hdl_rtc_bm8563_t, HDL_RTC_BM8563_VAR_SIZE, uint8_t, hdl_rtc_iface_t);

extern const hdl_rtc_iface_t hdl_rtc_bm8563_iface;

#endif /* HDL_I2C_BM8563_H_ */
