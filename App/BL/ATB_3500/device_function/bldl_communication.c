#include "device_logic.h"
#include "CodeLib.h"

typedef struct {
    hdl_module_t module;
    /* private */
    linked_list_t map_rx;
    linked_list_t map_tx;
} bldl_communication_private_t;

typedef struct {
    uint32_t offset;
    uint32_t size;
    /* private */
   __linked_list_object__;
} proto_map_mem_private_t;

_Static_assert(sizeof(bldl_communication_private_t) == sizeof(bldl_communication_t), "In bldl_communication.h data structure size of bldl_communication_t doesn't match, check BLDL_COMMUNICATION_PRV_SIZE");
_Static_assert(sizeof(proto_map_mem_private_t) == sizeof(proto_map_mem_t), "In bldl_communication.h data structure size of proto_map_mem_t doesn't match, check BLDL_COMMUNICATION_MAP_PRV_SIZE");

static uint8_t _map_rx_tx(bldl_communication_t *desc, proto_map_mem_t *map, uint8_t rx) {
    bldl_communication_private_t *comm = (bldl_communication_private_t*)desc;
    proto_map_mem_private_t *mem_map = (proto_map_mem_private_t*)map;
    //TODO: check intersection
    if(rx) {
        linked_list_insert_last(&comm->map_rx, linked_list_item(mem_map));
    }
    else {
        linked_list_insert_last(&comm->map_tx, linked_list_item(mem_map));
    }
    return HDL_TRUE;
}


uint8_t communication_map_rx(bldl_communication_t *desc, proto_map_mem_t *map) {
    return _map_rx_tx(desc, map, 1);
}

uint8_t communication_map_tx(bldl_communication_t *desc, proto_map_mem_t *map) {
    return _map_rx_tx(desc, map, 0);
}

static uint8_t _get_put(bldl_communication_t *desc, proto_map_mem_t *map, void *object, uint8_t get) {
    if((desc == NULL) || (map == NULL))
        return HDL_FALSE;
    bldl_communication_private_t *comm = (bldl_communication_private_t*)desc;
    proto_map_mem_private_t *mem_map = (proto_map_mem_private_t*)map;
    //TODO: check if comm contains map
    hdl_basic_buffer_t buf = {
        .data = object,
        .size = map->size
    };
    hdl_spi_mem_server_t *spi = (hdl_spi_mem_server_t *)comm->module.dependencies[0];
    return get? hdl_spi_mem_rx_buffer_take(spi, &buf, map->offset): hdl_spi_mem_tx_buffer_put(spi, &buf, map->offset);
}

uint32_t communication_epoch(bldl_communication_t *desc) {
    hdl_spi_mem_server_t *spi = (hdl_spi_mem_server_t *)desc->module.dependencies[0];
    return hdl_spi_mem_buffer_epoch(spi);
}

uint8_t communication_get(bldl_communication_t *desc, proto_map_mem_t *map, void *object) {
    return _get_put(desc, map, object, 1);
}

uint8_t communication_put(bldl_communication_t *desc, proto_map_mem_t *map, void *object) {
    return _get_put(desc, map, object, 0);
}

hdl_module_state_t communication(void *desc, uint8_t enable) {
    bldl_communication_private_t *comm = (bldl_communication_private_t*)desc;
    if(enable) {
        comm->map_rx = NULL;
        comm->map_tx = NULL;
        return HDL_MODULE_INIT_OK;
    }
    return HDL_MODULE_DEINIT_OK;
}
