#ifndef HDL_I2C_H_
#define HDL_I2C_H_

#include "port_i2c.h"

#define HDL_I2C_MESSAGE_PRV_SIZE            16
#define HDL_I2C_HW_MESSAGE_PRV_SIZE         8
#define HDL_I2C_PRV_SIZE                    24
#define HDL_I2C_TRANSACTION_PRV_SIZE        28

typedef enum {
  HDL_I2C_MESSAGE_START          = 0x01, /* Generate start condition */
  HDL_I2C_MESSAGE_ADDR           = 0x02, /* Send addr */
  HDL_I2C_MESSAGE_MRSW           = 0x04, /* Receiver mode if option set, else trasmitter mode */
  HDL_I2C_MESSAGE_ADDR_10        = 0x02, /* Send addr */
  HDL_I2C_MESSAGE_NACK_LAST      = 0x08, /* Send NACK in the end in receiver mode */
  HDL_I2C_MESSAGE_STOP           = 0x10, /* Generate stop condition */
} hdl_i2c_message_options_t;

typedef enum {
  HDL_I2C_MESSAGE_START_ON_BUS   = 0x01, /* Generate start condition */
  HDL_I2C_MESSAGE_ADDR_HIT       = 0x02, /* Send addr */
  HDL_I2C_MESSAGE_MRSW_SET       = 0x04, /* Receiver mode if option set, else trasmitter mode */
  HDL_I2C_MESSAGE_NACK_ON_BUS    = 0x08, /* NACK sent */
  HDL_I2C_MESSAGE_STOP_ON_BUS    = 0x10, /* Stop condition generated */
} hdl_i2c_message_state_t;

typedef struct {
  PRIVATE(hdl, HDL_I2C_MESSAGE_PRV_SIZE);
  PRIVATE(hw, HDL_I2C_HW_MESSAGE_PRV_SIZE);
  uint16_t address;
  uint8_t *buffer;
  uint16_t buffer_size;
  hdl_i2c_message_options_t options;
} hdl_i2c_message_t;

/* NOTE: Dont change order! */
typedef enum {
  HDL_I2C_TRANSACTION_UNKNOWN     = 0x00,
  HDL_I2C_TRANSACTION_NEW         = 0x01,
  HDL_I2C_TRANSACTION_ENQUEUED    = 0x02,
  HDL_I2C_TRANSACTION_EXECUTING   = 0x03,
  HDL_I2C_TRANSACTION_EXECUTED    = 0x04,
  HDL_I2C_TRANSACTION_TERMINATING = 0x05,
  HDL_I2C_TRANSACTION_TERMINATED  = 0x06,
  HDL_I2C_TRANSACTION_FAILED      = 0x07,
} hdl_i2c_transaction_state_t;

typedef struct {
  PRIVATE(hdl, HDL_I2C_TRANSACTION_PRV_SIZE);
} hdl_i2c_transaction_t;

typedef struct {
  hdl_module_t module;
  const hdl_i2c_client_hw_t *hw_conf;
  PRIVATE(hdl, HDL_I2C_PRV_SIZE);
  PRIVATE(hw, HDL_I2C_HAL_PRV_SIZE);
} hdl_i2c_client_t;

typedef enum {
  HDL_I2C_HW_CLIENT_XFER_STATE_ONGOING,
  HDL_I2C_HW_CLIENT_XFER_STATE_IDLE,
  HDL_I2C_HW_CLIENT_XFER_STATE_ERROR, //  HDL_I2C_HW_CLIENT_BUS_ERROR,   /* Bus stretching */
} hdl_i2c_hw_client_xfer_state_t;

/* Portable functions */
hdl_module_state_t hdl_i2c_hw(void *i2c, uint8_t enable);

void hdl_i2c_hw_client_xfer_message(hdl_i2c_client_t *i2c, hdl_i2c_message_t *msg);
hdl_i2c_hw_client_xfer_state_t hdl_i2c_hw_client_xfer_state(hdl_i2c_client_t *i2c);
void hdl_i2c_hw_client_bus_reset(hdl_i2c_client_t *i2c);
hdl_i2c_message_state_t hdl_i2c_client_message_state(hdl_i2c_message_t *msg);
uint16_t hdl_i2c_client_message_get_transfered(hdl_i2c_message_t *msg);
/* End portable */

hdl_module_state_t hdl_i2c(void *i2c, uint8_t enable);

uint8_t hdl_i2c_client_transaction_add_message(hdl_i2c_transaction_t *transaction, hdl_i2c_message_t *msg);
void hdl_i2c_client_enqueue_transaction(hdl_i2c_client_t *i2c, hdl_i2c_transaction_t *transaction);
void hdl_i2c_client_cancel_transaction(hdl_i2c_client_t *i2c, hdl_i2c_transaction_t *transaction);

hdl_i2c_transaction_state_t hdl_i2c_client_transaction_state(hdl_i2c_transaction_t *transaction);

#endif /* HDL_I2C_H_ */
