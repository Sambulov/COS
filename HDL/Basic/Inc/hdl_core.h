
#ifndef HDL_CORE_H_
#define HDL_CORE_H_

typedef struct{
  hdl_hardware_t hw;
  uint32_t flash_lantency;
  //event_handler_t nmi_handler;
  //event_handler_t hard_fault_handler;
  //event_handler_t sv_call_handler;
  //event_handler_t pend_sv_handler;
  //event_handler_t systick_handler;
} hdl_core_t;

#define HDL_NVIC_PRV_SIZE            512

typedef struct {
  hdl_hardware_t hw;
  uint32_t prio_bits;
  uint8_t __private[HDL_NVIC_PRV_SIZE];
} hdl_nvic_t;

typedef struct {
  IRQn_Type irq_type;
  uint8_t priority_group;
  uint8_t priority;
} hdl_nvic_interrupt_t;

hdl_init_state_t hdl_core(void *desc, uint8_t enable);
hdl_init_state_t hdl_nvic(void *desc, uint8_t enable);

typedef hdl_nvic_t hdl_interrupt_controller_t;
typedef hdl_nvic_interrupt_t hdl_interrupt_t;

uint8_t hdl_interrupt_request(hdl_interrupt_controller_t *ic, hdl_interrupt_t *interrupt, event_handler_t handler, void *context);

#endif // HDL_CORE_H_
