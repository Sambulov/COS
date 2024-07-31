#include "hdl_portable.h"
#include "CodeLib.h"

#if defined(__CORE_CM23_H_DEPENDANT)

typedef struct {
  hdl_nvic_irq_n_t irq_type;
  uint8_t priority_group;
  uint8_t priority;
  hdl_event_t event;
} hdl_nvic_interrupt_private_t;

_Static_assert(sizeof(hdl_nvic_interrupt_private_t) == sizeof(hdl_interrupt_t), "In hdl_core.h data structure size of hdl_nvic_interrupt_t doesn't match, check HDL_INTERRUPT_PRV_SIZE");

void __libc_init_array();
void main();

void nmi_handler()                      { call_isr(HDL_NVIC_EXCEPTION_NMI, 0); }
void hard_fault_handler()               { call_isr(HDL_NVIC_EXCEPTION_HF, 0); }
void pend_sv_handler()                  { call_isr(HDL_NVIC_EXCEPTION_PSV, 0); }
void systick_handler()                  { call_isr(HDL_NVIC_EXCEPTION_SysTick, 0); }

/* Returns the current value of the Link Register (LR). */ 
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_LR(void)  { 
  register uint32_t result; 
  __ASM volatile ("MOV %0, LR\n" : "=r" (result) ); 
  return(result); 
} 

void call_isr(hdl_nvic_irq_n_t irq, uint32_t event) {
  hdl_interrupt_controller_config_t *ic = (hdl_interrupt_controller_config_t *)((uint32_t *)SCB->VTOR)[0];
  hdl_interrupt_t **isrs = ic->interrupts;
  if(isrs != NULL) {
    while (*isrs != NULL) {
      if((*isrs)->irq_type == irq) {
        hdl_nvic_interrupt_private_t *isr = (hdl_nvic_interrupt_private_t *)*isrs;
        if(!hdl_event_raise(&isr->event, ic, event))
          NVIC_DisableIRQ(irq);
        return;
      }
      isrs++;
    }
  }
  //If you get stuck here, your code is missing some interrupt request. see interrupts in MIG file.
	asm("bkpt 255");
  for(;;) ;
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
	void **pSource, **pDest;
	for (pSource = &_sidata, pDest = &_sdata; pDest != &_edata; pSource++, pDest++)
	  *pDest = *pSource;

  for (pDest = &_sbss; pDest != &_ebss; pDest++)
    *pDest = 0;
  //TODO: turn off JTAG;
  __libc_init_array();
  main();
  for (;;) ;
}

/* TODO: check sv call, exapmple call: asm("SVC #6"); */
__attribute__ ((naked)) void svc_handler(void) {
  //asm("TST LR, 4"); // check LR to know which stack is used
  //asm("ITE EQ"); // 2 next instructions are conditional
  //asm("MRSEQ R0, MSP"); // save MSP if bit 2 is 0
  //asm("MRSNE R0, PSP"); // save PSP if bit 2 is 1

  asm("MRS R0, MSP"); // save MSP stack pointer value

  asm("B svc_handler_main"); // pass R0 as the argument
}

/* TODO: test with __attribute__ ((naked)) */
static void svc_handler_main(uint32_t *sp) {
  /* get the address of the instruction saved in PC */
  uint8_t *instruction = (uint8_t *)(sp[6]);
  /* go back 2 bytes (16-bit opcode) */
  instruction -= 2;
  /* get the opcode, in little endian */
  call_isr(SVCall_IRQn, (uint8_t)*instruction);
}

void irq_n_handler() {
  uint32_t prio = -1;
  IRQn_Type irq = 0;
  for(uint32_t i = 0; i < sizeof(NVIC->IABR)/sizeof(NVIC->IABR[0]); i++) {
    uint32_t iabr = NVIC->IABR[i];
    while(iabr) {
      uint8_t bit = 31 - __CLZ(iabr);
      IRQn_Type cur_irq = (32 * i) + bit;
      uint32_t cur_prio = NVIC_GetPriority(cur_irq);
      if(cur_prio < prio) {
        irq = cur_irq;
        prio = cur_prio;
      }
      iabr &= ~(1 << bit);
    }
  }
  call_isr(irq, 0);
}

hdl_module_state_t hdl_core(void *desc, uint8_t enable) {
  /* TODO: */
  if(enable) {
    hdl_core_t *core = (hdl_core_t *)desc;
    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | core->config->flash_latency;
    rcu_periph_clock_enable(RCU_CFGCMP);
    return HDL_MODULE_INIT_OK;
  }
  rcu_periph_clock_disable(RCU_CFGCMP);
  FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT_0;
  return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t hdl_interrupt_controller(void *desc, uint8_t enable) {
  if(enable) {
    hdl_interrupt_controller_t *nvic = (hdl_interrupt_controller_t *)desc;
    /* NVIC_SetPriorityGrouping   not available for Cortex-M23 */
    SYSCFG_CPU_IRQ_LAT = nvic->config->irq_latency;
    if(nvic->config->vector != NULL)
      SCB->VTOR = (uint32_t)nvic->config->vector;
    return HDL_MODULE_INIT_OK; 
  }
  else {
    //TODO: disable nvic
  }
  return HDL_MODULE_DEINIT_OK;
}

uint8_t hdl_interrupt_request(hdl_interrupt_controller_t *ic, const hdl_interrupt_t *isr, hdl_delegate_t *delegate) {
  if((hdl_state(&ic->module) != HDL_MODULE_INIT_OK) || (ic->config->interrupts == NULL) || (delegate == NULL) || (isr == NULL))
    return HDL_FALSE;
  hdl_nvic_interrupt_private_t *_isr = (hdl_nvic_interrupt_private_t *)isr;
  hdl_event_subscribe(&_isr->event, delegate);
  uint32_t prio = ((_isr->priority_group << (8U - ic->config->prio_bits)) | 
                  (_isr->priority & (0xFF >> ic->config->prio_bits)) & 
                  0xFFUL);
  uint32_t shift = _BIT_SHIFT(_isr->irq_type);
  volatile uint32_t *ipr = (_isr->irq_type < 0)? &(SCB->SHPR[_SHP_IDX(_isr->irq_type)]):
                                                    &(NVIC->IPR[_IP_IDX(_isr->irq_type)]);
  /* set priority for interrupt */
  *ipr = (*ipr & ~(0xFFUL << shift)) | (prio << shift);
  /* interrupt enable */
  if(_isr->irq_type < 0) {
    switch (_isr->irq_type) {
      case SysTick_IRQn:
        SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;                  /* Enable SysTick IRQ */
        break;
      case PendSV_IRQn:
        // TODO: enable if possible, ;
        break;
      case SVCall_IRQn:
        // TODO: enable if possible, ;
        break;
      case HardFault_IRQn:
        // TODO: enable if possible, ;
        break;
      case NonMaskableInt_IRQn:
        // TODO: enable if possible, ;
        break;
      default:
        return HDL_FALSE;
    }
  }
  else {
    NVIC_EnableIRQ(_isr->irq_type);
  }
  return HDL_TRUE;
}

#endif
