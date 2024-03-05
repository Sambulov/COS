#ifndef HDL_I2C_H_
#define HDL_I2C_H_

#include "CodeLib.h"

#if defined ( GD32E23X )
typedef struct {
  const hdl_gpio_t *scl;
  const hdl_gpio_t *sda;
  const uint32_t periph;       /* I2C0, I2C1 */
  const uint32_t dtcy;         /* I2C_DTCY_2 or I2C_DTCY_16_9 */
  const uint32_t speed;        /* Max 1000000 */
  const uint32_t mode;         /* 0 - disable, 1 - master, 2 - slave, 3 - both */
  const uint32_t stretching;   /* SCL stretching when data is not ready: 0 - disable, 1 - enable */
  const uint32_t general_call; /* responce on general call: 0 - disable, 1 - enable */
  const uint32_t addr_size;    /* I2C_ADDFORMAT_7BITS or I2C_ADDFORMAT_10BITS */
  const uint32_t addr;         /* address in slave mode */
} hdl_i2c_hw_t;
#endif

typedef enum {
  HDL_I2C_MESSAGE_READ           = 0x01, /* Receiver mode if option set, else trasmitter mode */
  HDL_I2C_MESSAGE_SEND_START     = 0x02, /* Generate start condition */
  HDL_I2C_MESSAGE_SEND_READ_NACK = 0x04, /* Send NACK at the end in receiver mode */
  HDL_I2C_MESSAGE_SEND_STOP      = 0x08, /* Send NACK at the end in receiver mode */
} hdl_i2c_message_options_t;

typedef struct {
  LinkedListItem_t __ll;
  uint32_t address;
  uint8_t *data;
  uint32_t length;
  hdl_i2c_message_options_t options;
} hdl_i2c_message_t;

typedef hdl_i2c_message_t *hdl_i2c_message_list_t;

typedef struct {
  LinkedListItem_t __ll;
  hdl_i2c_message_list_t message_list;
} hdl_i2c_transaction_t;

typedef struct {
  //hdl_callback_t *slave_receiver_cb;
  //hdl_callback_t *slave_transmitter_cb;
  void *context;
  /* private */
  hdl_i2c_hw_t hw;
  hdl_i2c_transaction_t *master_transaction_queue;
} hdl_i2c_t;


void hdl_i2c_init(hdl_i2c_t *desc, const hdl_i2c_hw_t *hw_conf);
void hdl_i2c_work(hdl_i2c_t *desc);
void hdl_i2c_deinit(hdl_i2c_t *desc);

void hdl_i2c_client_new_transaction(hdl_i2c_transaction_t *transaction);
void hdl_i2c_client_transaction_add_message(hdl_i2c_transaction_t *transaction, hdl_i2c_message_t *message);
void hdl_i2c_enqueue_transaction(hdl_i2c_t *desc, hdl_i2c_transaction_t *transaction);

#endif /* HDL_I2C_H_ */
