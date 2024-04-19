
#ifndef HDL_CORE_H_
#define HDL_CORE_H_

typedef struct{
  hdl_module_t module;
  uint32_t flash_latency;
} hdl_core_t;

#define HDL_INTERRUPT_PRV_SIZE       9

typedef IRQn_Type hdl_irq_n_t;

typedef struct {
  hdl_irq_n_t irq_type;
  uint8_t priority_group;
  uint8_t priority;
  uint8_t __private[HDL_INTERRUPT_PRV_SIZE];
} hdl_nvic_interrupt_t;

typedef enum {
  HDL_EXTI_TRIGGER_FALLING        = 0x01,
  HDL_EXTI_TRIGGER_RISING         = 0x02,
  HDL_EXTI_TRIGGER_RISING_FALLING = HDL_EXTI_TRIGGER_RISING | HDL_EXTI_TRIGGER_FALLING,
  HDL_EXTI_TRIGGER_NONE           = !(HDL_EXTI_TRIGGER_RISING | HDL_EXTI_TRIGGER_FALLING)
} hdl_exti_trig_type_t;

typedef exti_line_enum hdl_exti_line_t;
typedef exti_mode_enum hdl_exti_mode_t;

typedef struct {
  hdl_exti_line_t exti_line;
  uint8_t source_selection;
  hdl_exti_mode_t exti_mode;
  hdl_exti_trig_type_t trigger;
} hdl_nvic_exti_t;

#define hdl_interrupts(...) ((hdl_nvic_interrupt_t *[]){__VA_ARGS__, NULL})
#define hdl_exti_lines(...) ((hdl_nvic_exti_t *[]){__VA_ARGS__, NULL})

typedef struct {
  hdl_module_t module;
  uint32_t prio_bits;
  hdl_nvic_interrupt_t **interrupts;
  hdl_nvic_exti_t **exti_lines;
  uint8_t irq_latency; /* processor ensures that a minimum of irq_latency+1 hclk cycles exist between an interrupt becoming pended */
} hdl_nvic_t;

hdl_module_state_t hdl_core(void *desc, uint8_t enable);
hdl_module_state_t hdl_nvic(void *desc, uint8_t enable);

typedef hdl_nvic_t hdl_interrupt_controller_t;
typedef hdl_nvic_interrupt_t hdl_interrupt_t;


uint8_t hdl_interrupt_request(hdl_interrupt_controller_t *ic, hdl_irq_n_t interrupt, event_handler_t handler, void *context);

uint8_t hdl_exti_request(hdl_interrupt_controller_t *ic, hdl_exti_line_t exti_line);

void hdl_exti_sw_trigger(hdl_interrupt_controller_t *ic, hdl_exti_line_t exti_line);

#endif // HDL_CORE_H_
