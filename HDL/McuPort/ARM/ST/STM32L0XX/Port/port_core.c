#include "hdl_portable.h"

void WWDG_IRQHandler()                 { call_isr(HDL_NVIC_IRQ0_WWDG, 0); }
void PVD_IRQHandler()                  { call_isr(HDL_NVIC_IRQ1_PVD, 0); }
void RTC_IRQHandler()                  { call_isr(HDL_NVIC_IRQ2_RTC, 0); }
void FLASH_IRQHandler()                { call_isr(HDL_NVIC_IRQ3_FLASH, 0); }
void RCC_IRQHandler()                  { call_isr(HDL_NVIC_IRQ4_RCC, 0); }
void EXTI0_1_IRQHandler()              { call_isr(HDL_NVIC_IRQ5_EXTI0_1, 0); }
void EXTI2_3_IRQHandler()              { call_isr(HDL_NVIC_IRQ6_EXTI2_3, 0); }
void EXTI4_15_IRQHandler()             { call_isr(HDL_NVIC_IRQ7_EXTI4_15, 0); }
void DMA1_Channel1_IRQHandler()        { call_isr(HDL_NVIC_IRQ9_DMA1_Channel1, 0); }
void DMA1_Channel2_3_IRQHandler()      { call_isr(HDL_NVIC_IRQ0_DMA1_Channel2_3, 0); }
void DMA1_Channel4_5_6_7_IRQHandler()  { call_isr(HDL_NVIC_IRQ11_DMA1_Channel4_5_6_7, 0); }
void ADC1_COMP_IRQHandler()            { call_isr(HDL_NVIC_IRQ12_ADC1_COMP, 0); }
void LPTIM1_IRQHandler()               { call_isr(HDL_NVIC_IRQ13_LPTIM1, 0); }
void USART4_5_IRQHandler()             { call_isr(HDL_NVIC_IRQ14_USART4_5, 0); }
void TIM2_IRQHandler()                 { call_isr(HDL_NVIC_IRQ15_TIM2, 0); }
void TIM3_IRQHandler()                 { call_isr(HDL_NVIC_IRQ16_TIM3, 0); }
void TIM6_IRQHandler()                 { call_isr(HDL_NVIC_IRQ17_TIM6, 0); }
void TIM7_IRQHandler()                 { call_isr(HDL_NVIC_IRQ18_TIM7, 0); }
void TIM21_IRQHandler()                { call_isr(HDL_NVIC_IRQ20_TIM21, 0); }
void I2C3_IRQHandler()                 { call_isr(HDL_NVIC_IRQ21_I2C3, 0); }
void TIM22_IRQHandler()                { call_isr(HDL_NVIC_IRQ22_TIM22, 0); }
void I2C1_IRQHandler()                 { call_isr(HDL_NVIC_IRQ23_I2C1, 0); }
void I2C2_IRQHandler()                 { call_isr(HDL_NVIC_IRQ24_I2C2, 0); }
void SPI1_IRQHandler()                 { call_isr(HDL_NVIC_IRQ25_SPI1, 0); }
void SPI2_IRQHandler()                 { call_isr(HDL_NVIC_IRQ26_SPI2, 0); }
void USART1_IRQHandler()               { call_isr(HDL_NVIC_IRQ27_USART1, 0); }
void USART2_IRQHandler()               { call_isr(HDL_NVIC_IRQ28_USART2, 0); }
void LPUART1_IRQHandler()              { call_isr(HDL_NVIC_IRQ29_LPUART1, 0); }


hdl_module_state_t hdl_core(void *desc, uint8_t enable) {
  if(enable) {
    hdl_core_t *core = (hdl_core_t *)desc;
    //FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | core->config->flash_latency;
    //rcu_periph_clock_enable(RCU_CFGCMP);
    return HDL_MODULE_ACTIVE;
  }
  //rcu_periph_clock_disable(RCU_CFGCMP);
  //FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT_0;
  return HDL_MODULE_UNLOADED;
}
