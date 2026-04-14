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
  size_t (*mod_var)[((HDL_MODULE_VAR_SIZE + (sizeof(size_t) - 1)) >> 2)];
  void *obj_var;
  void *config;
} hdl_module_base_t;

#define hdl_module_dependencies(...) ((const void * const []){__VA_ARGS__, NULL})

#define hdl_module_config(type, ...) (const type []){{__VA_ARGS__}}

#define hdl_module_new_t(name, var_size, config_t, iface_t) \
  typedef struct { \
    const iface_t *iface; \
    const void * const *dependencies; \
    size_t (*mod_var)[((HDL_MODULE_VAR_SIZE + (sizeof(size_t) - 1)) >> 2)];\
    size_t (*obj_var)[((var_size + (sizeof(size_t) - 1)) >> 2)];\
    const config_t config; \
  } name;

#define hdl_module_full_size(mod_type)          (sizeof(mod_type) + sizeof(*((mod_type *)0)->mod_var) + sizeof(*((mod_type *)0)->obj_var))
#define hdl_module_sizeof_mod_var(mod_type)     (sizeof(*((mod_type *)0)->mod_var))
#define hdl_module_sizeof_obj_var(mod_type)     (sizeof(*((mod_type *)0)->obj_var))

#define MODULE_ASSERT(desc, err_res)        if(!(desc) || hdl_is_null_module(desc) || (hdl_state(desc) == HDL_MODULE_FAULT)) return err_res;
#define MODULE_ASSERT_FAST(desc, err_res)   if(!(desc) || hdl_is_null_module(desc)) return err_res;

__STATIC_INLINE uint8_t hdl_is_null_module(const void *desc) {
  hdl_module_base_t *mod = (hdl_module_base_t *)desc;
  return (mod->iface == NULL) && (mod->config == NULL) && (mod->dependencies == NULL);
}

uint8_t hdl_module_take(const void *desc, const void *owner);
uint8_t hdl_module_give(const void *desc, const void *owner);
void hdl_module_enable(const void *desc);
void hdl_module_kill(const void *desc);
hdl_module_state_t hdl_module_state(const void *desc);

/* Legacy */
uint8_t hdl_init_complete();
#define hdl_take hdl_module_take
#define hdl_give hdl_module_give
#define hdl_enable hdl_module_enable
#define hdl_kill hdl_module_kill
#define hdl_state hdl_module_state

#endif // HDL_MODULE_H_
