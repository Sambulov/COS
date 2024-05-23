
#ifndef HDL_CORE_H_
#define HDL_CORE_H_

#include "port_core.h"

hdl_module_state_t hdl_core(void *desc, uint8_t enable);
hdl_module_state_t hdl_nvic(void *desc, uint8_t enable);
uint8_t hdl_interrupt_request(hdl_interrupt_controller_t *ic, hdl_irq_n_t irq, hdl_delegate_t *delegate);
void hdl_interrupt_sw_trigger(hdl_irq_n_t interrupt);

#endif // HDL_CORE_H_
