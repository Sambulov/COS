#ifndef HDL_MODULE_H_
#define HDL_MODULE_H_

#define HDL_MODULE_VAR_SIZE              28

typedef enum {
  HDL_MODULE_LOADING   = 1,
  HDL_MODULE_ACTIVE    = 0,
  HDL_MODULE_UNLOADING = -1,
  HDL_MODULE_FAULT     = -2,
  HDL_MODULE_UNLOADED  = HDL_MODULE_FAULT
} hdl_module_state_t;

typedef hdl_module_state_t (*hdl_module_initializer_t)(const void *desc, uint8_t enable);

typedef struct {
  hdl_module_initializer_t init;
} hdl_module_base_iface_t;

typedef struct {
  const hdl_module_base_iface_t *iface;
  const void * const *dependencies;
  void *config;
  size_t (*mod_var)[((HDL_MODULE_VAR_SIZE + (sizeof(size_t) - 1)) >> 2)];
} hdl_module_base_t;

#define hdl_module_dependencies(...) ((const void * const []){__VA_ARGS__, NULL})

#define hdl_module_config(type, ...) (const type []){{__VA_ARGS__}}

#define hdl_module_new_t(name, var_size, config_t, iface_t) \
  typedef struct { \
    const iface_t *iface; \
    const void * const *dependencies; \
    const config_t *config; \
    size_t (*mod_var)[((HDL_MODULE_VAR_SIZE + (sizeof(size_t) - 1)) >> 2)];\
    size_t (*obj_var)[((var_size + (sizeof(size_t) - 1)) >> 2)];\
  } name;

#define MODULE_ASSERT(desc, err_res)        if(!(desc) || hdl_is_null_module(desc) || (hdl_state(desc) == HDL_MODULE_FAULT)) return err_res;
#define MODULE_ASSERT_FAST(desc, err_res)   if(!(desc) || hdl_is_null_module(desc)) return err_res;

__STATIC_INLINE uint8_t hdl_is_null_module(const void *desc) {
  hdl_module_base_t *mod = (hdl_module_base_t *)desc;
  return (mod->iface == NULL) && (mod->config == NULL) && (mod->dependencies == NULL);
}

uint8_t hdl_take(const void *desc, const void *owner);
uint8_t hdl_give(const void *desc, const void *owner);
void hdl_enable(const void *desc);
void hdl_kill(const void *desc);
hdl_module_state_t hdl_state(const void *desc);
uint8_t hdl_init_complete();

#endif // HDL_MODULE_H_
