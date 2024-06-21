#include "device_logic.h"
 
static hdl_gpio_pin_t *_get_gpio_pin(atb3500_io_t *desc, atb3500_io_port_t port) {
    //TODO: check if null module, ind valid
    uint32_t ind = 31 - __CLZ(port);
    if(ind >= ATB3500_IO_AMOUNT)
        return NULL;
    return (hdl_gpio_pin_t *)desc->module.dependencies[ind + 1];
}

typedef struct {
  uint32_t input;
  uint32_t output;
  uint32_t dir;
} atb3500_io_proto_t;

uint32_t atb3500_io_proto_size(void) {
    return sizeof(atb3500_io_proto_t);
}

typedef struct {
    hdl_module_t module;
    /* private */
    proto_map_mem_t *map_rx;
    proto_map_mem_t *map_tx;
    atb3500_io_proto_t data;
    uint32_t epoch;
} atb3500_io_private_t;

void atb3500_io_set(atb3500_io_t *desc, atb3500_io_port_t port, uint8_t active) {
    //TODO: check all
    hdl_gpio_pin_t *gpio = _get_gpio_pin(desc, port);
    if(gpio != NULL)
        hdl_gpio_write(gpio, active? !gpio->inactive_default: gpio->inactive_default);
}


uint32_t atb3500_io_proto_rx_size(void) {
    return sizeof(atb3500_io_proto_t);
}

void atb3500_io_proto_set_map_rx(atb3500_io_t *desc, proto_map_mem_t *map) {
    atb3500_io_private_t *io = (atb3500_io_private_t*)desc;
    io->map_rx = map;
}

uint32_t atb3500_io_proto_tx_size(void) {
    return sizeof(atb3500_io_proto_t);
}

void atb3500_io_proto_set_map_tx(atb3500_io_t *desc, proto_map_mem_t *map) {
    atb3500_io_private_t *io = (atb3500_io_private_t*)desc;
    io->map_tx = map;
}

static uint8_t _io_work(coroutine_desc_t this, uint8_t cancel, void *arg) {
    atb3500_io_private_t *io = (atb3500_io_private_t*)arg;
    bldl_communication_t *comm = (bldl_communication_t *)io->module.dependencies[0];
    uint32_t comm_ts = communication_epoch(comm);
    if(io->epoch != comm_ts) {
        io->epoch != comm_ts;
        if(communication_get(comm, io->map_rx, (void*)&io->data)) {
            uint32_t port = 1;
            while(port) {
                atb3500_io_set((atb3500_io_t *)io, (atb3500_io_port_t)port, (io->data.output & port) != 0);
                //TODO: read input
                io->data.input = io->data.output;
                port <<= 1;
            }
            communication_put(comm, io->map_tx, (void*)&io->data);
        }
    }
    return cancel;
}

hdl_module_state_t atb3500_io(void *desc, uint8_t enable) {
    static coroutine_desc_static_t io_task_buf;
    if(enable) {
        atb3500_io_private_t *io = (atb3500_io_private_t*)desc;
        io->data.dir = ATB3500_IO_ALL;
        coroutine_add_static(&io_task_buf, &_io_work, (void *)desc);
        return HDL_MODULE_INIT_OK;
    }
    return HDL_MODULE_DEINIT_OK;
}
