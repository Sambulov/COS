#include "app.h"
#include "CodeLib.h"

typedef struct {
    hdl_module_t module;
    /* private */
    atb3500_io_proto_tx_t tx_data;
} atb3500_io_private_t;

_Static_assert(sizeof(atb3500_io_private_t) == sizeof(atb3500_io_t), "In atb3500_io.h data structure size of atb3500_io_t doesn't match, check ATB3500_IO_PRV_SIZE");

static hdl_gpio_pin_t *_atb3500_get_gpio_pin(atb3500_io_private_t *io, atb3500_io_port_t port) {
    uint32_t ind = 31 - __CLZ(port);
    if((io == NULL) || (ind >= ATB3500_IO_AMOUNT))
        return NULL;
    return (hdl_gpio_pin_t *)io->module.dependencies[ind];
}

static uint8_t _atb3500_io_update_input(atb3500_io_private_t *io, atb3500_io_port_t port) {
    uint32_t in = io->tx_data.input;
    hdl_gpio_pin_t *gpio = _atb3500_get_gpio_pin(io, port);
    if(((gpio != NULL) && hdl_gpio_is_active(gpio)) ||
       ((gpio == NULL) && (io->tx_data.output & port))) {
        io->tx_data.input |= port;
    }
    else {
        io->tx_data.input &= ~port;
    }
    return (in != io->tx_data.input);
}

static uint8_t _atb3500_io_update_output(atb3500_io_private_t *io, atb3500_io_port_t port, uint8_t active) {
    hdl_gpio_pin_t *gpio = _atb3500_get_gpio_pin(io, port);
    uint32_t out = io->tx_data.output;
    if(gpio != NULL) {
        active? hdl_gpio_set_active(gpio): hdl_gpio_set_inactive(gpio);
    }
    if(active) {
        io->tx_data.output |= port;
    }
    else {
        io->tx_data.output &= ~port;
    }
    return (out != io->tx_data.output);
}

void atb3500_io_set(atb3500_io_t *desc, atb3500_io_port_t port, uint8_t active) {
    if(desc != NULL) {
        atb3500_io_private_t *io = (atb3500_io_private_t *)desc;
        _atb3500_io_update_output(io, port, active);
        _atb3500_io_update_input(io, port);
    }
}

hdl_module_state_t atb3500_io(void *desc, uint8_t enable) {
    if(enable) {
        atb3500_io_private_t *io = (atb3500_io_private_t*)desc;
        uint32_t port = 1;
        while(port) {
            //TODO: check all
            hdl_gpio_pin_t *gpio = _atb3500_get_gpio_pin(desc, port);
            if(gpio != NULL && hdl_gpio_is_active(gpio))
                io->tx_data.output |= port;
            else {
                io->tx_data.output &= ~port;
            }
            port <<= 1;
        }
        return HDL_MODULE_ACTIVE;
    }
    return HDL_MODULE_UNLOADED;
}

atb3500_io_proto_tx_t *atb3500_io_update(atb3500_io_t *desc, atb3500_io_proto_rx_t *rx_data) {
    atb3500_io_private_t *io = (atb3500_io_private_t*)desc;
    uint32_t err = rx_data->set & rx_data->reset;
    rx_data->set &= ~err;
    rx_data->reset &= ~err;
    atb3500_io_port_t port = 1;
    while(port) {
        if((rx_data->set | rx_data->reset) & port) {
            _atb3500_io_update_output(io, port, (rx_data->set & port) != 0);
        }
        _atb3500_io_update_input(io, port);
        port <<= 1;
    }
    rx_data->set = 0;
    rx_data->reset = 0;
    return &io->tx_data;
}
