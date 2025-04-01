#ifndef HDL_EEPROM_I2C_H_
#define HDL_EEPROM_I2C_H_

#define HDL_EEPROM_I2C_VAR_SIZE  52

typedef struct {
  uint16_t chip_address;
  uint16_t page_size;
  uint32_t write_time;
  uint32_t size;
} hdl_eeprom_i2c_config_t;

/* depends on
  hdl_i2c
  hdl_time_counter
  hdl_gpio  (WP optionally)
*/
hdl_module_new_t(hdl_eeprom_i2c_t, HDL_EEPROM_I2C_VAR_SIZE, hdl_eeprom_i2c_config_t, hdl_nvm_iface_t);

extern const hdl_nvm_iface_t hdl_eeprom_iface;

#endif /* HDL_EEPROM_I2C_H_ */
