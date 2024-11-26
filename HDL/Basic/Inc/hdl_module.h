#ifndef HDL_MODULE_H_
#define HDL_MODULE_H_

#define HDL_MODULE_DESC_PRIVATE_SIZE 20

typedef enum {
  HDL_MODULE_LOADING   = 1,
  HDL_MODULE_ACTIVE    = 0,
  HDL_MODULE_UNLOADING = -1,
  HDL_MODULE_FAULT     = -2,
  HDL_MODULE_UNLOADED  = HDL_MODULE_FAULT
} hdl_module_state_t;

typedef hdl_module_state_t (*hdl_module_initializer_t)(void *desc, uint8_t enable);

typedef struct _HDL_MODULE_T_ {
  hdl_module_initializer_t init;
  struct _HDL_MODULE_T_ **dependencies;
  void *reg;
  PRIVATE(hdl, HDL_MODULE_DESC_PRIVATE_SIZE);
} hdl_module_t;

extern hdl_module_t hdl_null_module;

#define HDL_IS_NULL_MODULE(mod)      ((void*)(mod) == (void*)&hdl_null_module)

#define hdl_module_dependencies(...) ((hdl_module_t *[]){__VA_ARGS__, NULL})

#define hdl_module_config(type, ...) (const type []){{__VA_ARGS__}}

void hdl_enable(hdl_module_t *desc);
void hdl_kill(hdl_module_t *desc);
hdl_module_state_t hdl_state(const hdl_module_t *desc);
uint8_t hdl_init_complete();

#endif // HDL_MODULE_H_
