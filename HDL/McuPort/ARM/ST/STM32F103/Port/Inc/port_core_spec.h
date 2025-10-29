
#ifndef PORT_CORE_SPEC_H_
#define PORT_CORE_SPEC_H_

#define HDL_VTOR_TAB_ALIGN         256  //(2 << SCB_VTOR_TBLOFF_Pos)

typedef enum {

  HDL_NVIC_EXCEPTION_NonMaskableInt   = -14, /*!< 2 Non Maskable Interrupt */
  HDL_NVIC_EXCEPTION_HardFault        = -13, /*!< 3 Cortex-M3 Hard Fault Interrupt */
  HDL_NVIC_EXCEPTION_MemoryManagement = -12, /*!< 4 Cortex-M3 Memory Management Interrupt */
  HDL_NVIC_EXCEPTION_BusFault         = -11, /*!< 5 Cortex-M3 Bus Fault Interrupt */
  HDL_NVIC_EXCEPTION_UsageFault       = -10, /*!< 6 Cortex-M3 Usage Fault Interrupt */

  HDL_NVIC_EXCEPTION_SVCall           = -5,  /*!< 11 Cortex-M3 SV Call Interrupt */
  HDL_NVIC_EXCEPTION_DebugMonitor     = -4,  /*!< 12 Cortex-M3 Debug Monitor Interrupt */
  HDL_NVIC_EXCEPTION_PendSV           = -2,  /*!< 14 Cortex-M3 Pend SV Interrupt */
  HDL_NVIC_EXCEPTION_SysTick          = -1,  /*!< 15 Cortex-M3 System Tick Interrupt */


  /* interruput numbers */
  HDL_NVIC_IRQ0_WWDG           = 0,        /*!< Window WatchDog Interrupt */
  HDL_NVIC_IRQ1_PVD            = 1,        /*!< PVD through EXTI Line detection Interrupt */
  HDL_NVIC_IRQ2_RTC            = 2,        /*!< Tamper Interrupt */
  HDL_NVIC_IRQ3_TAMPER         = 3,        /*!< RTC global Interrupt */
  HDL_NVIC_IRQ4_FLASH          = 4,        /*!< FLASH global Interrupt */
  HDL_NVIC_IRQ5_RCC            = 5,        /*!< RCC global Interrupt */
  HDL_NVIC_IRQ6_EXTI0          = 6,        /*!< EXTI Line0 Interrupt */
  HDL_NVIC_IRQ7_EXTI1          = 7,        /*!< EXTI Line1 Interrupt */
  HDL_NVIC_IRQ8_EXTI2          = 8,        /*!< EXTI Line2 Interrupt */
  HDL_NVIC_IRQ9_EXTI3          = 9,        /*!< EXTI Line3 Interrupt */
  HDL_NVIC_IRQ10_EXTI4         = 10,       /*!< EXTI Line4 Interrupt */
  HDL_NVIC_IRQ11_DMA1_Channel1 = 11,       /*!< DMA1 Channel 1 global Interrupt */
  HDL_NVIC_IRQ12_DMA1_Channel2 = 12,       /*!< DMA1 Channel 2 global Interrupt */
  HDL_NVIC_IRQ13_DMA1_Channel3 = 13,       /*!< DMA1 Channel 3 global Interrupt */
  HDL_NVIC_IRQ14_DMA1_Channel4 = 14,       /*!< DMA1 Channel 4 global Interrupt */
  HDL_NVIC_IRQ15_DMA1_Channel5 = 15,       /*!< DMA1 Channel 5 global Interrupt */
  HDL_NVIC_IRQ16_DMA1_Channel6 = 16,       /*!< DMA1 Channel 6 global Interrupt */
  HDL_NVIC_IRQ17_DMA1_Channel7 = 17,       /*!< DMA1 Channel 7 global Interrupt */
  HDL_NVIC_IRQ18_ADC1          = 18,       /*!< ADC1 global Interrupt */
  HDL_NVIC_IRQ19_USB_HP        = 19,       /*!< USB Device High Priority */
  HDL_NVIC_IRQ20_USB_LP        = 20,       /*!< USB Device Low Priority */
  HDL_NVIC_IRQ21               = 21,       /*!<  */
  HDL_NVIC_IRQ22               = 22,       /*!<  */
  HDL_NVIC_IRQ23_EXTI9_5       = 23,       /*!< External Line[9:5] Interrupts */
  HDL_NVIC_IRQ24               = 24,       /*!<  */
  HDL_NVIC_IRQ25               = 25,       /*!<  */
  HDL_NVIC_IRQ26               = 26,       /*!<  */
  HDL_NVIC_IRQ27               = 27,       /*!<  */
  HDL_NVIC_IRQ28_TIM2          = 28,       /*!< TIM2 global Interrupt */
  HDL_NVIC_IRQ29_TIM3          = 29,       /*!< TIM3 global Interrupt */
  HDL_NVIC_IRQ30_TIM4          = 30,       /*!< TIM4 global Interrupt */
  HDL_NVIC_IRQ31_I2C1_EV       = 31,       /*!< I2C1 Event Interrupt */
  HDL_NVIC_IRQ32_I2C1_ER       = 32,       /*!< I2C1 Error Interrupt */
  HDL_NVIC_IRQ33_I2C2_EV       = 33,       /*!< I2C2 Event Interrupt */
  HDL_NVIC_IRQ34_I2C2_ER       = 34,       /*!< I2C2 Error Interrupt */
  HDL_NVIC_IRQ35_SPI1          = 35,       /*!< SPI1 global Interrupt */
  HDL_NVIC_IRQ36_SPI2          = 36,       /*!< SPI2 global Interrupt */
  HDL_NVIC_IRQ37_USART1        = 37,       /*!< USART1 global Interrupt */
  HDL_NVIC_IRQ38_USART2        = 38,       /*!< USART2 global Interrupt */
  HDL_NVIC_IRQ39_USART3        = 39,       /*!< USART3 global Interrupt */
  HDL_NVIC_IRQ40_EXTI15_10     = 40,       /*!< External Line[15:10] Interrupts */
  HDL_NVIC_IRQ41_RTC_Alarm     = 41,       /*!< RTC Alarm through EXTI Line Interrupt */
  HDL_NVIC_IRQ42_USBWakeUp     = 42,       /*!< USB Device WakeUp from suspend through EXTI Line Interrupt */
} hdl_nvic_irq_n_t;

typedef struct{
  uint32_t flash_latency;
} hdl_core_config_t;

void WWDG_IRQHandler();
void PVD_IRQHandler();
void TAMPER_IRQHandler();
void RTC_IRQHandler();
void FLASH_IRQHandler();
void RCC_IRQHandler();
void EXTI0_IRQHandler();
void EXTI1_IRQHandler();
void EXTI2_IRQHandler();
void EXTI3_IRQHandler();
void EXTI4_IRQHandler();
void DMA1_Channel1_IRQHandler();
void DMA1_Channel2_IRQHandler();
void DMA1_Channel3_IRQHandler();
void DMA1_Channel4_IRQHandler();
void DMA1_Channel5_IRQHandler();
void DMA1_Channel6_IRQHandler();
void DMA1_Channel7_IRQHandler();
void ADC1_2_IRQHandler();
void USB_HP_CAN1_TX_IRQHandler();
void USB_LP_CAN1_RX0_IRQHandler();
void EXTI9_5_IRQHandler();
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
void USBWakeUp_IRQHandler();

#endif // PORT_CORE_SPEC_H_
