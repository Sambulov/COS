
#ifndef HDL_CORE_H_
#define HDL_CORE_H_

hdl_module_new_t(hdl_core_t, 0, void, hdl_module_base_iface_t);

uint8_t hdl_core_is_in_isr();

typedef struct {
  hdl_event_t event;
  const void *irq_cnf;
} hdl_interrupt_t;

typedef uint32_t (*hdl_interrupt_get_t)(void);
typedef uint8_t (*hdl_interrupt_request_t)(const void *, const hdl_interrupt_t *);
typedef void (*hdl_interrupt_sw_trigger_t)(const void *, const hdl_interrupt_t *);

typedef struct {
  hdl_module_initializer_t init;
  hdl_interrupt_request_t request;
  hdl_interrupt_sw_trigger_t trigger;
  hdl_interrupt_get_t current_int;
} hdl_interrupt_controller_iface_t;

hdl_module_new_t(hdl_interrupt_controller_t, 0, void, hdl_interrupt_controller_iface_t);

__STATIC_INLINE uint8_t hdl_interrupt_request(const void *desc, const hdl_interrupt_t *isr) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_interrupt_controller_t *)desc)->iface->request(desc, isr);
}

__STATIC_INLINE void hdl_interrupt_sw_trigger(const void *desc, const hdl_interrupt_t *isr) {
  MODULE_ASSERT(desc, );
  ((hdl_interrupt_controller_t *)desc)->iface->trigger(desc, isr);
}

#endif // HDL_CORE_H_
