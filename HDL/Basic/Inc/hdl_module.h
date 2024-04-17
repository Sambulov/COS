#ifndef HDL_MODULE_H_
#define HDL_MODULE_H_

#define HDL_MODULE_DESC_PRIVATE_SIZE 20

typedef enum {
  HDL_MODULE_INIT_ONGOING     = 1,
  HDL_MODULE_INIT_OK          = 0,
  HDL_MODULE_INIT_TERMINATING = -1,
  HDL_MODULE_INIT_FAILED      = -2,
  HDL_MODULE_INIT_UNKNOWN     = HDL_MODULE_INIT_FAILED,
  HDL_MODULE_DEINIT_OK        = HDL_MODULE_INIT_FAILED
} hdl_module_state_t;

typedef hdl_module_state_t (*hdl_module_initializer_t)(void *desc, uint8_t enable);

typedef struct _HDL_MODULE_T_ {
  uint8_t __private[HDL_MODULE_DESC_PRIVATE_SIZE];
  hdl_module_initializer_t init;
  struct _HDL_MODULE_T_ **dependencies;
  void *reg;
} hdl_module_t;

#define hdl_module_dependencies(...) ((hdl_module_t *[]){__VA_ARGS__, NULL})

void hdl_enable(hdl_module_t *desc);
void hdl_kill(hdl_module_t *desc);
hdl_module_state_t hdl_state(hdl_module_t *desc);

#endif // HDL_MODULE_H_