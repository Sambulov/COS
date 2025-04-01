#ifndef HDL_NVM_H_
#define HDL_NVM_H_

typedef enum {
  HDL_NVM_OPTION_READ            = 0x01,
  HDL_NVM_OPTION_WRITE           = 0x02,
  HDL_NVM_OPTION_ERASE           = 0x04,

  HDL_NVM_OPTION_VALIDATE        = 0x08,
} hdl_nvm_message_options_t;

typedef enum {
  HDL_NVM_STATE_BUSY             = 0x01,
  HDL_NVM_STATE_CANCELED         = 0x02,
  HDL_NVM_STATE_COMPLETE         = 0x08,
  HDL_NVM_STATE                  = 0x0F,
  HDL_NVM_ERROR_VALIDATION_FAULT = 0x10,
  HDL_NVM_ERROR_OUT_OF_RANGE     = 0x20,
  HDL_NVM_ERROR_LOCKED           = 0x30,
  HDL_NVM_ERROR_PAGE_UNALIGNED   = 0x40,
  HDL_NVM_ERROR_BUS_FAULT        = 0xD0,
  HDL_NVM_ERROR_BAD_ARG          = 0xE0,
  HDL_NVM_ERROR_INTERNAL_FAULT   = 0xF0,
  HDL_NVM_ERROR                  = 0xF0,
} hdl_nvm_message_status_t;

typedef struct {
  uint8_t *buffer;
  uint32_t size;
  uint32_t transferred;
  uint32_t address;
  hdl_nvm_message_options_t options;
  hdl_nvm_message_status_t status;
} hdl_nvm_message_t;

typedef struct {
  uint32_t volume;
  uint32_t page_size;
} hdl_nvm_info_t;

typedef uint8_t (* hdl_nvm_transfer_t)(const void *desc, hdl_nvm_message_t *message);
typedef uint8_t (* hdl_nvm_info_get_t)(const void *desc, hdl_nvm_info_t *out_info);

typedef struct{
  hdl_module_initializer_t init;
  hdl_nvm_info_get_t info;
  hdl_nvm_transfer_t transfer;
} hdl_nvm_iface_t;

hdl_module_new_t(hdl_nvm_t, 0, void, hdl_nvm_iface_t);

__STATIC_INLINE uint8_t hdl_nvm_transfer(const void *desc, hdl_nvm_message_t *message) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_nvm_iface_t *)((hdl_module_base_t *)desc)->iface)->transfer(desc, message);
}

__STATIC_INLINE uint8_t hdl_nvm_info_get(const void *desc, hdl_nvm_info_t *out_info) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_nvm_iface_t *)((hdl_module_base_t *)desc)->iface)->info(desc, out_info);
}

#endif /* HDL_NVM_H_ */
