#include "hdl_portable.h"
#include "CodeLib.h"

#define SPI_SERVER_RX_BUFFER_SZ    32
#define SPI_SERVER_TX_BUFFER_SZ    32

typedef struct {
  hdl_module_t module;
  hdl_spi_server_config_t *config;
  hdl_nvic_irq_n_t spi_iterrupt;
  hdl_nvic_irq_n_t nss_iterrupt; /* if HDL_SPI_CS_SOFT in HDL_SPI_SERVER mode */
  /* private */
  hdl_delegate_t spi_isr;
  hdl_delegate_t nss_isr;
  hdl_transceiver_t *transceiver;
  __linked_list_object__;
} hdl_spi_server_private_t;



typedef enum {
  NoError = 0,
  FrameError = SPI_STAT_FERR,
  OverRunError = SPI_STAT_RXORERR,
  ConfigError = SPI_STAT_CONFERR,
  CrcError = SPI_STAT_CRCERR,
  UnderRunError = SPI_STAT_TXURERR,
  StateMask = FrameError | OverRunError | ConfigError | CrcError | UnderRunError,
} spi_error_t;

_Static_assert(sizeof(hdl_spi_server_private_t) == sizeof(hdl_spi_server_t), "In port_spi.h data structure size of hdl_spi_server_t doesn't match, check SPI_SERVER_PRIVATE_SIZE");


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

static void event_spi_isr_server(uint32_t event, void *sender, void *context) {
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




// static uint8_t _spi_worker(coroutine_desc_t this, uint8_t cancel, void *arg) {
//   linked_list_t spis = (linked_list_t)arg;
//   linked_list_do_foreach(spis, &_spi_handler, NULL);
//   return cancel;
// }

// hdl_module_state_t hdl_spi(void *desc, uint8_t enable) {
//   static coroutine_desc_static_t spi_task_buf;
//   static linked_list_t spis = NULL;
//   hdl_spi_private_t *spi = (hdl_spi_private_t*)desc;
//   rcu_periph_enum rcu;
//   switch ((uint32_t)spi->module.reg) {
//     case SPI0: rcu = RCU_SPI0; break;
//     case SPI1: rcu = RCU_SPI1; break;
//     default: return HDL_MODULE_INIT_FAILED;
//   }
//   spi_i2s_deinit((uint32_t)spi->module.reg);
//   if(enable) {
//     rcu_periph_clock_enable(rcu);
//     linked_list_insert_last(&spis, linked_list_item(spi));
//     coroutine_add_static(&spi_task_buf, &_spi_worker, (void *)spis);
//     spi_parameter_struct init;
//     init.device_mode = spi->config->mode;
//     init.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
//     init.frame_size = SPI_FRAMESIZE_8BIT;
//     init.endian = spi->config->endian;
//     init.clock_polarity_phase = spi->config->polarity;
//     init.nss = spi->config->cs_mode;
//     init.prescale = spi->config->prescale;
//     spi_init((uint32_t)spi->module.reg, &init);
//     spi->transceiver = NULL;
//     hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)spi->module.dependencies[4];
//     hdl_interrupt_request(ic, spi->spi_iterrupt, &event_spi_isr, desc);
//     /* todo: enable interrupts */
//     spi_enable((uint32_t)spi->module.reg);
//     return HDL_MODULE_INIT_OK;
//   }
//   linked_list_unlink(linked_list_item(spi));
//   rcu_periph_clock_disable(rcu);
//   return HDL_MODULE_DEINIT_OK;
// }


hdl_module_state_t hdl_spi_server(void *desc, uint8_t enable) {
  static coroutine_desc_static_t spi_task_buf;
  static linked_list_t spis = NULL;
  hdl_spi_server_private_t *spi = (hdl_spi_server_private_t*)desc;
  rcu_periph_enum rcu;
  switch ((uint32_t)spi->module.reg) {
    case SPI0: rcu = RCU_SPI0; break;
    case SPI1: rcu = RCU_SPI1; break;
    default: return HDL_MODULE_INIT_FAILED;
  }
  spi_i2s_deinit((uint32_t)spi->module.reg);
  if(enable) {
    rcu_periph_clock_enable(rcu);
    linked_list_insert_last(&spis, linked_list_item(spi));
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
    spi->transceiver = NULL;
    spi->nss_isr.context = desc;
    spi->nss_isr.handler = &event_spi_nss;
    spi->spi_isr.context = desc;
    spi->spi_isr.handler = &event_spi_isr_server;
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)spi->module.dependencies[5];
    hdl_interrupt_request(ic, spi->spi_iterrupt, &spi->spi_isr);
    hdl_interrupt_request(ic, spi->nss_iterrupt, &spi->nss_isr);
    /* todo: enable interrupts */
    spi_enable((uint32_t)spi->module.reg);
    return HDL_MODULE_INIT_OK;
  }
  linked_list_unlink(linked_list_item(spi));
  rcu_periph_clock_disable(rcu);
  return HDL_MODULE_DEINIT_OK;
}

void hdl_spi_server_set_transceiver(hdl_spi_server_t *spi, hdl_transceiver_t *transceiver) {
  if(spi != NULL) {
    ((hdl_spi_server_private_t*)spi)->transceiver = transceiver;
  }
}


/**************** vvv  SPI master vvv  ******************/

typedef struct {
  hdl_module_t module;
  hdl_spi_client_config_t *config;
  hdl_nvic_irq_n_t spi_iterrupt;
  /* private */
  hdl_delegate_t spi_isr;
} hdl_spi_client_private_t;

typedef struct {
  hdl_module_t module;
  /* private */
  __linked_list_object__;
  linked_list_t msgs;
  linked_list_item_t *curent_msg;
} hdl_spi_client_ch_private_t;

typedef struct {
  __linked_list_object__;
  hdl_spi_message_state_t state; 
  /* public */
  uint8_t *tx_buffer;
  uint8_t *rx_buffer;
  uint32_t tx_len;
  uint32_t rx_skip;
  uint32_t rx_take;
} hdl_spi_message_private_t;

_Static_assert(sizeof(hdl_spi_client_private_t) == sizeof(hdl_spi_client_t), "In port_spi.h data structure size of hdl_spi_client_t doesn't match, check HDl_SPI_CLIENT_PRIVATE_SIZE");
_Static_assert(sizeof(hdl_spi_message_private_t) == sizeof(hdl_spi_message_t), "In hdl_spi.h data structure size of hdl_spi_message_t doesn't match, check HDL_SPI_MESSAGE_PRV_SIZE");
_Static_assert(sizeof(hdl_spi_client_ch_private_t) == sizeof(hdl_spi_client_ch_t), "In port_spi.h data structure size of hdl_spi_client_ch_t doesn't match, check HDl_SPI_CLIENT_CH_PRIVATE_SIZE");

static void event_spi_isr_client(uint32_t event, void *sender, void *context) {
}

static void _spi_client_handler(linked_list_item_t *spi_item, void *arg) {
  hdl_spi_client_ch_private_t *spi = (hdl_spi_client_ch_private_t *)spi_item;
  uint8_t spi_free = 1;
  hdl_spi_client_t *spi_hw = (hdl_spi_client_t *)spi->module.dependencies[0];
  if(spi_free) {
    hdl_spi_message_private_t *msg = linked_list_get_object(hdl_spi_message_private_t, spi->curent_msg);
    if(msg == NULL) {
      /* Send data */
      msg = linked_list_get_object(hdl_spi_message_private_t, spi->msgs);
      if(msg != NULL) {
        spi->curent_msg = linked_list_item(msg);
        msg->state = HDL_SPI_TRANSACTION_EXECUTING;
      }
    }
    if(msg != NULL) {
      /* Wait end of transmission data */
      uint8_t eot = 1;
      if(eot) {
        msg->state = HDL_SPI_TRANSACTION_EXECUTED;
        linked_list_unlink(linked_list_item(msg));
        spi->curent_msg = NULL;
      }
    }
  }
  
}

static uint8_t _spi_client_worker(coroutine_desc_t this, uint8_t cancel, void *arg) {
  linked_list_t spis = (linked_list_t)arg;
  /* TODO: Scheduler fair */
  linked_list_do_foreach(spis, &_spi_client_handler, NULL);
  return cancel;
}

hdl_module_state_t hdl_spi_client(void *desc, uint8_t enable) {
  hdl_spi_client_private_t *spi = (hdl_spi_client_private_t*)desc;
  rcu_periph_enum rcu;
  switch ((uint32_t)spi->module.reg) {
    case SPI0: rcu = RCU_SPI0; break;
    case SPI1: rcu = RCU_SPI1; break;
    default: return HDL_MODULE_INIT_FAILED;
  }
  spi_i2s_deinit((uint32_t)spi->module.reg);
  if(enable) {
    rcu_periph_clock_enable(rcu);
    spi_parameter_struct init;
    init.device_mode = SPI_MASTER;
    init.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    init.frame_size = SPI_FRAMESIZE_8BIT;
    init.endian = spi->config->endian;
    init.clock_polarity_phase = spi->config->polarity;
    init.nss = SPI_NSS_SOFT;
    init.prescale = spi->config->prescale;
    spi_init((uint32_t)spi->module.reg, &init);
    SPI_CTL1((uint32_t)spi->module.reg) |= SPI_CTL1_RBNEIE | SPI_CTL1_TBEIE | SPI_CTL1_ERRIE;
    //SPI_CTL0((uint32_t)spi->module.reg) |= SPI_CTL0_SWNSS; 

    spi->spi_isr.context = desc;
    spi->spi_isr.handler = &event_spi_isr_client;
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)spi->module.dependencies[5];
    hdl_interrupt_request(ic, spi->spi_iterrupt, &spi->spi_isr);
    /* todo: enable interrupts */
    spi_enable((uint32_t)spi->module.reg);
    return HDL_MODULE_INIT_OK;
  }
  
  rcu_periph_clock_disable(rcu);
  return HDL_MODULE_DEINIT_OK;
}


hdl_module_state_t hdl_spi_ch(void *desc, uint8_t enable) {
  static coroutine_desc_static_t spi_task_buf;
  static linked_list_t spis = NULL;
  hdl_spi_client_ch_private_t *spi = (hdl_spi_client_ch_private_t*)desc;
  if(enable) {
    linked_list_insert_last(&spis, linked_list_item(spi));
    coroutine_add_static(&spi_task_buf, &_spi_client_worker, (void *)spis);
    return HDL_MODULE_INIT_OK;
  }
  linked_list_unlink(linked_list_item(spi));
  return HDL_MODULE_DEINIT_OK;
}

hdl_spi_message_state_t hdl_spi_client_xfer(hdl_spi_client_ch_t *spi_ch, hdl_spi_message_t *message) {
  /* TODO: add check msg */
  if(spi_ch != NULL) {
    hdl_spi_client_ch_private_t *spi = (hdl_spi_client_ch_private_t *)spi_ch;
    hdl_spi_message_state_t msg_state = hdl_spi_message_get_state(message);
    hdl_spi_message_private_t *msg = (hdl_spi_message_private_t *)message; 
    if(msg_state <= HDL_SPI_TRANSACTION_TERMINATED) {
      linked_list_insert_last(&spi->msgs, linked_list_item(msg));
      msg->state = HDL_SPI_TRANSACTION_ENQUEUED;
    }
    return msg->state;
  }
  return HDL_SPI_TRANSACTION_FAILED;
}

#define spi_message_state_valid(state) (state <= HDL_SPI_TRANSACTION_EXECUTING)

hdl_spi_message_state_t hdl_spi_message_get_state(hdl_spi_message_t *message) {
  hdl_spi_message_private_t *msg = (hdl_spi_message_private_t *)message;
  if((msg != NULL) && spi_message_state_valid(msg->state)) {
    return msg->state;
  }
  return HDL_SPI_TRANSACTION_UNKNOWN;
}


