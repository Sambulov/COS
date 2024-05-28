
#ifndef PORT_CORE_H_
#define PORT_CORE_H_

#include  "Macros.h"

#define HDL_INTERRUPT_PRV_SIZE       4

typedef enum {
  HDL_NVIC_EXCEPTION_NMI               = NonMaskableInt_IRQn,        /*!< non maskable interrupt                                   */
  HDL_NVIC_EXCEPTION_HF                = HardFault_IRQn,             /*!< hardfault interrupt                                      */
  HDL_NVIC_EXCEPTION_SVC               = SVCall_IRQn,                /*!< sv call interrupt                                        */
  HDL_NVIC_EXCEPTION_PSV               = PendSV_IRQn,                /*!< pend sv interrupt                                        */
  HDL_NVIC_EXCEPTION_SysTick           = SysTick_IRQn,               /*!< system tick interrupt                                    */
  /* interruput numbers */
  HDL_NVIC_IRQ0_WWDGT                  = WWDGT_IRQn,                 /*!< window watchdog timer interrupt                          */
  HDL_NVIC_IRQ1_LVD                    = LVD_IRQn,                   /*!< LVD through EXTI line detect interrupt                   */
  HDL_NVIC_IRQ2_RTC                    = RTC_IRQn,                   /*!< RTC through EXTI line interrupt                          */
  HDL_NVIC_IRQ3_FMC                    = FMC_IRQn,                   /*!< FMC interrupt                                            */
  HDL_NVIC_IRQ4_RCU                    = RCU_IRQn,                   /*!< RCU interrupt                                            */
  HDL_NVIC_IRQ5_EXTI0_1                = EXTI0_1_IRQn,               /*!< EXTI line 0 and 1 interrupts                             */
  HDL_NVIC_IRQ6_EXTI2_3                = EXTI2_3_IRQn,               /*!< EXTI line 2 and 3 interrupts                             */
  HDL_NVIC_IRQ7_EXTI4_15               = EXTI4_15_IRQn,              /*!< EXTI line 4 to 15 interrupts                             */
  HDL_NVIC_IRQ8                        = 8,
  HDL_NVIC_IRQ9_DMA_CH0                = DMA_Channel0_IRQn,          /*!< DMA channel 0 interrupt                                  */
  HDL_NVIC_IRQ10_DMA_CH1_2             = DMA_Channel1_2_IRQn,        /*!< DMA channel 1 and channel 2 interrupts                   */
  HDL_NVIC_IRQ11_DMA_CH3_4             = DMA_Channel3_4_IRQn,        /*!< DMA channel 3 and channel 4 interrupts                   */
  HDL_NVIC_IRQ12_ADC_CMP               = ADC_CMP_IRQn,               /*!< ADC, CMP interrupts                                      */
  HDL_NVIC_IRQ13_TIMER0_BRK_UP_TRG_COM = TIMER0_BRK_UP_TRG_COM_IRQn, /*!< TIMER0 break, update, trigger and commutation interrupts */
  HDL_NVIC_IRQ14_TIMER0_CH             = TIMER0_Channel_IRQn,        /*!< TIMER0 channel capture compare interrupts                */
  HDL_NVIC_IRQ15                       = 15,
  HDL_NVIC_IRQ16_TIMER2                = TIMER2_IRQn,                /*!< TIMER2 interrupt                                         */
  HDL_NVIC_IRQ17_TIMER5                = TIMER5_IRQn,                /*!< TIMER5 interrupt                                         */
  HDL_NVIC_IRQ18                       = 18,
  HDL_NVIC_IRQ19_TIMER13               = TIMER13_IRQn,               /*!< TIMER13 interrupt                                        */
  HDL_NVIC_IRQ20_TIMER14               = TIMER14_IRQn,               /*!< TIMER14 interrupt                                        */
  HDL_NVIC_IRQ21_TIMER15               = TIMER15_IRQn,               /*!< TIMER15 interrupt                                        */
  HDL_NVIC_IRQ22_TIMER16               = TIMER16_IRQn,               /*!< TIMER16 interrupt                                        */
  HDL_NVIC_IRQ23_I2C0_EV               = I2C0_EV_IRQn,               /*!< I2C0 event interrupt                                     */
  HDL_NVIC_IRQ24_I2C1_EV               = I2C1_EV_IRQn,               /*!< I2C1 event interrupt                                     */
  HDL_NVIC_IRQ25_SPI0                  = SPI0_IRQn,                  /*!< SPI0 interrupt                                           */
  HDL_NVIC_IRQ26_SPI1                  = SPI1_IRQn,                  /*!< SPI1 interrupt                                           */
  HDL_NVIC_IRQ27_USART0                = USART0_IRQn,                /*!< USART0 interrupt                                         */
  HDL_NVIC_IRQ28_USART1                = USART1_IRQn,                /*!< USART1 interrupt                                         */
  HDL_NVIC_IRQ29                       = 29,
  HDL_NVIC_IRQ30                       = 30,
  HDL_NVIC_IRQ31                       = 31,
  HDL_NVIC_IRQ32_I2C0_ER               = I2C0_ER_IRQn,               /*!< I2C0 error interrupt                                     */
  HDL_NVIC_IRQ33                       = 33,
  HDL_NVIC_IRQ34_I2C1_ER               = I2C1_ER_IRQn,               /*!< I2C1 error interrupt                                     */
  HDL_NVIC_IRQ35                       = 35,
  HDL_NVIC_IRQ36                       = 36,
  HDL_NVIC_IRQ37                       = 37,
  HDL_NVIC_IRQ38                       = 38,
  HDL_NVIC_IRQ39                       = 39,
  HDL_NVIC_IRQ40                       = 40,
  HDL_NVIC_IRQ41                       = 41,
  HDL_NVIC_IRQ42                       = 42,
  HDL_NVIC_IRQ43                       = 43,
  HDL_NVIC_IRQ44                       = 44,
  HDL_NVIC_IRQ45                       = 45,
  HDL_NVIC_IRQ46                       = 46,
  HDL_NVIC_IRQ47                       = 47,
  HDL_NVIC_IRQ48                       = 48,
} hdl_nvic_irq_n_t;

typedef struct {
  hdl_nvic_irq_n_t irq_type;
  uint8_t priority_group;
  uint8_t priority;
  PRIVATE(HDL_INTERRUPT_PRV_SIZE);
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
  HDL_EXTI_LINE_20 = EXTI_20, /*!< EXTI line 20 Reserved */    
  HDL_EXTI_LINE_21 = EXTI_21, /*!< EXTI line 21 CMP output */
  HDL_EXTI_LINE_22 = EXTI_22, /*!< EXTI line 22 Reserved */
  HDL_EXTI_LINE_23 = EXTI_23, /*!< EXTI line 23 Reserved */
  HDL_EXTI_LINE_24 = EXTI_24, /*!< EXTI line 24 Reserved */
  HDL_EXTI_LINE_25 = EXTI_25, /*!< EXTI line 25 USART0 wakeup */
  HDL_EXTI_LINE_26 = EXTI_26, /*!< EXTI line 26 Reserved */
  HDL_EXTI_LINE_27 = EXTI_27, /*!< EXTI line 27 Reserved */
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
  uint8_t irq_latency; /* processor ensures that a minimum of irq_latency+1 hclk cycles exist between an interrupt becoming pended */
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

#endif // PORT_CORE_H_
