#include "hdl_iface.h"

typedef struct {
  hdl_spi_client_ch_sw_t *curent_spi_ch;
  uint16_t rx_cursor;
  uint16_t tx_cursor;
  uint32_t msg_len;
} hdl_spi_client_sw_var_t;

typedef struct {
  coroutine_t ch_worker;
  hdl_spi_message_t *curent_msg;
} hdl_spi_client_ch_sw_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_spi_client_sw_var_t, *((hdl_spi_client_sw_t *)0)->obj_var, HDL_SPI_CLIENT_SW_VAR_SIZE, "hdl_spi_client_sw.h");
HDL_ASSERRT_STRUCTURE_CAST(hdl_spi_client_ch_sw_var_t, *((hdl_spi_client_ch_sw_t *)0)->obj_var, HDL_SPI_CLIENT_CH_SW_VAR_SIZE, "hdl_spi_client_sw.h");

static inline void _spi_sw_delay(uint32_t delay) {
  while (delay--);
}

static uint8_t _spi_transfer_byte(hdl_spi_client_sw_t *spi, uint8_t byte) {
    hdl_gpio_pin_t *mosi = (hdl_gpio_pin_t *)spi->dependencies[0];
    hdl_gpio_pin_t *miso = (hdl_gpio_pin_t *)spi->dependencies[1];
    hdl_gpio_pin_t *sck = (hdl_gpio_pin_t *)spi->dependencies[2];
    if(spi->config->polarity & HDL_SPI_SW_EDGE2) {
      hdl_gpio_toggle(sck);
      _spi_sw_delay(spi->config->signal_min_delay);
    }
    uint8_t bit = 1;
    if(spi->config->endian == HDL_SPI_SW_BIG_ENDIAN) bit_reflect(byte, 8);
    while (bit) {
      if(byte & bit) hdl_gpio_set_active(mosi);
      else hdl_gpio_set_inactive(mosi);
      hdl_gpio_toggle(sck);
      _spi_sw_delay(spi->config->signal_min_delay);
      if(hdl_gpio_is_active(miso)) byte |= bit;
      else byte &= ~bit;
      hdl_gpio_toggle(sck);
      _spi_sw_delay(spi->config->signal_min_delay);
      bit <<= 1;
    }
    if(spi->config->endian == HDL_SPI_SW_BIG_ENDIAN) bit_reflect(byte, 8);
    return byte;
}

static uint8_t _spi_ch_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_spi_client_ch_sw_t *spi_ch = (hdl_spi_client_ch_sw_t*)arg;
  hdl_spi_client_ch_sw_var_t *spi_ch_var = (hdl_spi_client_ch_sw_var_t *)spi_ch->obj_var;
  hdl_spi_client_sw_t *spi = (hdl_spi_client_sw_t *)spi_ch->dependencies[0];
  hdl_spi_client_sw_var_t *spi_var = (hdl_spi_client_sw_var_t *)spi->obj_var;
  if((spi_var->curent_spi_ch == NULL) && (spi_ch_var->curent_msg != NULL)) {
    spi_var->curent_spi_ch = spi_ch;
  }
  if (spi_var->curent_spi_ch == spi_ch) {
    hdl_gpio_pin_t *pin_cs = (hdl_gpio_pin_t *)spi_ch->dependencies[1];
    hdl_spi_message_t *msg = spi_ch_var->curent_msg;
    if((msg != NULL) && hdl_take(spi, spi_ch)) {
      if (msg->status == HDL_SPI_MESSAGE_STATUS_INITIAL) {
        if(msg->options & HDL_SPI_MESSAGE_CH_SELECT) {
          hdl_gpio_set_active(pin_cs);
          _spi_sw_delay(spi_ch->config->cs_min_delay);
          msg->status |= HDL_SPI_MESSAGE_STATUS_BUS_HOLD;
        }
        spi_var->msg_len = msg->rx_skip + msg->rx_take;
        spi_var->msg_len = CL_MAX(msg->tx_len, spi_var->msg_len);
        if(spi_var->msg_len > 0) {
          spi_var->rx_cursor = 0;
          spi_var->tx_cursor = 0;
          msg->status |= HDL_SPI_MESSAGE_STATUS_XFER;
        }
        else {
          msg->status |= HDL_SPI_MESSAGE_STATUS_XFER_COMPLETE;
        }
      }
      if (msg->status & HDL_SPI_MESSAGE_STATUS_XFER) {
        uint8_t data = 0;
        if ((msg->tx_buffer != NULL) && (msg->tx_len > 0)) {
          if (spi_var->tx_cursor < msg->tx_len) data = msg->tx_buffer[spi_var->tx_cursor];
          else data = msg->tx_buffer[msg->tx_len - 1];
        }
        spi_var->tx_cursor++;
        data = _spi_transfer_byte(spi, data);
        if(msg->rx_buffer != NULL) {
          int32_t data_offset = ((int32_t)spi_var->rx_cursor) - msg->rx_skip;
          if((data_offset >= 0) && (data_offset < msg->rx_take)) msg->rx_buffer[data_offset] = data;
        }
        spi_var->rx_cursor++;
        if(spi_var->rx_cursor >= spi_var->msg_len) msg->status |= HDL_SPI_MESSAGE_STATUS_XFER_COMPLETE;
      }
      if(msg->status & HDL_SPI_MESSAGE_STATUS_XFER_COMPLETE) {
        spi_ch_var->curent_msg = NULL;
        if(msg->options & HDL_SPI_MESSAGE_CH_RELEASE) {
          hdl_gpio_set_inactive(pin_cs);
          msg->status |= HDL_SPI_MESSAGE_STATUS_BUS_RELEASE;
          spi_var->curent_spi_ch = NULL;
          hdl_give(spi, spi_ch);
        }
        msg->status |= HDL_SPI_MESSAGE_STATUS_COMPLETE;
      }
    }
  }
  return cancel;
}

static hdl_module_state_t _hdl_spi_client(const void *desc, uint8_t enable) {
  hdl_spi_client_sw_t *spi = (hdl_spi_client_sw_t*)desc;
  if(enable) {
    hdl_gpio_pin_t *sck = (hdl_gpio_pin_t *)spi->dependencies[2];
    if(spi->config->polarity & HDL_SPI_SW_CPOL0) hdl_gpio_set_inactive(sck);
    else hdl_gpio_set_active(sck);
    return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_spi_ch(const void *desc, uint8_t enable) {
  hdl_spi_client_ch_sw_t *spi_ch = (hdl_spi_client_ch_sw_t*)desc;
  hdl_spi_client_ch_sw_var_t *spi_ch_var = (hdl_spi_client_ch_sw_var_t *)spi_ch->obj_var;
  if(enable) {
    hdl_gpio_pin_t *cs = (hdl_gpio_pin_t *)spi_ch->dependencies[1];
    hdl_gpio_set_inactive(cs);
    coroutine_add(&spi_ch_var->ch_worker, &_spi_ch_worker, spi_ch);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&spi_ch_var->ch_worker);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_spi_transfer_message(const void *desc, hdl_spi_message_t *message) {
  hdl_spi_client_ch_sw_t *spi_ch = (hdl_spi_client_ch_sw_t*)desc;
  hdl_spi_client_ch_sw_var_t *spi_ch_var = (hdl_spi_client_ch_sw_var_t *)spi_ch->obj_var;
  if((spi_ch != NULL) && (hdl_state(spi_ch) != HDL_MODULE_FAULT) && (message != NULL)) {
    if(spi_ch_var->curent_msg == NULL) {
      spi_ch_var->curent_msg = message;
      message->transferred = 0;
      message->status = HDL_SPI_MESSAGE_STATUS_INITIAL;
      return HDL_TRUE;
    }
  }
  return HDL_FALSE;
}

const hdl_module_base_iface_t hdl_spi_client_sw_iface = {
  .init = &_hdl_spi_client
};

const hdl_spi_client_ch_iface_t hdl_spi_client_ch_sw_iface = {
  .init = &_hdl_spi_ch,
  .transfer = &_hdl_spi_transfer_message
};
