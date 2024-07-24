#include "app.h"
#include "CodeLib.h"

typedef struct {
    hdl_module_t module;
    /* Private */
    atb3500_i2c_gateway_proto_tx_t tx_data;
    hdl_i2c_message_t msg;
} atb3500_i2c_gateway_private_t;

hdl_module_state_t atb3500_i2c_gateway(void *desc, uint8_t enable) {
    return 0;
}
atb3500_i2c_gateway_proto_tx_t *atb3500_i2c_gateway_update(atb3500_i2c_gateway_t *desc, uint32_t recieved, atb3500_i2c_gateway_proto_rx_t *rx_data) {
    atb3500_i2c_gateway_private_t *i2c_gateway = (atb3500_i2c_gateway_private_t *)desc;
    if(rx_data->config_magic == ATB3500_I2C_GATEWAY_MAGIC) {
        i2c_gateway->msg.address = rx_data->address;
        i2c_gateway->msg.length = rx_data->length;
        i2c_gateway->msg.options = rx_data->options;
        uint32_t data_count = offsetof(atb3500_i2c_gateway_proto_rx_t, buff);
        if(data_count < recieved) {
            data_count = recieved - data_count;
            mem_cpy(i2c_gateway->tx_data.buff, rx_data->buff, data_count);
        }
            
    
    }
}


