#ifndef HDL_I2C_H_
#define HDL_I2C_H_

#define HDL_I2C_MESSAGE_PRV_SIZE            16
#define HDL_I2C_PRV_SIZE                    48
#define HDL_I2C_TRANSACTION_PRV_SIZE        28

#if defined ( GD32E23X ) || defined ( GD32F103VG ) || defined ( GD32F450 )
  #define HDL_I2C_HAL_PRV_SIZE                16

  typedef struct {
    hdl_module_t module;
    const uint32_t reg;       /* I2C0, I2C1 */
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
  HDL_I2C_EVENT_START,
  HDL_I2C_EVENT_ADDR,
  HDL_I2C_EVENT_RX_DATA,
  HDL_I2C_EVENT_TX_DATA,
  HDL_I2C_EVENT_OVER_UNDERRUN,
  HDL_I2C_EVENT_BUS_ERROR,
  HDL_I2C_EVENT_STOP,
} hdl_i2c_event_t;

typedef enum {
  HDL_I2C_MESSAGE_START          = 0x0001, /* Generate start condition */
  HDL_I2C_MESSAGE_ADDR           = 0x0002, /* Send addr */
  HDL_I2C_MESSAGE_MRSW           = 0x0004, /* Receiver mode if option set, else trasmitter mode */
  HDL_I2C_MESSAGE_NACK_LAST      = 0x0008, /* Send NACK in the end in receiver mode */
  HDL_I2C_MESSAGE_STOP           = 0x0010, /* Generate stop condition */
  
  HDL_I2C_MESSAGE_START_ON_BUS   = 0x0100, /* Start generated/detected */
  HDL_I2C_MESSAGE_ADDR_HIT       = 0x0200, /* Addr sent/received */
  HDL_I2C_MESSAGE_MRSW_SET       = 0x0400, /* Master reader / Slave writer flag */
  HDL_I2C_MESSAGE_NACK           = 0x0800, /* Send NACK in the end in receiver mode */
  HDL_I2C_MESSAGE_STOP_ON_BUS    = 0x1000, /* Generate stop condition */
} hdl_i2c_message_flags_t;

typedef struct {
  uint8_t __private[HDL_I2C_MESSAGE_PRV_SIZE];
  uint32_t address;
  uint8_t *buffer;
  uint32_t buffer_size;
  uint32_t data_transfered;
  hdl_i2c_message_flags_t flags;
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
	uint8_t __private[HDL_I2C_HAL_PRV_SIZE];
} hdl_hal_i2c_t;

typedef struct {
	uint8_t __private[HDL_I2C_PRV_SIZE];
  const hdl_i2c_hw_t *hw_conf;
  hdl_hal_i2c_t hal;
  event_handler_t *server_event_cb;
  void *context;
} hdl_i2c_t;

/* Portable functions */
hdl_module_state_t hdl_i2c(void *desc, uint8_t enable);
void hdl_hal_i2c_bus_state(hdl_i2c_t *desc);
void hdl_i2c_client_xfer_message(hdl_i2c_t *desc, hdl_i2c_message_t *msg);
//void hdl_i2c_server_xfer_message(hdl_i2c_t *desc, hdl_i2c_message_t *msg);
void hdl_hal_i2c_deinit(hdl_i2c_t *desc);
/* ================== */

//void hdl_i2c_work(hdl_i2c_t *desc);
void hdl_i2c_client_transaction_init_reset(hdl_i2c_transaction_t *transaction);
void hdl_i2c_client_transaction_add_message(hdl_i2c_transaction_t *transaction, hdl_i2c_message_t *message);
void hdl_i2c_client_enqueue_transaction(hdl_i2c_t *desc, hdl_i2c_transaction_t *transaction);
hdl_i2c_transaction_state_t hdl_i2c_client_transaction_state(hdl_i2c_transaction_t *transaction);

#endif /* HDL_I2C_H_ */
