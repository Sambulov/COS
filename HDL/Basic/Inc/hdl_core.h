
#ifndef HDL_CORE_H_
#define HDL_CORE_H_

#include "port_core.h"

hdl_module_new_t(hdl_core_t, 0, hdl_core_config_t, hdl_module_base_iface_t);

typedef uint8_t (*hdl_interrupt_request_t)(const void *, const hdl_interrupt_t *);
typedef void (*hdl_interrupt_sw_trigger_t)(const void *, const hdl_interrupt_t *);

typedef struct {
  hdl_module_initializer_t init;
  hdl_interrupt_request_t request;
  hdl_interrupt_sw_trigger_t trigger;
} hdl_interrupt_controller_iface_t;

hdl_module_new_t(hdl_interrupt_controller_t, 0, hdl_interrupt_controller_config_t, hdl_interrupt_controller_iface_t);

extern const hdl_module_base_iface_t hdl_core_iface;
extern const hdl_interrupt_controller_iface_t hdl_interrupt_controller_iface;

__STATIC_INLINE uint8_t hdl_interrupt_request(const void *ic, const hdl_interrupt_t *isr) {
  return ((hdl_interrupt_controller_iface_t *)((hdl_module_base_t *)ic)->iface)->request(ic, isr);
}

__STATIC_INLINE void hdl_interrupt_sw_trigger(const void *ic, const hdl_interrupt_t *isr) {
  ((hdl_interrupt_controller_iface_t *)((hdl_module_base_t *)ic)->iface)->trigger(ic, isr);
}

#endif // HDL_CORE_H_
