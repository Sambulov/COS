#include "hdl_portable.h"
#include "CodeLib.h"

#define SPI_SERVER_RX_BUFFER_SZ    32
#define SPI_SERVER_TX_BUFFER_SZ    32

typedef struct {
  hdl_module_t module;
  hdl_spi_server_config_t *config;
  hdl_nvic_irq_n_t spi_iterrupt;
  hdl_nvic_irq_n_t nss_iterrupt;
  /* private */
  hdl_delegate_t spi_isr;
  hdl_delegate_t nss_isr;
  hdl_transceiver_t *transceiver;
} hdl_spi_server_private_t;

_Static_assert(sizeof(hdl_spi_server_private_t) == sizeof(hdl_spi_server_t), "In port_spi.h data structure size of hdl_spi_server_t doesn't match, check SPI_SERVER_PRIVATE_SIZE");

static void event_spi_nss(uint32_t event, void *sender, void *context) {
  hdl_spi_server_private_t *spi = (hdl_spi_server_private_t*)context;
  hdl_gpio_pin_t *nss = (hdl_gpio_pin_t *)spi->module.dependencies[3];
  if(event & (uint32_t)nss->module.reg){
    if((spi->transceiver != NULL) && (spi->transceiver->end_of_transmission != NULL))
      spi->transceiver->end_of_transmission(spi->transceiver->proto_context);
  }
}

static void event_spi_isr_server(uint32_t event, void *sender, void *context) {
hdl_spi_server_private_t *spi = (hdl_spi_server_private_t *)context;
  uint32_t state = SPI_STAT((uint32_t)spi->module.reg);
  uint32_t cr1 = SPI_CTL1((uint32_t)spi->module.reg);
	if ((state & SPI_ERROR_MASK) == 0) {
		/* spi rx ---------------------------------------------------*/
		if (state & SPI_STAT_RBNE) {
			uint16_t data = SPI_DATA((uint32_t)spi->module.reg);
      if((spi->transceiver != NULL) && (spi->transceiver->rx_data != NULL)) {  
          spi->transceiver->rx_data(spi->transceiver->proto_context, (uint8_t *)&data, 1);
      }
		}
		/* spi tx ---------------------------------------------------*/
		if ((state & SPI_STAT_TBE) && (cr1 & SPI_CTL1_TBEIE)) {
      uint16_t data = 0;
      if((spi->transceiver != NULL) && (spi->transceiver->tx_empty != NULL)) {
        spi->transceiver->tx_empty(spi->transceiver->proto_context, (uint8_t *)&data, 1);
      }
      SPI_DATA((uint32_t)spi->module.reg) = data;
		}
	}
	else {
		hdl_spi_reset_status((uint32_t)spi->module.reg);
	}
}

hdl_module_state_t hdl_spi_server(void *desc, uint8_t enable) {
  static coroutine_desc_static_t spi_task_buf;
  static linked_list_t spis = NULL;
  hdl_spi_server_private_t *spi = (hdl_spi_server_private_t*)desc;
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
    spi_parameter_struct init;
    init.device_mode = SPI_SLAVE;
    init.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    init.frame_size = SPI_FRAMESIZE_8BIT;
    init.endian = spi->config->endian;
    init.clock_polarity_phase = spi->config->polarity;
    init.prescale = HDL_SPI_PSC_2;
    init.nss = SPI_NSS_HARD;
    spi_init((uint32_t)spi->module.reg, &init);
    SPI_CTL1((uint32_t)spi->module.reg) |= SPI_CTL1_RBNEIE | SPI_CTL1_TBEIE | SPI_CTL1_ERRIE;
    //SPI_CTL0((uint32_t)spi->module.reg) |= SPI_CTL0_SWNSS; 
    spi->transceiver = NULL;
    spi->nss_isr.context = desc;
    spi->nss_isr.handler = &event_spi_nss;
    spi->spi_isr.context = desc;
    spi->spi_isr.handler = &event_spi_isr_server;
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)spi->module.dependencies[5];
    hdl_interrupt_request(ic, spi->spi_iterrupt, &spi->spi_isr);
    hdl_interrupt_request(ic, spi->nss_iterrupt, &spi->nss_isr);
    spi_enable((uint32_t)spi->module.reg);
    return HDL_MODULE_INIT_OK;
  }
  rcu_periph_clock_disable(rcu);
  return HDL_MODULE_DEINIT_OK;
}

void hdl_spi_server_set_transceiver(hdl_spi_server_t *spi, hdl_transceiver_t *transceiver) {
  if(spi != NULL) {
    ((hdl_spi_server_private_t*)spi)->transceiver = transceiver;
  }
}
