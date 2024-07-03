#ifndef HDL_SPI_H_
#define HDL_SPI_H_

#include "port_spi.h"
#include "hdl_transceiver.h"

/* Don`t change order */
typedef enum {
  HDL_SPI_TRANSACTION_UNKNOWN     = 0x00,
  HDL_SPI_TRANSACTION_EXECUTED    = 0x01,
  HDL_SPI_TRANSACTION_FAILED      = 0x02,
  HDL_SPI_TRANSACTION_TERMINATED  = 0x03,
  HDL_SPI_TRANSACTION_TERMINATING = 0x04,
  HDL_SPI_TRANSACTION_ENQUEUED    = 0x05,
  HDL_SPI_TRANSACTION_EXECUTING   = 0x06,
} hdl_spi_message_state_t;

typedef struct {
  uint8_t *tx_buffer;
  uint8_t *rx_buffer;
  uint32_t tx_len;
  uint32_t rx_skip;
  uint32_t rx_take;
  PRIVATE(hdl, HDL_SPI_MESSAGE_PRV_SIZE);
} hdl_spi_message_t;

/**************** vvv  SPI slave vvv  ******************/
hdl_module_state_t hdl_spi_server(void *desc, uint8_t enable);
void hdl_spi_server_set_transceiver(hdl_spi_server_t *spi, hdl_transceiver_t *transceiver);

/**************** vvv  SPI master vvv  ******************/
hdl_module_state_t hdl_spi_client(void *desc, uint8_t enable);
hdl_module_state_t hdl_spi_ch(void *desc, uint8_t enable);
hdl_spi_message_state_t hdl_spi_client_xfer(hdl_spi_client_ch_t *spi_ch, hdl_spi_message_t *message);
hdl_spi_message_state_t hdl_spi_message_get_state(hdl_spi_message_t *message);

/**************** vvv  SPI slave DMA vvv  ******************/
hdl_module_state_t hdl_spi_server_dma(void *desc, uint8_t enable);
void hdl_spi_server_dma_set_handler(hdl_spi_server_dma_t *desc, hdl_event_handler_t handler, void *context);
uint8_t hdl_spi_server_dma_set_rx_buffer(hdl_spi_server_dma_t *desc, hdl_basic_buffer_t *buffer);
uint8_t hdl_spi_server_dma_set_tx_data(hdl_spi_server_dma_t *desc, hdl_basic_buffer_t *buffer);


#endif /* HDL_SPI_H_ */
