#ifndef HDL_I2C_H_
#define HDL_I2C_H_

typedef enum {
  HDL_I2C_MESSAGE_START          = 0x01, /* Generate start condition */
  HDL_I2C_MESSAGE_ADDR           = 0x02, /* Send addr */
  HDL_I2C_MESSAGE_ADDR_10        = 0x04, /* Send addr */
  HDL_I2C_MESSAGE_MRSW           = 0x08, /* Receiver mode if option set, else trasmitter mode */
  HDL_I2C_MESSAGE_NACK_LAST      = 0x10, /* Send NACK in the end in receiver mode */
  HDL_I2C_MESSAGE_STOP           = 0x20, /* Generate stop condition */
} hdl_i2c_message_options_t;

typedef enum {
  HDL_I2C_MESSAGE_STATUS_INITIAL           = 0x0000,
  HDL_I2C_MESSAGE_STATUS_START_ON_BUS      = 0x0001,
  HDL_I2C_MESSAGE_STATUS_ADDR_SENT         = 0x0002,
  HDL_I2C_MESSAGE_STATUS_DATA              = 0x0004,
  HDL_I2C_MESSAGE_STATUS_NACK              = 0x0008,
  HDL_I2C_MESSAGE_STATUS_STOP_ON_BUS       = 0x0010,
  HDL_I2C_MESSAGE_FAULT_ARBITRATION_LOST   = 0x0100,
  HDL_I2C_MESSAGE_FAULT_BUS_ERROR          = 0x0200,
  HDL_I2C_MESSAGE_FAULT_BAD_STATE          = 0x0400,
  HDL_I2C_MESSAGE_FAULT_MASK               = 0x0700,
  HDL_I2C_MESSAGE_STATUS_COMPLETE          = 0x8000
} hdl_i2c_message_status_t;

typedef struct {
  uint16_t address;
  hdl_i2c_message_options_t options;
  hdl_i2c_message_status_t status;
  uint16_t transferred;
  uint16_t length;
  uint8_t *buffer;
} hdl_i2c_message_t;

typedef struct {
  uint16_t address;
  const hdl_transceiver_t *transceiver;
} hdl_i2c_channel_t;

typedef struct {
  hdl_i2c_channel_t * const *channels;
  const void *hwc;
} hdl_i2c_config_t;

#define hdl_i2c_channels(...) ((hdl_i2c_channel_t * const []){__VA_ARGS__, NULL})

typedef uint8_t (* hdl_i2c_transfer_message_t)(const void *desc, hdl_i2c_message_t *message);

typedef struct {
  hdl_module_initializer_t init;
  hdl_i2c_transfer_message_t transfer;
  hdl_set_transceiver_t transceiver_set;
} hdl_i2c_iface_t;

__STATIC_INLINE uint8_t hdl_i2c_transfer(const void *desc, hdl_i2c_message_t *message) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_i2c_iface_t *)((hdl_module_base_t *)desc)->iface)->transfer(desc, message);
}

__STATIC_INLINE uint8_t hdl_i2c_transceiver_set(const void *desc, const hdl_transceiver_t *transceiver, uint32_t channel_id) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_i2c_iface_t *)((hdl_module_base_t *)desc)->iface)->transceiver_set(desc, transceiver, channel_id);
}

#endif /* HDL_I2C_H_ */
