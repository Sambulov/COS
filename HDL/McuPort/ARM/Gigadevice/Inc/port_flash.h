#ifndef PORT_FLASH_H_
#define PORT_FLASH_H_

#include "hdl_nvm.h"

#define HDL_MCU_FLASH_VAR_SIZE     32

typedef struct {
  uint32_t page_address;
  uint32_t pages_amount;
  uint32_t page_size;
} hdl_flash_mcu_config_t;

hdl_module_new_t(hdl_flash_mcu_t, HDL_MCU_FLASH_VAR_SIZE, hdl_flash_mcu_config_t, hdl_nvm_iface_t);

extern const hdl_nvm_iface_t hdl_flash_mcu_iface;

#endif /* PORT_FLASH_H_ */
