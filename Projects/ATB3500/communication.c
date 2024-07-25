#include "app.h"

#define EMPTY_CMD          0x0000000000
#define WDT_CMD            0x00000000ED
#define IO_CMD             0x00000000D0
#define SERIAL_CMD         0x0000000065
#define I2C_GW_CMD         0x0000012C6E

void spi_event_handler(uint32_t recieved, void *sender, void *context) {
    dev_context_t *cont = (dev_context_t *)context;
    if(recieved > 0) {
        uint32_t cmd = *(uint32_t *)cont->spi_buffer.data;
        uint8_t *res = NULL;
        if(cmd == WDT_CMD) {
            res = (uint8_t *)atb3500_watchdog_update(&mod_watchdog, (atb3500_watchdog_proto_rx_t *)(cont->spi_buffer.data + 4));
            cont->spi_tx_buffer.size = sizeof(atb3500_watchdog_proto_tx_t);
        }
        if(cmd == IO_CMD) {
            res = (uint8_t *)atb3500_io_update(&mod_carrier_io, (atb3500_io_proto_rx_t *)(cont->spi_buffer.data + 4));
            cont->spi_tx_buffer.size = sizeof(atb3500_io_proto_tx_t);
        }
        if(cmd == SERIAL_CMD) {
            res = (uint8_t *)atb3500_flash_serial_update(&mod_serial, (atb3500_flash_serial_proto_rx_t *)(cont->spi_buffer.data + 4));
            cont->spi_tx_buffer.size = sizeof(atb3500_flash_serial_proto_tx_t);
        }
        if(cmd == I2C_GW_CMD) {
            res = (uint8_t *)atb3500_i2c_gateway_update(&mod_i2c_gateway, recieved, (atb3500_i2c_gateway_proto_rx_t *)(cont->spi_buffer.data + 4));
            cont->spi_tx_buffer.size = sizeof(atb3500_i2c_gateway_proto_tx_t);
        }
        cont->spi_tx_buffer.data = (uint8_t *)res;        
        hdl_spi_server_dma_set_tx_data(&mod_spi3_server_dma, &cont->spi_tx_buffer);
    }
}
