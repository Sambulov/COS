
#ifndef HDL_CORE_H_
#define HDL_CORE_H_

typedef struct{
  hdl_module_t module;
  uint32_t flash_latency;
} hdl_core_t;

#define HDL_INTERRUPT_PRV_SIZE       9

typedef struct {
  IRQn_Type irq_type;
  uint8_t priority_group;
  uint8_t priority;
  uint8_t __private[HDL_INTERRUPT_PRV_SIZE];
} hdl_nvic_interrupt_t;

#define hdl_interrupts(...) ((hdl_nvic_interrupt_t *[]){__VA_ARGS__, NULL})

typedef struct {
  hdl_module_t module;
  uint32_t prio_bits;
  hdl_nvic_interrupt_t **interrupts;
} hdl_nvic_t;

hdl_module_state_t hdl_core(void *desc, uint8_t enable);
hdl_module_state_t hdl_nvic(void *desc, uint8_t enable);

typedef hdl_nvic_t hdl_interrupt_controller_t;
typedef hdl_nvic_interrupt_t hdl_interrupt_t;

uint8_t hdl_interrupt_request(hdl_interrupt_controller_t *ic, hdl_interrupt_t *interrupt, event_handler_t handler, void *context);

#endif // HDL_CORE_H_
