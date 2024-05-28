#include "hdl_portable.h"
#include "CodeLib.h"


typedef struct {
  hdl_module_t module;
  hdl_spi_server_config_t *config;
  hdl_nvic_irq_n_t spi_iterrupt;
  hdl_nvic_irq_n_t nss_iterrupt;
  hdl_spi_server_memory_t rx_mem;
  hdl_spi_server_memory_t tx_mem;
  /* private */
  hdl_delegate_t spi_isr;
  hdl_delegate_t nss_isr;
  __linked_list_object__;
} hdl_spi_mem_server_private_t;

_Static_assert(sizeof(hdl_spi_mem_server_private_t) == sizeof(hdl_spi_mem_server_t), "In port_spi.h data structure size of hdl_spi_mem_server_t doesn't match, check SPI_MEM_SERVER_PRIVATE_SIZE");


// static void _spi_handler(linked_list_item_t *spi_item, void *arg) {

// }

// static uint8_t _spi_worker(coroutine_desc_t this, uint8_t cancel, void *arg) {
//   linked_list_t spis = (linked_list_t)arg;
//   linked_list_do_foreach(spis, &_spi_handler, NULL);
//   return cancel;
// }

static void _rst_spi_status (hdl_spi_server_private_t *spi) {
  __IO uint32_t tmpreg = SPI_DATA((uint32_t)spi->module.reg);
	tmpreg = SPI_STAT((uint32_t)spi->module.reg);
  SPI_CTL0((uint32_t)spi->module.reg) = SPI_CTL0((uint32_t)spi->module.reg);
  SPI_STAT((uint32_t)spi->module.reg) &= ~(SPI_STAT_CRCERR | SPI_STAT_FERR);
	(void)tmpreg;
}

static void event_spi_nss(uint32_t event, void *sender, void *context) {
  hdl_spi_server_private_t *spi = (hdl_spi_server_private_t*)context;
  hdl_gpio_pin_t *nss = (hdl_gpio_pin_t *)spi->module.dependencies[3];
  if(event & (uint32_t)nss->module.reg){
    if((spi->transceiver != NULL) && (spi->transceiver->end_of_transmission != NULL))
      spi->transceiver->end_of_transmission(spi->transceiver->proto_context);
  }
}

static void event_spi_isr(uint32_t event, void *sender, void *context) {
hdl_spi_server_private_t *spi = (hdl_spi_server_private_t *)context;
  uint32_t state = SPI_STAT((uint32_t)spi->module.reg);
  uint32_t cr1 = SPI_CTL1((uint32_t)spi->module.reg);
	if ((state & StateMask) == NoError) {
		/* spi in mode Receiver ---------------------------------------------------*/
		if (state & SPI_STAT_RBNE) {
			uint16_t data = SPI_DATA((uint32_t)spi->module.reg);
      if((spi->transceiver != NULL) && (spi->transceiver->rx_data != NULL)) {  
          spi->transceiver->rx_data(spi->transceiver->proto_context, (uint8_t *)&data, 1);
      }
		}
		///* spi in mode Transmitter ------------------------------------------------*/
		if ((state & SPI_STAT_TBE) && (cr1 & SPI_CTL1_TBEIE)) {
      uint16_t data = 0;
      if((spi->transceiver != NULL) && (spi->transceiver->tx_empty != NULL)) {
        spi->transceiver->tx_empty(spi->transceiver->proto_context, (uint8_t *)&data, 1);
      }
      SPI_DATA((uint32_t)spi->module.reg) = data;
		}
	}
	else {
		_rst_spi_status(spi);
	}
}

hdl_module_state_t hdl_spi_server(void *desc, uint8_t enable) {
  static coroutine_desc_static_t spi_task_buf;
  //static linked_list_t spis = NULL;
  hdl_spi_mem_server_private_t *spi = (hdl_spi_mem_server_private_t*)desc;
  rcu_periph_enum rcu;
  switch ((uint32_t)spi->module.reg) {
    case SPI0: rcu = RCU_SPI0; break;
    case SPI1: rcu = RCU_SPI1; break;
    default: return HDL_MODULE_INIT_FAILED;
  }
  spi_i2s_deinit((uint32_t)spi->module.reg);
  if(enable) {
    rcu_periph_clock_enable(rcu);
    //linked_list_insert_last(&spis, linked_list_item(spi));
    //coroutine_add_static(&spi_task_buf, &_spi_worker, (void *)spis);
    spi_parameter_struct init;
    init.device_mode = SPI_SLAVE;
    init.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    init.frame_size = SPI_FRAMESIZE_8BIT;
    init.endian = spi->config->endian;
    init.clock_polarity_phase = spi->config->polarity;
    init.nss = SPI_NSS_HARD;
    init.prescale = spi->config->prescale;
    spi_init((uint32_t)spi->module.reg, &init);
    SPI_CTL1((uint32_t)spi->module.reg) |= SPI_CTL1_RBNEIE | SPI_CTL1_TBEIE | SPI_CTL1_ERRIE;
    //SPI_CTL0((uint32_t)spi->module.reg) |= SPI_CTL0_SWNSS; 

    hdl_dma_channel_t *dma_rx = (hdl_dma_channel_t *)hdl_adc->module.dependencies[6];
    hdl_dma_channel_t *dma_tx = (hdl_dma_channel_t *)hdl_adc->module.dependencies[7];

    spi->nss_isr.context = desc;
    spi->nss_isr.handler = &event_spi_nss;
    spi->spi_isr.context = desc;
    spi->spi_isr.handler = &event_spi_isr;
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)spi->module.dependencies[5];
    hdl_interrupt_request(ic, spi->spi_iterrupt, &spi->spi_isr);
    hdl_interrupt_request(ic, spi->nss_iterrupt, &spi->nss_isr);
    spi_enable((uint32_t)spi->module.reg);

    return HDL_MODULE_INIT_OK;
  }
  linked_list_unlink(linked_list_item(spi));
  rcu_periph_clock_disable(rcu);
  return HDL_MODULE_DEINIT_OK;
}
