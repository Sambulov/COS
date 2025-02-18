
#ifndef PORT_CORE_H_
#define PORT_CORE_H_

#include "hdl_core.h"
#include "port_core_spec.h"
#include "port_exti.h"

typedef struct {
  hdl_nvic_irq_n_t irq_type;
  uint8_t priority_group;
  uint8_t priority;
} hdl_interrupt_config_t;

typedef struct {
  hdl_interrupt_t * const *interrupts;
  uint8_t prio_bits;
  uint8_t irq_latency; /* processor ensures that a minimum of irq_latency+1 hclk cycles exist between an interrupt becoming pended */
  const void * const vector;
  uint32_t phy;
} hdl_interrupt_controller_config_t;

#define hdl_interrupts(...) ((hdl_interrupt_t * const []){__VA_ARGS__, NULL})

typedef struct{
  uint32_t flash_latency; /* WS_WSCNT_0: sys_clock <= 24MHz, WS_WSCNT_1: sys_clock <= 48MHz, WS_WSCNT_2: sys_clock <= 72MHz */
  uint32_t phy;
} hdl_core_config_t;

extern const void *_estack;
extern const void *_sidata, *_sdata, *_edata;
extern const void *_sbss, *_ebss;
extern const void *_eflash;

void call_isr(hdl_nvic_irq_n_t irq, uint32_t event);

hdl_module_new_t(hdl_core_t, 0, hdl_core_config_t, hdl_module_base_iface_t);
hdl_module_new_t(hdl_interrupt_controller_t, 0, hdl_interrupt_controller_config_t, hdl_interrupt_controller_iface_t);

extern const hdl_module_base_iface_t hdl_core_iface;
extern const hdl_interrupt_controller_iface_t hdl_interrupt_controller_iface;

#endif /* PORT_CORE_H_ */ 
