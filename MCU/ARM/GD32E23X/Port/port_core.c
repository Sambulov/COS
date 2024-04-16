#include "hdl_portable.h"

typedef struct {
  hdl_hardware_t hw;
  uint32_t prio_bits;
  void *int_context[64];
  event_handler_t int_handlers[64];
} hdl_nvic_private_t;

_Static_assert(sizeof(hdl_nvic_private_t) == sizeof(hdl_nvic_t), "In hdl_core.h data structure size of hdl_nvic_t doesn't match, check HDL_NVIC_PRV_SIZE");

extern void *_estack;
extern void *_sidata, *_sdata, *_edata;
extern void *_sbss, *_ebss;

//void SystemInit();
void __libc_init_array();
void main();
hdl_nvic_private_t *__ic = NULL;


void Reset_Handler();
void irq_n_handler();
void nmi_handler();
void hard_fault_handler();
void svc_handler();
void pend_sv_handler();
void systick_handler();
void WWDGT_IRQHandler()                   __attribute__ ((alias ("irq_n_handler")));
void LVD_IRQHandler()                     __attribute__ ((alias ("irq_n_handler")));
void RTC_IRQHandler()                     __attribute__ ((alias ("irq_n_handler")));
void FMC_IRQHandler()                     __attribute__ ((alias ("irq_n_handler")));
void RCU_IRQHandler()                     __attribute__ ((alias ("irq_n_handler")));
void EXTI0_1_IRQHandler()                 __attribute__ ((alias ("irq_n_handler")));
void EXTI2_3_IRQHandler()                 __attribute__ ((alias ("irq_n_handler")));
void EXTI4_15_IRQHandler()                __attribute__ ((alias ("irq_n_handler")));
void DMA_Channel0_IRQHandler()            __attribute__ ((alias ("irq_n_handler")));
void DMA_Channel1_2_IRQHandler()          __attribute__ ((alias ("irq_n_handler")));
void DMA_Channel3_4_IRQHandler()          __attribute__ ((alias ("irq_n_handler")));
void ADC_CMP_IRQHandler()                 __attribute__ ((alias ("irq_n_handler")));
void TIMER0_BRK_UP_TRG_COM_IRQHandler()   __attribute__ ((alias ("irq_n_handler")));
void TIMER0_Channel_IRQHandler()          __attribute__ ((alias ("irq_n_handler")));
void TIMER2_IRQHandler()                  __attribute__ ((alias ("irq_n_handler")));
void TIMER5_IRQHandler()                  __attribute__ ((alias ("irq_n_handler")));
void TIMER13_IRQHandler()                 __attribute__ ((alias ("irq_n_handler")));
void TIMER14_IRQHandler()                 __attribute__ ((alias ("irq_n_handler")));
void TIMER15_IRQHandler()                 __attribute__ ((alias ("irq_n_handler")));
void TIMER16_IRQHandler()                 __attribute__ ((alias ("irq_n_handler")));
void I2C0_EV_IRQHandler()                 __attribute__ ((alias ("irq_n_handler")));
void I2C1_EV_IRQHandler()                 __attribute__ ((alias ("irq_n_handler")));
void SPI0_IRQHandler()                    __attribute__ ((alias ("irq_n_handler")));
void SPI1_IRQHandler()                    __attribute__ ((alias ("irq_n_handler")));
void USART0_IRQHandler()                  __attribute__ ((alias ("irq_n_handler")));
void USART1_IRQHandler()                  __attribute__ ((alias ("irq_n_handler")));
void I2C0_ER_IRQHandler()                 __attribute__ ((alias ("irq_n_handler")));
void I2C1_ER_IRQHandler()                 __attribute__ ((alias ("irq_n_handler")));

#define NMI_HANDLER_OFFSET                0
#define HARD_FAULT_HANDLER_OFFSET         1
#define SVC_HANDLER_OFFSET                2
#define PEND_SV_HANDLER_OFFSET            3
#define SYSTICK_HANDLER_OFFSET            4
#define IRQ_N_HANDLERS_OFFSET             5

void * g_pfnVectors[0x40] __attribute__ ((section (".isr_vector"), used)) = {
  &_estack,
  &Reset_Handler,
  &nmi_handler,
  &hard_fault_handler,
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  &svc_handler,
  ((void *)0),
  ((void *)0),
  &pend_sv_handler,
  &systick_handler,
  
  &WWDGT_IRQHandler,
  &LVD_IRQHandler,
  &RTC_IRQHandler,
  &FMC_IRQHandler,
  &RCU_IRQHandler,
  &EXTI0_1_IRQHandler,
  &EXTI2_3_IRQHandler,
  &EXTI4_15_IRQHandler,
  ((void *)0),
  &DMA_Channel0_IRQHandler,
  &DMA_Channel1_2_IRQHandler,
  &DMA_Channel3_4_IRQHandler,
  &ADC_CMP_IRQHandler,
  &TIMER0_BRK_UP_TRG_COM_IRQHandler,
  &TIMER0_Channel_IRQHandler,
  ((void *)0),
  &TIMER2_IRQHandler,
  &TIMER5_IRQHandler,
  ((void *)0),
  &TIMER13_IRQHandler,
  &TIMER14_IRQHandler,
  &TIMER15_IRQHandler,
  &TIMER16_IRQHandler,
  &I2C0_EV_IRQHandler,
  &I2C1_EV_IRQHandler,
  &SPI0_IRQHandler,
  &SPI1_IRQHandler,
  &USART0_IRQHandler,
  &USART1_IRQHandler,
  ((void *)0),
  ((void *)0),
  ((void *)0),
  &I2C0_ER_IRQHandler,
  ((void *)0),
  &I2C1_ER_IRQHandler,
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
};

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

__attribute__((naked, noreturn)) void Reset_Handler() {
	//asm ("ldr sp, =_estack");
	void **pSource, **pDest;
	for (pSource = &_sidata, pDest = &_sdata; pDest != &_edata; pSource++, pDest++)
	  *pDest = *pSource;

  for (pDest = &_sbss; pDest != &_ebss; pDest++)
    *pDest = 0;
  //TODO: turn off JTAG;
  __libc_init_array();
  //SystemInit();
  main();
  for (;;) ;
}

/* TODO: test with __attribute__ ((naked)) */
void exception_handler(uint8_t exception, uint32_t event) {
  event_handler_t handler = __ic->int_handlers[exception];
  if(handler!=NULL) {
    handler(event, __ic, __ic->int_context[exception]);
    return;
  }
	//If you get stuck here, your code is missing a handler for some interrupt.
	asm("bkpt 255");
  for(;;) ;
}

/* TODO: test with __attribute__ ((naked)) */
void nmi_handler() {
  exception_handler(NMI_HANDLER_OFFSET, 0);
}

/* TODO: test with __attribute__ ((naked)) */
void hard_fault_handler() {
  exception_handler(HARD_FAULT_HANDLER_OFFSET, 0);
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
  exception_handler(SVC_HANDLER_OFFSET, (uint8_t)*instruction);
}

/* TODO: test with __attribute__ ((naked)) */
void pend_sv_handler() {
  exception_handler(PEND_SV_HANDLER_OFFSET, 0);
}

/* TODO: test with __attribute__ ((naked)) */
void systick_handler() {
  exception_handler(SYSTICK_HANDLER_OFFSET, 0);
}

void irq_n_handler() {
  uint32_t ind = 0;
  for(uint32_t i = 0; i< sizeof(NVIC->IABR); i++) {
    if(NVIC->IABR[i]) {
      ind = (32 * i) + (31 - __CLZ(NVIC->IABR[i]));
      /* TODO: check if it is the only bit in IABR */
      event_handler_t handler = __ic->int_handlers[IRQ_N_HANDLERS_OFFSET+ind];
      if(handler!=NULL) {
        handler(0, __ic, __ic->int_context[IRQ_N_HANDLERS_OFFSET+ind]);
        return;
      }
      break;
    }
  }
	//If you get stuck here, your code is missing a handler for some interrupt.
	asm("bkpt 255");
  for(;;) ;
}

hdl_init_state_t hdl_core(void *desc, uint8_t enable) {
  /* TODO: */
  if(enable) {
    hdl_core_t *core = (hdl_core_t *)desc;
    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | core->flash_latency;
    return HDL_HW_INIT_OK;
  }
  FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT_0;
  return HDL_HW_DEINIT_OK;
}

hdl_init_state_t hdl_nvic(void *desc, uint8_t enable) {
  if(enable) {
    hdl_nvic_t *nvic = (hdl_nvic_t *)desc;
    /* NVIC_SetPriorityGrouping   not available for Cortex-M23 */
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0);

    __ic = (hdl_nvic_private_t *)desc;
    /* TODO: fing wokaround to save context for interrupt vector */
    return HDL_HW_INIT_OK;
  }
  return HDL_HW_DEINIT_OK;
}

uint8_t hdl_interrupt_request(hdl_interrupt_controller_t *ic, hdl_interrupt_t *interrupt, event_handler_t handler, void *context) {
  if((ic != NULL) && (interrupt != NULL) && (handler != NULL)) {
    uint32_t prio = ((interrupt->priority_group << (8U - ic->prio_bits)) | 
                    (interrupt->priority & (0xFF >> ic->prio_bits)) & 
                    0xFFUL);
    uint32_t shift = _BIT_SHIFT(interrupt->irq_type);
    volatile uint32_t *ipr = (interrupt->irq_type < 0)? &(SCB->SHPR[_SHP_IDX(interrupt->irq_type)]):
                                                        &(NVIC->IPR[_IP_IDX(interrupt->irq_type)]);
    /* set priority for interrupt */
    *ipr = (*ipr & ~(0xFFUL << shift)) | (prio << shift);
    /* interrupt enable */
    if(interrupt->irq_type < 0) {
      switch (interrupt->irq_type) {
        case SysTick_IRQn:
          __ic->int_handlers[SYSTICK_HANDLER_OFFSET] = handler;
          __ic->int_context[SYSTICK_HANDLER_OFFSET] = context;
          SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;                  /* Enable SysTick IRQ */
          break;
        case PendSV_IRQn:
          __ic->int_handlers[PEND_SV_HANDLER_OFFSET] = handler;
          __ic->int_context[PEND_SV_HANDLER_OFFSET] = context;
          // TODO: enable if possible, ;
          break;
        case SVCall_IRQn:
          __ic->int_handlers[SVC_HANDLER_OFFSET] = handler;
          __ic->int_context[SVC_HANDLER_OFFSET] = context;
          // TODO: enable if possible, ;
          break;
        case HardFault_IRQn:
          __ic->int_handlers[HARD_FAULT_HANDLER_OFFSET] = handler;
          __ic->int_context[HARD_FAULT_HANDLER_OFFSET] = context;
          // TODO: enable if possible, ;
          break;
        case NonMaskableInt_IRQn:
          __ic->int_handlers[NMI_HANDLER_OFFSET] = handler;
          __ic->int_context[NMI_HANDLER_OFFSET] = context;
          // TODO: enable if possible, ;
          break;
        default:
          return HDL_FALSE;
      }
    }
    else {
      uint32_t handler_offset = IRQ_N_HANDLERS_OFFSET + interrupt->irq_type;
      __ic->int_handlers[handler_offset] = handler;
      __ic->int_context[handler_offset] = context;
      NVIC_EnableIRQ(interrupt->irq_type);
    }
    return HDL_TRUE;
  }
  return HDL_FALSE;
}
