#include "app.h"

#define MSG_SEND_DELAY  1000

void spi_cliet_sender(uint32_t event_trigger, void *sender, void *context) {
  static hdl_spi_message_t spi_msg = {.state = HDL_SPI_MESSAGE_STATUS_COMPLETE};
  static uint8_t rx_buffer[128];
  static uint8_t tx_buffer[5];  
  (void)event_trigger; (void)sender; (void)context;

  if(spi_msg.state & HDL_SPI_MESSAGE_STATUS_COMPLETE) {
    tx_buffer[0] = 0x0A;
    tx_buffer[1] = 0xA0;
    tx_buffer[2] = 0x05;
    tx_buffer[3] = 0x50;
    tx_buffer[4] = 0xFF; /* dummy byte, transfer while receiving data */
    spi_msg.tx_buffer = tx_buffer;
    spi_msg.tx_len = 5;
    spi_msg.rx_buffer = rx_buffer;
    spi_msg.rx_skip = 4;
    spi_msg.rx_take = 128;
    spi_msg.options = HDL_SPI_MESSAGE_CH_SELECT | HDL_SPI_MESSAGE_CH_RELEASE;
    hdl_spi_transfer_message(&mod_spi_client, &spi_msg);
  }
}

const hdl_timer_t spi_timer = {
  .iface = &hdl_timer_iface,
  .dependencies= hdl_module_dependencies(&mod_timer_ms),
  .config = hdl_module_config(hdl_timer_config_t,
    .mode = HDL_TIMER_MODE_LOOP,
    .delay = MSG_SEND_DELAY
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE),
  .obj_var = static_malloc(HDL_TIMER_VAR_SIZE)
};

void spi_client_test() {
  static hdl_delegate_t spi_client_delegate;
  spi_client_delegate.context = NULL;
  spi_client_delegate.handler = &spi_cliet_sender;
  hdl_enable(&spi_timer);
  hdl_timer_subscribe(&spi_timer, &spi_client_delegate);
}
