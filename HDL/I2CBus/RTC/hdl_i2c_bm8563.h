#ifndef HDL_I2C_BM8563_H_
#define HDL_I2C_BM8563_H_

#define HDL_I2C_BM8563_VAR_SIZE  76

/* chip_address 0x51 */

typedef struct {
  uint16_t chip_address;
} hdl_i2c_bm8563_config_t;

typedef struct {
  uint8_t sec;
  uint8_t min;
  uint8_t hour;
  uint8_t day;
  uint8_t dow;
  uint8_t month;
  uint8_t year;
  int8_t state; /* -1 fault; 0 busy; 1 ready */
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
hdl_module_new_t(hdl_i2c_bm8563_t, HDL_I2C_BM8563_VAR_SIZE, hdl_i2c_bm8563_config_t*, hdl_i2c_bm8563_iface_t);

extern const hdl_i2c_bm8563_iface_t hdl_i2c_bm8563_iface;

__STATIC_INLINE uint8_t hdl_i2c_bm8563_get(const void *desc, hdl_i2c_bm8563_data_t *out_data) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_i2c_bm8563_iface_t *)((hdl_module_base_t *)desc)->iface)->get(desc, out_data);
}

__STATIC_INLINE uint8_t hdl_i2c_bm8563_set(const void *desc, hdl_i2c_bm8563_data_t *data) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_i2c_bm8563_iface_t *)((hdl_module_base_t *)desc)->iface)->get(desc, data);
}

#endif /* HDL_I2C_BM8563_H_ */
