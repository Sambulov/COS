
#ifndef PORT_CORE_H_
#define PORT_CORE_H_

#define HDL_INTERRUPT_PRV_SIZE       9

typedef enum {
  /* Cortex-M3 processor exceptions numbers */
  HDL_NVIC_EXCEPTION_NonMaskableInt          = -14,    /*!< 2 non maskable interrupt                                 */
  HDL_NVIC_EXCEPTION_HardFault               = -13,    /*!< hardfault interrupt                                      */
  HDL_NVIC_EXCEPTION_MemoryManagement        = -12,    /*!< 4 Cortex-M3 memory management interrupt                  */
  HDL_NVIC_EXCEPTION_BusFault                = -11,    /*!< 5 Cortex-M3 bus fault interrupt                          */
  HDL_NVIC_EXCEPTION_UsageFault              = -10,    /*!< 6 Cortex-M3 usage fault interrupt                        */
  HDL_NVIC_EXCEPTION_SVCall                  = -5,     /*!< 11 Cortex-M3 SV call interrupt                           */
  HDL_NVIC_EXCEPTION_DebugMonitor            = -4,     /*!< 12 Cortex-M3 debug monitor interrupt                     */
  HDL_NVIC_EXCEPTION_PendSV                  = -2,     /*!< 14 Cortex-M3 pend SV interrupt                           */
  HDL_NVIC_EXCEPTION_SysTick                 = -1,     /*!< 15 Cortex-M3 system tick interrupt                       */

  /* interruput numbers */
  HDL_NVIC_IRQ0_WWDGT                        = 0,      /*!< window watchDog timer interrupt                          */
  HDL_NVIC_IRQ1_LVD                          = 1,      /*!< LVD through EXTI line detect interrupt                   */
  HDL_NVIC_IRQ2_TAMPER                       = 2,      /*!< tamper through EXTI line detect                          */
  HDL_NVIC_IRQ3_RTC                          = 3,      /*!< RTC through EXTI line interrupt                          */
  HDL_NVIC_IRQ4_FMC                          = 4,      /*!< FMC interrupt                                            */
  HDL_NVIC_IRQ5_RCU_CTC                      = 5,      /*!< RCU and CTC interrupt                                    */
  HDL_NVIC_IRQ6_EXTI0                        = 6,      /*!< EXTI line 0 interrupts                                   */
  HDL_NVIC_IRQ7_EXTI1                        = 7,      /*!< EXTI line 1 interrupts                                   */
  HDL_NVIC_IRQ8_EXTI2                        = 8,      /*!< EXTI line 2 interrupts                                   */
  HDL_NVIC_IRQ9_EXTI3                        = 9,      /*!< EXTI line 3 interrupts                                   */
  HDL_NVIC_IRQ10_EXTI4                       = 10,     /*!< EXTI line 4 interrupts                                   */
  HDL_NVIC_IRQ11_DMA0_Channel0               = 11,     /*!< DMA0 channel0 interrupt                                  */
  HDL_NVIC_IRQ12_DMA0_Channel1               = 12,     /*!< DMA0 channel1 interrupt                                  */
  HDL_NVIC_IRQ13_DMA0_Channel2               = 13,     /*!< DMA0 channel2 interrupt                                  */
  HDL_NVIC_IRQ14_DMA0_Channel3               = 14,     /*!< DMA0 channel3 interrupt                                  */
  HDL_NVIC_IRQ15_DMA0_Channel4               = 15,     /*!< DMA0 channel4 interrupt                                  */
  HDL_NVIC_IRQ16_DMA0_Channel5               = 16,     /*!< DMA0 channel5 interrupt                                  */
  HDL_NVIC_IRQ17_DMA0_Channel6               = 17,     /*!< DMA0 channel6 interrupt                                  */
  HDL_NVIC_IRQ18_ADC0_1                      = 18,     /*!< ADC0 and ADC1 interrupt                                  */

#ifdef GD32F10X_MD
  HDL_NVIC_IRQ19_USBD_HP_CAN0_TX             = 19,     /*!< CAN0 TX interrupts                                       */
  HDL_NVIC_IRQ20_USBD_LP_CAN0_RX0            = 20,     /*!< CAN0 RX0 interrupts                                      */
  HDL_NVIC_IRQ21_CAN0_RX1                    = 21,     /*!< CAN0 RX1 interrupts                                      */
  HDL_NVIC_IRQ22_CAN0_EWMC                   = 22,     /*!< CAN0 EWMC interrupts                                     */
  HDL_NVIC_IRQ23_EXTI5_9                     = 23,     /*!< EXTI[9:5] interrupts                                     */
  HDL_NVIC_IRQ24_TIMER0_BRK                  = 24,     /*!< TIMER0 break interrupts                                  */
  HDL_NVIC_IRQ25_TIMER0_UP                   = 25,     /*!< TIMER0 update interrupts                                 */
  HDL_NVIC_IRQ26_TIMER0_TRG_CMT              = 26,     /*!< TIMER0 trigger and commutation interrupts                */
  HDL_NVIC_IRQ27_TIMER0_Channel              = 27,     /*!< TIMER0 channel capture compare interrupts                */
  HDL_NVIC_IRQ28_TIMER1                      = 28,     /*!< TIMER1 interrupt                                         */
  HDL_NVIC_IRQ29_TIMER2                      = 29,     /*!< TIMER2 interrupt                                         */
  HDL_NVIC_IRQ30_TIMER3                      = 30,     /*!< TIMER3 interrupts                                        */
  HDL_NVIC_IRQ31_I2C0_EV                     = 31,     /*!< I2C0 event interrupt                                     */
  HDL_NVIC_IRQ32_I2C0_ER                     = 32,     /*!< I2C0 error interrupt                                     */
  HDL_NVIC_IRQ33_I2C1_EV                     = 33,     /*!< I2C1 event interrupt                                     */
  HDL_NVIC_IRQ34_I2C1_ER                     = 34,     /*!< I2C1 error interrupt                                     */
  HDL_NVIC_IRQ35_SPI0                        = 35,     /*!< SPI0 interrupt                                           */
  HDL_NVIC_IRQ36_SPI1                        = 36,     /*!< SPI1 interrupt                                           */
  HDL_NVIC_IRQ37_USART0                      = 37,     /*!< USART0 interrupt                                         */
  HDL_NVIC_IRQ38_USART1                      = 38,     /*!< USART1 interrupt                                         */
  HDL_NVIC_IRQ39_USART2                      = 39,     /*!< USART2 interrupt                                         */
  HDL_NVIC_IRQ40_EXTI10_15                   = 40,     /*!< EXTI[15:10] interrupts                                   */
  HDL_NVIC_IRQ41_RTC_Alarm                   = 41,     /*!< RTC alarm interrupt                                      */
  HDL_NVIC_IRQ42_USBD_WKUP                   = 42,     /*!< USBD Wakeup interrupt                                    */
  HDL_NVIC_IRQ43                             = 43,
  HDL_NVIC_IRQ44                             = 44,
  HDL_NVIC_IRQ45                             = 45,
  HDL_NVIC_IRQ46                             = 46,
  HDL_NVIC_IRQ47                             = 47,
  HDL_NVIC_IRQ48_EXMC                        = 48,     /*!< EXMC global interrupt                                    */
  HDL_NVIC_IRQ49                             = 49,
  HDL_NVIC_IRQ50                             = 50,
  HDL_NVIC_IRQ51                             = 51,
  HDL_NVIC_IRQ52                             = 52,
  HDL_NVIC_IRQ53                             = 53,
  HDL_NVIC_IRQ54                             = 54,
  HDL_NVIC_IRQ55                             = 55,
  HDL_NVIC_IRQ56                             = 56,
  HDL_NVIC_IRQ57                             = 57,
  HDL_NVIC_IRQ58                             = 58,
  HDL_NVIC_IRQ59                             = 59,
  HDL_NVIC_IRQ60                             = 60,
  HDL_NVIC_IRQ61                             = 61,
  HDL_NVIC_IRQ62                             = 62,
  HDL_NVIC_IRQ63                             = 63,
  HDL_NVIC_IRQ64                             = 64,
  HDL_NVIC_IRQ65                             = 65,
  HDL_NVIC_IRQ66                             = 66,
  HDL_NVIC_IRQ67                             = 67,
#endif /* GD32F10X_MD */

#ifdef GD32F10X_HD
  HDL_NVIC_IRQ19_USBD_HP_CAN0_TX             = 19,     /*!< CAN0 TX interrupts                                       */
  HDL_NVIC_IRQ20_USBD_LP_CAN0_RX0            = 20,     /*!< CAN0 RX0 interrupts                                      */
  HDL_NVIC_IRQ21_CAN0_RX1                    = 21,     /*!< CAN0 RX1 interrupts                                      */
  HDL_NVIC_IRQ22_CAN0_EWMC                   = 22,     /*!< CAN0 EWMC interrupts                                     */
  HDL_NVIC_IRQ23_EXTI5_9                     = 23,     /*!< EXTI[9:5] interrupts                                     */
  HDL_NVIC_IRQ24_TIMER0_BRK                  = 24,     /*!< TIMER0 break interrupts                                  */
  HDL_NVIC_IRQ25_TIMER0_UP                   = 25,     /*!< TIMER0 update interrupts                                 */
  HDL_NVIC_IRQ26_TIMER0_TRG_CMT              = 26,     /*!< TIMER0 trigger and commutation interrupts                */
  HDL_NVIC_IRQ27_TIMER0_Channel              = 27,     /*!< TIMER0 channel capture compare interrupts                */
  HDL_NVIC_IRQ28_TIMER1                      = 28,     /*!< TIMER1 interrupt                                         */
  HDL_NVIC_IRQ29_TIMER2                      = 29,     /*!< TIMER2 interrupt                                         */
  HDL_NVIC_IRQ30_TIMER3                      = 30,     /*!< TIMER3 interrupts                                        */
  HDL_NVIC_IRQ31_I2C0_EV                     = 31,     /*!< I2C0 event interrupt                                     */
  HDL_NVIC_IRQ32_I2C0_ER                     = 32,     /*!< I2C0 error interrupt                                     */
  HDL_NVIC_IRQ33_I2C1_EV                     = 33,     /*!< I2C1 event interrupt                                     */
  HDL_NVIC_IRQ34_I2C1_ER                     = 34,     /*!< I2C1 error interrupt                                     */
  HDL_NVIC_IRQ35_SPI0                        = 35,     /*!< SPI0 interrupt                                           */
  HDL_NVIC_IRQ36_SPI1                        = 36,     /*!< SPI1 interrupt                                           */
  HDL_NVIC_IRQ37_USART0                      = 37,     /*!< USART0 interrupt                                         */
  HDL_NVIC_IRQ38_USART1                      = 38,     /*!< USART1 interrupt                                         */
  HDL_NVIC_IRQ39_USART2                      = 39,     /*!< USART2 interrupt                                         */
  HDL_NVIC_IRQ40_EXTI10_15                   = 40,     /*!< EXTI[15:10] interrupts                                   */
  HDL_NVIC_IRQ41_RTC_Alarm                   = 41,     /*!< RTC alarm interrupt                                      */
  HDL_NVIC_IRQ42_USBD_WKUP                   = 42,     /*!< USBD Wakeup interrupt                                    */
  HDL_NVIC_IRQ43_TIMER7_BRK                  = 43,     /*!< TIMER7 break interrupts                                  */
  HDL_NVIC_IRQ44_TIMER7_UP                   = 44,     /*!< TIMER7 update interrupts                                 */
  HDL_NVIC_IRQ45_TIMER7_TRG_CMT              = 45,     /*!< TIMER7 trigger and commutation interrupts                */
  HDL_NVIC_IRQ46_TIMER7_Channel              = 46,     /*!< TIMER7 channel capture compare interrupts                */
  HDL_NVIC_IRQ47_ADC2                        = 47,     /*!< ADC2 global interrupt                                    */
  HDL_NVIC_IRQ48_EXMC                        = 48,     /*!< EXMC global interrupt                                    */
  HDL_NVIC_IRQ49_SDIO                        = 49,     /*!< SDIO global interrupt                                    */
  HDL_NVIC_IRQ50_TIMER4                      = 50,     /*!< TIMER4 global interrupt                                  */
  HDL_NVIC_IRQ51_SPI2                        = 51,     /*!< SPI2 global interrupt                                    */
  HDL_NVIC_IRQ52_UART3                       = 52,     /*!< UART3 global interrupt                                   */
  HDL_NVIC_IRQ53_UART4                       = 53,     /*!< UART4 global interrupt                                   */
  HDL_NVIC_IRQ54_TIMER5                      = 54,     /*!< TIMER5 global interrupt                                  */
  HDL_NVIC_IRQ55_TIMER6                      = 55,     /*!< TIMER6 global interrupt                                  */
  HDL_NVIC_IRQ56_DMA1_Channel0               = 56,     /*!< DMA1 channel0 global interrupt                           */
  HDL_NVIC_IRQ57_DMA1_Channel1               = 57,     /*!< DMA1 channel1 global interrupt                           */
  HDL_NVIC_IRQ58_DMA1_Channel2               = 58,     /*!< DMA1 channel2 global interrupt                           */
  HDL_NVIC_IRQ59_DMA1_Channel3_Channel4      = 59,     /*!< DMA1 channel3 and channel4 global Interrupt              */
  HDL_NVIC_IRQ60                             = 60,
  HDL_NVIC_IRQ61                             = 61,
  HDL_NVIC_IRQ62                             = 62,
  HDL_NVIC_IRQ63                             = 63,
  HDL_NVIC_IRQ64                             = 64,
  HDL_NVIC_IRQ65                             = 65,
  HDL_NVIC_IRQ66                             = 66,
  HDL_NVIC_IRQ67                             = 67,
#endif /* GD32F10X_HD */

#ifdef GD32F10X_XD
  HDL_NVIC_IRQ19_USBD_HP_CAN0_TX             = 19,     /*!< CAN0 TX interrupts                                       */
  HDL_NVIC_IRQ20_USBD_LP_CAN0_RX0            = 20,     /*!< CAN0 RX0 interrupts                                      */
  HDL_NVIC_IRQ21_CAN0_RX1                    = 21,     /*!< CAN0 RX1 interrupts                                      */
  HDL_NVIC_IRQ22_CAN0_EWMC                   = 22,     /*!< CAN0 EWMC interrupts                                     */
  HDL_NVIC_IRQ23_EXTI5_9                     = 23,     /*!< EXTI[9:5] interrupts                                     */
  HDL_NVIC_IRQ24_TIMER0_BRK_TIMER8           = 24,     /*!< TIMER0 break and TIMER8 interrupts                       */
  HDL_NVIC_IRQ25_TIMER0_UP_TIMER9            = 25,     /*!< TIMER0 update and TIMER9 interrupts                      */
  HDL_NVIC_IRQ26_TIMER0_TRG_CMT_TIMER10      = 26,     /*!< TIMER0 trigger and commutation and TIMER10 interrupts    */
  HDL_NVIC_IRQ27_TIMER0_Channel              = 27,     /*!< TIMER0 channel capture compare interrupts                */
  HDL_NVIC_IRQ28_TIMER1                      = 28,     /*!< TIMER1 interrupt                                         */
  HDL_NVIC_IRQ29_TIMER2                      = 29,     /*!< TIMER2 interrupt                                         */
  HDL_NVIC_IRQ30_TIMER3                      = 30,     /*!< TIMER3 interrupts                                        */
  HDL_NVIC_IRQ31_I2C0_EV                     = 31,     /*!< I2C0 event interrupt                                     */
  HDL_NVIC_IRQ32_I2C0_ER                     = 32,     /*!< I2C0 error interrupt                                     */
  HDL_NVIC_IRQ33_I2C1_EV                     = 33,     /*!< I2C1 event interrupt                                     */
  HDL_NVIC_IRQ34_I2C1_ER                     = 34,     /*!< I2C1 error interrupt                                     */
  HDL_NVIC_IRQ35_SPI0                        = 35,     /*!< SPI0 interrupt                                           */
  HDL_NVIC_IRQ36_SPI1                        = 36,     /*!< SPI1 interrupt                                           */
  HDL_NVIC_IRQ37_USART0                      = 37,     /*!< USART0 interrupt                                         */
  HDL_NVIC_IRQ38_USART1                      = 38,     /*!< USART1 interrupt                                         */
  HDL_NVIC_IRQ39_USART2                      = 39,     /*!< USART2 interrupt                                         */
  HDL_NVIC_IRQ40_EXTI10_15                   = 40,     /*!< EXTI[15:10] interrupts                                   */
  HDL_NVIC_IRQ41_RTC_Alarm                   = 41,     /*!< RTC alarm interrupt                                      */
  HDL_NVIC_IRQ42_USBD_WKUP                   = 42,     /*!< USBD wakeup interrupt                                    */
  HDL_NVIC_IRQ43_TIMER7_BRK_TIMER11          = 43,     /*!< TIMER7 break and TIMER11 interrupts                      */
  HDL_NVIC_IRQ44_TIMER7_UP_TIMER12           = 44,     /*!< TIMER7 update and TIMER12 interrupts                     */
  HDL_NVIC_IRQ45_TIMER7_TRG_CMT_TIMER13      = 45,     /*!< TIMER7 trigger and commutation and TIMER13 interrupts    */
  HDL_NVIC_IRQ46_TIMER7_Channel              = 46,     /*!< TIMER7 channel capture compare interrupts                */
  HDL_NVIC_IRQ47_ADC2                        = 47,     /*!< ADC2 global interrupt                                    */
  HDL_NVIC_IRQ48_EXMC                        = 48,     /*!< EXMC global interrupt                                    */
  HDL_NVIC_IRQ49_SDIO                        = 49,     /*!< SDIO global interrupt                                    */
  HDL_NVIC_IRQ50_TIMER4                      = 50,     /*!< TIMER4 global interrupt                                  */
  HDL_NVIC_IRQ51_SPI2                        = 51,     /*!< SPI2 global interrupt                                    */
  HDL_NVIC_IRQ52_UART3                       = 52,     /*!< UART3 global interrupt                                   */
  HDL_NVIC_IRQ53_UART4                       = 53,     /*!< UART4 global interrupt                                   */
  HDL_NVIC_IRQ54_TIMER5                      = 54,     /*!< TIMER5 global interrupt                                  */
  HDL_NVIC_IRQ55_TIMER6                      = 55,     /*!< TIMER6 global interrupt                                  */
  HDL_NVIC_IRQ56_DMA1_Channel0               = 56,     /*!< DMA1 channel0 global interrupt                           */
  HDL_NVIC_IRQ57_DMA1_Channel1               = 57,     /*!< DMA1 channel1 global interrupt                           */
  HDL_NVIC_IRQ58_DMA1_Channel2               = 58,     /*!< DMA1 channel2 global interrupt                           */
  HDL_NVIC_IRQ59_DMA1_Channel3_Channel4      = 59,     /*!< DMA1 channel3 and channel4 global interrupt              */
  HDL_NVIC_IRQ60                             = 60,
  HDL_NVIC_IRQ61                             = 61,
  HDL_NVIC_IRQ62                             = 62,
  HDL_NVIC_IRQ63                             = 63,
  HDL_NVIC_IRQ64                             = 64,
  HDL_NVIC_IRQ65                             = 65,
  HDL_NVIC_IRQ66                             = 66,
  HDL_NVIC_IRQ67                             = 67,
#endif /* GD32F10X_XD */

#ifdef GD32F10X_CL
  HDL_NVIC_IRQ19_CAN0_TX                     = 19,     /*!< CAN0 TX interrupts                                       */
  HDL_NVIC_IRQ20_CAN0_RX0                    = 20,     /*!< CAN0 RX0 interrupts                                      */
  HDL_NVIC_IRQ21_CAN0_RX1                    = 21,     /*!< CAN0 RX1 interrupts                                      */
  HDL_NVIC_IRQ22_CAN0_EWMC                   = 22,     /*!< CAN0 EWMC interrupts                                     */
  HDL_NVIC_IRQ23_EXTI5_9                     = 23,     /*!< EXTI[9:5] interrupts                                     */
  HDL_NVIC_IRQ24_TIMER0_BRK                  = 24,     /*!< TIMER0 break interrupts                                  */
  HDL_NVIC_IRQ25_TIMER0_UP                   = 25,     /*!< TIMER0 update interrupts                                 */
  HDL_NVIC_IRQ26_TIMER0_TRG_CMT              = 26,     /*!< TIMER0 trigger and commutation interrupts                */
  HDL_NVIC_IRQ27_TIMER0_Channel              = 27,     /*!< TIMER0 channel capture compare interrupts                */
  HDL_NVIC_IRQ28_TIMER1                      = 28,     /*!< TIMER1 interrupt                                         */
  HDL_NVIC_IRQ29_TIMER2                      = 29,     /*!< TIMER2 interrupt                                         */
  HDL_NVIC_IRQ30_TIMER3                      = 30,     /*!< TIMER3 interrupts                                        */
  HDL_NVIC_IRQ31_I2C0_EV                     = 31,     /*!< I2C0 event interrupt                                     */
  HDL_NVIC_IRQ32_I2C0_ER                     = 32,     /*!< I2C0 error interrupt                                     */
  HDL_NVIC_IRQ33_I2C1_EV                     = 33,     /*!< I2C1 event interrupt                                     */
  HDL_NVIC_IRQ34_I2C1_ER                     = 34,     /*!< I2C1 error interrupt                                     */
  HDL_NVIC_IRQ35_SPI0                        = 35,     /*!< SPI0 interrupt                                           */
  HDL_NVIC_IRQ36_SPI1                        = 36,     /*!< SPI1 interrupt                                           */
  HDL_NVIC_IRQ37_USART0                      = 37,     /*!< USART0 interrupt                                         */
  HDL_NVIC_IRQ38_USART1                      = 38,     /*!< USART1 interrupt                                         */
  HDL_NVIC_IRQ39_USART2                      = 39,     /*!< USART2 interrupt                                         */
  HDL_NVIC_IRQ40_EXTI10_15                   = 40,     /*!< EXTI[15:10] interrupts                                   */
  HDL_NVIC_IRQ41_RTC_ALARM                   = 41,     /*!< RTC alarm interrupt                                      */
  HDL_NVIC_IRQ42_USBFS_WKUP                  = 42,     /*!< USBFS wakeup interrupt                                   */
  HDL_NVIC_IRQ43_TIMER7_BRK                  = 43,     /*!< TIMER7 break interrupts                                  */
  HDL_NVIC_IRQ44_TIMER7_UP                   = 44,     /*!< TIMER7 update interrupts                                 */
  HDL_NVIC_IRQ45_TIMER7_TRG_CMT              = 45,     /*!< TIMER7 trigger and commutation interrupts                */
  HDL_NVIC_IRQ46_TIMER7_Channel              = 46,     /*!< TIMER7 channel capture compare interrupts                */
  HDL_NVIC_IRQ47                             = 47,
  HDL_NVIC_IRQ48_EXMC                        = 48,     /*!< EXMC global interrupt                                    */
  HDL_NVIC_IRQ49                             = 49,
  HDL_NVIC_IRQ50_TIMER4                      = 50,     /*!< TIMER4 global interrupt                                  */
  HDL_NVIC_IRQ51_SPI2                        = 51,     /*!< SPI2 global interrupt                                    */
  HDL_NVIC_IRQ52_UART3                       = 52,     /*!< UART3 global interrupt                                   */
  HDL_NVIC_IRQ53_UART4                       = 53,     /*!< UART4 global interrupt                                   */
  HDL_NVIC_IRQ54_TIMER5                      = 54,     /*!< TIMER5 global interrupt                                  */
  HDL_NVIC_IRQ55_TIMER6                      = 55,     /*!< TIMER6 global interrupt                                  */
  HDL_NVIC_IRQ56_DMA1_Channel0               = 56,     /*!< DMA1 channel0 global interrupt                           */
  HDL_NVIC_IRQ57_DMA1_Channel1               = 57,     /*!< DMA1 channel1 global interrupt                           */
  HDL_NVIC_IRQ58_DMA1_Channel2               = 58,     /*!< DMA1 channel2 global interrupt                           */
  HDL_NVIC_IRQ59_DMA1_Channel3               = 59,     /*!< DMA1 channel3 global interrupt                           */
  HDL_NVIC_IRQ60_DMA1_Channel4               = 60,     /*!< DMA1 channel3 global interrupt                           */
  HDL_NVIC_IRQ61_ENET                        = 61,     /*!< ENET global interrupt                                    */
  HDL_NVIC_IRQ62_ENET_WKUP                   = 62,     /*!< ENET Wakeup interrupt                                    */
  HDL_NVIC_IRQ63_CAN1_TX                     = 63,     /*!< CAN1 TX interrupt                                        */
  HDL_NVIC_IRQ64_CAN1_RX0                    = 64,     /*!< CAN1 RX0 interrupt                                       */
  HDL_NVIC_IRQ65_CAN1_RX1                    = 65,     /*!< CAN1 RX1 interrupt                                       */
  HDL_NVIC_IRQ66_CAN1_EWMC                   = 66,     /*!< CAN1 EWMC interrupt                                      */
  HDL_NVIC_IRQ67_USBFS                       = 67,     /*!< USBFS global interrupt                                   */
#endif /* GD32F10X_CL */
  HDL_NVIC_IRQ68                             = 68,
  HDL_NVIC_IRQ69                             = 69,
  HDL_NVIC_IRQ70                             = 70,
  HDL_NVIC_IRQ71                             = 71,
  HDL_NVIC_IRQ72                             = 72,
  HDL_NVIC_IRQ73                             = 73,
  HDL_NVIC_IRQ74                             = 74,
  HDL_NVIC_IRQ75                             = 75,
  HDL_NVIC_IRQ76                             = 76,
  HDL_NVIC_IRQ77                             = 77,
  HDL_NVIC_IRQ78                             = 78,
  HDL_NVIC_IRQ79                             = 79,
} hdl_nvic_irq_n_t;

typedef struct {
  hdl_nvic_irq_n_t irq_type;
  uint8_t priority_group;
  uint8_t priority;
  uint8_t __private[HDL_INTERRUPT_PRV_SIZE];
} hdl_nvic_interrupt_t;

typedef enum {
  HDL_EXTI_TRIGGER_FALLING        = 0x01,
  HDL_EXTI_TRIGGER_RISING         = 0x02,
  HDL_EXTI_TRIGGER_RISING_FALLING = HDL_EXTI_TRIGGER_RISING | HDL_EXTI_TRIGGER_FALLING,
  HDL_EXTI_TRIGGER_NONE           = !(HDL_EXTI_TRIGGER_RISING | HDL_EXTI_TRIGGER_FALLING)
} hdl_exti_trig_type_t;

typedef enum {
  HDL_EXTI_LINE_0 = EXTI_0,   /*!< EXTI line 0 PA0, PB0, PF0 */
  HDL_EXTI_LINE_1 = EXTI_1,   /*!< EXTI line 1 PA1, PB1, PF1 */
  HDL_EXTI_LINE_2 = EXTI_2,   /*!< EXTI line 2 PA2, PB2 */
  HDL_EXTI_LINE_3 = EXTI_3,   /*!< EXTI line 3 PA3, PB3 */
  HDL_EXTI_LINE_4 = EXTI_4,   /*!< EXTI line 4 PA4, PB4 */
  HDL_EXTI_LINE_5 = EXTI_5,   /*!< EXTI line 5 PA5, PB5 */
  HDL_EXTI_LINE_6 = EXTI_6,   /*!< EXTI line 6 PA6, PB6, PF6 */
  HDL_EXTI_LINE_7 = EXTI_7,   /*!< EXTI line 7 PA7, PB7, PF7 */
  HDL_EXTI_LINE_8 = EXTI_8,   /*!< EXTI line 8 PA8, PB8 */
  HDL_EXTI_LINE_9 = EXTI_9,   /*!< EXTI line 9 PA9, PB9 */
  HDL_EXTI_LINE_10 = EXTI_10, /*!< EXTI line 10 PA10, PB10 */
  HDL_EXTI_LINE_11 = EXTI_11, /*!< EXTI line 11 PA11, PB11 */
  HDL_EXTI_LINE_12 = EXTI_12, /*!< EXTI line 12 PA12, PB12 */
  HDL_EXTI_LINE_13 = EXTI_13, /*!< EXTI line 13 PA13, PB13, PC13 */
  HDL_EXTI_LINE_14 = EXTI_14, /*!< EXTI line 14 PA14, PB14, PC14 */
  HDL_EXTI_LINE_15 = EXTI_15, /*!< EXTI line 15 PA15, PB15, PC15 */
  HDL_EXTI_LINE_16 = EXTI_16, /*!< EXTI line 16 LVD */
  HDL_EXTI_LINE_17 = EXTI_17, /*!< EXTI line 17 RTC alarm */
  HDL_EXTI_LINE_18 = EXTI_18, /*!< EXTI line 18 Reserved */
  HDL_EXTI_LINE_19 = EXTI_19, /*!< EXTI line 19 RTC tamper and timestamp */
} hdl_exti_line_t;

typedef enum {
  HDL_EXTI_SOURCE_PA = 0b0000,
  HDL_EXTI_SOURCE_PB = 0b0001,
  HDL_EXTI_SOURCE_PC = 0b0010,
  HDL_EXTI_SOURCE_PD = 0b0011,
  HDL_EXTI_SOURCE_PE = 0b0100,
  HDL_EXTI_SOURCE_PF = 0b0101,
  HDL_EXTI_SOURCE_PG = 0b0110,
} hdl_exti_source_t;

typedef enum {
  HDL_EXTI_MODE_INTERRUPT = EXTI_INTERRUPT,
  HDL_EXTI_MODE_EVENT = EXTI_EVENT
} hdl_exti_mode_t;

typedef struct {
  hdl_exti_line_t line;
  hdl_exti_source_t source;
  hdl_exti_mode_t mode;
  hdl_exti_trig_type_t trigger;
} hdl_nvic_exti_t;

typedef struct {
  hdl_module_t module;
  uint32_t prio_bits;
  hdl_nvic_interrupt_t **interrupts;
  hdl_nvic_exti_t **exti_lines;
} hdl_nvic_t;

typedef struct{
  hdl_module_t module;
  uint32_t flash_latency;
} hdl_core_t;

#define hdl_interrupts(...) ((hdl_nvic_interrupt_t *[]){__VA_ARGS__, NULL})
#define hdl_exti_lines(...) ((hdl_nvic_exti_t *[]){__VA_ARGS__, NULL})

typedef hdl_nvic_t hdl_interrupt_controller_t;
typedef hdl_nvic_interrupt_t hdl_interrupt_t;
typedef hdl_nvic_irq_n_t hdl_irq_n_t;

void hdl_exti_sw_trigger(hdl_interrupt_controller_t *ic, hdl_exti_line_t line);

#endif // PORT_CORE_H_
