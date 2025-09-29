#ifndef HDL_I2C_LIS2DH12_H_
#define HDL_I2C_LIS2DH12_H_

/* MEMS digital output motion sensor: ultra-low-power high-performance 3-axis "femto" accelerometer */

#define HDL_I2C_LIS2DH12_VAR_SIZE  44

#define HDL_I2C_LIS2DH12_CTRL0_SDO_PU_DIS     0x80    /* Disconnect SDO/SA0 pull-up. */

#define HDL_I2C_LIS2DH12_CTRL1_SPEED_1HZ       0x10    /* HR / Normal / Low-power mode (1 Hz) */
#define HDL_I2C_LIS2DH12_CTRL1_SPEED_10HZ      0x20    /* HR / Normal / Low-power mode (10 Hz) */
#define HDL_I2C_LIS2DH12_CTRL1_SPEED_25HZ      0x30    /* HR / Normal / Low-power mode (25 Hz) */
#define HDL_I2C_LIS2DH12_CTRL1_SPEED_50HZ      0x40    /* HR / Normal / Low-power mode (50 Hz) */
#define HDL_I2C_LIS2DH12_CTRL1_SPEED_100HZ     0x50    /* HR / Normal / Low-power mode (100 Hz) */
#define HDL_I2C_LIS2DH12_CTRL1_SPEED_200HZ     0x60    /* HR / Normal / Low-power mode (200 Hz) */
#define HDL_I2C_LIS2DH12_CTRL1_SPEED_400HZ     0x70    /* HR / Normal / Low-power mode (400 Hz) */
#define HDL_I2C_LIS2DH12_CTRL1_SPEED_1_62HZ    0x80    /* Low-power mode (1.620 Hz) */
#define HDL_I2C_LIS2DH12_CTRL1_SPEED_1_34HZ    0x90    /* HR/ Normal (1.344 kHz); Low-power mode (5.376 kHz) */

#define HDL_I2C_LIS2DH12_CTRL1_LP_EN           0x08    /* Low-power mode enable. */
#define HDL_I2C_LIS2DH12_CTRL1_Z_AX_EN         0x04    /* Z-axis enable. */
#define HDL_I2C_LIS2DH12_CTRL1_Y_AX_EN         0x02    /* Y-axis enable. */
#define HDL_I2C_LIS2DH12_CTRL1_X_AX_EN         0x01    /* X-axis enable. */
#define HDL_I2C_LIS2DH12_CTRL1_XYZ_AX_EN       0x07    /* X,Y,Z-axis enable. */

#define HDL_I2C_LIS2DH12_CTRL4_HR              0x08    /* High resolution */

/* chip_address 0x18 */
typedef struct {
  uint8_t chip_address;
  uint8_t cnf_reg0;
  uint8_t cnf_reg1;
  uint8_t cnf_reg2;
  uint8_t cnf_reg3;
  uint8_t cnf_reg4;
  uint8_t cnf_reg5;
  uint8_t cnf_reg6;
} hdl_i2c_lis2dh12_config_t;

typedef struct {
  int16_t gx;
  int16_t gy;
  int16_t gz;
  int8_t state; /* -1 fault; 0 busy; 1 ready */
} hdl_i2c_lis2dh12_data_t;

typedef uint8_t (* hdl_i2c_lis2dh12_get_t)(const void *desc, hdl_i2c_lis2dh12_data_t *data);

typedef struct {
  hdl_module_initializer_t init;
  hdl_i2c_lis2dh12_get_t get;
} hdl_i2c_lis2dh12_iface_t;

/* depends on
  hdl_i2c
  hdl_gpio  (int1)
  hdl_gpio  (int2)
*/
hdl_module_new_t(hdl_i2c_lis2dh12_t, HDL_I2C_LIS2DH12_VAR_SIZE, hdl_i2c_lis2dh12_config_t*, hdl_i2c_lis2dh12_iface_t);

extern const hdl_i2c_lis2dh12_iface_t hdl_i2c_lis2dh12_iface;

__STATIC_INLINE uint8_t hdl_i2c_lis2dh12_get(const void *desc, hdl_i2c_lis2dh12_data_t *out_data) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_i2c_lis2dh12_iface_t *)((hdl_module_base_t *)desc)->iface)->get(desc, out_data);
}

#endif /* HDL_I2C_LIS2DH12_H_ */
