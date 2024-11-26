#ifndef APP_H_
#define APP_H_

#include "hdl.h"

#include "ms5194t.h"
#include "uspd_ain_port.h"
#include "som_link.h"

#include "mig.h"

typedef struct {
  uint8_t *data_ptr;
  uint16_t map_addr_start;
  uint16_t map_addr_end;
} app_mem_map_item_t;

#define mem_map_item(data, addr, offset)     {.data_ptr = (uint8_t*)&data, .map_addr_start = addr + offset, .map_addr_end = addr + sizeof(data) - 1}


typedef struct {
  uint32_t sync_key;
} app_circuit_config_t;

#define CIRCUIT_CONFIG_SYNC_KEY       0x12345678

typedef struct {
  uint16_t current_address;
  uint8_t state;
} som_proto_context_t;

#define MAP_ADDR_CIRCUIT_CONFIG      0x1000


extern hdl_module_t mod_app;


#define APP_R_MEM_MAP_SIZE       1
#define APP_W_MEM_MAP_SIZE       1

extern const app_mem_map_item_t app_r_mem_map[APP_R_MEM_MAP_SIZE];
extern const app_mem_map_item_t app_w_mem_map[APP_W_MEM_MAP_SIZE];

void main();

#endif /* APP_H_ */
