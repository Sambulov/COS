#include "hdl_portable.h"

void WWDG_IRQHandler()               { call_isr(HDL_NVIC_IRQ0_WWDG, 0); }
void PVD_IRQHandler()                { call_isr(HDL_NVIC_IRQ1_PVD, 0); }
void TAMP_STAMP_IRQHandler()         { call_isr(HDL_NVIC_IRQ2_TAMP_STAMP, 0); }
void RTC_WKUP_IRQHandler()           { call_isr(HDL_NVIC_IRQ3_RTC_WKUP, 0); }
void FLASH_IRQHandler()              { call_isr(HDL_NVIC_IRQ4_FLASH, 0); }
void RCC_IRQHandler()                { call_isr(HDL_NVIC_IRQ5_RCC, 0); }
void EXTI0_IRQHandler()              { call_isr(HDL_NVIC_IRQ6_EXTI0, 0); }
void EXTI1_IRQHandler()              { call_isr(HDL_NVIC_IRQ7_EXTI1, 0); }
void EXTI2_IRQHandler()              { call_isr(HDL_NVIC_IRQ8_EXTI2, 0); }
void EXTI3_IRQHandler()              { call_isr(HDL_NVIC_IRQ9_EXTI3, 0); }
void EXTI4_IRQHandler()              { call_isr(HDL_NVIC_IRQ10_EXTI4, 0); }
void DMA1_Stream0_IRQHandler()       { call_isr(HDL_NVIC_IRQ11_DMA1_Stream0, 0); }
void DMA1_Stream1_IRQHandler()       { call_isr(HDL_NVIC_IRQ12_DMA1_Stream1, 0); }
void DMA1_Stream2_IRQHandler()       { call_isr(HDL_NVIC_IRQ13_DMA1_Stream2, 0); }
void DMA1_Stream3_IRQHandler()       { call_isr(HDL_NVIC_IRQ14_DMA1_Stream3, 0); }
void DMA1_Stream4_IRQHandler()       { call_isr(HDL_NVIC_IRQ15_DMA1_Stream4, 0); }
void DMA1_Stream5_IRQHandler()       { call_isr(HDL_NVIC_IRQ16_DMA1_Stream5, 0); }
void DMA1_Stream6_IRQHandler()       { call_isr(HDL_NVIC_IRQ17_DMA1_Stream6, 0); }
void ADC_IRQHandler()                { call_isr(HDL_NVIC_IRQ18_ADC, 0); }
void CAN1_TX_IRQHandler()            { call_isr(HDL_NVIC_IRQ19_CAN1_TX, 0); }
void CAN1_RX0_IRQHandler()           { call_isr(HDL_NVIC_IRQ20_CAN1_RX0, 0); }
void CAN1_RX1_IRQHandler()           { call_isr(HDL_NVIC_IRQ21_CAN1_RX1, 0); }
void CAN1_SCE_IRQHandler()           { call_isr(HDL_NVIC_IRQ22_CAN1_SCE, 0); }
void EXTI9_5_IRQHandler()            { call_isr(HDL_NVIC_IRQ23_EXTI9_5, 0); }
void TIM1_BRK_TIM9_IRQHandler()      { call_isr(HDL_NVIC_IRQ24_TIM1_BRK_TIM9, 0); }
void TIM1_UP_TIM10_IRQHandler()      { call_isr(HDL_NVIC_IRQ25_TIM1_UP_TIM10, 0); }
void TIM1_TRG_COM_TIM11_IRQHandler() { call_isr(HDL_NVIC_IRQ26_TIM1_TRG_COM_TIM11, 0); }
void TIM1_CC_IRQHandler()            { call_isr(HDL_NVIC_IRQ27_TIM1_CC, 0); }
void TIM2_IRQHandler()               { call_isr(HDL_NVIC_IRQ28_TIM2, 0); }
void TIM3_IRQHandler()               { call_isr(HDL_NVIC_IRQ29_TIM3, 0); }
void TIM4_IRQHandler()               { call_isr(HDL_NVIC_IRQ30_TIM4, 0); }
void I2C1_EV_IRQHandler()            { call_isr(HDL_NVIC_IRQ31_I2C1_EV, 0); }
void I2C1_ER_IRQHandler()            { call_isr(HDL_NVIC_IRQ32_I2C1_ER, 0); }
void I2C2_EV_IRQHandler()            { call_isr(HDL_NVIC_IRQ33_I2C2_EV, 0); }
void I2C2_ER_IRQHandler()            { call_isr(HDL_NVIC_IRQ34_I2C2_ER, 0); }
void SPI1_IRQHandler()               { call_isr(HDL_NVIC_IRQ35_SPI1, 0); }
void SPI2_IRQHandler()               { call_isr(HDL_NVIC_IRQ36_SPI2, 0); }
void USART1_IRQHandler()             { call_isr(HDL_NVIC_IRQ37_USART1, 0); }
void USART2_IRQHandler()             { call_isr(HDL_NVIC_IRQ38_USART2, 0); }
void USART3_IRQHandler()             { call_isr(HDL_NVIC_IRQ39_USART3, 0); }
void EXTI15_10_IRQHandler()          { call_isr(HDL_NVIC_IRQ40_EXTI15_10, 0); }
void RTC_Alarm_IRQHandler()          { call_isr(HDL_NVIC_IRQ41_RTC_Alarm, 0); }
void OTG_FS_WKUP_IRQHandler()        { call_isr(HDL_NVIC_IRQ42_OTG_FS_WKUP, 0); }
void TIM8_BRK_TIM12_IRQHandler()     { call_isr(HDL_NVIC_IRQ43_TIM8_BRK_TIM12, 0); }
void TIM8_UP_TIM13_IRQHandler()      { call_isr(HDL_NVIC_IRQ44_TIM8_UP_TIM13, 0); }
void TIM8_TRG_COM_TIM14_IRQHandler() { call_isr(HDL_NVIC_IRQ45_TIM8_TRG_COM_TIM14, 0); }
void TIM8_CC_IRQHandler()            { call_isr(HDL_NVIC_IRQ46_TIM8_CC, 0); }
void DMA1_Stream7_IRQHandler()       { call_isr(HDL_NVIC_IRQ47_DMA1_Stream7, 0); }
void FSMC_IRQHandler()               { call_isr(HDL_NVIC_IRQ48_FSMC, 0); }
void SDIO_IRQHandler()               { call_isr(HDL_NVIC_IRQ49_SDIO, 0); }
void TIM5_IRQHandler()               { call_isr(HDL_NVIC_IRQ50_TIM5, 0); }
void SPI3_IRQHandler()               { call_isr(HDL_NVIC_IRQ51_SPI3, 0); }
void UART4_IRQHandler()              { call_isr(HDL_NVIC_IRQ52_UART4, 0); }
void UART5_IRQHandler()              { call_isr(HDL_NVIC_IRQ53_UART5, 0); }
void TIM6_DAC_IRQHandler()           { call_isr(HDL_NVIC_IRQ54_TIM6_DAC, 0); }
void TIM7_IRQHandler()               { call_isr(HDL_NVIC_IRQ55_TIM7, 0); }
void DMA2_Stream0_IRQHandler()       { call_isr(HDL_NVIC_IRQ56_DMA2_Stream0, 0); }
void DMA2_Stream1_IRQHandler()       { call_isr(HDL_NVIC_IRQ57_DMA2_Stream1, 0); }
void DMA2_Stream2_IRQHandler()       { call_isr(HDL_NVIC_IRQ58_DMA2_Stream2, 0); }
void DMA2_Stream3_IRQHandler()       { call_isr(HDL_NVIC_IRQ59_DMA2_Stream3, 0); }
void DMA2_Stream4_IRQHandler()       { call_isr(HDL_NVIC_IRQ60_DMA2_Stream4, 0); }
void ETH_IRQHandler()                { call_isr(HDL_NVIC_IRQ61_ETH, 0); }
void ETH_WKUP_IRQHandler()           { call_isr(HDL_NVIC_IRQ62_ETH_WKUP, 0); }
void CAN2_TX_IRQHandler()            { call_isr(HDL_NVIC_IRQ63_CAN2_TX, 0); }
void CAN2_RX0_IRQHandler()           { call_isr(HDL_NVIC_IRQ64_CAN2_RX0, 0); }
void CAN2_RX1_IRQHandler()           { call_isr(HDL_NVIC_IRQ65_CAN2_RX1, 0); }
void CAN2_SCE_IRQHandler()           { call_isr(HDL_NVIC_IRQ66_CAN2_SCE, 0); }
void OTG_FS_IRQHandler()             { call_isr(HDL_NVIC_IRQ67_OTG_FS, 0); }
void DMA2_Stream5_IRQHandler()       { call_isr(HDL_NVIC_IRQ68_DMA2_Stream5, 0); }
void DMA2_Stream6_IRQHandler()       { call_isr(HDL_NVIC_IRQ69_DMA2_Stream6, 0); }
void DMA2_Stream7_IRQHandler()       { call_isr(HDL_NVIC_IRQ70_DMA2_Stream7, 0); }
void USART6_IRQHandler()             { call_isr(HDL_NVIC_IRQ71_USART6, 0); }
void I2C3_EV_IRQHandler()            { call_isr(HDL_NVIC_IRQ72_I2C3_EV, 0); }
void I2C3_ER_IRQHandler()            { call_isr(HDL_NVIC_IRQ73_I2C3_ER, 0); }
void OTG_HS_EP1_OUT_IRQHandler()     { call_isr(HDL_NVIC_IRQ74_OTG_HS_EP1_OUT, 0); }
void OTG_HS_EP1_IN_IRQHandler()      { call_isr(HDL_NVIC_IRQ75_OTG_HS_EP1_IN, 0); }
void OTG_HS_WKUP_IRQHandler()        { call_isr(HDL_NVIC_IRQ76_OTG_HS_WKUP, 0); }
void OTG_HS_IRQHandler()             { call_isr(HDL_NVIC_IRQ77_OTG_HS, 0); }
void DCMI_IRQHandler()               { call_isr(HDL_NVIC_IRQ78_DCMI, 0); }
void HASH_RNG_IRQHandler()           { call_isr(HDL_NVIC_IRQ80_HASH_RNG, 0); }
void FPU_IRQHandler()                { call_isr(HDL_NVIC_IRQ81_FPU, 0); }

static hdl_module_state_t _hdl_core(const void *desc, uint8_t enable) {
  (void)desc;
  if(enable) {
    //hdl_core_t *core = (hdl_core_t *)desc;
    //FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | core->config->flash_latency;
    //rcu_periph_clock_enable(RCU_CFGCMP);
    return HDL_MODULE_ACTIVE;
  }
  //rcu_periph_clock_disable(RCU_CFGCMP);
  //FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT_0;
  return HDL_MODULE_UNLOADED;
}

const hdl_module_base_iface_t hdl_core_iface = {
  .init = _hdl_core
};
