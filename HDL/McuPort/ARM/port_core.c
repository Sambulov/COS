#include "hdl_iface.h"

void __libc_init_array();
void main();

/* Returns the current value of the Link Register (LR). */ 
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_LR(void)  { 
  register uint32_t result; 
  __ASM volatile ("MOV %0, LR\n" : "=r" (result) ); 
  return(result); 
} 

void call_isr(hdl_nvic_irq_n_t irq, uint32_t event) {
  hdl_interrupt_controller_config_t *ic = (hdl_interrupt_controller_config_t *)((uint32_t *)SCB->VTOR)[0];
  hdl_interrupt_t * const *isrs = ic->interrupts;
  // if(irq == -14) {
  //   NVIC_DisableIRQ(irq);
  // }
  if(isrs != NULL) {
    while (*isrs != NULL) {
      hdl_interrupt_config_t *isr_cnf = (hdl_interrupt_config_t *)(*isrs)->irq_cnf;
      if(isr_cnf->irq_type == irq) {
        hdl_interrupt_t *isr = *isrs;
        if(!hdl_event_raise(&isr->event, ic, event))
          NVIC_DisableIRQ((IRQn_Type)irq);
        return;
      }
      isrs++;
    }
  }
  //If you get stuck here, your code is missing some interrupt request. see interrupts in MIG file.
	asm("bkpt 255");
  while(irq < 0) ;
  NVIC_DisableIRQ((IRQn_Type)irq);
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

const void * const main_isr_vector[] __attribute__ ((section (".isr_vector"), used)) = {
  &_estack,
  &reset_handler,
};

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

void svc_handler() {
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
  call_isr(HDL_NVIC_EXCEPTION_SVCall, result);
}

void irq_n_handler() {
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
}

static hdl_module_state_t _hdl_interrupt_controller(const void *desc, uint8_t enable) {
  if(enable) {
    hdl_interrupt_controller_t *nvic = (hdl_interrupt_controller_t *)desc;
    /* NVIC_SetPriorityGrouping   not available for Cortex-M23 */
    SYSCFG_CPU_IRQ_LAT = nvic->config->irq_latency;
    if(nvic->config->vector != NULL)
      SCB->VTOR = (uint32_t)nvic->config->vector;
    return HDL_MODULE_ACTIVE; 
  }
  else {
    //TODO: disable nvic
  }
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_interrupt_request(const void *desc, const hdl_interrupt_t *isr) {
  hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)desc;
  if((hdl_state(desc) == HDL_MODULE_FAULT) || (ic->config->interrupts == NULL) || (isr == NULL)) return HDL_FALSE;
  hdl_interrupt_config_t *isr_cnf = (hdl_interrupt_config_t *)isr->irq_cnf;
  _hdl_isr_prio_set(isr_cnf->irq_type, isr_cnf->priority_group, isr_cnf->priority, ic->config->prio_bits);
  if(isr_cnf->irq_type < 0) return hdl_exception_irq_enable(isr_cnf->irq_type);
  else NVIC_EnableIRQ((IRQn_Type)isr_cnf->irq_type);
  return HDL_TRUE;
}

static void _hdl_interrupt_sw_trigger(const void *int_ctr, const hdl_interrupt_t *isr) {
  (void)int_ctr;
  hdl_interrupt_config_t *isr_cnf = (hdl_interrupt_config_t *)isr->irq_cnf;
  NVIC_SetPendingIRQ((IRQn_Type)isr_cnf->irq_type);
}

const hdl_interrupt_controller_iface_t hdl_interrupt_controller_iface = {
  .init = &_hdl_interrupt_controller,
  .request = &_hdl_interrupt_request,
  .trigger = &_hdl_interrupt_sw_trigger
};

__WEAK void _read(void) { }
__WEAK void _write(void) { }
__WEAK void _close(void) { }
__WEAK void _lseek(void) { }
__WEAK void _isatty(void) { }
__WEAK void _fstat(void) { }
