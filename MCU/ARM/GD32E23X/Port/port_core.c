#include "hdl_portable.h"
#include "Macros.h"
typedef struct {
  IRQn_Type irq_type;
  uint8_t priority_group;
  uint8_t priority;
  void *handler_context;
  event_handler_t handler;
} hdl_nvic_interrupt_private_t;

typedef struct {
  hdl_module_t module;
  uint32_t prio_bits;
  hdl_nvic_interrupt_private_t **interrupts;
} hdl_nvic_private_t;

#define hdl_exti_clear_pending(exti_line)       (EXTI_PD |= exti_line)

#define EXTI_LINES_ALL_POSSIBLE   EXTI_0  | EXTI_1  | EXTI_2  |  EXTI_3 | \
                                  EXTI_4  | EXTI_5  | EXTI_6  |  EXTI_7 | \
                                  EXTI_8  | EXTI_9  | EXTI_10 | EXTI_11 | \
                                  EXTI_12 | EXTI_13 | EXTI_14 | EXTI_15 | \
                                  EXTI_16 | EXTI_17 | EXTI_19 | EXTI_21

#define EXTI_LINES_4_15           EXTI_4  | EXTI_5  | EXTI_6  |  EXTI_7 | \
                                  EXTI_8  | EXTI_9  | EXTI_10 | EXTI_11 | \
                                  EXTI_12 | EXTI_13 | EXTI_14 | EXTI_15

_Static_assert(sizeof(hdl_nvic_interrupt_private_t) == sizeof(hdl_nvic_interrupt_t), "In hdl_core.h data structure size of hdl_nvic_interrupt_t doesn't match, check HDL_INTERRUPT_PRV_SIZE");

extern void *_estack;
extern void *_sidata, *_sdata, *_edata;
extern void *_sbss, *_ebss;

//void SystemInit();
void __libc_init_array();
void main();
hdl_nvic_private_t *__ic = NULL;

static void _call_isr(IRQn_Type irq, hdl_nvic_interrupt_private_t **isrs, uint32_t event) {
  if(isrs != NULL) {
    while (*isrs != NULL) {
      if((*isrs)->irq_type == irq) {
        (*isrs)->handler(event, __ic, (*isrs)->handler_context);
        return;
      }
      isrs++;
    }
  }
  //If you get stuck here, your code is missing a handler for some interrupt.
	asm("bkpt 255");
  for(;;) ;
}

/* TODO: test with __attribute__ ((naked)) */
void Reset_Handler();
void irq_n_handler();
void nmi_handler()                      { _call_isr(NonMaskableInt_IRQn, __ic->interrupts, 0); }
void hard_fault_handler()               { _call_isr(HardFault_IRQn, __ic->interrupts, 0); }
void svc_handler();
void pend_sv_handler()                  { _call_isr(PendSV_IRQn, __ic->interrupts, 0); }
void systick_handler()                  { _call_isr(SysTick_IRQn, __ic->interrupts, 0); }

void WWDGT_IRQHandler()                 { _call_isr(WWDGT_IRQn, __ic->interrupts, 0); }
void LVD_IRQHandler()                   { _call_isr(LVD_IRQn, __ic->interrupts, 0); }
void RTC_IRQHandler()                   { _call_isr(RTC_IRQn, __ic->interrupts, 0); }
void FMC_IRQHandler()                   { _call_isr(FMC_IRQn, __ic->interrupts, 0); }
void RCU_IRQHandler()                   { _call_isr(RCU_IRQn, __ic->interrupts, 0); }
void EXTI0_1_IRQHandler();
void EXTI2_3_IRQHandler();
void EXTI4_15_IRQHandler();
void DMA_Channel0_IRQHandler()          { _call_isr(DMA_Channel0_IRQn, __ic->interrupts, 0); }
void DMA_Channel1_2_IRQHandler()        { _call_isr(DMA_Channel1_2_IRQn, __ic->interrupts, 0); }
void DMA_Channel3_4_IRQHandler()        { _call_isr(DMA_Channel3_4_IRQn, __ic->interrupts, 0); }
void ADC_CMP_IRQHandler()               { _call_isr(ADC_CMP_IRQn, __ic->interrupts, 0); }
void TIMER0_BRK_UP_TRG_COM_IRQHandler() {
  _call_isr(TIMER0_BRK_UP_TRG_COM_IRQn, __ic->interrupts, 0);
  TIMER_INTF(TIMER0) &= ~TIMER_INTF_UPIF;
}
void TIMER0_Channel_IRQHandler()        { _call_isr(TIMER0_Channel_IRQn, __ic->interrupts, 0); }
void TIMER2_IRQHandler() {
  _call_isr(TIMER2_IRQn, __ic->interrupts, 0);
  TIMER_INTF(TIMER2) &= ~TIMER_INTF_UPIF;
}
void TIMER5_IRQHandler()                { _call_isr(TIMER5_IRQn, __ic->interrupts, 0); }
void TIMER13_IRQHandler()               { _call_isr(TIMER13_IRQn, __ic->interrupts, 0); }
void TIMER14_IRQHandler()               { _call_isr(TIMER14_IRQn, __ic->interrupts, 0); }
void TIMER15_IRQHandler()               { _call_isr(TIMER15_IRQn, __ic->interrupts, 0); }
void TIMER16_IRQHandler()               { _call_isr(TIMER16_IRQn, __ic->interrupts, 0); }
void I2C0_EV_IRQHandler()               { _call_isr(I2C0_EV_IRQn, __ic->interrupts, 0); }
void I2C1_EV_IRQHandler()               { _call_isr(I2C1_EV_IRQn, __ic->interrupts, 0); }
void SPI0_IRQHandler()                  { _call_isr(SPI0_IRQn, __ic->interrupts, 0); }
void SPI1_IRQHandler()                  { _call_isr(SPI1_IRQn, __ic->interrupts, 0); }
void USART0_IRQHandler()                { _call_isr(USART0_IRQn, __ic->interrupts, 0); }
void USART1_IRQHandler()                { _call_isr(USART1_IRQn, __ic->interrupts, 0); }
void I2C0_ER_IRQHandler()               { _call_isr(I2C0_ER_IRQn, __ic->interrupts, 0); }
void I2C1_ER_IRQHandler()               { _call_isr(I2C1_ER_IRQn, __ic->interrupts, 0); }

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
  &WWDGT_IRQHandler,                    /* IRQ0 */ 
  &LVD_IRQHandler,                      /* IRQ1 */
  &RTC_IRQHandler,                      /* IRQ2 */
  &FMC_IRQHandler,                      /* IRQ3 */
  &RCU_IRQHandler,                      /* IRQ4 */
  &EXTI0_1_IRQHandler,                  /* IRQ5 */
  &EXTI2_3_IRQHandler,                  /* IRQ6 */
  &EXTI4_15_IRQHandler,                 /* IRQ7 */
  &irq_n_handler,                       /* IRQ8 */
  &DMA_Channel0_IRQHandler,             /* IRQ9 */
  &DMA_Channel1_2_IRQHandler,           /* IRQ10 */
  &DMA_Channel3_4_IRQHandler,           /* IRQ11 */
  &ADC_CMP_IRQHandler,                  /* IRQ12 */
  &TIMER0_BRK_UP_TRG_COM_IRQHandler,    /* IRQ13 */
  &TIMER0_Channel_IRQHandler,           /* IRQ14 */
  &irq_n_handler,                       /* IRQ15 */
  &TIMER2_IRQHandler,                   /* IRQ16 */
  &TIMER5_IRQHandler,                   /* IRQ17 */
  &irq_n_handler,                       /* IRQ18 */
  &TIMER13_IRQHandler,                  /* IRQ19 */
  &TIMER14_IRQHandler,                  /* IRQ21 */
  &TIMER15_IRQHandler,                  /* IRQ22 */
  &TIMER16_IRQHandler,                  /* IRQ23 */
  &I2C0_EV_IRQHandler,                  /* IRQ24 */
  &I2C1_EV_IRQHandler,                  /* IRQ25 */
  &SPI0_IRQHandler,                     /* IRQ26 */
  &SPI1_IRQHandler,                     /* IRQ27 */
  &USART0_IRQHandler,                   /* IRQ28 */
  &USART1_IRQHandler,                   /* IRQ29 */
  &irq_n_handler,                       /* IRQ30 */
  &irq_n_handler,                       /* IRQ31 */
  &irq_n_handler,                       /* IRQ32 */
  &I2C0_ER_IRQHandler,                  /* IRQ33 */
  &irq_n_handler,                       /* IRQ34 */
  &I2C1_ER_IRQHandler,                  /* IRQ35 */
  &irq_n_handler,                       /* IRQ36 */
  &irq_n_handler,                       /* IRQ37 */
  &irq_n_handler,                       /* IRQ38 */
  &irq_n_handler,                       /* IRQ39 */
  &irq_n_handler,                       /* IRQ40 */
  &irq_n_handler,                       /* IRQ41 */
  &irq_n_handler,                       /* IRQ42 */
  &irq_n_handler,                       /* IRQ43 */
  &irq_n_handler,                       /* IRQ44 */
  &irq_n_handler,                       /* IRQ45 */
  &irq_n_handler,                       /* IRQ46 */
  &irq_n_handler,                       /* IRQ47 */
  &irq_n_handler,                       /* IRQ48 */
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
  _call_isr(SVCall_IRQn, __ic->interrupts, (uint8_t)*instruction);
}

void EXTI0_1_IRQHandler() {
  _call_isr(EXTI0_1_IRQn, __ic->interrupts, EXTI_PD);
  hdl_exti_clear_pending(EXTI_0 | EXTI_1);
}

void EXTI2_3_IRQHandler() {
  _call_isr(EXTI2_3_IRQn, __ic->interrupts, EXTI_PD);
  hdl_exti_clear_pending(EXTI_2 | EXTI_3);
}

void EXTI4_15_IRQHandler() {
  _call_isr(EXTI4_15_IRQn, __ic->interrupts, EXTI_PD);
  hdl_exti_clear_pending(EXTI_LINES_4_15);
}

void irq_n_handler() {
  uint32_t prio = -1;
  IRQn_Type irq = 0;
  for(uint32_t i = 0; i< sizeof(NVIC->IABR); i++) {
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
  _call_isr(irq, __ic->interrupts, 0);
}

hdl_module_state_t hdl_core(void *desc, uint8_t enable) {
  /* TODO: */
  if(enable) {
    hdl_core_t *core = (hdl_core_t *)desc;
    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | core->flash_latency;
    rcu_periph_clock_enable(RCU_CFGCMP);
    return HDL_MODULE_INIT_OK;
  }
  rcu_periph_clock_disable(RCU_CFGCMP);
  FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT_0;
  return HDL_MODULE_DEINIT_OK;
}

void _hdl_exti_request(hdl_interrupt_controller_t *ic) {
  hdl_nvic_exti_t **extis = ic->exti_lines;
  if(extis != NULL) {
    while (*extis != NULL) {
      uint8_t exti_no = 31 - __CLZ((*extis)->line);
      if(exti_no <= 15) { /* if GPIO exti lines */
        syscfg_exti_line_config((*extis)->source, exti_no);
      } /* other lines from internal modules are fixed */
      if((*extis)->trigger & HDL_EXTI_TRIGGER_FALLING) {
        EXTI_FTEN |= (*extis)->line;
      }
      else {
        EXTI_FTEN &= ~((*extis)->line);
      }
      if((*extis)->trigger & HDL_EXTI_TRIGGER_RISING) {
        EXTI_RTEN |= (*extis)->line;
      }
      else {
        EXTI_RTEN &= ~((*extis)->line);
      }
      EXTI_EVEN |= (*extis)->line;
      extis++;
    }
  }
}
hdl_module_state_t hdl_nvic(void *desc, uint8_t enable) {
  if(enable) {
    hdl_nvic_t *nvic = (hdl_nvic_t *)desc;
    /* NVIC_SetPriorityGrouping   not available for Cortex-M23 */
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0);
    SYSCFG_CPU_IRQ_LAT = nvic->irq_latency;
    __ic = (hdl_nvic_private_t *)desc;
    /* TODO: fing wokaround to save context for interrupt vector */
    _hdl_exti_request(nvic);
    return HDL_MODULE_INIT_OK;
  }
  else {
    //TODO: disable envic
    HDL_REG_CLEAR(EXTI_INTEN, EXTI_LINES_ALL_POSSIBLE);
    HDL_REG_CLEAR(EXTI_FTEN, EXTI_LINES_ALL_POSSIBLE);
    HDL_REG_CLEAR(EXTI_RTEN, EXTI_LINES_ALL_POSSIBLE);
    HDL_REG_CLEAR(EXTI_EVEN, EXTI_LINES_ALL_POSSIBLE);
    HDL_REG_CLEAR(EXTI_FTEN, EXTI_LINES_ALL_POSSIBLE);
    HDL_REG_CLEAR(EXTI_FTEN, EXTI_LINES_ALL_POSSIBLE);
    HDL_REG_CLEAR(EXTI_FTEN, EXTI_LINES_ALL_POSSIBLE);
    hdl_exti_clear_pending(EXTI_LINES_ALL_POSSIBLE);
  }
  return HDL_MODULE_DEINIT_OK;
}

static void _hdl_nvic_exti_interrupt_enable(hdl_nvic_t *ic, hdl_nvic_interrupt_private_t **isr) {
  hdl_nvic_exti_t **extis = ic->exti_lines;
  if(extis == NULL) return;
  hdl_exti_line_t exti_lines_int_en = 0;
  switch ((*isr)->irq_type) {
    case HDL_NVIC_IRQ5_EXTI0_1:
      exti_lines_int_en = EXTI_0 | EXTI_1;
      break;
    case HDL_NVIC_IRQ6_EXTI2_3:
      exti_lines_int_en = EXTI_2 | EXTI_3;
      break;
    case HDL_NVIC_IRQ7_EXTI4_15:
      exti_lines_int_en = EXTI_LINES_4_15;
      break;
    default:
      return;
  }
  while ((*extis != NULL) && (exti_lines_int_en)) {
    if((exti_lines_int_en & ((*extis)->line)) &&
       ((*extis)->mode == HDL_EXTI_MODE_INTERRUPT)) {
      EXTI_INTEN |= (*extis)->line;
    }
    exti_lines_int_en &= ~((*extis)->line);
    extis++;
  }
}

uint8_t hdl_interrupt_request(hdl_interrupt_controller_t *ic, hdl_nvic_irq_n_t irq, event_handler_t handler, void *context) {
  if((hdl_state(&ic->module) != HDL_MODULE_INIT_OK) || (ic->interrupts == NULL) || (handler == NULL))
    return HDL_FALSE;
  hdl_nvic_interrupt_private_t **isr = (hdl_nvic_interrupt_private_t **)ic->interrupts;
  while ((isr != NULL) && (*isr)->irq_type != irq) isr++;
  if(isr == NULL) return HDL_FALSE;
  (*isr)->handler = handler;
  (*isr)->handler_context = context;
  uint32_t prio = (((*isr)->priority_group << (8U - ic->prio_bits)) | 
                  ((*isr)->priority & (0xFF >> ic->prio_bits)) & 
                  0xFFUL);
  uint32_t shift = _BIT_SHIFT((*isr)->irq_type);
  volatile uint32_t *ipr = ((*isr)->irq_type < 0)? &(SCB->SHPR[_SHP_IDX((*isr)->irq_type)]):
                                                    &(NVIC->IPR[_IP_IDX((*isr)->irq_type)]);
  /* set priority for interrupt */
  *ipr = (*ipr & ~(0xFFUL << shift)) | (prio << shift);
  /* interrupt enable */
  if((*isr)->irq_type < 0) {
    switch ((*isr)->irq_type) {
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
    _hdl_nvic_exti_interrupt_enable(ic, isr);
    NVIC_EnableIRQ((*isr)->irq_type);
  }
  return HDL_TRUE;
}

void hdl_exti_sw_trigger(hdl_interrupt_controller_t *desc, hdl_exti_line_t line) {
  EXTI_SWIEV |= line;
}
