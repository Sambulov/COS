#include "hdl_portable.h"
#include "CodeLib.h"

void WWDGT_IRQHandler()                 { call_isr(WWDGT_IRQn, 0); }
void LVD_IRQHandler()                   { call_isr(LVD_IRQn, 0); }
void RTC_IRQHandler()                   { call_isr(RTC_IRQn, 0); }
void FMC_IRQHandler()                   { call_isr(FMC_IRQn, 0); }
void RCU_IRQHandler()                   { call_isr(RCU_IRQn, 0); }
void EXTI0_1_IRQHandler()               { call_isr(EXTI0_1_IRQn, 0); EXTI_PD |= (EXTI_0 | EXTI_1); }
void EXTI2_3_IRQHandler()               { call_isr(EXTI2_3_IRQn, 0); EXTI_PD |= (EXTI_2 | EXTI_3); }
void EXTI4_15_IRQHandler()              { call_isr(EXTI4_15_IRQn, 0); EXTI_PD |= EXTI_LINES_4_15; }
void DMA_Channel0_IRQHandler()          { call_isr(DMA_Channel0_IRQn, 0); }
void DMA_Channel1_2_IRQHandler()        { call_isr(DMA_Channel1_2_IRQn, 0); }
void DMA_Channel3_4_IRQHandler()        { call_isr(DMA_Channel3_4_IRQn, 0); }
void ADC_CMP_IRQHandler()               { call_isr(ADC_CMP_IRQn, 0); }
void TIMER0_BRK_UP_TRG_COM_IRQHandler() { call_isr(TIMER0_BRK_UP_TRG_COM_IRQn, 0); TIMER_INTF(TIMER0) &= ~TIMER_INTF_UPIF; }
void TIMER0_Channel_IRQHandler()        { call_isr(TIMER0_Channel_IRQn, 0); }
void TIMER2_IRQHandler()                { call_isr(TIMER2_IRQn, 0); TIMER_INTF(TIMER2) &= ~TIMER_INTF_UPIF; }
void TIMER5_IRQHandler()                { call_isr(TIMER5_IRQn, 0); }
void TIMER13_IRQHandler()               { call_isr(TIMER13_IRQn, 0); }
void TIMER14_IRQHandler()               { call_isr(TIMER14_IRQn, 0); }
void TIMER15_IRQHandler()               { call_isr(TIMER15_IRQn, 0); }
void TIMER16_IRQHandler()               { call_isr(TIMER16_IRQn, 0); }
void I2C0_EV_IRQHandler()               { call_isr(I2C0_EV_IRQn, 0); }
void I2C1_EV_IRQHandler()               { call_isr(I2C1_EV_IRQn, 0); }
void SPI0_IRQHandler()                  { call_isr(SPI0_IRQn, 0); }
void SPI1_IRQHandler()                  { call_isr(SPI1_IRQn, 0); }
void USART0_IRQHandler()                { call_isr(USART0_IRQn, 0); }
void USART1_IRQHandler()                { call_isr(USART1_IRQn, 0); }
void I2C0_ER_IRQHandler()               { call_isr(I2C0_ER_IRQn, 0); }
void I2C1_ER_IRQHandler()               { call_isr(I2C1_ER_IRQn, 0); }
