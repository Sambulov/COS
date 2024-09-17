#ifndef USPD_SOM_LINK_H_
#define USPD_SOM_LINK_H_

uint8_t bldl_som_link_read_byte_cb(uint16_t addr);
void bldl_som_link_write_byte_cb(uint16_t addr, uint8_t data);

void bldl_som_link_init();

#endif /* USPD_SOM_LINK_H_ */