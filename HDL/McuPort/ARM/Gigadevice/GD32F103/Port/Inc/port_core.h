
#ifndef PORT_CORE_H_
#define PORT_CORE_H_

#include "port_exti.h"

#define HDL_INTERRUPT_PRV_SIZE       4

#define HDL_VTOR_TAB_ALIGN         256  //(2 << SCB_VTOR_TBLOFF_Pos)

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
  PRIVATE(hw, HDL_INTERRUPT_PRV_SIZE);
} hdl_interrupt_t;


typedef struct {
  uint32_t prio_bits;
  uint32_t prio_group;
  hdl_interrupt_t **interrupts;
  uint8_t irq_latency; /* processor ensures that a minimum of irq_latency+1 hclk cycles exist between an interrupt becoming pended */
  const void * const vector;
} hdl_interrupt_controller_config_t;

#define hdl_interrupts(...) ((hdl_interrupt_t *[]){__VA_ARGS__, NULL})

typedef struct{
  uint32_t flash_latency;
} hdl_core_config_t;

extern const void *_estack;
extern const void *_sidata, *_sdata, *_edata;
extern const void *_sbss, *_ebss;
extern const void *_eflash; 

void call_isr(hdl_nvic_irq_n_t irq, uint32_t event);

void irq_n_handler();

void reset_handler();

void svc_handler();
void nmi_handler();
void hard_fault_handler();
void pend_sv_handler();
void systick_handler();

void mem_manage_handler();
void bus_fault_handler();
void usage_fault_handler();
void debug_mon_handler();

void wwdgt_handler();
void lvd_handler();
void TAMPER_IRQHandler();
void rtc_handler();
void fmc_handler();
void rcu_handler();
void EXTI0_IRQHandler();
void EXTI1_IRQHandler();
void EXTI2_IRQHandler();
void EXTI3_IRQHandler();
void EXTI4_IRQHandler();
void DMA0_Channel0_IRQHandler();
void DMA0_Channel1_IRQHandler();
void DMA0_Channel2_IRQHandler();
void DMA0_Channel3_IRQHandler();
void DMA0_Channel4_IRQHandler();
void DMA0_Channel5_IRQHandler();
void DMA0_Channel6_IRQHandler();
void ADC0_1_IRQHandler();
#ifdef GD32F10X_MD
void USBD_HP_CAN0_TX_IRQHandler();
void USBD_LP_CAN0_RX0_IRQHandler();
void CAN0_RX1_IRQHandler();
void CAN0_EWMC_IRQHandler();
void EXTI5_9_IRQHandler();
void TIMER0_BRK_IRQHandler();
void TIMER0_UP_IRQHandler();
void TIMER0_TRG_CMT_IRQHandler();
void timer0_channel_handler();
void TIMER1_IRQHandler();
void timer2_handler();
void TIMER3_IRQHandler();
void i2c0_ev_handler();
void i2c0_er_handler();
void i2c1_ev_handler();
void i2c1_er_handler();
void spi0_handler();
void spi1_handler();
void usart0_handler();
void usart1_handler();
void USART2_IRQHandler();
void EXTI10_15_IRQHandler();
void RTC_Alarm_IRQHandler();
void USBD_WKUP_IRQHandler();
void EXMC_IRQHandler();
#endif /* GD32F10X_MD */
#ifdef GD32F10X_HD
void USBD_HP_CAN0_TX_IRQHandler();
void USBD_LP_CAN0_RX0_IRQHandler();
void CAN0_RX1_IRQHandler();
void CAN0_EWMC_IRQHandler();
void EXTI5_9_IRQHandler();
void TIMER0_BRK_IRQHandler();
void TIMER0_UP_IRQHandler();
void TIMER0_TRG_CMT_IRQHandler();
void timer0_channel_handler();
void TIMER1_IRQHandler();
void timer2_handler();
void TIMER3_IRQHandler();
void i2c0_ev_handler();
void i2c0_er_handler();
void i2c1_ev_handler();
void i2c1_er_handler();
void spi0_handler();
void spi1_handler();
void usart0_handler();
void usart1_handler();
void USART2_IRQHandler();
void EXTI10_15_IRQHandler();
void RTC_Alarm_IRQHandler();
void USBD_WKUP_IRQHandler();
void TIMER7_BRK_IRQHandler();
void TIMER7_UP_IRQHandler();
void TIMER7_TRG_CMT_IRQHandler();
void TIMER7_Channel_IRQHandler();
void ADC2_IRQHandler();
void EXMC_IRQHandler();
void SDIO_IRQHandler();
void TIMER4_IRQHandler();
void SPI2_IRQHandler();
void UART3_IRQHandler();
void UART4_IRQHandler();
void timer5_handler();
void TIMER6_IRQHandler();
void DMA1_Channel0_IRQHandler();
void DMA1_Channel1_IRQHandler();
void DMA1_Channel2_IRQHandler();
void DMA1_Channel3_4_IRQHandler();
#endif /* GD32F10X_HD */
#ifdef GD32F10X_XD
void USBD_HP_CAN0_TX_IRQHandler();
void USBD_LP_CAN0_RX0_IRQHandler();
void CAN0_RX1_IRQHandler();
void CAN0_EWMC_IRQHandler();
void EXTI5_9_IRQHandler();
void TIMER0_BRK_TIMER8_IRQHandler();
void TIMER0_UP_TIMER9_IRQHandler();
void TIMER0_TRG_CMT_TIMER10_IRQHandler();
void timer0_channel_handler();
void TIMER1_IRQHandler();
void timer2_handler();
void TIMER3_IRQHandler();
void i2c0_ev_handler();
void i2c0_er_handler();
void i2c1_ev_handler();
void i2c1_er_handler();
void spi0_handler();
void spi1_handler();
void usart0_handler();
void usart1_handler();
void USART2_IRQHandler();
void EXTI10_15_IRQHandler();
void RTC_Alarm_IRQHandler();
void USBD_WKUP_IRQHandler();
void TIMER7_BRK_TIMER11_IRQHandler();
void TIMER7_UP_TIMER12_IRQHandler();
void TIMER7_TRG_CMT_TIMER13_IRQHandler();
void TIMER7_Channel_IRQHandler();
void ADC2_IRQHandler();
void EXMC_IRQHandler();
void SDIO_IRQHandler();
void TIMER4_IRQHandler();
void SPI2_IRQHandler();
void UART3_IRQHandler();
void UART4_IRQHandler();
void timer5_handler();
void TIMER6_IRQHandler();
void DMA1_Channel0_IRQHandler();
void DMA1_Channel1_IRQHandler();
void DMA1_Channel2_IRQHandler();
void DMA1_Channel3_4_IRQHandler();
#endif /* GD32F10X_XD */
#ifdef GD32F10X_CL
void CAN0_TX_IRQHandler();
void CAN0_RX0_IRQHandler();
void CAN0_RX1_IRQHandler();
void CAN0_EWMC_IRQHandler();
void EXTI5_9_IRQHandler();
void TIMER0_BRK_IRQHandler();
void TIMER0_UP_IRQHandler();
void TIMER0_TRG_CMT_IRQHandler();
void timer0_channel_handler();
void TIMER1_IRQHandler();
void timer2_handler();
void TIMER3_IRQHandler();
void i2c0_ev_handler();
void i2c0_er_handler();
void i2c1_ev_handler();
void i2c1_er_handler();
void spi0_handler();
void spi1_handler();
void usart0_handler();
void usart1_handler();
void USART2_IRQHandler();
void EXTI10_15_IRQHandler();
void RTC_Alarm_IRQHandler();
void USBFS_WKUP_IRQHandler();
void TIMER7_BRK_IRQHandler();
void TIMER7_UP_IRQHandler();
void TIMER7_TRG_CMT_IRQHandler();
void TIMER7_Channel_IRQHandler(); 
void EXMC_IRQHandler();
void TIMER4_IRQHandler();
void SPI2_IRQHandler();
void UART3_IRQHandler();
void UART4_IRQHandler();
void timer5_handler();
void TIMER6_IRQHandler();
void DMA1_Channel0_IRQHandler();
void DMA1_Channel1_IRQHandler();
void DMA1_Channel2_IRQHandler();
void DMA1_Channel3_IRQHandler();
void DMA1_Channel4_IRQHandler();
void ENET_IRQHandler();
void ENET_WKUP_IRQHandler();
void CAN1_TX_IRQHandler();
void CAN1_RX0_IRQHandler();
void CAN1_RX1_IRQHandler();
void CAN1_EWMC_IRQHandler();
void USBFS_IRQHandler();
#endif /* GD32F10X_CL */

#endif // PORT_CORE_H_
