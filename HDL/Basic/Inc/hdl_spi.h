#ifndef HDL_SPI_H_
#define HDL_SPI_H_

//#include "port_spi.h"

#define HDL_SPI_MESSAGE_PRV_SIZE           16

typedef enum {
  HDL_SPI_TRANSACTION_UNKNOWN     = 0x00,
  HDL_SPI_TRANSACTION_NEW         = 0x01,
  HDL_SPI_TRANSACTION_ENQUEUED    = 0x02,
  HDL_SPI_TRANSACTION_EXECUTING   = 0x03,
  HDL_SPI_TRANSACTION_EXECUTED    = 0x04,
  HDL_SPI_TRANSACTION_TERMINATING = 0x05,
  HDL_SPI_TRANSACTION_TERMINATED  = 0x06,
  HDL_SPI_TRANSACTION_FAILED      = 0x07,
} hdl_spi_message_state_t;

typedef struct {
  hdl_module_t module;
} hdl_spi_t;

typedef struct {
  hdl_module_t module;
} hdl_spi_channel_t;

typedef struct {
  uint8_t __private[HDL_SPI_MESSAGE_PRV_SIZE];
  uint32_t address;
  uint8_t *tx_buffer;
  uint8_t *rx_buffer;
  uint32_t rx_skip;
  uint32_t message_len;
} hdl_spi_message_t;

/* Initialization */
hdl_module_state_t hdl_spi(void *desc, uint8_t enable);
hdl_module_state_t hdl_spi_channel(void *desc, uint8_t enable);

void hdl_spi_client_enqueue_message(hdl_spi_channel_t *spi_ch, hdl_spi_message_t *message);

#endif /* HDL_SPI_H_ */