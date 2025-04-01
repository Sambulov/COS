#include "hdl_iface.h"

#define SPI_MEM_FLAGS_RX_BUFFER_READY    (uint32_t)(1)
#define SPI_MEM_FLAGS_SWITCH_TX_REQUEST  (uint32_t)(2)

typedef struct {
  coroutine_t worker;
  hdl_basic_buffer_t *rx_mem;
  uint32_t received;
  hdl_delegate_t spi_isr;
  hdl_delegate_t nss_isr;
  hdl_event_t event;
} hdl_spi_server_dma_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_spi_server_dma_var_t, *((hdl_spi_server_dma_mcu_t *)0)->obj_var, HDL_SPI_SERVER_DMA_VAR_SIZE, port_spi.h);

static void _hdl_spi_mem_full_reset(hdl_spi_server_dma_mcu_t *spi) {
  hdl_dma_channel_t *dma_rx = (hdl_dma_channel_t *)spi->dependencies[6];
  hdl_dma_channel_t *dma_tx = (hdl_dma_channel_t *)spi->dependencies[7];
  spi_i2s_deinit(spi->config->phy);
  hdl_dma_channel_stop(dma_rx);
  hdl_dma_channel_stop(dma_tx);
  spi_parameter_struct init;
  init.device_mode = SPI_SLAVE;
  init.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
  init.frame_size = SPI_FRAMESIZE_8BIT;
  init.endian = spi->config->endian;
  init.clock_polarity_phase = spi->config->polarity;
  init.nss = SPI_NSS_HARD;
  init.prescale = HDL_SPI_PSC_2;
  spi_init(spi->config->phy, &init);
  SPI_CTL1(spi->config->phy) |= SPI_CTL1_ERRIE;
  SPI_CTL1(spi->config->phy) |= SPI_CTL1_DMATEN | SPI_CTL1_DMAREN;
}

static void event_spi_nss(uint32_t event, void *sender, void *context) {
  (void)sender;
  hdl_spi_server_dma_mcu_t *spi = (hdl_spi_server_dma_mcu_t*)context;
  hdl_spi_server_dma_var_t *spi_var = (hdl_spi_server_dma_var_t *)spi->obj_var;
  hdl_gpio_pin_t *nss = (hdl_gpio_pin_t *)spi->dependencies[3];
  if((event & (uint32_t)nss->config->pin) && hdl_gpio_is_inactive(nss)) {

    if(spi_var->rx_mem) {
      hdl_dma_channel_t *dma_rx = (hdl_dma_channel_t *)spi->dependencies[6];
      spi_var->received = (spi_var->rx_mem->size - hdl_dma_channel_get_counter(dma_rx));
    }
  }
}

static void event_spi_isr(uint32_t event, void *sender, void *context) {
  (void)event; (void)sender;
  hdl_spi_server_dma_mcu_t *spi = (hdl_spi_server_dma_mcu_t *)context;
  uint32_t state = SPI_STAT(spi->config->phy);
  uint32_t cr1 = SPI_CTL1(spi->config->phy);
  (void)cr1;
  if ((state & SPI_ERROR_MASK) != 0) {
    hdl_spi_reset_status(spi->config->phy);
  }
}

static void _hdl_spi_server_dma_subscribe(const void *desc, hdl_delegate_t *delegate) {
  hdl_spi_server_dma_mcu_t *spi = (hdl_spi_server_dma_mcu_t*)desc;
  hdl_spi_server_dma_var_t *spi_var = (hdl_spi_server_dma_var_t *)spi->obj_var;
  if(desc != NULL) 
    hdl_event_subscribe(&spi_var->event, delegate);
}

static uint8_t _hdl_spi_server_dma_set_rx_buffer(const void *desc, hdl_basic_buffer_t *buffer) {
  hdl_spi_server_dma_mcu_t *spi = (hdl_spi_server_dma_mcu_t*)desc;
  hdl_spi_server_dma_var_t *spi_var = (hdl_spi_server_dma_var_t *)spi->obj_var;
  if((desc != NULL) && (hdl_state(desc) == HDL_MODULE_ACTIVE)) {
    hdl_dma_channel_t *dma_rx = (hdl_dma_channel_t *)spi->dependencies[6];
    spi_var->rx_mem = buffer;
    if(buffer != NULL) {
      hdl_dma_channel_run(dma_rx, (uint32_t)&SPI_DATA(spi->config->phy), (uint32_t)buffer->data , (uint32_t)buffer->size);
    }
    else {
      hdl_dma_channel_stop(dma_rx);
    }
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static uint8_t _hdl_spi_server_dma_set_tx_data(const void *desc, hdl_basic_buffer_t *buffer) {
  hdl_spi_server_dma_mcu_t *spi = (hdl_spi_server_dma_mcu_t*)desc;
  if((desc != NULL) && (hdl_state(desc) == HDL_MODULE_ACTIVE)) {
    hdl_dma_channel_t *dma_tx = (hdl_dma_channel_t *)spi->dependencies[7];
    if(buffer != NULL) {
      hdl_dma_channel_run(dma_tx, (uint32_t)&SPI_DATA(spi->config->phy), (uint32_t)buffer->data , (uint32_t)buffer->size);
    }
    else {
      hdl_dma_channel_stop(dma_tx);
    }
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static uint8_t _spi_server_dma_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_spi_server_dma_mcu_t *spi = (hdl_spi_server_dma_mcu_t *) arg;
  hdl_spi_server_dma_var_t *spi_var = (hdl_spi_server_dma_var_t *)spi->obj_var;
  if(spi_var->received != 0) {
    _hdl_spi_mem_full_reset(spi);
    hdl_event_raise(&spi_var->event, spi, spi_var->received);
    spi_var->received = 0;
    if(spi_var->rx_mem != NULL) {
      hdl_dma_channel_t *dma_rx = (hdl_dma_channel_t *)spi->dependencies[6];
      hdl_dma_channel_run(dma_rx, (uint32_t)&SPI_DATA(spi->config->phy), (uint32_t)spi_var->rx_mem->data , (uint32_t)spi_var->rx_mem->size);
    }
    spi_enable(spi->config->phy);
  }
  return cancel;
}

static hdl_module_state_t _hdl_spi_server_dma(const void *desc, uint8_t enable) {
  hdl_spi_server_dma_mcu_t *spi = (hdl_spi_server_dma_mcu_t*)desc;
  hdl_spi_server_dma_var_t *spi_var = (hdl_spi_server_dma_var_t *)spi->obj_var;
  spi_i2s_deinit(spi->config->phy);
  if(enable) {
    rcu_periph_clock_enable(spi->config->rcu);
    spi_parameter_struct init;
    init.device_mode = SPI_SLAVE;
    init.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    init.frame_size = SPI_FRAMESIZE_8BIT;
    init.endian = spi->config->endian;
    init.clock_polarity_phase = spi->config->polarity;
    init.nss = SPI_NSS_HARD;
    init.prescale = HDL_SPI_PSC_2;
    spi_init(spi->config->phy, &init);
    SPI_CTL1(spi->config->phy) |= SPI_CTL1_ERRIE;
    spi_var->nss_isr.context = spi;
    spi_var->nss_isr.handler = &event_spi_nss;
    spi_var->spi_isr.context = spi;
    spi_var->spi_isr.handler = &event_spi_isr;
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)spi->dependencies[5];
    hdl_event_subscribe(&spi->config->spi_interrupt->event, &spi_var->spi_isr);
    hdl_interrupt_request(ic, spi->config->spi_interrupt);
    hdl_event_subscribe(&spi->config->nss_interrupt->event, &spi_var->nss_isr);
    hdl_interrupt_request(ic, spi->config->nss_interrupt);
    spi_var->received = 0;
    SPI_CTL1(spi->config->phy) |= SPI_CTL1_DMATEN | SPI_CTL1_DMAREN;
    spi_enable(spi->config->phy);
    coroutine_add(&spi_var->worker, &_spi_server_dma_worker, spi);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&spi_var->worker);
  rcu_periph_clock_disable(spi->config->rcu);
  return HDL_MODULE_UNLOADED;
}

const hdl_spi_server_dma_iface_t hdl_spi_server_dma_iface = {
  .init = &_hdl_spi_server_dma,
  .subscribe = &_hdl_spi_server_dma_subscribe,
  .set_rx_buf = &_hdl_spi_server_dma_set_rx_buffer,
  .set_tx_buf = &_hdl_spi_server_dma_set_tx_data
};



