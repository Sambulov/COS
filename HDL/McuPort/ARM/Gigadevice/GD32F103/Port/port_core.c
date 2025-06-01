#include "hdl_portable.h"

void wwdgt_handler()                    { call_isr(HDL_NVIC_IRQ0_WWDGT, 0); }
void lvd_handler()                      { call_isr(HDL_NVIC_IRQ1_LVD, 0); }
void TAMPER_IRQHandler()                { call_isr(HDL_NVIC_IRQ2_TAMPER, 0); }   
void rtc_handler()                      { call_isr(HDL_NVIC_IRQ3_RTC, 0); }
void fmc_handler()                      { call_isr(HDL_NVIC_IRQ4_FMC, 0); }
void rcu_handler()                      { call_isr(HDL_NVIC_IRQ5_RCU_CTC, 0); }
void EXTI0_IRQHandler()                 { call_isr(HDL_NVIC_IRQ6_EXTI0, EXTI_0); EXTI_PD |= EXTI_0; }
void EXTI1_IRQHandler()                 { call_isr(HDL_NVIC_IRQ7_EXTI1, EXTI_1); EXTI_PD |= EXTI_1; }
void EXTI2_IRQHandler()                 { call_isr(HDL_NVIC_IRQ8_EXTI2, EXTI_2); EXTI_PD |= EXTI_2; }
void EXTI3_IRQHandler()                 { call_isr(HDL_NVIC_IRQ9_EXTI3, EXTI_3); EXTI_PD |= EXTI_3; }
void EXTI4_IRQHandler()                 { call_isr(HDL_NVIC_IRQ10_EXTI4, EXTI_4); EXTI_PD |= EXTI_4; }
void DMA0_Channel0_IRQHandler()         { call_isr(HDL_NVIC_IRQ11_DMA0_Channel0, 0); }
void DMA0_Channel1_IRQHandler()         { call_isr(HDL_NVIC_IRQ12_DMA0_Channel1, 0); }
void DMA0_Channel2_IRQHandler()         { call_isr(HDL_NVIC_IRQ13_DMA0_Channel2, 0); }
void DMA0_Channel3_IRQHandler()         { call_isr(HDL_NVIC_IRQ14_DMA0_Channel3, 0); }
void DMA0_Channel4_IRQHandler()         { call_isr(HDL_NVIC_IRQ15_DMA0_Channel4, 0); }
void DMA0_Channel5_IRQHandler()         { call_isr(HDL_NVIC_IRQ16_DMA0_Channel5, 0); } 
void DMA0_Channel6_IRQHandler()         { call_isr(HDL_NVIC_IRQ17_DMA0_Channel6, 0); }
void ADC0_1_IRQHandler()                { call_isr(HDL_NVIC_IRQ18_ADC0_1, 0); }

#ifdef GD32F10X_MD
void USBD_HP_CAN0_TX_IRQHandler()       { call_isr(HDL_NVIC_IRQ19_USBD_HP_CAN0_TX, 0); } /* IRQ19 */
void USBD_LP_CAN0_RX0_IRQHandler()      { call_isr(HDL_NVIC_IRQ20_USBD_LP_CAN0_RX0, 0); } /* IRQ20 */
void CAN0_RX1_IRQHandler()              { call_isr(HDL_NVIC_IRQ21_CAN0_RX1, 0); } /* IRQ21 */
void CAN0_EWMC_IRQHandler()             { call_isr(HDL_NVIC_IRQ22_CAN0_EWMC, 0); } /* IRQ22 */
void EXTI5_9_IRQHandler()               { call_isr(HDL_NVIC_IRQ23_EXTI5_9, EXTI_PD & EXTI_LINES_5_9); EXTI_PD |= EXTI_LINES_5_9; } /* IRQ23 */
void TIMER0_BRK_IRQHandler()            { call_isr(HDL_NVIC_IRQ24_TIMER0_BRK, 0); } /* IRQ24 */
void TIMER0_UP_IRQHandler()             { call_isr(HDL_NVIC_IRQ25_TIMER0_UP, 0); } /* IRQ25 */
void TIMER0_TRG_CMT_IRQHandler()        { call_isr(HDL_NVIC_IRQ26_TIMER0_TRG_CMT, 0); } /* IRQ26 */
void timer0_channel_handler()           { call_isr(HDL_NVIC_IRQ27_TIMER0_Channel, 0); } /* IRQ27 */
void TIMER1_IRQHandler()                { call_isr(HDL_NVIC_IRQ28_TIMER1, 0); } /* IRQ28 */
void timer2_handler()                   { call_isr(HDL_NVIC_IRQ29_TIMER2, 0); } /* IRQ29 */
void TIMER3_IRQHandler()                { call_isr(HDL_NVIC_IRQ30_TIMER3, 0); } /* IRQ30 */
void i2c0_ev_handler()                  { call_isr(HDL_NVIC_IRQ31_I2C0_EV, 0); } /* IRQ31 */
void i2c0_er_handler()                  { call_isr(HDL_NVIC_IRQ32_I2C0_ER, 0); } /* IRQ32 */
void i2c1_ev_handler()                  { call_isr(HDL_NVIC_IRQ33_I2C1_EV, 0); } /* IRQ33 */
void i2c1_er_handler()                  { call_isr(HDL_NVIC_IRQ34_I2C1_ER, 0); } /* IRQ34 */
void spi0_handler()                     { call_isr(HDL_NVIC_IRQ35_SPI0, 0); } /* IRQ35 */
void spi1_handler()                     { call_isr(HDL_NVIC_IRQ36_SPI1, 0); } /* IRQ36 */
void usart0_handler()                   { call_isr(HDL_NVIC_IRQ37_USART0, 0); } /* IRQ37 */
void usart1_handler()                   { call_isr(HDL_NVIC_IRQ38_USART1, 0); } /* IRQ38 */
void USART2_IRQHandler()                { call_isr(HDL_NVIC_IRQ39_USART2, 0); } /* IRQ39 */
void EXTI10_15_IRQHandler()             { call_isr(HDL_NVIC_IRQ40_EXTI10_15, EXTI_PD & EXTI_LINES_10_15); EXTI_PD |= EXTI_LINES_10_15; } /* IRQ40 */
void RTC_Alarm_IRQHandler()             { call_isr(HDL_NVIC_IRQ41_RTC_Alarm, 0); } /* IRQ41 */
void USBD_WKUP_IRQHandler()             { call_isr(HDL_NVIC_IRQ42_USBD_WKUP, 0); } /* IRQ42 */
void EXMC_IRQHandler()                  { call_isr(HDL_NVIC_IRQ48_EXMC, 0); } /* IRQ48 */
#endif /* GD32F10X_MD */
#ifdef GD32F10X_HD
void USBD_HP_CAN0_TX_IRQHandler()       { call_isr(HDL_NVIC_IRQ19_USBD_HP_CAN0_TX, 0); } /* IRQ19 */
void USBD_LP_CAN0_RX0_IRQHandler()      { call_isr(HDL_NVIC_IRQ20_USBD_LP_CAN0_RX0, 0); } /* IRQ20 */
void CAN0_RX1_IRQHandler()              { call_isr(HDL_NVIC_IRQ21_CAN0_RX1, 0); } /* IRQ21 */
void CAN0_EWMC_IRQHandler()             { call_isr(HDL_NVIC_IRQ22_CAN0_EWMC, 0); } /* IRQ22 */
void EXTI5_9_IRQHandler()               { call_isr(HDL_NVIC_IRQ23_EXTI5_9, EXTI_PD & EXTI_LINES_5_9); EXTI_PD |= EXTI_LINES_5_9; } /* IRQ23 */
void TIMER0_BRK_IRQHandler()            { call_isr(HDL_NVIC_IRQ24_TIMER0_BRK, 0); } /* IRQ24 */
void TIMER0_UP_IRQHandler()             { call_isr(HDL_NVIC_IRQ25_TIMER0_UP, 0); } /* IRQ25 */
void TIMER0_TRG_CMT_IRQHandler()        { call_isr(HDL_NVIC_IRQ26_TIMER0_TRG_CMT, 0); } /* IRQ26 */
void timer0_channel_handler()           { call_isr(HDL_NVIC_IRQ27_TIMER0_Channel, 0); } /* IRQ27 */
void TIMER1_IRQHandler()                { call_isr(HDL_NVIC_IRQ28_TIMER1, 0); } /* IRQ28 */
void timer2_handler()                   { call_isr(HDL_NVIC_IRQ29_TIMER2, 0); } /* IRQ29 */
void TIMER3_IRQHandler()                { call_isr(HDL_NVIC_IRQ30_TIMER3, 0); } /* IRQ30 */
void i2c0_ev_handler()                  { call_isr(HDL_NVIC_IRQ31_I2C0_EV, 0); } /* IRQ31 */
void i2c0_er_handler()                  { call_isr(HDL_NVIC_IRQ32_I2C0_ER, 0); } /* IRQ32 */
void i2c1_ev_handler()                  { call_isr(HDL_NVIC_IRQ33_I2C1_EV, 0); } /* IRQ33 */
void i2c1_er_handler()                  { call_isr(HDL_NVIC_IRQ34_I2C1_ER, 0); } /* IRQ34 */
void spi0_handler()                     { call_isr(HDL_NVIC_IRQ35_SPI0, 0); } /* IRQ35 */
void spi1_handler()                     { call_isr(HDL_NVIC_IRQ36_SPI1, 0); } /* IRQ36 */
void usart0_handler()                   { call_isr(HDL_NVIC_IRQ37_USART0, 0); } /* IRQ37 */
void usart1_handler()                   { call_isr(HDL_NVIC_IRQ38_USART1, 0); } /* IRQ38 */
void USART2_IRQHandler()                { call_isr(HDL_NVIC_IRQ39_USART2, 0); } /* IRQ39 */
void EXTI10_15_IRQHandler()             { call_isr(HDL_NVIC_IRQ40_EXTI10_15, EXTI_PD & EXTI_LINES_10_15); EXTI_PD |= EXTI_LINES_10_15; } /* IRQ40 */
void RTC_Alarm_IRQHandler()             { call_isr(HDL_NVIC_IRQ41_RTC_Alarm, 0); } /* IRQ41 */
void USBD_WKUP_IRQHandler()             { call_isr(HDL_NVIC_IRQ42_USBD_WKUP, 0); } /* IRQ42 */
void TIMER7_BRK_IRQHandler()            { call_isr(HDL_NVIC_IRQ43_TIMER7_BRK, 0); } /* IRQ43 */
void TIMER7_UP_IRQHandler()             { call_isr(HDL_NVIC_IRQ44_TIMER7_UP, 0); } /* IRQ44 */
void TIMER7_TRG_CMT_IRQHandler()        { call_isr(HDL_NVIC_IRQ45_TIMER7_TRG_CMT, 0); } /* IRQ45 */
void TIMER7_Channel_IRQHandler()        { call_isr(HDL_NVIC_IRQ46_TIMER7_Channel, 0); } /* IRQ46 */
void ADC2_IRQHandler()                  { call_isr(HDL_NVIC_IRQ47_ADC2, 0); } /* IRQ47 */
void EXMC_IRQHandler()                  { call_isr(HDL_NVIC_IRQ48_EXMC, 0); } /* IRQ48 */
void SDIO_IRQHandler()                  { call_isr(HDL_NVIC_IRQ49_SDIO, 0); } /* IRQ49 */
void TIMER4_IRQHandler()                { call_isr(HDL_NVIC_IRQ50_TIMER4, 0); } /* IRQ50 */
void SPI2_IRQHandler()                  { call_isr(HDL_NVIC_IRQ51_SPI2, 0); } /* IRQ51 */
void UART3_IRQHandler()                 { call_isr(HDL_NVIC_IRQ52_UART3, 0); } /* IRQ52 */
void UART4_IRQHandler()                 { call_isr(HDL_NVIC_IRQ53_UART4, 0); } /* IRQ53 */
void timer5_handler()                   { call_isr(HDL_NVIC_IRQ54_TIMER5, 0); } /* IRQ54 */
void TIMER6_IRQHandler()                { call_isr(HDL_NVIC_IRQ55_TIMER6, 0); } /* IRQ55 */
void DMA1_Channel0_IRQHandler()         { call_isr(HDL_NVIC_IRQ56_DMA1_Channel0, 0); } /* IRQ56 */
void DMA1_Channel1_IRQHandler()         { call_isr(HDL_NVIC_IRQ57_DMA1_Channel1, 0); } /* IRQ57 */
void DMA1_Channel2_IRQHandler()         { call_isr(HDL_NVIC_IRQ58_DMA1_Channel2, 0); } /* IRQ58 */
void DMA1_Channel3_4_IRQHandler()       { call_isr(HDL_NVIC_IRQ59_DMA1_Channel3_Channel4, 0); } /* IRQ59 */
#endif /* GD32F10X_HD */
#ifdef GD32F10X_XD
void USBD_HP_CAN0_TX_IRQHandler()       { call_isr(HDL_NVIC_IRQ19_USBD_HP_CAN0_TX, 0); } /* IRQ19 */
void USBD_LP_CAN0_RX0_IRQHandler()      { call_isr(HDL_NVIC_IRQ20_USBD_LP_CAN0_RX0, 0); } /* IRQ20 */
void CAN0_RX1_IRQHandler()              { call_isr(HDL_NVIC_IRQ21_CAN0_RX1, 0); } /* IRQ21 */
void CAN0_EWMC_IRQHandler()             { call_isr(HDL_NVIC_IRQ22_CAN0_EWMC, 0); }  /* IRQ22 */
void EXTI5_9_IRQHandler()               { call_isr(HDL_NVIC_IRQ23_EXTI5_9, EXTI_PD & EXTI_LINES_5_9); EXTI_PD |= EXTI_LINES_5_9; } /* IRQ23 */
void TIMER0_BRK_TIMER8_IRQHandler()     { call_isr(HDL_NVIC_IRQ24_TIMER0_BRK_TIMER8, 0); } /* IRQ24 */
void TIMER0_UP_TIMER9_IRQHandler()      { call_isr(HDL_NVIC_IRQ25_TIMER0_UP_TIMER9, 0);
                                          TIMER_INTF(TIMER0) &= ~TIMER_INTF_UPIF;
                                          TIMER_INTF(TIMER9) &= ~TIMER_INTF_UPIF;
                                        } /* IRQ25 */
void TIMER0_TRG_CMT_TIMER10_IRQHandler(){ call_isr(HDL_NVIC_IRQ26_TIMER0_TRG_CMT_TIMER10, 0); } /* IRQ26 */
void timer0_channel_handler()           { call_isr(HDL_NVIC_IRQ27_TIMER0_Channel, 0); } /* IRQ27 */
void TIMER1_IRQHandler()                { call_isr(HDL_NVIC_IRQ28_TIMER1, 0);
                                          TIMER_INTF(TIMER1) &= ~TIMER_INTF_UPIF;
                                        } /* IRQ28 */
void timer2_handler()                   { call_isr(HDL_NVIC_IRQ29_TIMER2, 0); } /* IRQ29 */
void TIMER3_IRQHandler()                { call_isr(HDL_NVIC_IRQ30_TIMER3, 0); } /* IRQ30 */
void i2c0_ev_handler()                  { call_isr(HDL_NVIC_IRQ31_I2C0_EV, 0); } /* IRQ31 */
void i2c0_er_handler()                  { call_isr(HDL_NVIC_IRQ32_I2C0_ER, 0); } /* IRQ32 */
void i2c1_ev_handler()                  { call_isr(HDL_NVIC_IRQ33_I2C1_EV, 0); } /* IRQ33 */
void i2c1_er_handler()                  { call_isr(HDL_NVIC_IRQ34_I2C1_ER, 0); } /* IRQ34 */
void spi0_handler()                     { call_isr(HDL_NVIC_IRQ35_SPI0, 0); } /* IRQ35 */
void spi1_handler()                     { call_isr(HDL_NVIC_IRQ36_SPI1, 0); } /* IRQ36 */
void usart0_handler()                   { call_isr(HDL_NVIC_IRQ37_USART0, 0); } /* IRQ37 */
void usart1_handler()                   { call_isr(HDL_NVIC_IRQ38_USART1, 0); } /* IRQ38 */
void USART2_IRQHandler()                { call_isr(HDL_NVIC_IRQ39_USART2, 0); } /* IRQ39 */
void EXTI10_15_IRQHandler()             { call_isr(HDL_NVIC_IRQ40_EXTI10_15, EXTI_PD & EXTI_LINES_10_15); EXTI_PD |= EXTI_LINES_10_15; } /* IRQ40 */
void RTC_Alarm_IRQHandler()             { call_isr(HDL_NVIC_IRQ41_RTC_Alarm, 0); } /* IRQ41 */
void USBD_WKUP_IRQHandler()             { call_isr(HDL_NVIC_IRQ42_USBD_WKUP, 0); } /* IRQ42 */
void TIMER7_BRK_TIMER11_IRQHandler()    { call_isr(HDL_NVIC_IRQ43_TIMER7_BRK_TIMER11, 0); } /* IRQ43 */
void TIMER7_UP_TIMER12_IRQHandler()     { call_isr(HDL_NVIC_IRQ44_TIMER7_UP_TIMER12, 0); } /* IRQ44 */
void TIMER7_TRG_CMT_TIMER13_IRQHandler(){ call_isr(HDL_NVIC_IRQ45_TIMER7_TRG_CMT_TIMER13, 0); } /* IRQ45 */
void TIMER7_Channel_IRQHandler()        { call_isr(HDL_NVIC_IRQ46_TIMER7_Channel, 0); }  /* IRQ46 */
void ADC2_IRQHandler()                  { call_isr(HDL_NVIC_IRQ47_ADC2, 0); } /* IRQ47 */
void EXMC_IRQHandler()                  { call_isr(HDL_NVIC_IRQ48_EXMC, 0); } /* IRQ48 */
void SDIO_IRQHandler()                  { call_isr(HDL_NVIC_IRQ49_SDIO, 0); } /* IRQ49 */
void TIMER4_IRQHandler()                { call_isr(HDL_NVIC_IRQ50_TIMER4, 0); } /* IRQ50 */
void SPI2_IRQHandler()                  { call_isr(HDL_NVIC_IRQ51_SPI2, 0); } /* IRQ51 */
void UART3_IRQHandler()                 { call_isr(HDL_NVIC_IRQ52_UART3, 0); } /* IRQ52 */
void UART4_IRQHandler()                 { call_isr(HDL_NVIC_IRQ53_UART4, 0); } /* IRQ53 */
void timer5_handler()                   { call_isr(HDL_NVIC_IRQ54_TIMER5, 0); } /* IRQ54 */
void TIMER6_IRQHandler()                { call_isr(HDL_NVIC_IRQ55_TIMER6, 0); } /* IRQ55 */
void DMA1_Channel0_IRQHandler()         { call_isr(HDL_NVIC_IRQ56_DMA1_Channel0, 0); } /* IRQ56 */
void DMA1_Channel1_IRQHandler()         { call_isr(HDL_NVIC_IRQ57_DMA1_Channel1, 0); } /* IRQ57 */
void DMA1_Channel2_IRQHandler()         { call_isr(HDL_NVIC_IRQ58_DMA1_Channel2, 0); } /* IRQ58 */
void DMA1_Channel3_4_IRQHandler()       { call_isr(HDL_NVIC_IRQ59_DMA1_Channel3_Channel4, 0); } /* IRQ59 */
#endif /* GD32F10X_XD */
#ifdef GD32F10X_CL
void CAN0_TX_IRQHandler()               { call_isr(HDL_NVIC_IRQ19_CAN0_TX, 0); } /* IRQ19 */
void CAN0_RX0_IRQHandler()              { call_isr(HDL_NVIC_IRQ20_CAN0_RX0, 0); } /* IRQ20 */
void CAN0_RX1_IRQHandler()              { call_isr(HDL_NVIC_IRQ21_CAN0_RX1, 0); } /* IRQ21 */
void CAN0_EWMC_IRQHandler()             { call_isr(HDL_NVIC_IRQ22_CAN0_EWMC, 0); } /* IRQ22 */
void EXTI5_9_IRQHandler()               { call_isr(HDL_NVIC_IRQ23_EXTI5_9, EXTI_PD & EXTI_LINES_5_9); } /* IRQ23 */
void TIMER0_BRK_IRQHandler()            { call_isr(HDL_NVIC_IRQ24_TIMER0_BRK, 0); } /* IRQ24 */
void TIMER0_UP_IRQHandler()             { call_isr(HDL_NVIC_IRQ25_TIMER0_UP, 0); } /* IRQ25 */
void TIMER0_TRG_CMT_IRQHandler()        { call_isr(HDL_NVIC_IRQ26_TIMER0_TRG_CMT, 0); } /* IRQ26 */
void timer0_channel_handler()           { call_isr(HDL_NVIC_IRQ27_TIMER0_Channel, 0); } /* IRQ27 */
void TIMER1_IRQHandler()                { call_isr(HDL_NVIC_IRQ28_TIMER1, 0); } /* IRQ28 */
void timer2_handler()                   { call_isr(HDL_NVIC_IRQ29_TIMER2, 0); } /* IRQ29 */
void TIMER3_IRQHandler()                { call_isr(HDL_NVIC_IRQ30_TIMER3, 0); } /* IRQ30 */
void i2c0_ev_handler()                  { call_isr(HDL_NVIC_IRQ31_I2C0_EV, 0); } /* IRQ31 */
void i2c0_er_handler()                  { call_isr(HDL_NVIC_IRQ32_I2C0_ER, 0); } /* IRQ32 */
void i2c1_ev_handler()                  { call_isr(HDL_NVIC_IRQ33_I2C1_EV, 0); } /* IRQ33 */
void i2c1_er_handler()                  { call_isr(HDL_NVIC_IRQ34_I2C1_ER, 0); } /* IRQ34 */
void spi0_handler()                     { call_isr(HDL_NVIC_IRQ35_SPI0, 0); } /* IRQ35 */
void spi1_handler()                     { call_isr(HDL_NVIC_IRQ36_SPI1, 0); } /* IRQ36 */
void usart0_handler()                   { call_isr(HDL_NVIC_IRQ37_USART0, 0); } /* IRQ37 */
void usart1_handler()                   { call_isr(HDL_NVIC_IRQ38_USART1, 0); } /* IRQ38 */
void USART2_IRQHandler()                { call_isr(HDL_NVIC_IRQ39_USART2, 0); } /* IRQ39 */
void EXTI10_15_IRQHandler()             { call_isr(HDL_NVIC_IRQ40_EXTI10_15, EXTI_PD & EXTI_LINES_10_15); EXTI_PD |= EXTI_LINES_10_15; } /* IRQ40 */
void RTC_Alarm_IRQHandler()             { call_isr(HDL_NVIC_IRQ41_RTC_ALARM, 0); } /* IRQ41 */
void USBFS_WKUP_IRQHandler()            { call_isr(HDL_NVIC_IRQ42_USBFS_WKUP, 0); } /* IRQ42 */
void TIMER7_BRK_IRQHandler()            { call_isr(HDL_NVIC_IRQ43_TIMER7_BRK, 0); } /* IRQ43 */
void TIMER7_UP_IRQHandler()             { call_isr(HDL_NVIC_IRQ44_TIMER7_UP, 0); } /* IRQ44 */
void TIMER7_TRG_CMT_IRQHandler()        { call_isr(HDL_NVIC_IRQ45_TIMER7_TRG_CMT, 0); } /* IRQ45 */
void TIMER7_Channel_IRQHandler()        { call_isr(HDL_NVIC_IRQ46_TIMER7_Channel, 0); } /* IRQ46 */ 
void EXMC_IRQHandler()                  { call_isr(HDL_NVIC_IRQ48_EXMC, 0); } /* IRQ48 */
void TIMER4_IRQHandler()                { call_isr(HDL_NVIC_IRQ50_TIMER4, 0); } /* IRQ50 */
void SPI2_IRQHandler()                  { call_isr(HDL_NVIC_IRQ51_SPI2, 0); } /* IRQ51 */
void UART3_IRQHandler()                 { call_isr(HDL_NVIC_IRQ52_UART3, 0); } /* IRQ52 */
void UART4_IRQHandler()                 { call_isr(HDL_NVIC_IRQ53_UART4, 0); } /* IRQ53 */
void timer5_handler()                   { call_isr(HDL_NVIC_IRQ54_TIMER5, 0); } /* IRQ54 */
void TIMER6_IRQHandler()                { call_isr(HDL_NVIC_IRQ55_TIMER6, 0); } /* IRQ55 */
void DMA1_Channel0_IRQHandler()         { call_isr(HDL_NVIC_IRQ56_DMA1_Channel0, 0); } /* IRQ56 */
void DMA1_Channel1_IRQHandler()         { call_isr(HDL_NVIC_IRQ57_DMA1_Channel1, 0); } /* IRQ57 */
void DMA1_Channel2_IRQHandler()         { call_isr(HDL_NVIC_IRQ58_DMA1_Channel2, 0); } /* IRQ58 */
void DMA1_Channel3_IRQHandler()         { call_isr(HDL_NVIC_IRQ59_DMA1_Channel3, 0); } /* IRQ59 */
void DMA1_Channel4_IRQHandler()         { call_isr(HDL_NVIC_IRQ60_DMA1_Channel4, 0); } /* IRQ60 */
void ENET_IRQHandler()                  { call_isr(HDL_NVIC_IRQ61_ENET, 0); } /* IRQ61 */
void ENET_WKUP_IRQHandler()             { call_isr(HDL_NVIC_IRQ62_ENET_WKUP, 0); } /* IRQ62 */
void CAN1_TX_IRQHandler()               { call_isr(HDL_NVIC_IRQ63_CAN1_TX, 0); } /* IRQ63 */
void CAN1_RX0_IRQHandler()              { call_isr(HDL_NVIC_IRQ64_CAN1_RX0, 0); } /* IRQ64 */
void CAN1_RX1_IRQHandler()              { call_isr(HDL_NVIC_IRQ65_CAN1_RX1, 0); } /* IRQ65 */
void CAN1_EWMC_IRQHandler()             { call_isr(HDL_NVIC_IRQ66_CAN1_EWMC, 0); } /* IRQ66 */
void USBFS_IRQHandler()                 { call_isr(HDL_NVIC_IRQ67_USBFS, 0); } /* IRQ67 */
#endif /* GD32F10X_CL */

static hdl_module_state_t _hdl_core(const void *desc, uint8_t enable) {
  /* TODO: */
  if(enable) {
    hdl_core_arm_t *core = (hdl_core_arm_t *)desc;
    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | core->config->flash_latency;
    rcu_periph_clock_enable(RCU_AF);
    return HDL_MODULE_ACTIVE;
  }
  rcu_periph_clock_disable(RCU_AF);
  FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT_0;
  return HDL_MODULE_UNLOADED;
}

const hdl_module_base_iface_t hdl_core_arm_iface = {
  .init = _hdl_core
};
