#ifndef HDL_RTC_DS3231_H_
#define HDL_RTC_DS3231_H_

#define HDL_RTC_DS3231_VAR_SIZE          36
#define HDL_RTC_DS3231_CHIP_ADDRESS    0x68

/* depends on:
  hdl_i2c_mem
  config: 
  chip adress
*/
hdl_module_new_t(hdl_rtc_ds3231_t, HDL_RTC_DS3231_VAR_SIZE, uint8_t, hdl_rtc_iface_t);

extern const hdl_rtc_iface_t hdl_rtc_ds3231_iface;

#endif /* HDL_RTC_DS3231_H_ */
