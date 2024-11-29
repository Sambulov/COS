#ifndef USPD_SOM_LINK_H_
#define USPD_SOM_LINK_H_

typedef struct {
  uint8_t *data_ptr;
  uint16_t map_addr_start;
  uint16_t map_addr_end;
} app_mem_map_item_t;

#define mem_map_item(data, addr, offset)     {.data_ptr = (uint8_t*)&data, .map_addr_start = addr + offset, .map_addr_end = addr + sizeof(data) - 1}

uint8_t bldl_som_link_read_byte_cb(uint16_t addr);
void bldl_som_link_write_byte_cb(uint16_t addr, uint8_t data);

void bldl_som_link_init();

#endif /* USPD_SOM_LINK_H_ */