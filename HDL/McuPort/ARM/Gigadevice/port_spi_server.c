#include "hdl_portable.h"
#include "CodeLib.h"

typedef struct {
  hdl_delegate_t spi_isr;
  hdl_delegate_t nss_isr;
  hdl_transceiver_t *transceiver;
} hdl_spi_server_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_spi_server_var_t, *((hdl_spi_server_t *)0)->obj_var, HDL_SPI_SERVER_VAR_SIZE, port_spi.h);

static void event_spi_nss(uint32_t event, void *sender, void *context) {
  (void)sender;
  hdl_spi_server_t *spi = (hdl_spi_server_t*)context;
  hdl_spi_server_var_t *spi_var = (hdl_spi_server_var_t*)spi->obj_var;
  hdl_gpio_pin_t *nss = (hdl_gpio_pin_t *)spi->dependencies[3];
  if(event & (uint32_t)nss->config->pin){
    if((spi_var->transceiver != NULL) && (spi_var->transceiver->end_of_transmission != NULL))
      spi_var->transceiver->end_of_transmission(spi_var->transceiver->proto_context);
  }
}

static void event_spi_isr_server(uint32_t event, void *sender, void *context) {
  (void)event; (void)sender;
  hdl_spi_server_t *spi = (hdl_spi_server_t *)context;
  hdl_spi_server_var_t *spi_var = (hdl_spi_server_var_t*)spi->obj_var;
  uint32_t state = SPI_STAT(spi->config->phy);
  uint32_t cr1 = SPI_CTL1(spi->config->phy);
	if ((state & SPI_ERROR_MASK) == 0) {
		/* spi rx ---------------------------------------------------*/
		if (state & SPI_STAT_RBNE) {
			uint16_t data = SPI_DATA(spi->config->phy);
      if((spi_var->transceiver != NULL) && (spi_var->transceiver->rx_data != NULL)) {  
          spi_var->transceiver->rx_data(spi_var->transceiver->proto_context, (uint8_t *)&data, 1);
      }
		}
		/* spi tx ---------------------------------------------------*/
		if ((state & SPI_STAT_TBE) && (cr1 & SPI_CTL1_TBEIE)) {
      uint16_t data = 0;
      if((spi_var->transceiver != NULL) && (spi_var->transceiver->tx_empty != NULL)) {
        spi_var->transceiver->tx_empty(spi_var->transceiver->proto_context, (uint8_t *)&data, 1);
      }
      SPI_DATA(spi->config->phy) = data;
		}
	}
	else {
		hdl_spi_reset_status(spi->config->phy);
	}
}

static hdl_module_state_t _hdl_spi_server(const void *desc, uint8_t enable) {
  hdl_spi_server_t *spi = (hdl_spi_server_t*)desc;
  hdl_spi_server_var_t *spi_var = (hdl_spi_server_var_t*)spi->obj_var;
  spi_i2s_deinit(spi->config->phy);
  if(enable) {
    rcu_periph_clock_enable(spi->config->rcu);
    spi_parameter_struct init;
    init.device_mode = SPI_SLAVE;
    init.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    init.frame_size = SPI_FRAMESIZE_8BIT;
    init.endian = spi->config->endian;
    init.clock_polarity_phase = spi->config->polarity;
    init.prescale = HDL_SPI_PSC_2;
    init.nss = SPI_NSS_HARD;
    spi_init(spi->config->phy, &init);
    SPI_CTL1(spi->config->phy) |= SPI_CTL1_RBNEIE | SPI_CTL1_TBEIE | SPI_CTL1_ERRIE;
    //SPI_CTL0(spi->config->phy) |= SPI_CTL0_SWNSS; 
    spi_var->transceiver = NULL;
    spi_var->nss_isr.context = spi;
    spi_var->nss_isr.handler = &event_spi_nss;
    spi_var->spi_isr.context = spi;
    spi_var->spi_isr.handler = &event_spi_isr_server;
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)spi->dependencies[5];
    hdl_event_subscribe(&spi->config->spi_interrupt->event, &spi_var->spi_isr);
    hdl_interrupt_request(ic, spi->config->nss_interrupt);
    hdl_event_subscribe(&spi->config->spi_interrupt->event, &spi_var->spi_isr);
    hdl_interrupt_request(ic, spi->config->nss_interrupt);
    spi_enable(spi->config->phy);
    return HDL_MODULE_ACTIVE;
  }
  rcu_periph_clock_disable(spi->config->rcu);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_spi_server_set_transceiver(const void *desc, hdl_transceiver_t *transceiver, uint32_t channel_id) {
  (void)channel_id;
  hdl_spi_server_t *spi = (hdl_spi_server_t*)desc;
  if(spi != NULL) {
    ((hdl_spi_server_var_t *)((hdl_spi_server_t*)spi)->obj_var)->transceiver = transceiver;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

const hdl_spi_server_iface_t hdl_spi_server_iface = {
  .init = &_hdl_spi_server,
  .transceiver_set = &_hdl_spi_server_set_transceiver
};