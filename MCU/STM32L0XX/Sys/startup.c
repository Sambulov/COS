#include <stddef.h>
extern void *_estack;

void Reset_Handler();
void Default_Handler();
void NMI_Handler()                     __attribute__ ((weak, alias ("Default_Handler")));
void HardFault_Handler()               __attribute__ ((weak, alias ("Default_Handler")));
void SVC_Handler()                     __attribute__ ((weak, alias ("Default_Handler")));
void PendSV_Handler()                  __attribute__ ((weak, alias ("Default_Handler")));
void SysTick_Handler()                 __attribute__ ((weak, alias ("Default_Handler")));
void WWDG_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void PVD_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void RTC_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void FLASH_IRQHandler()                __attribute__ ((weak, alias ("Default_Handler")));
void RCC_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void EXTI0_1_IRQHandler()              __attribute__ ((weak, alias ("Default_Handler")));
void EXTI2_3_IRQHandler()              __attribute__ ((weak, alias ("Default_Handler")));
void EXTI4_15_IRQHandler()             __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Channel1_IRQHandler()        __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Channel2_3_IRQHandler()      __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Channel4_5_6_7_IRQHandler()  __attribute__ ((weak, alias ("Default_Handler")));
void ADC1_COMP_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void LPTIM1_IRQHandler()               __attribute__ ((weak, alias ("Default_Handler")));
void USART4_5_IRQHandler()             __attribute__ ((weak, alias ("Default_Handler")));
void TIM2_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void TIM3_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void TIM6_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void TIM7_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void TIM21_IRQHandler()                __attribute__ ((weak, alias ("Default_Handler")));
void I2C3_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void TIM22_IRQHandler()                __attribute__ ((weak, alias ("Default_Handler")));
void I2C1_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void I2C2_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void SPI1_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void SPI2_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void USART1_IRQHandler()               __attribute__ ((weak, alias ("Default_Handler")));
void USART2_IRQHandler()               __attribute__ ((weak, alias ("Default_Handler")));
void LPUART1_IRQHandler()              __attribute__ ((weak, alias ("Default_Handler")));

void * g_pfnVectors[0x30] __attribute__ ((section (".isr_vector"), used)) = 
{
	&_estack,
	&Reset_Handler,
	&NMI_Handler,
	&HardFault_Handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&SVC_Handler,
	NULL,
	NULL,
	&PendSV_Handler,
	&SysTick_Handler,
	&WWDG_IRQHandler,
	&PVD_IRQHandler,
	&RTC_IRQHandler,
	&FLASH_IRQHandler,
	&RCC_IRQHandler,
	&EXTI0_1_IRQHandler,
	&EXTI2_3_IRQHandler,
	&EXTI4_15_IRQHandler,
	NULL,
	&DMA1_Channel1_IRQHandler,
	&DMA1_Channel2_3_IRQHandler,
	&DMA1_Channel4_5_6_7_IRQHandler,
	&ADC1_COMP_IRQHandler,
	&LPTIM1_IRQHandler,
	&USART4_5_IRQHandler,
	&TIM2_IRQHandler,
	&TIM3_IRQHandler,
	&TIM6_IRQHandler,
	&TIM7_IRQHandler,
	NULL,
	&TIM21_IRQHandler,
	&I2C3_IRQHandler,
	&TIM22_IRQHandler,
	&I2C1_IRQHandler,
	&I2C2_IRQHandler,
	&SPI1_IRQHandler,
	&SPI2_IRQHandler,
	&USART1_IRQHandler,
	&USART2_IRQHandler,
	&LPUART1_IRQHandler,
	NULL,
	NULL,
};

void SystemInit();
void __libc_init_array();
void main();

extern void *_sidata, *_sdata, *_edata;
extern void *_sbss, *_ebss;

void __attribute__((naked, noreturn)) Reset_Handler() {
	//__asm ("ldr sp, =_estack");

	void **pSource, **pDest;
	for (pSource = &_sidata, pDest = &_sdata; pDest < &_edata; pSource++, pDest++)
		*pDest = *pSource;

	for (pDest = &_sbss; pDest < &_ebss; pDest++)
		*pDest = 0;

	SystemInit();
	__libc_init_array();
	main();
	for (;;) ;
}

void __attribute__((naked, noreturn)) Default_Handler() {
	//If you get stuck here, your code is missing a handler for some interrupt.
	for (;;) ;
}
