extern void *_estack;
extern void *_sidata, *_sdata, *_edata;
extern void *_sbss, *_ebss;

void Reset_Handler();
void DefaultHandler();
void NMI_Handler()                        __attribute__ ((weak, alias ("DefaultHandler")));
void HardFault_Handler()                  __attribute__ ((weak, alias ("DefaultHandler")));
void SVC_Handler()                        __attribute__ ((weak, alias ("DefaultHandler")));
void PendSV_Handler()                     __attribute__ ((weak, alias ("DefaultHandler")));
void SysTick_Handler()                    __attribute__ ((weak, alias ("DefaultHandler")));
void WWDGT_IRQHandler()                   __attribute__ ((weak, alias ("DefaultHandler")));
void LVD_IRQHandler()                     __attribute__ ((weak, alias ("DefaultHandler")));
void RTC_IRQHandler()                     __attribute__ ((weak, alias ("DefaultHandler")));
void FMC_IRQHandler()                     __attribute__ ((weak, alias ("DefaultHandler")));
void RCU_IRQHandler()                     __attribute__ ((weak, alias ("DefaultHandler")));
void EXTI0_1_IRQHandler()                 __attribute__ ((weak, alias ("DefaultHandler")));
void EXTI2_3_IRQHandler()                 __attribute__ ((weak, alias ("DefaultHandler")));
void EXTI4_15_IRQHandler()                __attribute__ ((weak, alias ("DefaultHandler")));
void DMA_Channel0_IRQHandler()            __attribute__ ((weak, alias ("DefaultHandler")));
void DMA_Channel1_2_IRQHandler()          __attribute__ ((weak, alias ("DefaultHandler")));
void DMA_Channel3_4_IRQHandler()          __attribute__ ((weak, alias ("DefaultHandler")));
void ADC_CMP_IRQHandler()                 __attribute__ ((weak, alias ("DefaultHandler")));
void TIMER0_BRK_UP_TRG_COM_IRQHandler()   __attribute__ ((weak, alias ("DefaultHandler")));
void TIMER0_Channel_IRQHandler()          __attribute__ ((weak, alias ("DefaultHandler")));
void TIMER2_IRQHandler()                  __attribute__ ((weak, alias ("DefaultHandler")));
void TIMER5_IRQHandler()                  __attribute__ ((weak, alias ("DefaultHandler")));
void TIMER13_IRQHandler()                 __attribute__ ((weak, alias ("DefaultHandler")));
void TIMER14_IRQHandler()                 __attribute__ ((weak, alias ("DefaultHandler")));
void TIMER15_IRQHandler()                 __attribute__ ((weak, alias ("DefaultHandler")));
void TIMER16_IRQHandler()                 __attribute__ ((weak, alias ("DefaultHandler")));
void I2C0_EV_IRQHandler()                 __attribute__ ((weak, alias ("DefaultHandler")));
void I2C1_EV_IRQHandler()                 __attribute__ ((weak, alias ("DefaultHandler")));
void SPI0_IRQHandler()                    __attribute__ ((weak, alias ("DefaultHandler")));
void SPI1_IRQHandler()                    __attribute__ ((weak, alias ("DefaultHandler")));
void USART0_IRQHandler()                  __attribute__ ((weak, alias ("DefaultHandler")));
void USART1_IRQHandler()                  __attribute__ ((weak, alias ("DefaultHandler")));
void I2C0_ER_IRQHandler()                 __attribute__ ((weak, alias ("DefaultHandler")));
void I2C1_ER_IRQHandler()                 __attribute__ ((weak, alias ("DefaultHandler")));

void * g_pfnVectors[0x40] __attribute__ ((section (".isr_vector"), used)) = {
  &_estack,
  &Reset_Handler,
  &NMI_Handler,
  &HardFault_Handler,
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  &SVC_Handler,
  ((void *)0),
  ((void *)0),
  &PendSV_Handler,
  &SysTick_Handler,
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

void SystemInit();
void __libc_init_array();
void main();

void __attribute__((naked, noreturn)) Reset_Handler() {
	//asm ("ldr sp, =_estack");
	void **pSource, **pDest;
	for (pSource = &_sidata, pDest = &_sdata; pDest != &_edata; pSource++, pDest++)
	  *pDest = *pSource;

  for (pDest = &_sbss; pDest != &_ebss; pDest++)
    *pDest = 0;
  //turn off JTAG;
  __libc_init_array();
  SystemInit();
  main();
  for (;;) ;
}

void __attribute__((naked, noreturn)) DefaultHandler() {
	//If you get stuck here, your code is missing a handler for some interrupt.
	asm("bkpt 255");
}
