#ifndef ATB3500_I2C_GATEWAY_H_
#define ATB3500_I2C_GATEWAY_H_

#define ATB3500_I2C_GATEWAY_MAGIC           ((uint32_t)(0x17AF61EA))

#define ATB3500_I2C_GATEWAY_OVN             ((uint32_t)(0x1234567A))
#define ATB3500_MAX_BUFFER_SIZE             ((uint32_t)4096)

typedef struct {
  uint32_t config_magic;
  uint16_t address;
  hdl_i2c_message_options_t options;
  uint16_t length;
  uint8_t buff[ATB3500_MAX_BUFFER_SIZE];
} atb3500_i2c_gateway_proto_rx_t;

typedef struct {
  uint32_t ovn;
  uint16_t address;
  hdl_i2c_message_options_t options;
  hdl_i2c_message_status_t status;
  uint16_t transfered;
  uint16_t length;
  uint8_t buff[ATB3500_MAX_BUFFER_SIZE];
} atb3500_i2c_gateway_proto_tx_t;


/*
  depends on:
  communication
*/
typedef struct {
    hdl_module_t module;
    PRIVATE(bldl, ATB3500_WATCHDOG_PRV_SIZE);
} atb3500_i2c_gateway_t;

hdl_module_state_t atb3500_i2c_gateway(void *desc, uint8_t enable);
atb3500_i2c_gateway_proto_tx_t *atb3500_i2c_gateway_update(atb3500_i2c_gateway_t *desc, uint32_t recieved, atb3500_i2c_gateway_proto_rx_t *rx_data);

#endif //ATB3500_I2C_GATEWAY_H_
