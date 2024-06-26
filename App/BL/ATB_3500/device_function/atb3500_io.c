#include "device_logic.h"
#include "CodeLib.h"
typedef struct {
  atb3500_io_port_t set;
  atb3500_io_port_t reset;
} atb3500_io_proto_rx_t;

typedef struct {
  atb3500_io_port_t input;
  atb3500_io_port_t output;
} atb3500_io_proto_tx_t;

typedef struct {
    hdl_module_t module;
    /* private */
    proto_map_mem_t *map_rx;
    proto_map_mem_t *map_tx;
    atb3500_io_proto_tx_t tx_data;
    uint32_t epoch;
} atb3500_io_private_t;

_Static_assert(sizeof(atb3500_io_private_t) == sizeof(atb3500_io_t), "In atb3500_io.h data structure size of atb3500_io_t doesn't match, check ATB3500_IO_PRV_SIZE");

static hdl_gpio_pin_t *_atb3500_get_gpio_pin(atb3500_io_private_t *io, atb3500_io_port_t port) {
    uint32_t ind = 31 - __CLZ(port);
    if((io == NULL) || (ind >= ATB3500_IO_AMOUNT))
        return NULL;
    return (hdl_gpio_pin_t *)io->module.dependencies[ind + 1];
}

static uint8_t _atb3500_io_update_input(atb3500_io_private_t *io, atb3500_io_port_t port) {
    uint32_t in = io->tx_data.input;
    hdl_gpio_pin_t *gpio = _atb3500_get_gpio_pin(io, port);
    if(((gpio != NULL) && (hdl_gpio_read(gpio) != gpio->inactive_default)) ||
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
        hdl_gpio_write(gpio, active? !gpio->inactive_default: gpio->inactive_default);
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
        if(_atb3500_io_update_output(io, port, active) || _atb3500_io_update_input(io, port)) {
            bldl_communication_t *comm = (bldl_communication_t *)io->module.dependencies[0];
            communication_put(comm, io->map_tx, (void*)&io->map_tx);
        }
    }
}

uint32_t atb3500_io_proto_rx_size(void) {
    return sizeof(atb3500_io_proto_rx_t);
}

void atb3500_io_proto_set_map_rx(atb3500_io_t *desc, proto_map_mem_t *map) {
    atb3500_io_private_t *io = (atb3500_io_private_t*)desc;
    io->map_rx = map;
}

uint32_t atb3500_io_proto_tx_size(void) {
    return sizeof(atb3500_io_proto_tx_t);
}

void atb3500_io_proto_set_map_tx(atb3500_io_t *desc, proto_map_mem_t *map) {
    if((desc != NULL) && (map != NULL)) {
        // if init ok
        atb3500_io_private_t *io = (atb3500_io_private_t*)desc;
        io->map_tx = map;
        bldl_communication_t *comm = (bldl_communication_t *)io->module.dependencies[0];
        communication_put(comm, io->map_tx, (void*)&io->tx_data);
    }
}

static uint8_t _io_work(coroutine_desc_t this, uint8_t cancel, void *arg) {
    atb3500_io_private_t *io = (atb3500_io_private_t*)arg;
    bldl_communication_t *comm = (bldl_communication_t *)io->module.dependencies[0];
    uint32_t comm_ts = communication_epoch(comm);
    atb3500_io_proto_rx_t rx_data;
    uint8_t rx_valid = (io->epoch != comm_ts)? communication_get(comm, io->map_rx, (void*)&rx_data): HDL_FALSE;
    if(rx_valid) {
        uint32_t err = rx_data.set & rx_data.reset;
        rx_data.set &= ~err;
        rx_data.reset &= ~err;
        io->epoch = comm_ts;
    }
    atb3500_io_port_t port = 1;
    uint8_t changed = 0;
    while(port) {
        if(rx_valid && ((rx_data.set | rx_data.reset) & port)) {
            changed |= _atb3500_io_update_output(io, port, (rx_data.set & port) != 0);
        }
        changed |= _atb3500_io_update_input(io, port);
        port <<= 1;
    }
    if(changed) {
        communication_put(comm, io->map_tx, (void*)&io->tx_data);
    }
    return cancel;
}

hdl_module_state_t atb3500_io(void *desc, uint8_t enable) {
    static coroutine_desc_static_t io_task_buf;
    if(enable) {
        atb3500_io_private_t *io = (atb3500_io_private_t*)desc;
        uint32_t port = 1;
        while(port) {
            //TODO: check all
            hdl_gpio_pin_t *gpio = _atb3500_get_gpio_pin(desc, port);
            if(gpio != NULL && (hdl_gpio_read_output(gpio) != gpio->inactive_default) )
                io->tx_data.output |= port;
            else {
                io->tx_data.output &= ~port;
            }
            port <<= 1;
        }
        coroutine_add_static(&io_task_buf, &_io_work, (void *)desc);
        return HDL_MODULE_INIT_OK;
    }
    return HDL_MODULE_DEINIT_OK;
}
