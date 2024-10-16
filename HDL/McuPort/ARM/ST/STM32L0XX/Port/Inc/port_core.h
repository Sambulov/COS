
#ifndef PORT_CORE_H_
#define PORT_CORE_H_

#define HDL_INTERRUPT_PRV_SIZE       4

#define HDL_VTOR_TAB_ALIGN         256  //(2 << SCB_VTOR_TBLOFF_Pos)

typedef enum {
  /* Cortex-M4 processor exceptions numbers */
  HDL_NVIC_EXCEPTION_NonMaskableInt = -14,       /*!< 2 non maskable interrupt                                      */
  HDL_NVIC_EXCEPTION_HardFault = -13,            /*!< hardfault interrupt                                           */
  HDL_NVIC_EXCEPTION_SVCall = -5,                /*!< 11 Cortex-M3 SV call interrupt                                */
  HDL_NVIC_EXCEPTION_PendSV = -2,                /*!< 14 Cortex-M3 pend SV interrupt                                */
  HDL_NVIC_EXCEPTION_SysTick = -1,               /*!< 15 Cortex-M3 system tick interrupt                            */
  /* interruput numbers */
  HDL_NVIC_IRQ0_WWDG                 = 0,        /*!< Window WatchDog Interrupt                                     */
  HDL_NVIC_IRQ1_PVD                  = 1,        /*!< PVD through EXTI Line detect Interrupt                        */
  HDL_NVIC_IRQ2_RTC                  = 2,        /*!< RTC through EXTI Line Interrupt                               */
  HDL_NVIC_IRQ3_FLASH                = 3,        /*!< FLASH Interrupt                                               */
  HDL_NVIC_IRQ4_RCC                  = 4,        /*!< RCC Interrupt                                                 */
  HDL_NVIC_IRQ5_EXTI0_1              = 5,        /*!< EXTI Line 0 and 1 Interrupts                                  */
  HDL_NVIC_IRQ6_EXTI2_3              = 6,        /*!< EXTI Line 2 and 3 Interrupts                                  */
  HDL_NVIC_IRQ7_EXTI4_15             = 7,        /*!< EXTI Line 4 to 15 Interrupts                                  */
  HDL_NVIC_IRQ8                      = 8,        /*!<                                                               */
  HDL_NVIC_IRQ9_DMA1_Channel1        = 9,        /*!< DMA1 Channel 1 Interrupt                                      */
  HDL_NVIC_IRQ0_DMA1_Channel2_3      = 10,       /*!< DMA1 Channel 2 and Channel 3 Interrupts                       */
  HDL_NVIC_IRQ11_DMA1_Channel4_5_6_7 = 11,       /*!< DMA1 Channel 4, Channel 5, Channel 6 and Channel 7 Interrupts */
  HDL_NVIC_IRQ12_ADC1_COMP           = 12,       /*!< ADC1, COMP1 and COMP2 Interrupts                              */
  HDL_NVIC_IRQ13_LPTIM1              = 13,       /*!< LPTIM1 Interrupt                                              */
  HDL_NVIC_IRQ14_USART4_5            = 14,       /*!< USART4 and USART5 Interrupt                                   */
  HDL_NVIC_IRQ15_TIM2                = 15,       /*!< TIM2 Interrupt                                                */
  HDL_NVIC_IRQ16_TIM3                = 16,       /*!< TIM3 Interrupt                                                */
  HDL_NVIC_IRQ17_TIM6                = 17,       /*!< TIM6  Interrupt                                               */
  HDL_NVIC_IRQ18_TIM7                = 18,       /*!< TIM7 Interrupt                                                */
  HDL_NVIC_IRQ19                     = 19,       /*!<                                                               */
  HDL_NVIC_IRQ20_TIM21               = 20,       /*!< TIM21 Interrupt                                               */
  HDL_NVIC_IRQ21_I2C3                = 21,       /*!< I2C3 Interrupt                                                */
  HDL_NVIC_IRQ22_TIM22               = 22,       /*!< TIM22 Interrupt                                               */
  HDL_NVIC_IRQ23_I2C1                = 23,       /*!< I2C1 Interrupt                                                */
  HDL_NVIC_IRQ24_I2C2                = 24,       /*!< I2C2 Interrupt                                                */
  HDL_NVIC_IRQ25_SPI1                = 25,       /*!< SPI1 Interrupt                                                */
  HDL_NVIC_IRQ26_SPI2                = 26,       /*!< SPI2 Interrupt                                                */
  HDL_NVIC_IRQ27_USART1              = 27,       /*!< USART1 Interrupt                                              */
  HDL_NVIC_IRQ28_USART2              = 28,       /*!< USART2 Interrupt                                              */
  HDL_NVIC_IRQ29_LPUART1             = 29,       /*!< LPUART1 Interrupt                                             */
  HDL_NVIC_IRQ30                     = 30,       /*!<                                                               */
  HDL_NVIC_IRQ31                     = 31,       /*!<                                                               */
} hdl_nvic_irq_n_t;

typedef struct {
  hdl_nvic_irq_n_t irq_type;
  uint8_t priority_group;
  uint8_t priority;
  PRIVATE(hw, HDL_INTERRUPT_PRV_SIZE);
} hdl_interrupt_t;

typedef struct {
  uint32_t prio_bits;
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

void reset_handler();
void irq_n_handler();
void nmi_handler();
void hard_fault_handler();
void svc_handler();
void pend_sv_handler();
void systick_handler();

void WWDG_IRQHandler();
void PVD_IRQHandler();
void RTC_IRQHandler();
void FLASH_IRQHandler();
void RCC_IRQHandler();
void EXTI0_1_IRQHandler();
void EXTI2_3_IRQHandler();
void EXTI4_15_IRQHandler();
void DMA1_Channel1_IRQHandler();
void DMA1_Channel2_3_IRQHandler();
void DMA1_Channel4_5_6_7_IRQHandler();
void ADC1_COMP_IRQHandler();
void LPTIM1_IRQHandler();
void USART4_5_IRQHandler();
void TIM2_IRQHandler();
void TIM3_IRQHandler();
void TIM6_IRQHandler();
void TIM7_IRQHandler();
void TIM21_IRQHandler();
void I2C3_IRQHandler();
void TIM22_IRQHandler();
void I2C1_IRQHandler();
void I2C2_IRQHandler();
void SPI1_IRQHandler();
void SPI2_IRQHandler();
void USART1_IRQHandler();
void USART2_IRQHandler();
void LPUART1_IRQHandler();

#endif // PORT_CORE_H_
