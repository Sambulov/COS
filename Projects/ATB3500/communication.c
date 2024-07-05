#include "app.h"

#define WDT_CMD    0x00000000ED
#define IO_CMD     0x00000000D0

void spi_event_handler(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *cont = (dev_context_t *)context;
    if(event_trigger > 0) {
        uint32_t cmd = *(uint32_t *)cont->spi_buffer.data;
        if(cmd == WDT_CMD) {
            atb3500_watchdog_proto_tx_t *res = atb3500_watchdog_update(&mod_watchdog, (atb3500_watchdog_proto_rx_t *)(cont->spi_buffer.data+4));
            cont->spi_tx_buffer.data = (uint8_t *)res;
            cont->spi_tx_buffer.size = sizeof(atb3500_watchdog_proto_tx_t);
            hdl_spi_server_dma_set_tx_data(&mod_spi3_server_dma, &cont->spi_tx_buffer);
        }
        if(cmd == IO_CMD) {
            atb3500_io_proto_tx_t *res = atb3500_io_update(&mod_carrier_io, (atb3500_io_proto_rx_t *)(cont->spi_buffer.data+4));
            cont->spi_tx_buffer.data = (uint8_t *)res;
            cont->spi_tx_buffer.size = sizeof(atb3500_io_proto_tx_t);
            hdl_spi_server_dma_set_tx_data(&mod_spi3_server_dma, &cont->spi_tx_buffer);
        }
    }
}
