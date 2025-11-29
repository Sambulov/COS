#ifndef HDL_ENTROPY_H_
#define HDL_ENTROPY_H_

typedef uint32_t (*hdl_entropy_poll_t)(const void *desc, void *output, uint32_t amount);

typedef struct {
  hdl_module_initializer_t init;
  hdl_entropy_poll_t poll;
} hdl_entropy_iface_t;

hdl_module_new_t(hdl_entropy_t, 0, void*, hdl_entropy_iface_t);

__STATIC_INLINE uint32_t hdl_entropy_poll(const void *desc, void *output, uint32_t amount) {
  MODULE_ASSERT(desc, 0);
  return ((hdl_entropy_iface_t *)((hdl_module_base_t *)desc)->iface)->poll(desc, output, amount);
}

#endif /* HDL_ENTROPY_H_ */