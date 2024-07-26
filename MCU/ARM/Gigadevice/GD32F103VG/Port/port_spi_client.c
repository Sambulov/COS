#include "hdl_portable.h"
#include "CodeLib.h"

typedef struct {
  hdl_module_t module;
  /* private */
  __linked_list_object__;
  linked_list_t msgs;
  linked_list_item_t *curent_msg;
} hdl_spi_client_ch_private_t;

typedef struct {
  /* public */
  uint8_t *tx_buffer;
  uint8_t *rx_buffer;
  uint32_t tx_len;
  uint32_t rx_skip;
  uint32_t rx_take;
  /* private */
  __linked_list_object__;
  hdl_spi_message_state_t state; 
  uint32_t tx_cursor;
  uint32_t rx_cursor;
} hdl_spi_message_private_t;

typedef struct {
  hdl_module_t module;
  hdl_spi_client_config_t *config;
  hdl_nvic_irq_n_t spi_iterrupt;
  /* private */
  coroutine_t worker;
  hdl_delegate_t spi_isr;
  hdl_spi_message_private_t *current_msg;
  linked_list_t spi_ch_list;
  linked_list_item_t *curent_spi_ch;
} hdl_spi_client_private_t;

_Static_assert(sizeof(hdl_spi_client_private_t) == sizeof(hdl_spi_client_t), "In port_spi.h data structure size of hdl_spi_client_t doesn't match, check HDl_SPI_CLIENT_PRIVATE_SIZE");
_Static_assert(sizeof(hdl_spi_message_private_t) == sizeof(hdl_spi_message_t), "In hdl_spi.h data structure size of hdl_spi_message_t doesn't match, check HDL_SPI_MESSAGE_PRV_SIZE");
_Static_assert(sizeof(hdl_spi_client_ch_private_t) == sizeof(hdl_spi_client_ch_t), "In port_spi.h data structure size of hdl_spi_client_ch_t doesn't match, check HDl_SPI_CLIENT_CH_PRIVATE_SIZE");

static void event_spi_isr_client(uint32_t event, void *sender, void *context) {
  hdl_spi_client_private_t *spi = (hdl_spi_client_private_t *)context;
  hdl_spi_message_private_t *msg = spi->current_msg;
  uint32_t len = msg->rx_skip + msg->rx_take;
  len = MAX(msg->tx_len, len);
  uint32_t state = SPI_STAT((uint32_t)spi->module.reg);
  uint32_t cr1 = SPI_CTL1((uint32_t)spi->module.reg);
  if ((state & (SPI_ERROR_MASK)) == 0) {
    /* RX ---------------------------------------------------*/
    if (state & SPI_STAT_RBNE) {
      uint16_t data = SPI_DATA((uint32_t)spi->module.reg);
      if (msg->rx_buffer != NULL && msg->rx_take > 0 && msg->rx_cursor >= msg->rx_skip) {
        msg->rx_buffer[msg->rx_cursor - msg->rx_skip] = data;
      }
      msg->rx_cursor++;
      if(msg->tx_cursor == len && ((msg->rx_cursor - msg->rx_skip) == msg->rx_take)) {
        SPI_CTL1((uint32_t)spi->module.reg) &= ~(SPI_CTL1_RBNEIE);
      }
    }
    ///* TX ------------------------------------------------*/
    if ((state & SPI_STAT_TBE) && (cr1 & SPI_CTL1_TBEIE)) {
      if (msg->tx_cursor == len) {
        SPI_CTL1((uint32_t)spi->module.reg) &= ~(SPI_CTL1_TBEIE);
      }
      else {
        uint16_t data = 0;
        if (msg->tx_buffer != NULL && msg->tx_cursor < msg->tx_len)
        {
          data = msg->tx_buffer[msg->tx_cursor];
        }
        SPI_DATA((uint32_t)spi->module.reg) = data;
        msg->tx_cursor++;
      }
    }
  }
  else {
    hdl_spi_reset_status((uint32_t)spi->module.reg);
  }
}

static uint8_t _spi_client_channel_handler(hdl_spi_client_ch_private_t *ch, hdl_spi_client_private_t *spi) {
  uint8_t spi_free = !(SPI_CTL1((uint32_t)spi->module.reg) & (SPI_CTL1_TBEIE | SPI_CTL1_RBNEIE));
  if(spi_free) {
    hdl_gpio_pin_t *pin_cs = (hdl_gpio_pin_t *)ch->module.dependencies[1];
    if(ch->curent_msg != NULL) {
      hdl_spi_message_private_t *msg = linked_list_get_object(hdl_spi_message_private_t,ch->curent_msg);
      msg->state = HDL_SPI_TRANSACTION_EXECUTED;
      linked_list_unlink(linked_list_item(msg));
      ch->curent_msg = NULL;
      hdl_gpio_set_inactive(pin_cs);
    }
    if(ch->curent_msg == NULL && ch->msgs != NULL) {  
      ch->curent_msg = ch->msgs;
      hdl_spi_message_private_t *msg = linked_list_get_object(hdl_spi_message_private_t, ch->curent_msg);
      msg->state = HDL_SPI_TRANSACTION_EXECUTING;
    }
    if(ch->curent_msg != NULL) {
      hdl_spi_message_private_t *msg = linked_list_get_object(hdl_spi_message_private_t, ch->curent_msg);
      msg->rx_cursor = 0;
      msg->tx_cursor = 0;
      spi->current_msg = msg;
      hdl_gpio_set_active(pin_cs);
      SPI_CTL1((uint32_t)spi->module.reg) |= (SPI_CTL1_TBEIE | SPI_CTL1_RBNEIE);
    }
    return 1;
  }
  return 0;
}

static uint8_t _spi_client_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  hdl_spi_client_private_t *spi = (hdl_spi_client_private_t *) arg;
  if(!linked_list_contains(spi->spi_ch_list, spi->curent_spi_ch))
    spi->curent_spi_ch = NULL;
  if(spi->curent_spi_ch == NULL && spi->spi_ch_list != NULL) {
    spi->curent_spi_ch = spi->spi_ch_list;
  }
  if(spi->curent_spi_ch != NULL && _spi_client_channel_handler(linked_list_get_object(hdl_spi_client_ch_private_t, spi->curent_spi_ch), spi)) {
      spi->curent_spi_ch = linked_list_find_next_overlap(spi->curent_spi_ch, NULL, NULL);
  }  
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
    //SPI_CTL1((uint32_t)spi->module.reg) |= SPI_CTL1_RBNEIE;
    //SPI_CTL1((uint32_t)spi->module.reg) |= SPI_CTL1_RBNEIE | SPI_CTL1_TBEIE | SPI_CTL1_ERRIE;
    //SPI_CTL0((uint32_t)spi->module.reg) |= SPI_CTL0_SWNSS; 
    spi->spi_isr.context = desc;
    spi->spi_isr.handler = &event_spi_isr_client;
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)spi->module.dependencies[4];
    hdl_interrupt_request(ic, spi->spi_iterrupt, &spi->spi_isr);
    spi_enable((uint32_t)spi->module.reg);
    coroutine_add(&spi->worker, &_spi_client_worker, desc);
    return HDL_MODULE_INIT_OK;
  }
  coroutine_cancel(&spi->worker);
  rcu_periph_clock_disable(rcu);
  return HDL_MODULE_DEINIT_OK;
}


hdl_module_state_t hdl_spi_ch(void *desc, uint8_t enable) {
  hdl_spi_client_ch_private_t *spi_ch = (hdl_spi_client_ch_private_t*)desc;
  hdl_spi_client_private_t *spi_hw = (hdl_spi_client_private_t *)spi_ch->module.dependencies[0];
  if(enable) {
    linked_list_insert_last(&spi_hw->spi_ch_list, linked_list_item(spi_ch));
    return HDL_MODULE_INIT_OK;
  }
  linked_list_unlink(linked_list_item(spi_ch));
  return HDL_MODULE_DEINIT_OK;
}

hdl_spi_message_state_t hdl_spi_client_xfer(hdl_spi_client_ch_t *spi_ch, hdl_spi_message_t *message) {
  if((spi_ch != NULL) && (message != NULL) && ((message->tx_len + message->rx_take) > 0)) {
    hdl_spi_client_ch_private_t *spi = (hdl_spi_client_ch_private_t *)spi_ch;
    hdl_spi_message_private_t *msg = (hdl_spi_message_private_t *)message; 
    if(hdl_spi_message_get_state(message) <= HDL_SPI_TRANSACTION_TERMINATED) {
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


