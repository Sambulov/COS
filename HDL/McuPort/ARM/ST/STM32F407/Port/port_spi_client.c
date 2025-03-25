#include "hdl_iface.h"

typedef struct {
  hdl_delegate_t isr;
  hdl_spi_client_ch_mcu_t *curent_spi_ch;
  uint16_t rx_cursor;
  uint16_t tx_cursor;
  uint32_t timer;
} hdl_spi_client_var_t;

typedef struct {
  coroutine_t ch_worker;
  hdl_spi_message_t *curent_msg;
} hdl_spi_client_ch_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_spi_client_var_t, *((hdl_spi_client_mcu_t *)0)->obj_var, HDL_SPI_CLIENT_VAR_SIZE, port_spi.h);
HDL_ASSERRT_STRUCTURE_CAST(hdl_spi_client_ch_var_t, *((hdl_spi_client_ch_mcu_t *)0)->obj_var, HDL_SPI_CLIENT_CH_VAR_SIZE, port_spi.h);

static void event_spi_isr_client(uint32_t event, void *sender, void *context) {
  (void)event; (void)sender;
  hdl_spi_client_mcu_t *spi = (hdl_spi_client_mcu_t *)context;
  hdl_spi_client_var_t *spi_var = (hdl_spi_client_var_t *)spi->obj_var;
  hdl_spi_client_ch_var_t *ch_var = (hdl_spi_client_ch_var_t *)spi_var->curent_spi_ch->obj_var;
  hdl_spi_message_t *msg = ch_var->curent_msg;
  SPI_TypeDef *phy = (SPI_TypeDef *)spi->config->phy;

  uint32_t msg_len = msg->rx_skip + msg->rx_take;
  msg_len = MAX(msg->tx_len, msg_len);

  

  uint32_t state = phy->SR;
  if ((state & SPI_ERROR_MASK) == 0) {
    /* RX ---------------------------------------------------*/
    if (state & SPI_SR_RXNE) {
      uint16_t data = phy->DR;
      if(msg->rx_buffer != NULL) {
        int32_t data_offset = ((int32_t)spi_var->rx_cursor) - msg->rx_skip;
        if((data_offset >= 0) && (data_offset < msg->rx_take)) msg->rx_buffer[data_offset] = data;
      }
      spi_var->rx_cursor++;
      if(spi_var->rx_cursor >= msg_len) {
        phy->CR2 &= ~SPI_CR2_RXNEIE;
        msg->state |= HDL_SPI_MESSAGE_STATUS_XFER_COMPLETE;
      }
    }

    ///* TX ------------------------------------------------*/
    if((state & SPI_SR_TXE) && (spi_var->tx_cursor < msg_len)) {
      uint16_t data = 0;
      if ((msg->tx_buffer != NULL) && (msg->tx_len > 0)) {
        if (spi_var->tx_cursor < msg->tx_len) data = msg->tx_buffer[spi_var->tx_cursor];
        else data = msg->tx_buffer[msg->tx_len - 1];
      }
      phy->DR = data;
      spi_var->tx_cursor++;
      if(spi_var->tx_cursor >= msg_len) phy->CR2 &= ~SPI_CR2_TXEIE;
    }
    msg->transferred = MIN(spi_var->rx_cursor, spi_var->tx_cursor);
  }
  else {
    hdl_spi_reset_status(phy);
  }
}

static uint8_t _spi_ch_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_spi_client_ch_mcu_t *ch = (hdl_spi_client_ch_mcu_t*)arg;
  hdl_spi_client_ch_var_t *spi_ch_var = (hdl_spi_client_ch_var_t *)ch->obj_var;
  hdl_spi_client_mcu_t *spi = (hdl_spi_client_mcu_t *)ch->dependencies[0];
  hdl_spi_client_var_t *spi_var = (hdl_spi_client_var_t *)spi->obj_var;
  SPI_TypeDef *phy = (SPI_TypeDef *)spi->config->phy;

  if((spi_var->curent_spi_ch == NULL) && (spi_ch_var->curent_msg != NULL)) {
    spi_var->curent_spi_ch = ch;
  }
  if (spi_var->curent_spi_ch == ch) {
    hdl_gpio_pin_t *pin_cs = (hdl_gpio_pin_t *)ch->dependencies[1];
    hdl_spi_message_t *msg = spi_ch_var->curent_msg;
    if((msg != NULL) && hdl_take(spi, ch)) {
      if (msg->state == HDL_SPI_MESSAGE_STATUS_INITIAL) {
        //TODO: cs delay
        if(msg->options & HDL_SPI_MESSAGE_CH_SELECT) {
          hdl_gpio_set_active(pin_cs);
          msg->state |= HDL_SPI_MESSAGE_STATUS_BUS_HOLD;
        }
        uint32_t msg_len = msg->rx_skip + msg->rx_take;
        msg_len = MAX(msg->tx_len, msg_len);
        if(msg_len > 0) {
          spi_var->rx_cursor = 0;
          spi_var->tx_cursor = 0;
          hdl_spi_reset_status(phy);
          msg->state |= HDL_SPI_MESSAGE_STATUS_XFER;
          phy->CR2 |= SPI_CR2_TXEIE | SPI_CR2_RXNEIE;

        }
        else {
          msg->state |= HDL_SPI_MESSAGE_STATUS_XFER_COMPLETE;
        }
      }
      if(msg->state & HDL_SPI_MESSAGE_STATUS_XFER_COMPLETE) {
        spi_ch_var->curent_msg = NULL;
        if(msg->options & HDL_SPI_MESSAGE_CH_RELEASE) {
          hdl_gpio_set_inactive(pin_cs);
          msg->state |= HDL_SPI_MESSAGE_STATUS_BUS_RELEASE;
          spi_var->curent_spi_ch = NULL;
          hdl_give(spi, ch);
        }
        msg->state |= HDL_SPI_MESSAGE_STATUS_COMPLETE;
      }
      else if()
    }
  }
  return cancel;
}

static volatile uint32_t *_hdl_spi_reset(hdl_spi_client_mcu_t *spi) {
  volatile uint32_t *rcc_en = &RCC->APB1ENR;
  volatile uint32_t *rcc_rst = &RCC->APB1RSTR;
  SPI_TypeDef *periph = (SPI_TypeDef *)spi->config->phy;
  switch ((uint32_t)periph) {
    case (uint32_t)SPI1:
      rcc_en = &RCC->APB2ENR;
      rcc_rst = &RCC->APB2RSTR;
      break;
    case (uint32_t)SPI2:
    case (uint32_t)SPI3:
      break;
    //case (uint32_t)SPI4:
    //case (uint32_t)SPI5:
    //case (uint32_t)SPI6:
    default:
      return NULL;
  }
  HDL_REG_SET(*rcc_rst, spi->config->rcu);
  HDL_REG_CLEAR(*rcc_rst, spi->config->rcu);
  return rcc_en;
}

static hdl_module_state_t _hdl_spi_client(const void *desc, uint8_t enable) {
  hdl_spi_client_mcu_t *spi = (hdl_spi_client_mcu_t*)desc;
  hdl_spi_client_var_t *spi_var = (hdl_spi_client_var_t *)spi->obj_var;
  SPI_TypeDef *phy = (SPI_TypeDef *)spi->config->phy;
  volatile uint32_t *rcc_en = _hdl_spi_reset(spi);
  if(rcc_en == NULL) return HDL_MODULE_FAULT;
  if(enable) {
    HDL_REG_SET(*rcc_en, spi->config->rcu);
    hdl_clock_t *clk = (hdl_clock_t *)spi->dependencies[3];
    hdl_clock_freq_t freq;
    hdl_clock_get(clk, &freq);
    HDL_REG_SET(*rcc_en, spi->config->rcu);
    phy->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | 
      spi->config->endian | spi->config->prescale | spi->config->polarity;
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)spi->dependencies[4];
    spi_var->isr.context = spi;
    spi_var->isr.handler = &event_spi_isr_client;
    hdl_event_subscribe(&spi->config->interrupt->event, &spi_var->isr);
    hdl_interrupt_request(ic, spi->config->interrupt);
    phy->CR1 |= SPI_CR1_SPE;
    return HDL_MODULE_ACTIVE;
  }
  HDL_REG_CLEAR(*rcc_en, spi->config->rcu);
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_spi_ch(const void *desc, uint8_t enable) {
  hdl_spi_client_ch_mcu_t *spi_ch = (hdl_spi_client_ch_mcu_t*)desc;
  hdl_spi_client_ch_var_t *spi_ch_var = (hdl_spi_client_ch_var_t *)spi_ch->obj_var;
  if(enable) {
    coroutine_add(&spi_ch_var->ch_worker, &_spi_ch_worker, spi_ch);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&spi_ch_var->ch_worker);
  // TODO: free bus
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_spi_transfer_message(const void *desc, hdl_spi_message_t *message) {
  hdl_spi_client_ch_mcu_t *spi_ch = (hdl_spi_client_ch_mcu_t*)desc;
  hdl_spi_client_ch_var_t *spi_ch_var = (hdl_spi_client_ch_var_t *)spi_ch->obj_var;
  if((spi_ch != NULL) && (hdl_state(spi_ch) != HDL_MODULE_FAULT) && (message != NULL)) {
    if(spi_ch_var->curent_msg == NULL) {
      spi_ch_var->curent_msg = message;
      message->transferred = 0;
      message->state = HDL_SPI_MESSAGE_STATUS_INITIAL;
      return HDL_TRUE;
    }
  }
  return HDL_FALSE;
}

const hdl_module_base_iface_t hdl_spi_client_iface = {
  .init = &_hdl_spi_client
};

const hdl_spi_client_ch_iface_t hdl_spi_client_ch_iface = {
  .init = &_hdl_spi_ch,
  .transfer = &_hdl_spi_transfer_message
};
