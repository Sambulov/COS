
#ifndef PORT_CORE_SPEC_H_
#define PORT_CORE_SPEC_H_

#define HDL_VTOR_TAB_ALIGN         256  //(2 << SCB_VTOR_TBLOFF_Pos)


typedef enum {
  /* Cortex-M4 processor exceptions numbers */
  HDL_NVIC_EXCEPTION_NonMaskableInt         = -14,    /*!< 2 Non Maskable Interrupt                                          */
  HDL_NVIC_EXCEPTION_HardFault              = -13,    /*!< hardfault interrupt                                           */
  HDL_NVIC_EXCEPTION_MemoryManagement       = -12,    /*!< 4 Cortex-M4 Memory Management Interrupt                           */
  HDL_NVIC_EXCEPTION_BusFault               = -11,    /*!< 5 Cortex-M4 Bus Fault Interrupt                                   */
  HDL_NVIC_EXCEPTION_UsageFault             = -10,    /*!< 6 Cortex-M4 Usage Fault Interrupt                                 */
  HDL_NVIC_EXCEPTION_SVCall                 = -5,     /*!< 11 Cortex-M4 SV Call Interrupt                                    */
  HDL_NVIC_EXCEPTION_DebugMonitor           = -4,     /*!< 12 Cortex-M4 Debug Monitor Interrupt                              */
  HDL_NVIC_EXCEPTION_PendSV                 = -2,     /*!< 14 Cortex-M4 Pend SV Interrupt                                    */
  HDL_NVIC_EXCEPTION_SysTick                = -1,     /*!< 15 Cortex-M4 System Tick Interrupt                                */
  /* interruput numbers */
  HDL_NVIC_IRQ0_WWDG                  = 0,      /*!< Window WatchDog Interrupt                                         */
  HDL_NVIC_IRQ1_PVD                   = 1,      /*!< PVD through EXTI Line detection Interrupt                         */
  HDL_NVIC_IRQ2_TAMP_STAMP            = 2,      /*!< Tamper and TimeStamp interrupts through the EXTI line             */
  HDL_NVIC_IRQ3_RTC_WKUP              = 3,      /*!< RTC Wakeup interrupt through the EXTI line                        */
  HDL_NVIC_IRQ4_FLASH                 = 4,      /*!< FLASH global Interrupt                                            */
  HDL_NVIC_IRQ5_RCC                   = 5,      /*!< RCC global Interrupt                                              */
  HDL_NVIC_IRQ6_EXTI0                 = 6,      /*!< EXTI Line0 Interrupt                                              */
  HDL_NVIC_IRQ7_EXTI1                 = 7,      /*!< EXTI Line1 Interrupt                                              */
  HDL_NVIC_IRQ8_EXTI2                 = 8,      /*!< EXTI Line2 Interrupt                                              */
  HDL_NVIC_IRQ9_EXTI3                 = 9,      /*!< EXTI Line3 Interrupt                                              */
  HDL_NVIC_IRQ10_EXTI4                = 10,     /*!< EXTI Line4 Interrupt                                              */
  HDL_NVIC_IRQ11_DMA1_Stream0         = 11,     /*!< DMA1 Stream 0 global Interrupt                                    */
  HDL_NVIC_IRQ12_DMA1_Stream1         = 12,     /*!< DMA1 Stream 1 global Interrupt                                    */
  HDL_NVIC_IRQ13_DMA1_Stream2         = 13,     /*!< DMA1 Stream 2 global Interrupt                                    */
  HDL_NVIC_IRQ14_DMA1_Stream3         = 14,     /*!< DMA1 Stream 3 global Interrupt                                    */
  HDL_NVIC_IRQ15_DMA1_Stream4         = 15,     /*!< DMA1 Stream 4 global Interrupt                                    */
  HDL_NVIC_IRQ16_DMA1_Stream5         = 16,     /*!< DMA1 Stream 5 global Interrupt                                    */
  HDL_NVIC_IRQ17_DMA1_Stream6         = 17,     /*!< DMA1 Stream 6 global Interrupt                                    */
  HDL_NVIC_IRQ18_ADC                  = 18,     /*!< ADC1, ADC2 and ADC3 global Interrupts                             */
  HDL_NVIC_IRQ19_CAN1_TX              = 19,     /*!< CAN1 TX Interrupt                                                 */
  HDL_NVIC_IRQ20_CAN1_RX0             = 20,     /*!< CAN1 RX0 Interrupt                                                */
  HDL_NVIC_IRQ21_CAN1_RX1             = 21,     /*!< CAN1 RX1 Interrupt                                                */
  HDL_NVIC_IRQ22_CAN1_SCE             = 22,     /*!< CAN1 SCE Interrupt                                                */
  HDL_NVIC_IRQ23_EXTI9_5              = 23,     /*!< External Line[9:5] Interrupts                                     */
  HDL_NVIC_IRQ24_TIM1_BRK_TIM9        = 24,     /*!< TIM1 Break interrupt and TIM9 global interrupt                    */
  HDL_NVIC_IRQ25_TIM1_UP_TIM10        = 25,     /*!< TIM1 Update Interrupt and TIM10 global interrupt                  */
  HDL_NVIC_IRQ26_TIM1_TRG_COM_TIM11   = 26,     /*!< TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
  HDL_NVIC_IRQ27_TIM1_CC              = 27,     /*!< TIM1 Capture Compare Interrupt                                    */
  HDL_NVIC_IRQ28_TIM2                 = 28,     /*!< TIM2 global Interrupt                                             */
  HDL_NVIC_IRQ29_TIM3                 = 29,     /*!< TIM3 global Interrupt                                             */
  HDL_NVIC_IRQ30_TIM4                 = 30,     /*!< TIM4 global Interrupt                                             */
  HDL_NVIC_IRQ31_I2C1_EV              = 31,     /*!< I2C1 Event Interrupt                                              */
  HDL_NVIC_IRQ32_I2C1_ER              = 32,     /*!< I2C1 Error Interrupt                                              */
  HDL_NVIC_IRQ33_I2C2_EV              = 33,     /*!< I2C2 Event Interrupt                                              */
  HDL_NVIC_IRQ34_I2C2_ER              = 34,     /*!< I2C2 Error Interrupt                                              */  
  HDL_NVIC_IRQ35_SPI1                 = 35,     /*!< SPI1 global Interrupt                                             */
  HDL_NVIC_IRQ36_SPI2                 = 36,     /*!< SPI2 global Interrupt                                             */
  HDL_NVIC_IRQ37_USART1               = 37,     /*!< USART1 global Interrupt                                           */
  HDL_NVIC_IRQ38_USART2               = 38,     /*!< USART2 global Interrupt                                           */
  HDL_NVIC_IRQ39_USART3               = 39,     /*!< USART3 global Interrupt                                           */
  HDL_NVIC_IRQ40_EXTI15_10            = 40,     /*!< External Line[15:10] Interrupts                                   */
  HDL_NVIC_IRQ41_RTC_Alarm            = 41,     /*!< RTC Alarm (A and B) through EXTI Line Interrupt                   */
  HDL_NVIC_IRQ42_OTG_FS_WKUP          = 42,     /*!< USB OTG FS Wakeup through EXTI line interrupt                     */    
  HDL_NVIC_IRQ43_TIM8_BRK_TIM12       = 43,     /*!< TIM8 Break Interrupt and TIM12 global interrupt                   */
  HDL_NVIC_IRQ44_TIM8_UP_TIM13        = 44,     /*!< TIM8 Update Interrupt and TIM13 global interrupt                  */
  HDL_NVIC_IRQ45_TIM8_TRG_COM_TIM14   = 45,     /*!< TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
  HDL_NVIC_IRQ46_TIM8_CC              = 46,     /*!< TIM8 Capture Compare Interrupt                                    */
  HDL_NVIC_IRQ47_DMA1_Stream7         = 47,     /*!< DMA1 Stream7 Interrupt                                            */
  HDL_NVIC_IRQ48_FSMC                 = 48,     /*!< FSMC global Interrupt                                             */
  HDL_NVIC_IRQ49_SDIO                 = 49,     /*!< SDIO global Interrupt                                             */
  HDL_NVIC_IRQ50_TIM5                 = 50,     /*!< TIM5 global Interrupt                                             */
  HDL_NVIC_IRQ51_SPI3                 = 51,     /*!< SPI3 global Interrupt                                             */
  HDL_NVIC_IRQ52_UART4                = 52,     /*!< UART4 global Interrupt                                            */
  HDL_NVIC_IRQ53_UART5                = 53,     /*!< UART5 global Interrupt                                            */
  HDL_NVIC_IRQ54_TIM6_DAC             = 54,     /*!< TIM6 global and DAC1&2 underrun error  interrupts                 */
  HDL_NVIC_IRQ55_TIM7                 = 55,     /*!< TIM7 global interrupt                                             */
  HDL_NVIC_IRQ56_DMA2_Stream0         = 56,     /*!< DMA2 Stream 0 global Interrupt                                    */
  HDL_NVIC_IRQ57_DMA2_Stream1         = 57,     /*!< DMA2 Stream 1 global Interrupt                                    */
  HDL_NVIC_IRQ58_DMA2_Stream2         = 58,     /*!< DMA2 Stream 2 global Interrupt                                    */
  HDL_NVIC_IRQ59_DMA2_Stream3         = 59,     /*!< DMA2 Stream 3 global Interrupt                                    */
  HDL_NVIC_IRQ60_DMA2_Stream4         = 60,     /*!< DMA2 Stream 4 global Interrupt                                    */
  HDL_NVIC_IRQ61_ETH                  = 61,     /*!< Ethernet global Interrupt                                         */
  HDL_NVIC_IRQ62_ETH_WKUP             = 62,     /*!< Ethernet Wakeup through EXTI line Interrupt                       */
  HDL_NVIC_IRQ63_CAN2_TX              = 63,     /*!< CAN2 TX Interrupt                                                 */
  HDL_NVIC_IRQ64_CAN2_RX0             = 64,     /*!< CAN2 RX0 Interrupt                                                */
  HDL_NVIC_IRQ65_CAN2_RX1             = 65,     /*!< CAN2 RX1 Interrupt                                                */
  HDL_NVIC_IRQ66_CAN2_SCE             = 66,     /*!< CAN2 SCE Interrupt                                                */
  HDL_NVIC_IRQ67_OTG_FS               = 67,     /*!< USB OTG FS global Interrupt                                       */
  HDL_NVIC_IRQ68_DMA2_Stream5         = 68,     /*!< DMA2 Stream 5 global interrupt                                    */
  HDL_NVIC_IRQ69_DMA2_Stream6         = 69,     /*!< DMA2 Stream 6 global interrupt                                    */
  HDL_NVIC_IRQ70_DMA2_Stream7         = 70,     /*!< DMA2 Stream 7 global interrupt                                    */
  HDL_NVIC_IRQ71_USART6               = 71,     /*!< USART6 global interrupt                                           */
  HDL_NVIC_IRQ72_I2C3_EV              = 72,     /*!< I2C3 event interrupt                                              */
  HDL_NVIC_IRQ73_I2C3_ER              = 73,     /*!< I2C3 error interrupt                                              */
  HDL_NVIC_IRQ74_OTG_HS_EP1_OUT       = 74,     /*!< USB OTG HS End Point 1 Out global interrupt                       */
  HDL_NVIC_IRQ75_OTG_HS_EP1_IN        = 75,     /*!< USB OTG HS End Point 1 In global interrupt                        */
  HDL_NVIC_IRQ76_OTG_HS_WKUP          = 76,     /*!< USB OTG HS Wakeup through EXTI interrupt                          */
  HDL_NVIC_IRQ77_OTG_HS               = 77,     /*!< USB OTG HS global interrupt                                       */
  HDL_NVIC_IRQ78_DCMI                 = 78,     /*!< DCMI global interrupt                                             */
  HDL_NVIC_IRQ79                      = 79,
  HDL_NVIC_IRQ80_HASH_RNG             = 80,     /*!< Hash and RNG global interrupt                                     */
  HDL_NVIC_IRQ81_FPU                  = 81,     /*!< FPU global interrupt                                              */
  HDL_NVIC_IRQ82                      = 82,
  HDL_NVIC_IRQ83                      = 83,
  HDL_NVIC_IRQ84                      = 84,
  HDL_NVIC_IRQ85                      = 85,
  HDL_NVIC_IRQ86                      = 86,
  HDL_NVIC_IRQ87                      = 87,
  HDL_NVIC_IRQ88                      = 88,
  HDL_NVIC_IRQ89                      = 89,
  HDL_NVIC_IRQ90                      = 90,
  HDL_NVIC_IRQ91                      = 91,
  HDL_NVIC_IRQ92                      = 92,
  HDL_NVIC_IRQ93                      = 93,
  HDL_NVIC_IRQ94                      = 94,
  HDL_NVIC_IRQ95                      = 95,
  HDL_NVIC_IRQ96                      = 96,
  HDL_NVIC_IRQ97                      = 97,
  HDL_NVIC_IRQ98                      = 98,
  HDL_NVIC_IRQ99                      = 99,
  HDL_NVIC_IRQ100                     = 100,
  HDL_NVIC_IRQ101                     = 101,
  HDL_NVIC_IRQ102                     = 102,
  HDL_NVIC_IRQ103                     = 103,
  HDL_NVIC_IRQ104                     = 104,
  HDL_NVIC_IRQ105                     = 105,
  HDL_NVIC_IRQ106                     = 106,
  HDL_NVIC_IRQ107                     = 107,
  HDL_NVIC_IRQ108                     = 108,
  HDL_NVIC_IRQ109                     = 109,
  HDL_NVIC_IRQ110                     = 110,
  HDL_NVIC_IRQ111                     = 111,
  HDL_NVIC_IRQ112                     = 112,
  HDL_NVIC_IRQ113                     = 113,
  HDL_NVIC_IRQ114                     = 114,
  HDL_NVIC_IRQ115                     = 115,
  HDL_NVIC_IRQ116                     = 116,
  HDL_NVIC_IRQ117                     = 117,
  HDL_NVIC_IRQ118                     = 118,
  HDL_NVIC_IRQ119                     = 119,
  HDL_NVIC_IRQ120                     = 120,
  HDL_NVIC_IRQ121                     = 121,
  HDL_NVIC_IRQ122                     = 122,
  HDL_NVIC_IRQ123                     = 123,
  HDL_NVIC_IRQ124                     = 124,
  HDL_NVIC_IRQ125                     = 125,
  HDL_NVIC_IRQ126                     = 126,
  HDL_NVIC_IRQ127                     = 127
} hdl_nvic_irq_n_t;

typedef struct {
  uint32_t flash_latency;
  uint32_t phy;
} hdl_core_config_t;

void NMI_Handler();
void HardFault_Handler();
void MemManage_Handler();
void BusFault_Handler();
void UsageFault_Handler();
void SVC_Handler();
void DebugMon_Handler();
void PendSV_Handler();
void SysTick_Handler();
void WWDG_IRQHandler();
void PVD_IRQHandler();
void TAMP_STAMP_IRQHandler();
void RTC_WKUP_IRQHandler();
void FLASH_IRQHandler();
void RCC_IRQHandler();
void EXTI0_IRQHandler();
void EXTI1_IRQHandler();
void EXTI2_IRQHandler();
void EXTI3_IRQHandler();
void EXTI4_IRQHandler();
void DMA1_Stream0_IRQHandler();
void DMA1_Stream1_IRQHandler();
void DMA1_Stream2_IRQHandler();
void DMA1_Stream3_IRQHandler();
void DMA1_Stream4_IRQHandler();
void DMA1_Stream5_IRQHandler();
void DMA1_Stream6_IRQHandler();
void ADC_IRQHandler();
void CAN1_TX_IRQHandler();
void CAN1_RX0_IRQHandler();
void CAN1_RX1_IRQHandler();
void CAN1_SCE_IRQHandler();
void EXTI9_5_IRQHandler();
void TIM1_BRK_TIM9_IRQHandler();
void TIM1_UP_TIM10_IRQHandler();
void TIM1_TRG_COM_TIM11_IRQHandler();
void TIM1_CC_IRQHandler();
void TIM2_IRQHandler();
void TIM3_IRQHandler();
void TIM4_IRQHandler();
void I2C1_EV_IRQHandler();
void I2C1_ER_IRQHandler();
void I2C2_EV_IRQHandler();
void I2C2_ER_IRQHandler();
void SPI1_IRQHandler();
void SPI2_IRQHandler();
void USART1_IRQHandler();
void USART2_IRQHandler();
void USART3_IRQHandler();
void EXTI15_10_IRQHandler();
void RTC_Alarm_IRQHandler();
void OTG_FS_WKUP_IRQHandler();
void TIM8_BRK_TIM12_IRQHandler();
void TIM8_UP_TIM13_IRQHandler();
void TIM8_TRG_COM_TIM14_IRQHandler();
void TIM8_CC_IRQHandler();
void DMA1_Stream7_IRQHandler();
void FSMC_IRQHandler();
void SDIO_IRQHandler();
void TIM5_IRQHandler();
void SPI3_IRQHandler();
void UART4_IRQHandler();
void UART5_IRQHandler();
void TIM6_DAC_IRQHandler();
void TIM7_IRQHandler();
void DMA2_Stream0_IRQHandler();
void DMA2_Stream1_IRQHandler();
void DMA2_Stream2_IRQHandler();
void DMA2_Stream3_IRQHandler();
void DMA2_Stream4_IRQHandler();
void ETH_IRQHandler();
void ETH_WKUP_IRQHandler();
void CAN2_TX_IRQHandler();
void CAN2_RX0_IRQHandler();
void CAN2_RX1_IRQHandler();
void CAN2_SCE_IRQHandler();
void OTG_FS_IRQHandler();
void DMA2_Stream5_IRQHandler();
void DMA2_Stream6_IRQHandler();
void DMA2_Stream7_IRQHandler();
void USART6_IRQHandler();
void I2C3_EV_IRQHandler();
void I2C3_ER_IRQHandler();
void OTG_HS_EP1_OUT_IRQHandler();
void OTG_HS_EP1_IN_IRQHandler();
void OTG_HS_WKUP_IRQHandler();
void OTG_HS_IRQHandler();
void DCMI_IRQHandler();
void HASH_RNG_IRQHandler();
void FPU_IRQHandler();

#endif // PORT_CORE_SPEC_H_
