#include "hdl_portable.h"
#include "CodeLib.h"


#define SPI_MEM_FLAGS_RX_BUFFER_READY    (uint32_t)(1)
#define SPI_MEM_FLAGS_SWITCH_TX_REQUEST  (uint32_t)(2)

typedef struct {
  hdl_module_t module;
  hdl_spi_server_config_t *config;
  hdl_nvic_irq_n_t spi_iterrupt;
  hdl_nvic_irq_n_t nss_iterrupt;
  /* private */
  __linked_list_object__;
  hdl_basic_buffer_t *rx_mem;
  uint32_t received;
  hdl_delegate_t spi_isr;
  hdl_delegate_t nss_isr;
  hdl_event_handler_t spi_cb;
  void *context;
} hdl_spi_server_dma_private_t;

_Static_assert(sizeof(hdl_spi_server_dma_private_t) == sizeof(hdl_spi_server_dma_t), "In port_spi.h data structure size of hdl_spi_mem_server_t doesn't match, check SPI_SERVER_DMA_PRIVATE_SIZE");

static void event_spi_nss(uint32_t event, void *sender, void *context) {
  hdl_spi_server_dma_private_t *spi = (hdl_spi_server_dma_private_t*)context;
  hdl_gpio_pin_t *nss = (hdl_gpio_pin_t *)spi->module.dependencies[3];
  if((event & (uint32_t)nss->module.reg) && (hdl_gpio_read(nss) == nss->inactive_default)) {
    if(spi->rx_mem) {
      hdl_dma_channel_t *dma_rx = (hdl_dma_channel_t *)spi->module.dependencies[6];
      spi->received = (spi->rx_mem->size - hdl_dma_get_counter(dma_rx));
    }
  }
}

static void event_spi_isr(uint32_t event, void *sender, void *context) {
  hdl_spi_server_dma_private_t *spi = (hdl_spi_server_dma_private_t *)context;
  uint32_t state = SPI_STAT((uint32_t)spi->module.reg);
  uint32_t cr1 = SPI_CTL1((uint32_t)spi->module.reg);
  if ((state & SPI_ERROR_MASK) != 0) {
    hdl_spi_reset_status((uint32_t)spi->module.reg);
  }
}

void hdl_spi_server_dma_set_handler(hdl_spi_server_dma_t *desc, hdl_event_handler_t handler, void *context) {
  hdl_spi_server_dma_private_t *spi = (hdl_spi_server_dma_private_t*)desc;
  if(desc != NULL) {
    spi->spi_cb = handler;
    spi->context = context;
  }
}

static void _spi_reset_dma() {

}


uint8_t hdl_spi_server_dma_set_rx_buffer(hdl_spi_server_dma_t *desc, hdl_basic_buffer_t *buffer) {
  hdl_spi_server_dma_private_t *spi = (hdl_spi_server_dma_private_t*)desc;
  if((desc != NULL) && (hdl_state(&desc->module) == HDL_MODULE_INIT_OK)) {
    hdl_dma_channel_t *dma_rx = (hdl_dma_channel_t *)spi->module.dependencies[6];
    spi->rx_mem = buffer;
    if(buffer != NULL) {
      hdl_dma_run(dma_rx, (uint32_t)&SPI_DATA((uint32_t)spi->module.reg), (uint32_t)buffer->data , (uint32_t)buffer->size);
    }
    else {
      hdl_dma_stop(dma_rx);
    }
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

uint8_t hdl_spi_server_dma_set_tx_data(hdl_spi_server_dma_t *desc, hdl_basic_buffer_t *buffer) {
  hdl_spi_server_dma_private_t *spi = (hdl_spi_server_dma_private_t*)desc;
  if((desc != NULL) && (hdl_state(&desc->module) == HDL_MODULE_INIT_OK)) {
    hdl_dma_channel_t *dma_tx = (hdl_dma_channel_t *)spi->module.dependencies[7];
    if(buffer != NULL) {
      hdl_dma_run(dma_tx, (uint32_t)&SPI_DATA((uint32_t)spi->module.reg), (uint32_t)buffer->data , (uint32_t)buffer->size);
    }
    else {
      hdl_dma_stop(dma_tx);
    }
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static void _spi_server_dma_handler(linked_list_item_t *spi_item, void *arg) {
  hdl_spi_server_dma_private_t *spi = linked_list_get_object(hdl_spi_server_dma_private_t, spi_item);
  if(spi->received != 0) {
    if(spi->spi_cb != NULL) {
      spi->spi_cb(spi->received, spi, spi->context);
    }
    spi->received = 0;
    if(spi->rx_mem != NULL) {
      hdl_dma_channel_t *dma_rx = (hdl_dma_channel_t *)spi->module.dependencies[6];
      hdl_dma_run(dma_rx, (uint32_t)&SPI_DATA((uint32_t)spi->module.reg), (uint32_t)spi->rx_mem->data , (uint32_t)spi->rx_mem->size);
    }
  }
}

static uint8_t _spi_server_dma_worker(coroutine_desc_t this, uint8_t cancel, void *arg) {
  linked_list_t spis = (linked_list_t)arg;
  linked_list_do_foreach(spis, &_spi_server_dma_handler, NULL);
  return cancel;
}

hdl_module_state_t hdl_spi_server_dma(void *desc, uint8_t enable) {
  static coroutine_desc_static_t spi_client_task_buff;
  static linked_list_t spis = NULL;
  hdl_spi_server_dma_private_t *spi = (hdl_spi_server_dma_private_t*)desc;
  rcu_periph_enum rcu;
  switch ((uint32_t)spi->module.reg) {
    case SPI0: rcu = RCU_SPI0; break;
    case SPI1: rcu = RCU_SPI1; break;
    case SPI2: rcu = RCU_SPI2; break;
    default: return HDL_MODULE_INIT_FAILED;
  }
  spi_i2s_deinit((uint32_t)spi->module.reg);
  if(enable) {
    rcu_periph_clock_enable(rcu);
    spi_parameter_struct init;
    init.device_mode = SPI_SLAVE;
    init.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    init.frame_size = SPI_FRAMESIZE_8BIT;
    init.endian = spi->config->endian;
    init.clock_polarity_phase = spi->config->polarity;
    init.nss = SPI_NSS_HARD;
    init.prescale = HDL_SPI_PSC_2;
    spi_init((uint32_t)spi->module.reg, &init);
    SPI_CTL1((uint32_t)spi->module.reg) |= SPI_CTL1_ERRIE;
    spi->nss_isr.context = desc;
    spi->nss_isr.handler = &event_spi_nss;
    spi->spi_isr.context = desc;
    spi->spi_isr.handler = &event_spi_isr;
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)spi->module.dependencies[5];
    hdl_interrupt_request(ic, spi->spi_iterrupt, &spi->spi_isr);
    hdl_interrupt_request(ic, spi->nss_iterrupt, &spi->nss_isr);
    spi->received = 0;
    SPI_CTL1((uint32_t)spi->module.reg) |= SPI_CTL1_DMATEN | SPI_CTL1_DMAREN;
    spi_enable((uint32_t)spi->module.reg);
    linked_list_insert_last(&spis, linked_list_item(spi));
    coroutine_add_static(&spi_client_task_buff, &_spi_server_dma_worker, (void *)spis);
    return HDL_MODULE_INIT_OK;
  }
  linked_list_unlink(linked_list_item(spi));
  rcu_periph_clock_disable(rcu);
  return HDL_MODULE_DEINIT_OK;
}
