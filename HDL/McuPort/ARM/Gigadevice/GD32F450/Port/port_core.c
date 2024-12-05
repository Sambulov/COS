#include "hdl_portable.h"
#include "Macros.h"

void wwdgt_handler()                    { call_isr(HDL_NVIC_IRQ0_WWDGT, 0); } 
void lvd_handler()                      { call_isr(HDL_NVIC_IRQ1_LVD, 0); }    
void TAMPER_STAMP_IRQHandler()          { call_isr(HDL_NVIC_IRQ2_TAMPER_STAMP, 0); }
void RTC_WKUP_IRQHandler()              { call_isr( HDL_NVIC_IRQ3_RTC_WKUP, 0); }
void fmc_handler()                      { call_isr( HDL_NVIC_IRQ4_FMC, 0); }
void RCU_CTC_IRQHandler()               { call_isr( HDL_NVIC_IRQ5_RCU_CTC, 0); }
void EXTI0_IRQHandler()                 { call_isr( HDL_NVIC_IRQ6_EXTI0, EXTI_0); EXTI_PD |= EXTI_0; }
void EXTI1_IRQHandler()                 { call_isr( HDL_NVIC_IRQ7_EXTI1, EXTI_1); EXTI_PD |= EXTI_1; }
void EXTI2_IRQHandler()                 { call_isr( HDL_NVIC_IRQ8_EXTI2, EXTI_2); EXTI_PD |= EXTI_2; }
void EXTI3_IRQHandler()                 { call_isr( HDL_NVIC_IRQ9_EXTI3, EXTI_3); EXTI_PD |= EXTI_3; }
void EXTI4_IRQHandler()                 { call_isr( HDL_NVIC_IRQ10_EXTI4, EXTI_4); EXTI_PD |= EXTI_4; }
void DMA0_Channel0_IRQHandler()         { call_isr( HDL_NVIC_IRQ11_DMA0_Channel0, 0); }
void DMA0_Channel1_IRQHandler()         { call_isr( HDL_NVIC_IRQ12_DMA0_Channel1, 0); }
void DMA0_Channel2_IRQHandler()         { call_isr( HDL_NVIC_IRQ13_DMA0_Channel2, 0); }
void DMA0_Channel3_IRQHandler()         { call_isr( HDL_NVIC_IRQ14_DMA0_Channel3, 0); }
void DMA0_Channel4_IRQHandler()         { call_isr( HDL_NVIC_IRQ15_DMA0_Channel4, 0); }
void DMA0_Channel5_IRQHandler()         { call_isr( HDL_NVIC_IRQ16_DMA0_Channel5, 0); }
void DMA0_Channel6_IRQHandler()         { call_isr( HDL_NVIC_IRQ17_DMA0_Channel6, 0); }
void ADC_IRQHandler()                   { call_isr( HDL_NVIC_IRQ18_ADC, 0); }
void CAN0_TX_IRQHandler()               { call_isr( HDL_NVIC_IRQ19_CAN0_TX, 0); }
void CAN0_RX0_IRQHandler()              { call_isr( HDL_NVIC_IRQ20_CAN0_RX0, 0); }
void CAN0_RX1_IRQHandler()              { call_isr( HDL_NVIC_IRQ21_CAN0_RX1, 0); }
void CAN0_EWMC_IRQHandler()             { call_isr( HDL_NVIC_IRQ22_CAN0_EWMC, 0); }
void EXTI5_9_IRQHandler()               { call_isr( HDL_NVIC_IRQ23_EXTI5_9, EXTI_PD & EXTI_LINES_5_9); EXTI_PD |= EXTI_LINES_5_9; }
void TIMER0_BRK_TIMER8_IRQHandler()     { call_isr( HDL_NVIC_IRQ24_TIMER0_BRK_TIMER8, TIMER_INTF(TIMER8)); 
                                          TIMER_INTF(TIMER0) &= ~TIMER_INTF_BRKIF;
                                          TIMER_INTF(TIMER8) &= ~TIMER_INTF(TIMER8);
                                        }
void TIMER0_UP_TIMER9_IRQHandler()      { call_isr(HDL_NVIC_IRQ25_TIMER0_UP_TIMER9, TIMER_INTF(TIMER9));
                                          TIMER_INTF(TIMER0) &= ~TIMER_INTF_UPIF;
                                          TIMER_INTF(TIMER9) &= ~(TIMER_INTF(TIMER9));
                                        }
void TIMER0_TRG_CMT_TIMER10_IRQHandler(){ call_isr( HDL_NVIC_IRQ26_TIMER0_TRG_CMT_TIMER10, TIMER_INTF(TIMER10));
                                          TIMER_INTF(TIMER0) &= ~TIMER_INTF_CMTIF;
                                          TIMER_INTF(TIMER10) &= ~(TIMER_INTF(TIMER10));
                                        }
void timer0_channel_handler()           { call_isr( HDL_NVIC_IRQ27_TIMER0_Channel, 0); }
void TIMER1_IRQHandler()                { call_isr(HDL_NVIC_IRQ28_TIMER1, TIMER_INTF(TIMER1));
                                          TIMER_INTF(TIMER1) &= ~TIMER_INTF(TIMER1);
                                        }
void timer2_handler()                   { call_isr( HDL_NVIC_IRQ29_TIMER2, TIMER_INTF(TIMER2)); TIMER_INTF(TIMER2) &= ~TIMER_INTF(TIMER2); }
void TIMER3_IRQHandler()                { call_isr( HDL_NVIC_IRQ30_TIMER3, TIMER_INTF(TIMER3)); TIMER_INTF(TIMER3) &= ~TIMER_INTF(TIMER3); }
void i2c0_ev_handler()                  { call_isr( HDL_NVIC_IRQ31_I2C0_EV, 0); }
void i2c0_er_handler()                  { call_isr( HDL_NVIC_IRQ32_I2C0_ER, 0); }
void i2c1_ev_handler()                  { call_isr( HDL_NVIC_IRQ33_I2C1_EV, 0); }
void i2c1_er_handler()                  { call_isr( HDL_NVIC_IRQ34_I2C1_ER, 0); }
void spi0_handler()                     { call_isr( HDL_NVIC_IRQ35_SPI0, 0); }
void spi1_handler()                     { call_isr( HDL_NVIC_IRQ36_SPI1, 0); }
void usart0_handler()                   { call_isr( HDL_NVIC_IRQ37_USART0, 0); }
void usart1_handler()                   { call_isr( HDL_NVIC_IRQ38_USART1, 0); }
void USART2_IRQHandler()                { call_isr( HDL_NVIC_IRQ39_USART2, 0); }
void EXTI10_15_IRQHandler()             { call_isr( HDL_NVIC_IRQ40_EXTI10_15, EXTI_LINES_10_15); EXTI_PD |= EXTI_LINES_10_15; }
void RTC_Alarm_IRQHandler()             { call_isr( HDL_NVIC_IRQ41_RTC_Alarm, 0); }
void USBFS_WKUP_IRQHandler()            { call_isr( HDL_NVIC_IRQ42_USBFS_WKUP, 0); }
void TIMER7_BRK_TIMER11_IRQHandler()    { call_isr( HDL_NVIC_IRQ43_TIMER7_BRK_TIMER11, 0); }
void TIMER7_UP_TIMER12_IRQHandler()     { call_isr( HDL_NVIC_IRQ44_TIMER7_UP_TIMER12, 0); }
void TIMER7_TRG_CMT_TIMER13_IRQHandler(){ call_isr( HDL_NVIC_IRQ45_TIMER7_TRG_CMT_TIMER13, 0); }
void TIMER7_Channel_IRQHandler()        { call_isr( HDL_NVIC_IRQ46_TIMER7_Channel, TIMER_INTF(TIMER7)); TIMER_INTF(TIMER7) &= ~TIMER_INTF(TIMER7); }
void DMA0_Channel7_IRQHandler()         { call_isr( HDL_NVIC_IRQ47_DMA0_Channel7, 0); }
#if defined (GD32F450) || defined (GD32F470)
void EXMC_IRQHandler()                  { call_isr( HDL_NVIC_IRQ48_EXMC, 0); }
void SDIO_IRQHandler()                  { call_isr( HDL_NVIC_IRQ49_SDIO, 0); }
void TIMER4_IRQHandler()                { call_isr( HDL_NVIC_IRQ50_TIMER4, TIMER_INTF(TIMER4)); TIMER_INTF(TIMER4) &= ~TIMER_INTF(TIMER4); }
void SPI2_IRQHandler()                  { call_isr( HDL_NVIC_IRQ51_SPI2, 0); }
void UART3_IRQHandler()                 { call_isr( HDL_NVIC_IRQ52_UART3, 0); }
void UART4_IRQHandler()                 { call_isr( HDL_NVIC_IRQ53_UART4, 0); }
void TIMER5_DAC_IRQHandler()            { call_isr( HDL_NVIC_IRQ54_TIMER5_DAC, 0); }
void TIMER6_IRQHandler()                { call_isr( HDL_NVIC_IRQ55_TIMER6, TIMER_INTF(TIMER6)); TIMER_INTF(TIMER6) &= ~TIMER_INTF(TIMER6); }
void DMA1_Channel0_IRQHandler()         { call_isr( HDL_NVIC_IRQ56_DMA1_Channel0, 0); }
void DMA1_Channel1_IRQHandler()         { call_isr( HDL_NVIC_IRQ57_DMA1_Channel1, 0); }
void DMA1_Channel2_IRQHandler()         { call_isr( HDL_NVIC_IRQ58_DMA1_Channel2, 0); }
void DMA1_Channel3_IRQHandler()         { call_isr( HDL_NVIC_IRQ59_DMA1_Channel3, 0); }
void DMA1_Channel4_IRQHandler()         { call_isr( HDL_NVIC_IRQ60_DMA1_Channel4, 0); }
void ENET_IRQHandler()                  { call_isr( HDL_NVIC_IRQ61_ENET, 0); }
void ENET_WKUP_IRQHandler()             { call_isr( HDL_NVIC_IRQ62_ENET_WKUP, 0); }
void CAN1_TX_IRQHandler()               { call_isr( HDL_NVIC_IRQ63_CAN1_TX, 0); }
void CAN1_RX0_IRQHandler()              { call_isr( HDL_NVIC_IRQ64_CAN1_RX0, 0); }
void CAN1_RX1_IRQHandler()              { call_isr( HDL_NVIC_IRQ65_CAN1_RX1, 0); }
void CAN1_EWMC_IRQHandler()             { call_isr( HDL_NVIC_IRQ66_CAN1_EWMC, 0); }
void USBFS_IRQHandler()                 { call_isr( HDL_NVIC_IRQ67_USBFS, 0); }
void DMA1_Channel5_IRQHandler()         { call_isr( HDL_NVIC_IRQ68_DMA1_Channel5, 0); }
void DMA1_Channel6_IRQHandler()         { call_isr( HDL_NVIC_IRQ69_DMA1_Channel6, 0); }
void DMA1_Channel7_IRQHandler()         { call_isr( HDL_NVIC_IRQ70_DMA1_Channel7, 0); }
void USART5_IRQHandler()                { call_isr( HDL_NVIC_IRQ71_USART5, 0); }
void I2C2_EV_IRQHandler()               { call_isr( HDL_NVIC_IRQ72_I2C2_EV, 0); }
void I2C2_ER_IRQHandler()               { call_isr( HDL_NVIC_IRQ73_I2C2_ER, 0); }
void USBHS_EP1_Out_IRQHandler()         { call_isr( HDL_NVIC_IRQ74_USBHS_EP1_Out, 0); }
void USBHS_EP1_In_IRQHandler()          { call_isr( HDL_NVIC_IRQ75_USBHS_EP1_In, 0); }
void USBHS_WKUP_IRQHandler()            { call_isr( HDL_NVIC_IRQ76_USBHS_WKUP, 0); }
void USBHS_IRQHandler()                 { call_isr( HDL_NVIC_IRQ77_USBHS, 0); }
void DCI_IRQHandler()                   { call_isr( HDL_NVIC_IRQ78_DCI, 0); }
void TRNG_IRQHandler()                  { call_isr( HDL_NVIC_IRQ80_TRNG, 0); }
void FPU_IRQHandler()                   { call_isr( HDL_NVIC_IRQ81_FPU, 0); }
void UART6_IRQHandler()                 { call_isr( HDL_NVIC_IRQ82_UART6, 0); }
void UART7_IRQHandler()                 { call_isr(HDL_NVIC_IRQ83_UART7 , 0); }
void SPI3_IRQHandler()                  { call_isr( HDL_NVIC_IRQ84_SPI3, 0); }
void SPI4_IRQHandler()                  { call_isr( HDL_NVIC_IRQ85_SPI4, 0); }
void SPI5_IRQHandler()                  { call_isr( HDL_NVIC_IRQ86_SPI5, 0); }
void TLI_IRQHandler()                   { call_isr( HDL_NVIC_IRQ88_TLI, 0); }
void TLI_ER_IRQHandler()                { call_isr( HDL_NVIC_IRQ89_TLI_ER, 0); }
void IPA_IRQHandler()                   { call_isr( HDL_NVIC_IRQ90_IPA, 0); }
#endif /* GD32F450_470 */
#if defined (GD32F405) || defined (GD32F425)
void SDIO_IRQHandler()                  { call_isr( HDL_NVIC_IRQ49_SDIO, 0); }
void TIMER4_IRQHandler()                { call_isr( HDL_NVIC_IRQ50_TIMER4, TIMER_INTF(TIMER4)); TIMER_INTF(TIMER4) &= ~TIMER_INTF(TIMER4); }
void SPI2_IRQHandler()                  { call_isr( HDL_NVIC_IRQ51_SPI2, 0); }
void UART3_IRQHandler()                 { call_isr( HDL_NVIC_IRQ52_UART3, 0); }
void UART4_IRQHandler()                 { call_isr( HDL_NVIC_IRQ53_UART4, 0); }
void TIMER5_DAC_IRQHandler()            { call_isr( HDL_NVIC_IRQ54_TIMER5_DAC, 0); }
void TIMER6_IRQHandler()                { call_isr( HDL_NVIC_IRQ55_TIMER6, TIMER_INTF(TIMER6)); TIMER_INTF(TIMER6) &= ~TIMER_INTF(TIMER6); }
void DMA1_Channel0_IRQHandler()         { call_isr( HDL_NVIC_IRQ56_DMA1_Channel0, 0); }
void DMA1_Channel1_IRQHandler()         { call_isr( HDL_NVIC_IRQ57_DMA1_Channel1, 0); }
void DMA1_Channel2_IRQHandler()         { call_isr( HDL_NVIC_IRQ58_DMA1_Channel2, 0); }
void DMA1_Channel3_IRQHandler()         { call_isr( HDL_NVIC_IRQ59_DMA1_Channel3, 0); }
void DMA1_Channel4_IRQHandler()         { call_isr( HDL_NVIC_IRQ60_DMA1_Channel4, 0); }
void CAN1_TX_IRQHandler()               { call_isr( HDL_NVIC_IRQ63_CAN1_TX, 0); }
void CAN1_RX0_IRQHandler()              { call_isr( HDL_NVIC_IRQ64_CAN1_RX0, 0); }
void CAN1_RX1_IRQHandler()              { call_isr( HDL_NVIC_IRQ65_CAN1_RX1, 0); }
void CAN1_EWMC_IRQHandler()             { call_isr( HDL_NVIC_IRQ66_CAN1_EWMC, 0); }
void USBFS_IRQHandler()                 { call_isr( HDL_NVIC_IRQ67_USBFS, 0); }
void DMA1_Channel5_IRQHandler()         { call_isr( HDL_NVIC_IRQ68_DMA1_Channel5, 0); }
void DMA1_Channel6_IRQHandler()         { call_isr( HDL_NVIC_IRQ69_DMA1_Channel6, 0); }
void DMA1_Channel7_IRQHandler()         { call_isr( HDL_NVIC_IRQ70_DMA1_Channel7, 0); }
void USART5_IRQHandler()                { call_isr( HDL_NVIC_IRQ71_USART5, 0); }
void I2C2_EV_IRQHandler()               { call_isr( HDL_NVIC_IRQ72_I2C2_EV, 0); }
void I2C2_ER_IRQHandler()               { call_isr( HDL_NVIC_IRQ73_I2C2_ER, 0); }
void USBHS_EP1_Out_IRQHandler()         { call_isr( HDL_NVIC_IRQ74_USBHS_EP1_Out, 0); }
void USBHS_EP1_In_IRQHandler()          { call_isr( HDL_NVIC_IRQ75_USBHS_EP1_In, 0); }
void USBHS_WKUP_IRQHandler()            { call_isr( HDL_NVIC_IRQ76_USBHS_WKUP, 0); }
void USBHS_IRQHandler()                 { call_isr( HDL_NVIC_IRQ77_USBHS, 0); }
void DCI_IRQHandler()                   { call_isr( HDL_NVIC_IRQ78_DCI, 0); }
void TRNG_IRQHandler()                  { call_isr( HDL_NVIC_IRQ80_TRNG, 0); }
void FPU_IRQHandler()                   { call_isr( HDL_NVIC_IRQ81_FPU, 0); }
#endif /* GD32F405_425 */
#if defined (GD32F407) || defined (GD32F427)
void EXMC_IRQHandler()                  { call_isr( HDL_NVIC_IRQ48_EXMC, 0); }
void SDIO_IRQHandler()                  { call_isr( HDL_NVIC_IRQ49_SDIO, 0); }
void TIMER4_IRQHandler()                { call_isr( HDL_NVIC_IRQ50_TIMER4, TIMER_INTF(TIMER4)); TIMER_INTF(TIMER4) &= ~TIMER_INTF(TIMER4) }
void SPI2_IRQHandler()                  { call_isr( HDL_NVIC_IRQ51_SPI2, 0); }
void UART3_IRQHandler()                 { call_isr( HDL_NVIC_IRQ52_UART3, 0); }
void UART4_IRQHandler()                 { call_isr( HDL_NVIC_IRQ53_UART4, 0); }
void TIMER5_DAC_IRQHandler()            { call_isr( HDL_NVIC_IRQ54_TIMER5_DAC, 0); }
void TIMER6_IRQHandler()                { call_isr( HDL_NVIC_IRQ55_TIMER6, TIMER_INTF(TIMER4)); TIMER_INTF(TIMER6) &= ~TIMER_INTF(TIMER6)}
void DMA1_Channel0_IRQHandler()         { call_isr( HDL_NVIC_IRQ56_DMA1_Channel0, 0); }
void DMA1_Channel1_IRQHandler()         { call_isr( HDL_NVIC_IRQ57_DMA1_Channel1, 0); }
void DMA1_Channel2_IRQHandler()         { call_isr( HDL_NVIC_IRQ58_DMA1_Channel2, 0); }
void DMA1_Channel3_IRQHandler()         { call_isr( HDL_NVIC_IRQ59_DMA1_Channel3, 0); }
void DMA1_Channel4_IRQHandler()         { call_isr( HDL_NVIC_IRQ60_DMA1_Channel4, 0); }
void ENET_IRQHandler()                  { call_isr( HDL_NVIC_IRQ61_ENET, 0); }
void ENET_WKUP_IRQHandler()             { call_isr( HDL_NVIC_IRQ62_ENET_WKUP, 0); }
void CAN1_TX_IRQHandler()               { call_isr( HDL_NVIC_IRQ63_CAN1_TX, 0); }
void CAN1_RX0_IRQHandler()              { call_isr( HDL_NVIC_IRQ64_CAN1_RX0, 0); }
void CAN1_RX1_IRQHandler()              { call_isr( HDL_NVIC_IRQ65_CAN1_RX1, 0); }
void CAN1_EWMC_IRQHandler()             { call_isr( HDL_NVIC_IRQ66_CAN1_EWMC, 0); }
void USBFS_IRQHandler()                 { call_isr( HDL_NVIC_IRQ67_USBFS, 0); }
void DMA1_Channel5_IRQHandler()         { call_isr( HDL_NVIC_IRQ68_DMA1_Channel5, 0); }
void DMA1_Channel6_IRQHandler()         { call_isr( HDL_NVIC_IRQ69_DMA1_Channel6, 0); }
void DMA1_Channel7_IRQHandler()         { call_isr( HDL_NVIC_IRQ70_DMA1_Channel7, 0); }
void USART5_IRQHandler()                { call_isr( HDL_NVIC_IRQ71_USART5, 0); }
void I2C2_EV_IRQHandler()               { call_isr( HDL_NVIC_IRQ72_I2C2_EV, 0); }
void I2C2_ER_IRQHandler()               { call_isr( HDL_NVIC_IRQ73_I2C2_ER, 0); }
void USBHS_EP1_Out_IRQHandler()         { call_isr( HDL_NVIC_IRQ74_USBHS_EP1_Out, 0); }
void USBHS_EP1_In_IRQHandler()          { call_isr( HDL_NVIC_IRQ75_USBHS_EP1_In, 0); }
void USBHS_WKUP_IRQHandler()            { call_isr( HDL_NVIC_IRQ76_USBHS_WKUP, 0); }
void USBHS_IRQHandler()                 { call_isr( HDL_NVIC_IRQ77_USBHS, 0); }
void DCI_IRQHandler()                   { call_isr( HDL_NVIC_IRQ78_DCI, 0); }
void TRNG_IRQHandler()                  { call_isr( HDL_NVIC_IRQ80_TRNG, 0); }
void FPU_IRQHandler()                   { call_isr( HDL_NVIC_IRQ81_FPU, 0); }
#endif /* GD32F407_427 */

hdl_module_state_t hdl_core(void *desc, uint8_t enable) {
  /* TODO: */
  if(enable) {
    hdl_core_t *core = (hdl_core_t *)desc;
    rcu_periph_clock_enable(RCU_SYSCFG);
    rcu_periph_clock_enable(RCU_PMU);
    //FMC_WS = (FMC_WS & (~FMC_WC_WSCNT)) | core->flash_latency;
    return HDL_MODULE_ACTIVE;
  }
  rcu_periph_clock_disable(RCU_SYSCFG);
  //FMC_WS = (FMC_WS & (~FMC_WC_WSCNT)) | WS_WSCNT_0;
  return HDL_MODULE_UNLOADED;
}

