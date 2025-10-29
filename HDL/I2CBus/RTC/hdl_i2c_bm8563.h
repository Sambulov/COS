#ifndef HDL_I2C_BM8563_H_
#define HDL_I2C_BM8563_H_

#define HDL_I2C_BM8563_VAR_SIZE  32

/* chip_address 0x51 */

typedef enum {
  HDL_I2C_BM8563_XFER_STATE_FAIL  = 0xff,
  HDL_I2C_BM8563_XFER_STATE_BUSY  = 0x00,
  HDL_I2C_BM8563_XFER_STATE_READY = 0x01,
} hdl_i2c_bm8563_xfer_sate_t;

typedef struct {
  uint8_t sec   :7, /* 00-59 seconds BCD */
          vl    :1; /* reliable clock/calendar information is 0: guaranteed; 1: no longer guaranteed */
  uint8_t min   :7, /* 00-59 minutes BCD */
          res0  :1; /* reserved */
  uint8_t hour  :6, /* 00-23 hours BCD */
          res1  :2; /* reserved */
  uint8_t day   :6, /* 01-31 days BCD */
          res2  :2; /* reserved */
  uint8_t dow   :3, /* Sun 1-7 Sat day of week BCD */
          res3  :5; /* reserved */
  uint8_t month :5, /* 01-12 month BCD */
          res4  :2, /* reserved */
          cent  :1; /* centure 0:20xx; 1:19xx */
  uint8_t year;     /* 00-99 year BCD */
  hdl_i2c_bm8563_xfer_sate_t state;
} hdl_i2c_bm8563_data_t;

typedef uint8_t (* hdl_i2c_bm8563_xfer_t)(const void *desc, hdl_i2c_bm8563_data_t *data);

typedef struct {
  hdl_module_initializer_t init;
  hdl_i2c_bm8563_xfer_t get;
  hdl_i2c_bm8563_xfer_t set;
} hdl_i2c_bm8563_iface_t;

/* depends on
  hdl_i2c
  hdl_gpio  (int)
*/
hdl_module_new_t(hdl_i2c_bm8563_t, HDL_I2C_BM8563_VAR_SIZE, uint8_t, hdl_i2c_bm8563_iface_t);

extern const hdl_i2c_bm8563_iface_t hdl_i2c_bm8563_iface;

__STATIC_INLINE uint8_t hdl_i2c_bm8563_get(const void *desc, hdl_i2c_bm8563_data_t *out_data) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_i2c_bm8563_iface_t *)((hdl_module_base_t *)desc)->iface)->get(desc, out_data);
}

__STATIC_INLINE uint8_t hdl_i2c_bm8563_set(const void *desc, hdl_i2c_bm8563_data_t *data) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_i2c_bm8563_iface_t *)((hdl_module_base_t *)desc)->iface)->set(desc, data);
}

#define TO_BCD_BYTE(val)  ((uint8_t)((val % 10) | ((val / 10) % 10) * 16))

__STATIC_INLINE uint8_t hdl_i2c_bm8563_build(hdl_i2c_bm8563_data_t *data, 
  uint16_t year, uint8_t month, uint8_t day, uint8_t week_day, uint8_t hour, uint8_t min, uint8_t sec) {
  if((data != NULL) && 
     (year >= 1900) && (year <= 2099) &&
     (month >= 1) && (month <= 12) &&
     (day >= 1) && (day <= 31) &&
     (week_day >= 1) && (week_day <= 7) &&
     (hour <= 23) &&
     (min < 60) &&
     (sec < 60)) {
    data->year = TO_BCD_BYTE(year);
    data->cent = (year < 2000);
    data->month = TO_BCD_BYTE(month);
    data->day = TO_BCD_BYTE(day);
    data->dow = week_day % 7;
    data->hour = TO_BCD_BYTE(hour);
    data->min = TO_BCD_BYTE(min);
    data->sec = TO_BCD_BYTE(sec);
    data->state = HDL_I2C_BM8563_XFER_STATE_READY;
    return HDL_TRUE;
  }
  return HDL_FALSE;
} 

#endif /* HDL_I2C_BM8563_H_ */
