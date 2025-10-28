#include "hdl_iface.h"

typedef struct {
  hdl_nvic_irq_t *irq;
} hdl_nvic_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_nvic_var_t, *((hdl_nvic_t *)0)->obj_var, HDL_NVIC_VAR_SIZE, port_core.h);

typedef struct {
  hdl_nvic_irq_t *next;
  hdl_event_t event;
} hdl_nvic_irq_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_nvic_irq_var_t, *((hdl_nvic_irq_t *)0)->obj_var, HDL_NVIC_IRQ_VAR_SIZE, port_core.h);

typedef struct {
  uint32_t cor_sp;
  coroutine_handler_t cor;
  uint32_t stack_size;
  uint32_t stack[];
} ccb_t;

void hdl_coroutine_add(coroutine_ex_t *cor_buf, void *cor_cb, uint32_t size, coroutine_handler_t handler, void *arg) {
  ccb_t *ccb = (ccb_t *)cor_cb;
  ccb->stack_size = (size - sizeof(ccb_t));
  ccb->cor_sp = 0;
  ccb->cor = handler;
  cor_buf->ccb = ccb;
  coroutine_add(&cor_buf->coroutine, &hdl_coroutine_run_yielding, arg);
}

/* Saved context structure
   MSP     Stack      PSP
------------------------------
|  main |  offset  |coroutine|
------------------------------
|  ccb  | x + 0x00 |   LR    | <-- SP
|  PC   | x + 0x04 |   R12   |
|  R12  | x + 0x08 |   R8    |
|  R11  | x + 0x0C |   R9    |
|  R10  | x + 0x10 |   R10   |
|  R9   | x + 0x14 |   R11   |
|  R8   | x + 0x18 |  dummy  |
|  R7   | x + 0x1C |   R3    |
|  R6   | x + 0x20 |   R4    |
|  R5   | x + 0x24 |   R5    |
|  R4   | x + 0x28 |   R6    |
|  R3   | x + 0x2C |   R7    |
|  LR   | x + 0x30 |   R0    |
          x + 0x34 |   R1    |
          x + 0x38 |   R2    |
          x + 0x3C |   PC    |
*dummy word, for 8 bytes align
*/

__attribute__((naked, noreturn)) void hdl_coroutine_yield_return() {
  __asm volatile("PUSH {R0-R2, LR}");     /* save r0-r2, LR as place holder for PC */
  __asm volatile("PUSH {R2-R7}");         /* save r3-r7, r2 to dummy */
  __asm volatile("MOV  R0, R8");
  __asm volatile("MOV  R1, R9");
  __asm volatile("MOV  R2, R10");
  __asm volatile("MOV  R3, R11");
  __asm volatile("PUSH {R0-R3}");         /* save R8-R11 */
  __asm volatile("SUB  SP, #8");          /* LR + R12 */
  __asm volatile("LDR  R0, =cor_resume_from");
  __asm volatile("MOV  R1, #1");
  __asm volatile("ORR  R0, R1");
  __asm volatile("STR  R0, [SP, #0x3C]"); /* save resume PC */
  __asm volatile("MOV  R0, R12");
  __asm volatile("STR  R0, [SP, #0x04]"); /* save R12 */
  __asm volatile("MOV  R0, LR");
  __asm volatile("STR  R0, [SP, #0]");    /* save LR */
  __asm volatile("MRS  R0, CONTROL");
  __asm volatile("MOV  R1, #0x02");
  __asm volatile("BIC  R0, R1");
  __asm volatile("MOV  R1, SP");          /* PSP t0 R1 */
  __asm volatile("MSR  CONTROL, R0");     /* switch to MSP */
  __asm volatile("ISB");
  __asm volatile("POP  {R0, PC}");
  __asm volatile("cor_resume_from:");
  __asm volatile("BX   LR");
}

__attribute__((naked, noreturn)) uint8_t hdl_coroutine_run_yielding(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this; (void)cancel; (void)arg;
  __asm volatile("PUSH {R3-R7, LR}");
  __asm volatile("MOV  R3, R8");
  __asm volatile("MOV  R4, R9");
  __asm volatile("MOV  R5, R10");
  __asm volatile("MOV  R6, R11");
  __asm volatile("MOV  R7, R12");
  __asm volatile("PUSH {R2-R7}");

  __asm volatile("CBZ  R0, __cor_exit_cancel"); /* if coroutine == NULL  then exit cancel */
  __asm volatile("LDR  R3, [R0, #"CL_TOSTR(CO_ROUTINE_DESC_SIZE)"]");      /* R3 = ccb */
  __asm volatile("CBZ  R4, __cor_exit_cancel"); /* if ccb == NULL then exit cancel */
  __asm volatile("LDR  R4, [R3, #8]");
  __asm volatile("CMP  R4, #127");
  __asm volatile("BLS  __cor_exit_cancel");  /* stack_size <= 127 too small, exit cancel */
  __asm volatile("LDR  R6, [R3]");           /* R6 = ccb->cor_sp */

  __asm volatile("CBNZ R6, __cor_resume");   /* if ccb->cor_sp == 0 init coroutine */
    __asm volatile("MOV R5, #12");
    __asm volatile("ADD R5, R4");
    __asm volatile("ADD R5, R3");
    __asm volatile("LSR R5, #0X03");
    __asm volatile("LSL R5, #0X03");         /* R5 = handlers stack bottom 8 bytes aligned */
    __asm volatile("SUB R5, #0x40");         /* Handler context struct */
    __asm volatile("LDR R4, [R3, #4]");
    __asm volatile("MOV R6, #1");
    __asm volatile("ORR R4, R6");
    __asm volatile("STR R4, [R5, #0x3C]");   /* push PC = ccb->cor */
    __asm volatile("STR R2, [R5, #0x38]");
    __asm volatile("STR R1, [R5, #0x34]");
    __asm volatile("STR R0, [R5, #0x30]");   /* push args */
    __asm volatile("STR R3, [R5, #0x20]");   /* push ccb */
    __asm volatile("LDR R4, =__cor_exit");
    __asm volatile("ORR R4, R6");
    __asm volatile("STR R4, [R5, #0x00]");   /* handler returns to LR = __cor_exit */
    __asm volatile("STR R5, [R3, #0x00]");   /* ccb->psp = handler stack */
    __asm volatile("MOV R6, R5");

  __asm volatile("__cor_resume:");
    __asm volatile("LDR  R4, =__cor_yielded");
    __asm volatile("MOV  R5, #1");
    __asm volatile("ORR  R4, R5");
    __asm volatile("PUSH {R3, R4}");         /* push to main stack ccb & yield return PC = __cor_yielded */

    __asm volatile("MSR  PSP, R6");          /* set PSP */

    __asm volatile("MRS  R4, CONTROL");
    __asm volatile("MOV  R5, #0x02");
    __asm volatile("ORR  R4, R5");
    __asm volatile("MSR  CONTROL, R4");      /* switch to PSP */
    __asm volatile("ISB");

    __asm volatile("POP  {R0-R1}");          /* restore LR, R12 */
    __asm volatile("MOVS LR, R0");
    __asm volatile("MOVS R12, R1");
    __asm volatile("POP  {R0-R3}");          /* restore gp registers 8-11 */
    __asm volatile("MOVS R8,  R0");
    __asm volatile("MOVS R9,  R1");
    __asm volatile("MOVS R10, R2");
    __asm volatile("MOVS R11, R3");
    __asm volatile("POP  {R2-R7}");          /* dummy + restore gp registers 3-7 */
    __asm volatile("POP  {R0-R2, PC}");      /* resume handler */

  __asm volatile("__cor_exit_cancel:");
    __asm volatile("MOVS R0, #1");           /* return 1 */

  __asm volatile("__cor_end:");
    __asm volatile("POP {R2-R7}");
    __asm volatile("MOV R8, R3");
    __asm volatile("MOV R9, R4");
    __asm volatile("MOV R10, R5");
    __asm volatile("MOV R11, R6");
    __asm volatile("MOV R12, R7");
    __asm volatile("POP {R3-R7, PC}");

  __asm volatile("__cor_yielded:");
    __asm volatile("STR  R1, [R0]");         /* save PSP to ccb */
    __asm volatile("EOR  R0, R0");           /* return 0 */
    __asm volatile("B    __cor_end");

  __asm volatile("__cor_exit:");
    __asm volatile("EOR  R3, R3");
    __asm volatile("STR  R3, [R4, #0x00]");  /* ccb->psp == 0 */
    __asm volatile("MRS  R4, CONTROL");
    __asm volatile("MOV  R3, #0x02");
    __asm volatile("BIC  R4, R3");
    __asm volatile("MSR  CONTROL, R0");      /* switch to MSP */
    __asm volatile("ISB");
    __asm volatile("POP  {R1-R2}");          /* pop ccb & yield return ptr */
    __asm volatile("B    __cor_end");
}

void nmi_handler()                      { call_isr(HDL_NVIC_EXCEPTION_NonMaskableInt, 0); }
void hard_fault_handler()               { call_isr(HDL_NVIC_EXCEPTION_HardFault, 0); }
void pend_sv_handler()                  { call_isr(HDL_NVIC_EXCEPTION_PendSV, 0); }
void systick_handler()                  { call_isr(HDL_NVIC_EXCEPTION_SysTick, 0); }

#if defined(__CORE_CM4_H_DEPENDANT) || defined(__CORE_CM3_H_DEPENDANT)
void mem_manage_handler()               { call_isr(HDL_NVIC_EXCEPTION_MemoryManagement, 0); }
void bus_fault_handler()                { call_isr(HDL_NVIC_EXCEPTION_BusFault, 0); }
void usage_fault_handler()              { call_isr(HDL_NVIC_EXCEPTION_UsageFault, 0); }
void debug_mon_handler()                { call_isr(HDL_NVIC_EXCEPTION_DebugMonitor, 0); }

__STATIC_INLINE void _hdl_isr_prio_set(hdl_nvic_irq_n_t irq, uint8_t priority_group, uint8_t priority, uint8_t prio_bits) {
  uint8_t prio = (((priority_group << (8U - prio_bits)) | (priority & (0xFF >> prio_bits))) & 0xFFUL);
  volatile uint8_t *ipr = (irq < 0)? &(SCB->SHP[(irq & 0xFUL) - 4UL]): &(NVIC->IP[irq]);
  *ipr = prio;
}

__STATIC_INLINE uint8_t hdl_exception_irq_enable(hdl_nvic_irq_n_t irq) {
  switch (irq) {
    case HDL_NVIC_EXCEPTION_SysTick:
      SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; /* Enable SysTick IRQ */
      break;
    case HDL_NVIC_EXCEPTION_PendSV:
    case HDL_NVIC_EXCEPTION_SVCall:
    case HDL_NVIC_EXCEPTION_HardFault:
    case HDL_NVIC_EXCEPTION_NonMaskableInt:
    case HDL_NVIC_EXCEPTION_MemoryManagement:
    case HDL_NVIC_EXCEPTION_BusFault:
    case HDL_NVIC_EXCEPTION_DebugMonitor:
    case HDL_NVIC_EXCEPTION_UsageFault:
      // TODO: enable if possible;
      break;
    default:
      return HDL_FALSE;
  }
  return HDL_TRUE;
}

static uint8_t _hdl_interrupt_controller_spec(const hdl_nvic_t *desc, uint8_t enable) {
  (void)desc; (void)enable;
  return HDL_TRUE;
}

#endif

#if defined(__CORE_CM0PLUS_H_DEPENDANT) || defined(__CORE_CM23_H_DEPENDANT)
__STATIC_INLINE void _hdl_isr_prio_set(hdl_nvic_irq_n_t irq, uint8_t priority_group, uint8_t priority, uint8_t prio_bits) {
  uint32_t prio = ((priority_group << (8U - prio_bits)) | ((priority & (0xFF >> prio_bits)) & 0xFFUL));
  uint32_t shift = _BIT_SHIFT(irq);
  #if defined(__CORE_CM23_H_DEPENDANT)
    volatile uint32_t *ipr = (irq < 0)? &(SCB->SHPR[_SHP_IDX(irq)]): &(NVIC->IPR[_IP_IDX(irq)]);
  #else
    volatile uint32_t *ipr = (irq < 0)? &(SCB->SHP[_SHP_IDX(irq)]): &(NVIC->IP[_IP_IDX(irq)]);
  #endif
  *ipr = (*ipr & ~(0xFFUL << shift)) | (prio << shift);
}

__STATIC_INLINE uint8_t hdl_exception_irq_enable(hdl_nvic_irq_n_t irq) {
  switch (irq) {
    case HDL_NVIC_EXCEPTION_SysTick:
      SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; /* Enable SysTick IRQ */
      break;
    case HDL_NVIC_EXCEPTION_PendSV:
    case HDL_NVIC_EXCEPTION_SVCall:
    case HDL_NVIC_EXCEPTION_HardFault:
    case HDL_NVIC_EXCEPTION_NonMaskableInt:
      // TODO: enable if possible;
      break;
    default:
      return HDL_FALSE;
  }
  return HDL_TRUE;
}

  #if defined(__CORE_CM23_H_DEPENDANT)
  static uint8_t _hdl_interrupt_controller_spec(const hdl_nvic_t *desc, uint8_t enable) {
    if(enable) SYSCFG_CPU_IRQ_LAT = desc->config->irq_latency;
    return HDL_TRUE;
  }
  #else
  static uint8_t _hdl_interrupt_controller_spec(const hdl_nvic_t *desc, uint8_t enable) {
    (void)desc; (void)enable;
    return HDL_TRUE;
  }
  #endif
#endif

void __libc_init_array();
void main();

/* Returns the current value of the Link Register (LR). */ 
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_LR(void)  { 
  register uint32_t result; 
  __ASM volatile ("MOV %0, LR\n" : "=r" (result) ); 
  return(result); 
} 

void call_isr(hdl_nvic_irq_n_t irqn, void *event_trigger) {
  hdl_nvic_t *ic = (hdl_nvic_t *)((uint32_t *)SCB->VTOR)[0];
  hdl_nvic_var_t *ic_var = (hdl_nvic_var_t *)ic->obj_var;
  hdl_nvic_irq_t *irq = ic_var->irq;

  // if(irq == -14) {
  //   NVIC_DisableIRQ(irq);
  // }
  uint8_t no_handler = 1;
  if(irq != NULL) {
    while (irq != NULL) {
      hdl_nvic_irq_var_t *irq_var = (hdl_nvic_irq_var_t *)irq->obj_var;
      uint32_t trg = ((uint32_t)event_trigger) & irq->config->event_mask;
      if((irq->config->irq_type == irqn) && (trg == irq->config->event_id)) {
        hdl_event_raise(&irq_var->event, ic, (void *)trg);
        no_handler = 0;
      }
      irq = irq_var->next;
    }
  }
  if(no_handler) {
    #if DEBUG
    //If you get stuck here, your code is missing some interrupt request. see interrupts in MIG file.
    asm("bkpt 255");
    #endif
    while(irqn < 0) ;
    NVIC_DisableIRQ((IRQn_Type)irqn);
  }
}

// __attribute__((naked)) void switch_to_psp(void) {
//   // change PSP
//   asm("LDR R0, =_psp_stack");
//   asm("MSR PSP, R0");
//   // set SPSEL bit
//   asm("MRS R1, CONTROL");
//   asm("ORR R1, R1, #2");
//   asm("MSR CONTROL, R1");
//   // return
//   asm("BX LR");
// }

// __attribute__ ((naked)) void switch_to_unprivileged(void) {
//   // set nPRIV bit
//   asm("MRS R0, CONTROL");
//   asm("ORR R0, R0, #1");
//   asm("MSR CONTROL, r0");
//   // return
//   asm("BX LR");
// }

__attribute__((naked, noreturn)) void reset_handler() {
	asm ("ldr r0, =_estack");
	asm ("mov r13, r0");
	const void **pSource, **pDest;
	for (pSource = &_sidata, pDest = &_sdata; pDest != &_edata; pSource++, pDest++)
	  *pDest = *pSource;
  for (pDest = &_sbss; pDest != &_ebss; pDest++)
    *pDest = 0;
  __libc_init_array();
  main();
  for (;;) ;
}

const void * const main_isr_vector[] __attribute__ ((section (".isr_vector"), used)) = {
  &_estack,
  &reset_handler,
};

void svc_handler() {
  #if !defined(__CORE_CM0PLUS_H_DEPENDANT)
  register uint32_t result;
  //__ASM volatile ("MRS %0, msp" : "=r" (result) );
  asm ("MRS            R1, MSP");
  //__ASM volatile ("MOV %0, LR\n" : "=r" (result) );
  asm ("MOV            R3, LR");
  //if(lr & 4) {
  asm ("LDR            R2, =4");
  asm ("AND            R3, R2");
  asm ("CBZ            R3, test");
  asm ("MRS            R1, PSP");
  asm ("test:\n\
        ADD            R1, R1, #40");
  asm ("MOV            %0, R1" : "=r" (result));
  result = *(uint32_t*)result;
  result = (uint8_t)(*(uint16_t*)(result - 2));
  call_isr(HDL_NVIC_EXCEPTION_SVCall, (void *)result);
  #endif
}

void irq_n_handler() {
  #if !defined(__CORE_CM0PLUS_H_DEPENDANT)
  uint32_t prio = -1;
  hdl_nvic_irq_n_t irq = 0;
  for(uint32_t i = 0; i < sizeof(NVIC->IABR)/sizeof(NVIC->IABR[0]); i++) {
    uint32_t iabr = NVIC->IABR[i];
    while(iabr) {
      uint8_t bit = 31 - __CLZ(iabr);
      hdl_nvic_irq_n_t cur_irq = (32 * i) + bit;
      uint32_t cur_prio = NVIC_GetPriority((IRQn_Type)cur_irq);
      if(cur_prio < prio) {
        irq = cur_irq;
        prio = cur_prio;
      }
      iabr &= ~(1 << bit);
    }
  }
  call_isr(irq, 0);
  #endif
}

uint8_t hdl_core_is_in_isr() {
	uint32_t interrupt;
	/* Obtain the number of the currently executing interrupt. */
	asm ("mrs %0, ipsr" : "=r"(interrupt));
	return (interrupt != 0)? HDL_TRUE: HDL_FALSE;
}

static hdl_module_state_t _hdl_interrupt_controller(const void *desc, uint8_t enable) {
  hdl_nvic_t *nvic = (hdl_nvic_t *)desc;
  if(enable) {
    ((hdl_nvic_var_t *)nvic->obj_var)->irq = NULL;
    _hdl_interrupt_controller_spec(nvic, enable);
    if(nvic->config->vector != NULL) SCB->VTOR = (uint32_t)nvic->config->vector;
    return HDL_MODULE_ACTIVE;
  }
  _hdl_interrupt_controller_spec(nvic, enable);
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_interrupt(const void *desc, uint8_t enable) {
  hdl_nvic_irq_t *irq = (hdl_nvic_irq_t *)desc;
  hdl_nvic_irq_var_t *irq_var = (hdl_nvic_irq_var_t *)irq->obj_var;
  hdl_nvic_irq_config_t *irq_cnf = (hdl_nvic_irq_config_t *)irq->config;
  hdl_nvic_t *ic = (hdl_nvic_t *)irq->dependencies[0];
  if(enable) {
    _hdl_isr_prio_set(irq_cnf->irq_type, irq_cnf->priority_group, irq_cnf->priority, ic->config->prio_bits);
    irq_var->next = NULL;
    return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_interrupt_request(const void *desc, hdl_delegate_t *isr) {
  hdl_nvic_irq_t *irq = (hdl_nvic_irq_t *)desc;
  hdl_nvic_irq_var_t *irq_var = (hdl_nvic_irq_var_t *)irq->obj_var;
  hdl_nvic_t *ic = (hdl_nvic_t *)irq->dependencies[0];
  hdl_nvic_var_t *ic_var = (hdl_nvic_var_t *)ic->obj_var;
  hdl_nvic_irq_config_t *irq_cnf = (hdl_nvic_irq_config_t *)irq->config;
  hdl_event_subscribe(&irq_var->event, isr);
  hdl_nvic_irq_t *irq_list = ic_var->irq;
  while (irq_list != NULL) {
    if(irq_list == irq) return HDL_TRUE;
    irq_list = ((hdl_nvic_irq_var_t *)irq_list->obj_var)->next;
  }
  irq_var->next = ic_var->irq;
  ic_var->irq = irq;
  if(irq_cnf->irq_type < 0) return hdl_exception_irq_enable(irq_cnf->irq_type);
  else NVIC_EnableIRQ((IRQn_Type)irq_cnf->irq_type);
  return HDL_TRUE;
}

static void _hdl_interrupt_sw_trigger(const void *desc) {
  hdl_nvic_irq_t *irq = (hdl_nvic_irq_t *)desc;
  NVIC_SetPendingIRQ((IRQn_Type)irq->config->irq_type);
}

const hdl_module_base_iface_t hdl_nvic_iface = {
  .init = &_hdl_interrupt_controller
};

const hdl_interrupt_iface_t hdl_nvic_irq_iface = {
  .init = &_hdl_interrupt,
  .request = &_hdl_interrupt_request,
  .trigger = &_hdl_interrupt_sw_trigger
};
