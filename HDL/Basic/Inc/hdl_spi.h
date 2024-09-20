#ifndef HDL_SPI_H_
#define HDL_SPI_H_

#include "port_spi.h"
#include "hdl_transceiver.h"

/* Don`t change order */
typedef enum {
  HDL_SPI_MESSAGE_STATUS_INITIAL           = 0x00,
  HDL_SPI_MESSAGE_STATUS_BUS_HOLD          = 0x01,
  HDL_SPI_MESSAGE_STATUS_XFER              = 0x02,
  HDL_SPI_MESSAGE_STATUS_XFER_COMPLETE     = 0x04,
  HDL_SPI_MESSAGE_STATUS_BUS_RELEASE       = 0x08,
  HDL_SPI_MESSAGE_FAULT_BUS_ERROR          = 0x10,
  HDL_SPI_MESSAGE_STATUS_COMPLETE          = 0x80
} hdl_spi_message_status_t;

typedef enum {
  HDL_SPI_MESSAGE_CH_SELECT                = 0x01,
  HDL_SPI_MESSAGE_CH_RELEASE               = 0x02,
} hdl_spi_message_options_t;

typedef struct {
  uint8_t *tx_buffer;
  uint8_t *rx_buffer;
  uint16_t tx_len;
  uint16_t rx_skip;
  uint16_t rx_take;
  uint16_t transferred;
  hdl_spi_message_status_t state;
  hdl_spi_message_options_t options;
} hdl_spi_message_t;

/**************** vvv  SPI slave vvv  ******************/
typedef struct {
  hdl_module_t module;
  hdl_spi_server_config_t *config;
  PRIVATE(hw, SPI_SERVER_PRIVATE_SIZE);
} hdl_spi_server_t;

hdl_module_state_t hdl_spi_server(void *desc, uint8_t enable);
void hdl_spi_server_set_transceiver(hdl_spi_server_t *spi, hdl_transceiver_t *transceiver);

/**************** vvv  SPI master vvv  ******************/
typedef struct {
  hdl_module_t module;
  const hdl_spi_client_ch_config_t *config;
  PRIVATE(hw, HDl_SPI_CLIENT_CH_PRIVATE_SIZE);
} hdl_spi_client_ch_t;

typedef struct {
  hdl_module_t module;
  const hdl_spi_client_config_t *config;
  PRIVATE(hw, HDl_SPI_CLIENT_PRIVATE_SIZE);
} hdl_spi_client_t;

hdl_module_state_t hdl_spi_client(void *desc, uint8_t enable);
hdl_module_state_t hdl_spi_ch(void *desc, uint8_t enable);
uint8_t hdl_spi_transfer_message(hdl_spi_client_ch_t *spi_ch, hdl_spi_message_t *message);

/**************** vvv  SPI slave DMA vvv  ******************/
typedef struct {
  hdl_module_t module;
  hdl_spi_server_config_t *config;
  PRIVATE(hw, SPI_SERVER_DMA_PRIVATE_SIZE);
} hdl_spi_server_dma_t;

hdl_module_state_t hdl_spi_server_dma(void *desc, uint8_t enable);
void hdl_spi_server_dma_set_handler(hdl_spi_server_dma_t *desc, hdl_event_handler_t handler, void *context);
uint8_t hdl_spi_server_dma_set_rx_buffer(hdl_spi_server_dma_t *desc, hdl_basic_buffer_t *buffer);
uint8_t hdl_spi_server_dma_set_tx_data(hdl_spi_server_dma_t *desc, hdl_basic_buffer_t *buffer);

#endif /* HDL_SPI_H_ */
