#include "app.h"

typedef struct {
  uint8_t *mem_ptr;
  uint16_t map_addr;
  uint16_t size;
} mem_map_block_t;

#define mem_map_block(ptr, addr, sz)                   (&((mem_map_block_t){.mem_ptr = (uint8_t *)(ptr) , .map_addr = (addr), .size = (sz)}))
#define mem_map_struct(ptr, addr, type)                mem_map_block(ptr, addr, sizeof(type))
#define mem_map_struct_part(ptr, addr, type, from, to) mem_map_block(((uint8_t *)ptr) + offsetof(type, from), addr + offsetof(type, from), offsetof(circuit_config_t, to) - offsetof(circuit_config_t, from) + sizeof(((type *)(ptr))->to))
#define mem_map_array(ptr, addr, type, count)          mem_map_block(ptr, addr, (count)  *sizeof(type))

// mem_map_block_t *read_mem_map[] = {
//   mem_map_array(&measures, MAP_ADDR_MEASURES, measure_t, MEASURES_AMOUNT),
//   mem_map_struct(&ai_circuit_config, MAP_ADDR_CIRCUIT_CONFIG, circuit_config_t),
//   NULL
// };

// mem_map_block_t *write_mem_map[] = {
//   mem_map_struct_part(&ai_circuit_config, MAP_ADDR_CIRCUIT_CONFIG, circuit_config_t, user, sync_key),
//   NULL
// };

// void define_mem_block(som_proto_context_t *proto) {
//   mem_map_block_t **mem_alloc = read_mem_map;
//   while (*mem_alloc != NULL) {
//     if(proto->current_address >= (*mem_alloc)->map_addr) {
//       if(((*mem_alloc)->size > (proto->current_address - (*mem_alloc)->map_addr))) {
//         proto->current_block.mem_ptr = (*mem_alloc)->mem_ptr;
//         proto->current_block.size = (*mem_alloc)->size;
//         proto->current_block.map_addr = (*mem_alloc)->map_addr;
//         return;
//       }
//       else {
//         proto->current_block.mem_ptr = NULL;
//         proto->current_block.map_addr = (*mem_alloc)->map_addr + (*mem_alloc)->size;
//       }
//     }
//     else {
//       proto->current_block.size = proto->current_address - (*mem_alloc)->map_addr;
//       return;
//     }
//     mem_alloc++;
//   }
//   proto->current_block.size = proto->current_address - read_mem_map[0]->map_addr;
// }

// uint32_t som_trunsmitter(void *context, uint8_t *data, uint32_t length) {
//   som_proto_context_t *proto = (som_proto_context_t *)context;
//   uint16_t mem_end = 2048;
//   if(((proto->current_block.mem_ptr == NULL) && 
//      (proto->current_block.size == 0)) || 
//      (proto->current_address >= (proto->current_block.map_addr+proto->current_block.size)))
//     define_mem_block(proto);
//   if(proto->current_block.mem_ptr != NULL) {
//     *data = proto->current_block.mem_ptr[proto->current_address - proto->current_block.map_addr];
//   }
//   else {
//     *data = 0;
//   }
//   proto->current_address++;
//   return 1;
// }

#if DEBUG
void assert_mem_map() {

}
#endif
