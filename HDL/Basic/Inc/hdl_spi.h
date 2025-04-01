#ifndef HDL_SPI_H_
#define HDL_SPI_H_

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
  hdl_module_initializer_t init;
  hdl_set_transceiver_t transceiver_set;
} hdl_spi_server_iface_t;

hdl_module_new_t(hdl_spi_server_t, 0, void, hdl_spi_server_iface_t);

__STATIC_INLINE uint8_t hdl_spi_server_transceiver_set(const void *desc, hdl_transceiver_t *transceiver, uint32_t channel_id) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_spi_server_iface_t *)((hdl_module_base_t *)desc)->iface)->transceiver_set(desc, transceiver, channel_id);
}

/**************** vvv  SPI slave DMA vvv  ******************/

typedef void (*hdl_spi_server_dma_set_handler_t)(const void *desc, hdl_delegate_t *delegate);
typedef uint8_t (*hdl_spi_server_dma_set_buffer_t)(const void *desc, hdl_basic_buffer_t *buffer);

typedef struct {
  hdl_module_initializer_t init;
  hdl_spi_server_dma_set_handler_t subscribe;
  hdl_spi_server_dma_set_buffer_t set_rx_buf;
  hdl_spi_server_dma_set_buffer_t set_tx_buf;
} hdl_spi_server_dma_iface_t;

hdl_module_new_t(hdl_spi_server_dma_t, 0, void, hdl_spi_server_dma_iface_t);

__STATIC_INLINE void hdl_spi_server_dma_subscribe(const void *desc, hdl_delegate_t *delegate) {
  MODULE_ASSERT(desc, );
  ((hdl_spi_server_dma_iface_t *)((hdl_module_base_t *)desc)->iface)->subscribe(desc, delegate);
}

__STATIC_INLINE uint8_t hdl_spi_server_dma_set_rx_buffer(const void *desc, hdl_basic_buffer_t *buffer) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_spi_server_dma_iface_t *)((hdl_module_base_t *)desc)->iface)->set_rx_buf(desc, buffer);
}

__STATIC_INLINE uint8_t hdl_spi_server_dma_set_tx_data(const void *desc, hdl_basic_buffer_t *buffer) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_spi_server_dma_iface_t *)((hdl_module_base_t *)desc)->iface)->set_tx_buf(desc, buffer);
}

/**************** vvv  SPI master vvv  ******************/

typedef uint8_t (* hdl_spi_transfer_message_t)(const void *desc, hdl_spi_message_t *message);

typedef struct {
  hdl_module_initializer_t init;
  hdl_spi_transfer_message_t transfer;
} hdl_spi_client_ch_iface_t;

hdl_module_new_t(hdl_spi_client_ch_t, 0, void, hdl_spi_client_ch_iface_t);

__STATIC_INLINE uint8_t hdl_spi_transfer_message(const void *desc, hdl_spi_message_t *message) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_spi_client_ch_t *)desc)->iface->transfer(desc, message);
}

#endif /* HDL_SPI_H_ */
