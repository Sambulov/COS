#ifndef BLDL_COMMUNICATION_H_
#define BLDL_COMMUNICATION_H_

#define BLDL_COMMUNICATION_PRV_SIZE    8
#define BLDL_COMMUNICATION_MAP_PRV_SIZE    16
/*
  depends on:
  spi_mem
*/
typedef struct {
    hdl_module_t module;
    PRIVATE(bldl, BLDL_COMMUNICATION_PRV_SIZE);
} bldl_communication_t;

typedef struct {
    uint32_t offset;
    uint32_t size;
    PRIVATE(bldl, BLDL_COMMUNICATION_MAP_PRV_SIZE);
} communication_mem_map_t;

hdl_module_state_t communication(void *desc, uint8_t enable);

uint8_t communication_map_rx(bldl_communication_t *desc, communication_mem_map_t *map);
uint8_t communication_map_tx(bldl_communication_t *desc, communication_mem_map_t *map);

uint8_t communication_get(bldl_communication_t *desc, communication_mem_map_t *map, void *object);
uint8_t communication_put(bldl_communication_t *desc, communication_mem_map_t *map, void *object);

#endif //BLDL_COMMUNICATION_H_