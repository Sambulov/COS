
#ifndef PORT_CORE_H_
#define PORT_CORE_H_

#include "hdl_core.h"
#include "port_core_spec.h"

#define HDL_NVIC_VAR_SIZE        4
#define HDL_NVIC_IRQ_VAR_SIZE    8

typedef struct {
  hdl_nvic_irq_n_t irq_type;
  uint8_t priority_group;
  uint8_t priority;
  uint32_t event_mask;
  uint32_t event_id;
} hdl_nvic_irq_config_t;

typedef struct {
  uint8_t prio_bits;
  uint8_t irq_latency; /* processor ensures that a minimum of irq_latency+1 hclk cycles exist between an interrupt becoming pended */
  const void * const vector;
  void *phy;
} hdl_nvic_config_t;

extern const void *_sram;
extern const void *_estack;
extern const void *_sidata, *_sdata, *_edata;
extern const void *_sbss, *_ebss;
extern const void *_eflash;

void call_isr(hdl_nvic_irq_n_t irq, void *event_trigger);

hdl_module_new_t(hdl_core_arm_t, 0, hdl_core_config_t*, hdl_module_base_iface_t);
hdl_module_new_t(hdl_nvic_t, HDL_NVIC_VAR_SIZE, hdl_nvic_config_t*, hdl_module_base_iface_t);
hdl_module_new_t(hdl_nvic_irq_t, HDL_NVIC_IRQ_VAR_SIZE, hdl_nvic_irq_config_t*, hdl_interrupt_iface_t);

extern const hdl_module_base_iface_t hdl_core_arm_iface;
extern const hdl_module_base_iface_t hdl_nvic_iface;
extern const hdl_interrupt_iface_t hdl_nvic_irq_iface;

void irq_n_handler();
void reset_handler();

void nmi_handler();
void hard_fault_handler();
void pend_sv_handler();
void systick_handler();
void mem_manage_handler();
void bus_fault_handler();
void usage_fault_handler();
void debug_mon_handler();
void svc_handler();

#endif /* PORT_CORE_H_ */ 
