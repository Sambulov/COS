#ifndef HDL_I2C_H_
#define HDL_I2C_H_

#if defined ( GD32E23X )
typedef struct {
  const hdl_gpio_t *scl;
  const hdl_gpio_t *sda;
  const uint32_t periph;       /* I2C0, I2C1 */
  const uint32_t dtcy;         /* I2C_DTCY_2 or I2C_DTCY_16_9 */
  const uint32_t speed;        /* Max 1000000 */
  const uint32_t mode;         /* 0 - disable, 1 - client, 2 - server, 3 - both */
  const uint32_t stretching;   /* SCL stretching when data is not ready: 0 - disable, 1 - enable */
  const uint32_t general_call; /* responce on general call: 0 - disable, 1 - enable */
  const uint32_t addr_size;    /* I2C_ADDFORMAT_7BITS or I2C_ADDFORMAT_10BITS */
  const uint32_t addr;         /* address in server mode */
} hdl_i2c_hw_t;
#endif

typedef enum {
  HDL_I2C_ADDR,
  HDL_I2C_TX_EMPTY,
  HDL_I2C_RX_NOT_EMPTY,
  HDL_I2C_OVERUNDERUN,
  HDL_I2C_BUS_ERROR,
  HDL_I2C_STOP,
} hdl_i2c_event_t;

#define HDL_I2C_MESSAGE_PRV_SIZE            16
#define HDL_I2C_HAL_PRV_SIZE                16
#define HDL_I2C_PRV_SIZE                    16
#define HDL_I2C_TRANSACTION_PRV_SIZE        28

typedef enum {
  HDL_I2C_MESSAGE_START          = 0x01, /* Generate start condition */
  HDL_I2C_MESSAGE_ADDR           = 0x02, /* Send addr */
  HDL_I2C_MESSAGE_READ           = 0x04, /* Receiver mode if option set, else trasmitter mode */
  HDL_I2C_MESSAGE_NACK_LAST      = 0x08, /* Send NACK in the end in receiver mode */
  HDL_I2C_MESSAGE_STOP           = 0x10, /* Generate stop condition */
} hdl_i2c_message_options_t;

typedef struct {
  uint8_t __private[HDL_I2C_MESSAGE_PRV_SIZE];
  uint32_t address;
  uint8_t *data;
  uint32_t length;
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
	uint8_t __private[HDL_I2C_TRANSACTION_PRV_SIZE];
} hdl_i2c_transaction_t;

typedef struct {
	uint8_t __private[HDL_I2C_PRV_SIZE];
	uint8_t __hal_private[HDL_I2C_HAL_PRV_SIZE];
  const hdl_i2c_hw_t *hw_conf;
  hdl_callback_t *slave_event_cb;
} hdl_i2c_t;

/* Portable functions */
void hdl_hal_i2c_init(hdl_i2c_t *desc);
// void hdl_i2c_client_xfer_message(hdl_i2c_t *desc, hdl_i2c_message_t *msg);
// void hdl_i2c_server_xfer_message(hdl_i2c_t *desc, hdl_i2c_message_t *msg);
// void hdl_hal_i2c_work(hdl_i2c_t *desc);

void hdl_i2c_reset(hdl_i2c_t *desc);
void hdl_hal_i2c_deinit(hdl_i2c_t *desc);
/* ================== */

void hdl_i2c_init(hdl_i2c_t *desc);
void hdl_i2c_work(hdl_i2c_t *desc);
void hdl_i2c_deinit(hdl_i2c_t *desc);

void hdl_i2c_client_transaction_init_reset(hdl_i2c_transaction_t *transaction);
void hdl_i2c_client_transaction_add_message(hdl_i2c_transaction_t *transaction, hdl_i2c_message_t *message);
void hdl_i2c_enqueue_transaction(hdl_i2c_t *desc, hdl_i2c_transaction_t *transaction);
hdl_i2c_transaction_state_t hdl_i2c_transaction_status(hdl_i2c_transaction_t *transaction);

#endif /* HDL_I2C_H_ */
