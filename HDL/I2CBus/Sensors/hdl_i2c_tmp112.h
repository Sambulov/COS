#ifndef HDL_I2C_TMP112_H_
#define HDL_I2C_TMP112_H_

/* TMP112x High-Accuracy, Low-Power, Digital Temperature Sensor */

#define HDL_I2C_TMP112_VAR_SIZE  32

/* chip_address 0x48 */

typedef struct {
  int32_t temp;
  int8_t state; /* -1 fault; 0 busy; 1 ready */
} hdl_i2c_tmp112_data_t;


typedef uint8_t (* hdl_i2c_tmp112_get_t)(const void *desc, hdl_i2c_tmp112_data_t *data);

typedef struct {
  hdl_module_initializer_t init;
  hdl_i2c_tmp112_get_t get;
} hdl_i2c_tmp112_iface_t;

/* depends on
  hdl_i2c
  hdl_gpio  (int)
*/
hdl_module_new_t(hdl_i2c_tmp112_t, HDL_I2C_TMP112_VAR_SIZE, uint8_t, hdl_i2c_tmp112_iface_t);

extern const hdl_i2c_tmp112_iface_t hdl_i2c_tmp112_iface;

__STATIC_INLINE uint8_t hdl_i2c_tmp112_get(const void *desc, hdl_i2c_tmp112_data_t *out_data) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_i2c_tmp112_iface_t *)((hdl_module_base_t *)desc)->iface)->get(desc, out_data);
}

#endif /* HDL_I2C_TMP112_H_ */
