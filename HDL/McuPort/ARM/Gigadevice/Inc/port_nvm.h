#ifndef PORT_NVM_H_
#define PORT_NVM_H_

#include "hdl_nvm.h"

#define HDL_MCU_FLASH_VAR_SIZE     32

typedef struct {
  uint32_t page_address;
  uint32_t pages_amount;
  uint32_t page_size;
} hdl_mcu_flash_config_t;

hdl_module_new_t(hdl_mcu_flash_t, HDL_MCU_FLASH_VAR_SIZE, hdl_mcu_flash_config_t, hdl_nvm_iface_t);

extern hdl_nvm_iface_t mcu_flash_iface;

#endif /* PORT_NVM_H_ */
