#ifndef PORT_NVM_H_
#define PORT_NVM_H_

#define HDL_NVM_PRV_SIZE     32

typedef struct {
  uint32_t address_start;
  uint32_t size;
  uint32_t max_write_unit;
  uint32_t min_erase_unit;
} hdl_nvm_config_t;


#endif /* PORT_NVM_H_ */
