#include "hdl_portable.h"
#include "CodeLib.h"

#define SPI_MEM_FLAGS_RX_BUFFER_READY    (uint32_t)(1)
#define SPI_MEM_FLAGS_SWITCH_TX_REQUEST  (uint32_t)(2)

typedef struct {
  hdl_module_t module;
  hdl_spi_server_config_t *config;
  hdl_nvic_irq_n_t spi_iterrupt;
  hdl_nvic_irq_n_t nss_iterrupt;
  hdl_double_buffer_t *rx_mem;
  hdl_double_buffer_t *tx_mem;
  /* private */
  uint32_t flags;
  hdl_delegate_t spi_isr;
  hdl_delegate_t nss_isr;
  uint32_t xfer_epoch;
} hdl_spi_mem_server_private_t;

_Static_assert(sizeof(hdl_spi_mem_server_private_t) <= sizeof(hdl_spi_mem_server_t), "In port_spi.h data structure size of hdl_spi_mem_server_t doesn't match, check SPI_MEM_SERVER_PRIVATE_SIZE");

static void _hdl_spi_mem_full_reset(hdl_spi_mem_server_private_t *spi) {
  spi_i2s_deinit((uint32_t)spi->module.reg);
  spi_parameter_struct init;
  init.device_mode = SPI_SLAVE;
  init.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
  init.frame_size = SPI_FRAMESIZE_8BIT;
  init.endian = spi->config->endian;
  init.clock_polarity_phase = spi->config->polarity;
  init.prescale = HDL_SPI_PSC_2;
  init.nss = SPI_NSS_HARD;
  spi_init((uint32_t)spi->module.reg, &init);
  SPI_DATA((uint32_t)spi->module.reg) = 0xAA;
  SPI_CTL1((uint32_t)spi->module.reg) |= SPI_CTL1_ERRIE;
  hdl_dma_run(
    (hdl_dma_channel_t *)spi->module.dependencies[6], 
    (uint32_t)&SPI_DATA((uint32_t)spi->module.reg), 
    (uint32_t)spi->rx_mem->data[spi->rx_mem->active_buffer_number], 
    (uint32_t)spi->rx_mem->size);
  hdl_dma_run((hdl_dma_channel_t *)spi->module.dependencies[7], 
    (uint32_t)&SPI_DATA((uint32_t)spi->module.reg), 
    (uint32_t)spi->tx_mem->data[spi->tx_mem->active_buffer_number], 
    (uint32_t)spi->tx_mem->size);
  SPI_CTL1((uint32_t)spi->module.reg) |= (SPI_CTL1_DMATEN | SPI_CTL1_DMAREN);
  spi_enable((uint32_t)spi->module.reg);
}

static void _spi_mem_transaction_complete(hdl_spi_mem_server_private_t *spi) {
  hdl_dma_channel_t *dma_rx = (hdl_dma_channel_t *)spi->module.dependencies[6];
  hdl_dma_channel_t *dma_tx = (hdl_dma_channel_t *)spi->module.dependencies[7];
  hdl_spi_reset_status((uint32_t)spi->module.reg);
  /* TX update*/
  if(spi->flags & SPI_MEM_FLAGS_SWITCH_TX_REQUEST) {
    hdl_double_buffer_switch(spi->tx_mem);
    spi->flags &= ~SPI_MEM_FLAGS_SWITCH_TX_REQUEST;
  }
  /* RX update*/
  if((hdl_dma_get_counter(dma_rx) == 0)) {
    hdl_timer_t *timer = (hdl_timer_t *)spi->module.dependencies[9];
    spi->flags |= SPI_MEM_FLAGS_RX_BUFFER_READY;
    spi->xfer_epoch = hdl_timer_get(timer);
    hdl_double_buffer_switch(spi->rx_mem);
    hdl_dma_run(dma_rx, (uint32_t)&SPI_DATA((uint32_t)spi->module.reg), (uint32_t)spi->rx_mem->data[spi->rx_mem->active_buffer_number], (uint32_t)spi->rx_mem->size);
    hdl_dma_run(dma_tx, (uint32_t)&SPI_DATA((uint32_t)spi->module.reg), (uint32_t)spi->tx_mem->data[spi->tx_mem->active_buffer_number], (uint32_t)spi->tx_mem->size);
  }
  else {
    _hdl_spi_mem_full_reset(spi);
  }
}

static void event_spi_nss(uint32_t event, void *sender, void *context) {
  hdl_spi_mem_server_private_t *spi = (hdl_spi_mem_server_private_t*)context;
  hdl_gpio_pin_t *nss = (hdl_gpio_pin_t *)spi->module.dependencies[3];
  if((event & (uint32_t)nss->module.reg) && hdl_gpio_is_inactive_default(nss)) {
    _spi_mem_transaction_complete(spi);
  }
}

static void event_spi_isr(uint32_t event, void *sender, void *context) {
  hdl_spi_mem_server_private_t *spi = (hdl_spi_mem_server_private_t *)context;
  uint32_t state = SPI_STAT((uint32_t)spi->module.reg);
  uint32_t cr1 = SPI_CTL1((uint32_t)spi->module.reg);
  if ((state & SPI_ERROR_MASK) != 0) {
    hdl_spi_reset_status((uint32_t)spi->module.reg);
  }
}

hdl_module_state_t hdl_spi_mem_server(void *desc, uint8_t enable) {
  hdl_spi_mem_server_private_t *spi = (hdl_spi_mem_server_private_t*)desc;
  rcu_periph_enum rcu;
  switch ((uint32_t)spi->module.reg) {
    case SPI0: rcu = RCU_SPI0; break;
    case SPI1: rcu = RCU_SPI1; break;
    case SPI2: rcu = RCU_SPI2; break;
    case SPI3: rcu = RCU_SPI3; break;
    case SPI4: rcu = RCU_SPI4; break;
    case SPI5: rcu = RCU_SPI5; break;
    default: return HDL_MODULE_INIT_FAILED;
  }
  spi_i2s_deinit((uint32_t)spi->module.reg);
  if(enable) {
    rcu_periph_clock_enable(rcu);
    spi->nss_isr.context = desc;
    spi->nss_isr.handler = &event_spi_nss;
    spi->spi_isr.context = desc;
    spi->spi_isr.handler = &event_spi_isr;
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)spi->module.dependencies[5];
    hdl_interrupt_request(ic, spi->spi_iterrupt, &spi->spi_isr);
    hdl_interrupt_request(ic, spi->nss_iterrupt, &spi->nss_isr);
    spi->flags = 0;
    spi->xfer_epoch = 0;
    _hdl_spi_mem_full_reset(spi);
    return HDL_MODULE_INIT_OK;
  }
  rcu_periph_clock_disable(rcu);
  return HDL_MODULE_DEINIT_OK;
}

uint32_t hdl_spi_mem_buffer_epoch(hdl_spi_mem_server_t *spi) {
  hdl_spi_mem_server_private_t *spi_private = (hdl_spi_mem_server_private_t*)spi;
  return spi_private->xfer_epoch;
}

uint8_t hdl_spi_mem_rx_buffer_take(hdl_spi_mem_server_t *spi, hdl_basic_buffer_t *buffer, uint32_t offset) {
  hdl_spi_mem_server_private_t *spi_private = (hdl_spi_mem_server_private_t*)spi;
  if((spi != NULL) && (buffer != NULL) && (spi->rx_mem->size >= (buffer->size + offset))) {
    if(spi_private->flags & SPI_MEM_FLAGS_RX_BUFFER_READY) {
      hdl_dma_channel_t *dma_mem = (hdl_dma_channel_t *)spi->module.dependencies[8];
      hdl_dma_run(dma_mem, (uint32_t)&spi->rx_mem->data[!spi->rx_mem->active_buffer_number][offset], (uint32_t)buffer->data,  buffer->size);
      while (hdl_dma_get_counter(dma_mem));
      return HDL_TRUE;
    }
  }
  return HDL_FALSE;
}

uint8_t hdl_spi_mem_tx_buffer_put(hdl_spi_mem_server_t *spi, hdl_basic_buffer_t *buffer, uint32_t offset) {
  hdl_spi_mem_server_private_t *spi_private = (hdl_spi_mem_server_private_t*)spi;
  if((spi != NULL) && (buffer != NULL) && (spi->tx_mem->size >= (buffer->size + offset))) {
    hdl_dma_channel_t *dma_mem = (hdl_dma_channel_t *)spi->module.dependencies[8];
    hdl_gpio_pin_t *nss = (hdl_gpio_pin_t *)spi->module.dependencies[3];
    hdl_dma_channel_t *dma_tx = (hdl_dma_channel_t *)spi->module.dependencies[7];
    uint8_t nss_inactive = hdl_gpio_is_inactive_default(nss);
    uint8_t *data = spi->tx_mem->data[!spi->tx_mem->active_buffer_number];
    spi_private->flags &= ~SPI_MEM_FLAGS_SWITCH_TX_REQUEST;
    if(nss_inactive) {
      data = spi->tx_mem->data[spi->tx_mem->active_buffer_number];
      hdl_dma_run(dma_tx, (uint32_t)&SPI_DATA((uint32_t)spi->module.reg), (uint32_t)spi->tx_mem->data[spi->tx_mem->active_buffer_number], (uint32_t)spi->tx_mem->size);
    }
    hdl_dma_run(dma_mem, (uint32_t)buffer->data, (uint32_t)data, buffer->size);
    while (hdl_dma_get_counter(dma_mem));
    if(!nss_inactive) {
      spi_private->flags |= SPI_MEM_FLAGS_SWITCH_TX_REQUEST;
    }
    return HDL_TRUE;
  }
  return HDL_FALSE;
}
