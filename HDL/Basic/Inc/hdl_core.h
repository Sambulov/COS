
#ifndef HDL_CORE_H_
#define HDL_CORE_H_

#include "port_core.h"

typedef struct{
  hdl_module_t module;
  const hdl_core_config_t *config;
} hdl_core_t;

typedef struct {
  hdl_module_t module;
  const hdl_interrupt_controller_config_t *config;
} hdl_interrupt_controller_t;

hdl_module_state_t hdl_core(void *desc, uint8_t enable);
hdl_module_state_t hdl_interrupt_controller(void *desc, uint8_t enable);
uint8_t hdl_interrupt_request(hdl_interrupt_controller_t *ic, const hdl_interrupt_t *irq, hdl_delegate_t *delegate);
void hdl_interrupt_sw_trigger(const hdl_interrupt_t *irq);

#endif // HDL_CORE_H_
