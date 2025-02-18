#include "hdl_iface.h"

#if defined(__CORE_CM23_H_DEPENDANT)

void nmi_handler()                      { call_isr(HDL_NVIC_EXCEPTION_NonMaskableInt, 0); }
void hard_fault_handler()               { call_isr(HDL_NVIC_EXCEPTION_HardFault, 0); }
void pend_sv_handler()                  { call_isr(HDL_NVIC_EXCEPTION_PendSV, 0); }
void systick_handler()                  { call_isr(HDL_NVIC_EXCEPTION_SysTick, 0); }

#endif
