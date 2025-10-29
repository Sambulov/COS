#include "hdl_portable.h"

void WWDG_IRQHandler()            { call_isr(HDL_NVIC_IRQ0_WWDG, 0); }
void PVD_IRQHandler()             { call_isr(HDL_NVIC_IRQ1_PVD, 0); }
void RTC_IRQHandler()             { call_isr(HDL_NVIC_IRQ2_RTC, 0); }
void TAMPER_IRQHandler()          { call_isr(HDL_NVIC_IRQ3_TAMPER, 0); }
void FLASH_IRQHandler()           { call_isr(HDL_NVIC_IRQ4_FLASH, 0); }
void RCC_IRQHandler()             { call_isr(HDL_NVIC_IRQ5_RCC, 0); }
void EXTI0_IRQHandler()           { call_isr(HDL_NVIC_IRQ6_EXTI0, 0); }
void EXTI1_IRQHandler()           { call_isr(HDL_NVIC_IRQ7_EXTI1, 0); }
void EXTI2_IRQHandler()           { call_isr(HDL_NVIC_IRQ8_EXTI2, 0); }
void EXTI3_IRQHandler()           { call_isr(HDL_NVIC_IRQ9_EXTI3, 0); }
void EXTI4_IRQHandler()           { call_isr(HDL_NVIC_IRQ10_EXTI4, 0); }
void DMA1_Channel1_IRQHandler()   { call_isr(HDL_NVIC_IRQ11_DMA1_Channel1, 0); }
void DMA1_Channel2_IRQHandler()   { call_isr(HDL_NVIC_IRQ12_DMA1_Channel2, 0); }
void DMA1_Channel3_IRQHandler()   { call_isr(HDL_NVIC_IRQ13_DMA1_Channel3, 0); }
void DMA1_Channel4_IRQHandler()   { call_isr(HDL_NVIC_IRQ14_DMA1_Channel4, 0); }
void DMA1_Channel5_IRQHandler()   { call_isr(HDL_NVIC_IRQ15_DMA1_Channel5, 0); }
void DMA1_Channel6_IRQHandler()   { call_isr(HDL_NVIC_IRQ16_DMA1_Channel6, 0); }
void DMA1_Channel7_IRQHandler()   { call_isr(HDL_NVIC_IRQ17_DMA1_Channel7, 0); }
void ADC1_2_IRQHandler()          { call_isr(HDL_NVIC_IRQ18_ADC1, 0); }
void USB_HP_CAN1_TX_IRQHandler()  { call_isr(HDL_NVIC_IRQ19_USB_HP, 0); }
void USB_LP_CAN1_RX0_IRQHandler() { call_isr(HDL_NVIC_IRQ20_USB_LP, 0); }
void EXTI9_5_IRQHandler()         { call_isr(HDL_NVIC_IRQ23_EXTI9_5, 0); }
void TIM2_IRQHandler()            { call_isr(HDL_NVIC_IRQ28_TIM2, 0); }
void TIM3_IRQHandler()            { call_isr(HDL_NVIC_IRQ29_TIM3, 0); }
void TIM4_IRQHandler()            { call_isr(HDL_NVIC_IRQ30_TIM4, 0); }
void I2C1_EV_IRQHandler()         { call_isr(HDL_NVIC_IRQ31_I2C1_EV, 0); }
void I2C1_ER_IRQHandler()         { call_isr(HDL_NVIC_IRQ32_I2C1_ER, 0); }
void I2C2_EV_IRQHandler()         { call_isr(HDL_NVIC_IRQ33_I2C2_EV, 0); }
void I2C2_ER_IRQHandler()         { call_isr(HDL_NVIC_IRQ34_I2C2_ER, 0); }
void SPI1_IRQHandler()            { call_isr(HDL_NVIC_IRQ35_SPI1, 0); }
void SPI2_IRQHandler()            { call_isr(HDL_NVIC_IRQ36_SPI2, 0); }
void USART1_IRQHandler()          { call_isr(HDL_NVIC_IRQ37_USART1, 0); }
void USART2_IRQHandler()          { call_isr(HDL_NVIC_IRQ38_USART2, 0); }
void USART3_IRQHandler()          { call_isr(HDL_NVIC_IRQ39_USART3, 0); }
void EXTI15_10_IRQHandler()       { call_isr(HDL_NVIC_IRQ40_EXTI15_10, 0); }
void RTC_Alarm_IRQHandler()       { call_isr(HDL_NVIC_IRQ41_RTC_Alarm, 0); }
void USBWakeUp_IRQHandler()       { call_isr(HDL_NVIC_IRQ42_USBWakeUp, 0); }

static hdl_module_state_t _hdl_core(const void *desc, uint8_t enable) {
  (void)desc;
  if(enable) {
    hdl_core_arm_t *core = (hdl_core_arm_t *)desc;
    // Unlock Flash Regs
    //FLASH->KEYR = FLASH_KEY1;
    //FLASH->KEYR = FLASH_KEY2;
    CL_REG_MODIFY(FLASH->ACR, FLASH_ACR_LATENCY, core->config->flash_latency);
    CL_REG_SET(FLASH->ACR, FLASH_ACR_PRFTBE);
    //rcu_periph_clock_enable(RCU_CFGCMP);
    if(CL_REG_GET(FLASH->ACR, FLASH_ACR_LATENCY) == core->config->flash_latency) 
      return HDL_MODULE_ACTIVE;
  }
  //rcu_periph_clock_disable(RCU_CFGCMP);
  //FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT_0;
  return HDL_MODULE_UNLOADED;
}

const hdl_module_base_iface_t hdl_core_arm_iface = {
  .init = _hdl_core
};
