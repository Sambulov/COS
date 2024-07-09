#ifndef HDL_I2C_H_
#define HDL_I2C_H_

#include "port_i2c.h"

#define HDL_I2C_MESSAGE_PRV_SIZE            16
#define HDL_I2C_HW_MESSAGE_PRV_SIZE         8
#define HDL_I2C_TRANSACTION_PRV_SIZE        28

typedef enum {
  HDL_I2C_HW_CLIENT_XFER_STATE_ONGOING,
  HDL_I2C_HW_CLIENT_XFER_STATE_IDLE,
  HDL_I2C_HW_CLIENT_XFER_STATE_ERROR, //  HDL_I2C_HW_CLIENT_BUS_ERROR,   /* Bus stretching */
} hdl_i2c_hw_client_xfer_state_t;

typedef enum {
  HDL_I2C_MESSAGE_START          = 0x01, /* Generate start condition */
  HDL_I2C_MESSAGE_ADDR           = 0x02, /* Send addr */
  HDL_I2C_MESSAGE_MRSW           = 0x04, /* Receiver mode if option set, else trasmitter mode */
  HDL_I2C_MESSAGE_ADDR_10        = 0x02, /* Send addr */
  HDL_I2C_MESSAGE_NACK_LAST      = 0x08, /* Send NACK in the end in receiver mode */
  HDL_I2C_MESSAGE_STOP           = 0x10, /* Generate stop condition */
} hdl_i2c_message_options_t;

typedef struct {
  uint16_t address;
  uint8_t *buffer;
  uint16_t buffer_size;
  hdl_i2c_message_options_t options;
  PRIVATE(hdl, HDL_I2C_HW_MESSAGE_PRV_SIZE);
} hdl_i2c_message_t;

typedef struct {
  hdl_i2c_message_t **messages;
  PRIVATE(hdl, HDL_I2C_TRANSACTION_PRV_SIZE);
} hdl_i2c_transaction_t;

#define hdl_i2c_transaction_messages(...) ((hdl_i2c_message_t *[]){__VA_ARGS__, NULL})

typedef struct {
  hdl_module_t module;
  hdl_i2c_client_config_t *config;
  PRIVATE(hdl, HDL_I2C_CLIENT_PRV_SIZE);
} hdl_i2c_client_t;

hdl_module_state_t hdl_i2c_client(void *i2c, uint8_t enable);









/* NOTE: Dont change order! */
// typedef enum {
//   HDL_I2C_TRANSACTION_UNKNOWN     = 0x00,
//   HDL_I2C_TRANSACTION_NEW         = 0x01,
//   HDL_I2C_TRANSACTION_ENQUEUED    = 0x02,
//   HDL_I2C_TRANSACTION_EXECUTING   = 0x03,
//   HDL_I2C_TRANSACTION_EXECUTED    = 0x04,
//   HDL_I2C_TRANSACTION_TERMINATING = 0x05,
//   HDL_I2C_TRANSACTION_TERMINATED  = 0x06,
//   HDL_I2C_TRANSACTION_FAILED      = 0x07,
// } hdl_i2c_transaction_state_t;




// typedef struct {
//   uint32_t messages_transfered;
//   //lihygyij
// } hdl_i2c_transaction_state_t;

// /*
//   depends on:
//   hdl_i2c_client_phy

// */

// void hdl_i2c_client_enqueue_transaction(hdl_i2c_client_t *i2c, hdl_i2c_transaction_t *transaction);
// void hdl_i2c_client_transaction_state(hdl_i2c_transaction_t *transaction, hdl_i2c_transaction_state_t *out_state);

#endif /* HDL_I2C_H_ */
