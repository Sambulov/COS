
#ifndef PORT_CORE_H_
#define PORT_CORE_H_

#include "port_exti.h"

#define HDL_INTERRUPT_PRV_SIZE       4

typedef enum
{
  /* Cortex-M4 processor exceptions numbers */
  HDL_NVIC_EXCEPTION_NonMaskableInt = -14,   /*!< 2 non maskable interrupt                                 */
  HDL_NVIC_EXCEPTION_HardFault = -13,        /*!< hardfault interrupt                                      */
  HDL_NVIC_EXCEPTION_MemoryManagement = -12, /*!< 4 Cortex-M3 memory management interrupt                  */
  HDL_NVIC_EXCEPTION_BusFault = -11,         /*!< 5 Cortex-M3 bus fault interrupt                          */
  HDL_NVIC_EXCEPTION_UsageFault = -10,       /*!< 6 Cortex-M3 usage fault interrupt                        */
  HDL_NVIC_EXCEPTION_SVCall = -5,            /*!< 11 Cortex-M3 SV call interrupt                           */
  HDL_NVIC_EXCEPTION_DebugMonitor = -4,      /*!< 12 Cortex-M3 debug monitor interrupt                     */
  HDL_NVIC_EXCEPTION_PendSV = -2,            /*!< 14 Cortex-M3 pend SV interrupt                           */
  HDL_NVIC_EXCEPTION_SysTick = -1,           /*!< 15 Cortex-M3 system tick interrupt                       */
  /* interruput numbers */
  HDL_NVIC_IRQ0_WWDGT = 0,                    /*!< window watchdog timer interrupt                          */
  HDL_NVIC_IRQ1_LVD = 1,                      /*!< LVD through EXTI line detect interrupt                   */
  HDL_NVIC_IRQ2_TAMPER_STAMP = 2,             /*!< tamper and timestamp through EXTI line detect            */
  HDL_NVIC_IRQ3_RTC_WKUP = 3,                 /*!< RTC wakeup through EXTI line interrupt                   */
  HDL_NVIC_IRQ4_FMC = 4,                      /*!< FMC interrupt                                            */
  HDL_NVIC_IRQ5_RCU_CTC = 5,                  /*!< RCU and CTC interrupt                                    */
  HDL_NVIC_IRQ6_EXTI0 = 6,                    /*!< EXTI line 0 interrupts                                   */
  HDL_NVIC_IRQ7_EXTI1 = 7,                    /*!< EXTI line 1 interrupts                                   */
  HDL_NVIC_IRQ8_EXTI2 = 8,                    /*!< EXTI line 2 interrupts                                   */
  HDL_NVIC_IRQ9_EXTI3 = 9,                    /*!< EXTI line 3 interrupts                                   */
  HDL_NVIC_IRQ10_EXTI4 = 10,                  /*!< EXTI line 4 interrupts                                   */
  HDL_NVIC_IRQ11_DMA0_Channel0 = 11,          /*!< DMA0 channel0 Interrupt                                  */
  HDL_NVIC_IRQ12_DMA0_Channel1 = 12,          /*!< DMA0 channel1 Interrupt                                  */
  HDL_NVIC_IRQ13_DMA0_Channel2 = 13,          /*!< DMA0 channel2 interrupt                                  */
  HDL_NVIC_IRQ14_DMA0_Channel3 = 14,          /*!< DMA0 channel3 interrupt                                  */
  HDL_NVIC_IRQ15_DMA0_Channel4 = 15,          /*!< DMA0 channel4 interrupt                                  */
  HDL_NVIC_IRQ16_DMA0_Channel5 = 16,          /*!< DMA0 channel5 interrupt                                  */
  HDL_NVIC_IRQ17_DMA0_Channel6 = 17,          /*!< DMA0 channel6 interrupt                                  */
  HDL_NVIC_IRQ18_ADC = 18,                    /*!< ADC interrupt                                            */
  HDL_NVIC_IRQ19_CAN0_TX = 19,                /*!< CAN0 TX interrupt                                        */
  HDL_NVIC_IRQ20_CAN0_RX0 = 20,               /*!< CAN0 RX0 interrupt                                       */
  HDL_NVIC_IRQ21_CAN0_RX1 = 21,               /*!< CAN0 RX1 interrupt                                       */
  HDL_NVIC_IRQ22_CAN0_EWMC = 22,              /*!< CAN0 EWMC interrupt                                      */
  HDL_NVIC_IRQ23_EXTI5_9 = 23,                /*!< EXTI[9:5] interrupts                                     */
  HDL_NVIC_IRQ24_TIMER0_BRK_TIMER8 = 24,      /*!< TIMER0 break and TIMER8 interrupts                       */
  HDL_NVIC_IRQ25_TIMER0_UP_TIMER9 = 25,       /*!< TIMER0 update and TIMER9 interrupts                      */
  HDL_NVIC_IRQ26_TIMER0_TRG_CMT_TIMER10 = 26, /*!< TIMER0 trigger and commutation  and TIMER10 interrupts   */
  HDL_NVIC_IRQ27_TIMER0_Channel = 27,         /*!< TIMER0 channel capture compare interrupt                 */
  HDL_NVIC_IRQ28_TIMER1 = 28,                 /*!< TIMER1 interrupt                                         */
  HDL_NVIC_IRQ29_TIMER2 = 29,                 /*!< TIMER2 interrupt                                         */
  HDL_NVIC_IRQ30_TIMER3 = 30,                 /*!< TIMER3 interrupts                                        */
  HDL_NVIC_IRQ31_I2C0_EV = 31,                /*!< I2C0 event interrupt                                     */
  HDL_NVIC_IRQ32_I2C0_ER = 32,                /*!< I2C0 error interrupt                                     */
  HDL_NVIC_IRQ33_I2C1_EV = 33,                /*!< I2C1 event interrupt                                     */
  HDL_NVIC_IRQ34_I2C1_ER = 34,                /*!< I2C1 error interrupt                                     */
  HDL_NVIC_IRQ35_SPI0 = 35,                   /*!< SPI0 interrupt                                           */
  HDL_NVIC_IRQ36_SPI1 = 36,                   /*!< SPI1 interrupt                                           */
  HDL_NVIC_IRQ37_USART0 = 37,                 /*!< USART0 interrupt                                         */
  HDL_NVIC_IRQ38_USART1 = 38,                 /*!< USART1 interrupt                                         */
  HDL_NVIC_IRQ39_USART2 = 39,                 /*!< USART2 interrupt                                         */
  HDL_NVIC_IRQ40_EXTI10_15 = 40,              /*!< EXTI[15:10] interrupts                                   */
  HDL_NVIC_IRQ41_RTC_Alarm = 41,              /*!< RTC alarm interrupt                                      */
  HDL_NVIC_IRQ42_USBFS_WKUP = 42,             /*!< USBFS wakeup interrupt                                   */
  HDL_NVIC_IRQ43_TIMER7_BRK_TIMER11 = 43,     /*!< TIMER7 break and TIMER11 interrupts                      */
  HDL_NVIC_IRQ44_TIMER7_UP_TIMER12 = 44,      /*!< TIMER7 update and TIMER12 interrupts                     */
  HDL_NVIC_IRQ45_TIMER7_TRG_CMT_TIMER13 = 45, /*!< TIMER7 trigger and commutation and TIMER13 interrupts    */
  HDL_NVIC_IRQ46_TIMER7_Channel = 46,         /*!< TIMER7 channel capture compare interrupt                 */
  HDL_NVIC_IRQ47_DMA0_Channel7 = 47,          /*!< DMA0 channel7 interrupt                                  */
#if defined(GD32F450) || defined(GD32F470)    /* CPU type */
  HDL_NVIC_IRQ48_EXMC = 48,                   /*!< EXMC interrupt                                           */
  HDL_NVIC_IRQ49_SDIO = 49,                   /*!< SDIO interrupt                                           */
  HDL_NVIC_IRQ50_TIMER4 = 50,                 /*!< TIMER4 interrupt                                         */
  HDL_NVIC_IRQ51_SPI2 = 51,                   /*!< SPI2 interrupt                                           */
  HDL_NVIC_IRQ52_UART3 = 52,                  /*!< UART3 interrupt                                          */
  HDL_NVIC_IRQ53_UART4 = 53,                  /*!< UART4 interrupt                                          */
  HDL_NVIC_IRQ54_TIMER5_DAC = 54,             /*!< TIMER5 and DAC0 DAC1 underrun error interrupts           */
  HDL_NVIC_IRQ55_TIMER6 = 55,                 /*!< TIMER6 interrupt                                         */
  HDL_NVIC_IRQ56_DMA1_Channel0 = 56,          /*!< DMA1 channel0 interrupt                                  */
  HDL_NVIC_IRQ57_DMA1_Channel1 = 57,          /*!< DMA1 channel1 interrupt                                  */
  HDL_NVIC_IRQ58_DMA1_Channel2 = 58,          /*!< DMA1 channel2 interrupt                                  */
  HDL_NVIC_IRQ59_DMA1_Channel3 = 59,          /*!< DMA1 channel3 interrupt                                  */
  HDL_NVIC_IRQ60_DMA1_Channel4 = 60,          /*!< DMA1 channel4 interrupt                                  */
  HDL_NVIC_IRQ61_ENET = 61,                   /*!< ENET interrupt                                           */
  HDL_NVIC_IRQ62_ENET_WKUP = 62,              /*!< ENET wakeup through EXTI line interrupt                  */
  HDL_NVIC_IRQ63_CAN1_TX = 63,                /*!< CAN1 TX interrupt                                        */
  HDL_NVIC_IRQ64_CAN1_RX0 = 64,               /*!< CAN1 RX0 interrupt                                       */
  HDL_NVIC_IRQ65_CAN1_RX1 = 65,               /*!< CAN1 RX1 interrupt                                       */
  HDL_NVIC_IRQ66_CAN1_EWMC = 66,              /*!< CAN1 EWMC interrupt                                      */
  HDL_NVIC_IRQ67_USBFS = 67,                  /*!< USBFS interrupt                                          */
  HDL_NVIC_IRQ68_DMA1_Channel5 = 68,          /*!< DMA1 channel5 interrupt                                  */
  HDL_NVIC_IRQ69_DMA1_Channel6 = 69,          /*!< DMA1 channel6 interrupt                                  */
  HDL_NVIC_IRQ70_DMA1_Channel7 = 70,          /*!< DMA1 channel7 interrupt                                  */
  HDL_NVIC_IRQ71_USART5 = 71,                 /*!< USART5 interrupt                                         */
  HDL_NVIC_IRQ72_I2C2_EV = 72,                /*!< I2C2 event interrupt                                     */
  HDL_NVIC_IRQ73_I2C2_ER = 73,                /*!< I2C2 error interrupt                                     */
  HDL_NVIC_IRQ74_USBHS_EP1_Out = 74,          /*!< USBHS endpoint 1 out interrupt                           */
  HDL_NVIC_IRQ75_USBHS_EP1_In = 75,           /*!< USBHS endpoint 1 in interrupt                            */
  HDL_NVIC_IRQ76_USBHS_WKUP = 76,             /*!< USBHS wakeup through EXTI line interrupt                 */
  HDL_NVIC_IRQ77_USBHS = 77,                  /*!< USBHS interrupt                                          */
  HDL_NVIC_IRQ78_DCI = 78,                    /*!< DCI interrupt                                            */
  HDL_NVIC_IRQ79 = 79,                        /* */
  HDL_NVIC_IRQ80_TRNG = 80,                   /*!< TRNG interrupt                                           */
  HDL_NVIC_IRQ81_FPU = 81,                    /*!< FPU interrupt                                            */
  HDL_NVIC_IRQ82_UART6 = 82,                  /*!< UART6 interrupt                                          */
  HDL_NVIC_IRQ83_UART7 = 83,                  /*!< UART7 interrupt                                          */
  HDL_NVIC_IRQ84_SPI3 = 84,                   /*!< SPI3 interrupt                                           */
  HDL_NVIC_IRQ85_SPI4 = 85,                   /*!< SPI4 interrupt                                           */
  HDL_NVIC_IRQ86_SPI5 = 86,                   /*!< SPI5 interrupt                                           */
  HDL_NVIC_IRQ87 = 87,                        /* */
  HDL_NVIC_IRQ88_TLI = 88,                    /*!< TLI interrupt                                            */
  HDL_NVIC_IRQ89_TLI_ER = 89,                 /*!< TLI error interrupt                                      */
  HDL_NVIC_IRQ90_IPA = 90,                    /*!< IPA interrupt                                            */
#endif                                        /* GD32F450 and GD32F470 */
#if defined(GD32F405) || defined(GD32F425)    /* CPU type */
  HDL_NVIC_IRQ48 = 48,                        /* */
  HDL_NVIC_IRQ49_SDIO = 49,                   /*!< SDIO interrupt                                           */
  HDL_NVIC_IRQ50_TIMER4 = 50,                 /*!< TIMER4 interrupt                                         */
  HDL_NVIC_IRQ51_SPI2 = 51,                   /*!< SPI2 interrupt                                           */
  HDL_NVIC_IRQ52_UART3 = 52,                  /*!< UART3 interrupt                                          */
  HDL_NVIC_IRQ53_UART4 = 53,                  /*!< UART4 interrupt                                          */
  HDL_NVIC_IRQ54_TIMER5_DAC = 54,             /*!< TIMER5 and DAC0 DAC1 underrun error interrupts           */
  HDL_NVIC_IRQ55_TIMER6 = 55,                 /*!< TIMER6 interrupt                                         */
  HDL_NVIC_IRQ56_DMA1_Channel0 = 56,          /*!< DMA1 channel0 interrupt                                  */
  HDL_NVIC_IRQ57_DMA1_Channel1 = 57,          /*!< DMA1 channel1 interrupt                                  */
  HDL_NVIC_IRQ58_DMA1_Channel2 = 58,          /*!< DMA1 channel2 interrupt                                  */
  HDL_NVIC_IRQ59_DMA1_Channel3 = 59,          /*!< DMA1 channel3 interrupt                                  */
  HDL_NVIC_IRQ60_DMA1_Channel4 = 60,          /*!< DMA1 channel4 interrupt                                  */
  HDL_NVIC_IRQ61 = 61,                        /* */
  HDL_NVIC_IRQ62 = 61,                        /* */
  HDL_NVIC_IRQ63_CAN1_TX = 63,                /*!< CAN1 TX interrupt                                        */
  HDL_NVIC_IRQ64_CAN1_RX0 = 64,               /*!< CAN1 RX0 interrupt                                       */
  HDL_NVIC_IRQ65_CAN1_RX1 = 65,               /*!< CAN1 RX1 interrupt                                       */
  HDL_NVIC_IRQ66_CAN1_EWMC = 66,              /*!< CAN1 EWMC interrupt                                      */
  HDL_NVIC_IRQ67_USBFS = 67,                  /*!< USBFS interrupt                                          */
  HDL_NVIC_IRQ68_DMA1_Channel5 = 68,          /*!< DMA1 channel5 interrupt                                  */
  HDL_NVIC_IRQ69_DMA1_Channel6 = 69,          /*!< DMA1 channel6 interrupt                                  */
  HDL_NVIC_IRQ70_DMA1_Channel7 = 70,          /*!< DMA1 channel7 interrupt                                  */
  HDL_NVIC_IRQ71_USART5 = 71,                 /*!< USART5 interrupt                                         */
  HDL_NVIC_IRQ72_I2C2_EV = 72,                /*!< I2C2 event interrupt                                     */
  HDL_NVIC_IRQ73_I2C2_ER = 73,                /*!< I2C2 error interrupt                                     */
  HDL_NVIC_IRQ74_USBHS_EP1_Out = 74,          /*!< USBHS endpoint 1 Out interrupt                           */
  HDL_NVIC_IRQ75_USBHS_EP1_In = 75,           /*!< USBHS endpoint 1 in interrupt                            */
  HDL_NVIC_IRQ76_USBHS_WKUP = 76,             /*!< USBHS wakeup through EXTI line interrupt                 */
  HDL_NVIC_IRQ77_USBHS = 77,                  /*!< USBHS interrupt                                          */
  HDL_NVIC_IRQ78_DCI = 78,                    /*!< DCI interrupt                                            */
  HDL_NVIC_IRQ79 = 79                         /* */
  HDL_NVIC_IRQ80_TRNG = 80,                   /*!< TRNG interrupt                                           */
  HDL_NVIC_IRQ81_FPU = 81,                    /*!< FPU interrupt                                            */
  HDL_NVIC_IRQ82 = 82,                        /* */
  HDL_NVIC_IRQ83 = 83,                        /* */
  HDL_NVIC_IRQ84 = 84,                        /* */
  HDL_NVIC_IRQ85 = 85,                        /* */
  HDL_NVIC_IRQ86 = 86,                        /* */
  HDL_NVIC_IRQ87 = 87,                        /* */
  HDL_NVIC_IRQ88 = 88,                        /* */
  HDL_NVIC_IRQ89 = 89,                        /* */
  HDL_NVIC_IRQ90 = 90,                        /* */
#endif                                        /* GD32F405 and GD32F425 */
#if defined(GD32F407) || defined(GD32F427)    /* CPU type */
  HDL_NVIC_IRQ48_EXMC = 48,                   /*!< EXMC interrupt                                           */
  HDL_NVIC_IRQ49_SDIO = 49,                   /*!< SDIO interrupt                                           */
  HDL_NVIC_IRQ50_TIMER4 = 50,                 /*!< TIMER4 interrupt                                         */
  HDL_NVIC_IRQ51_SPI2 = 51,                   /*!< SPI2 interrupt                                           */
  HDL_NVIC_IRQ52_UART3 = 52,                  /*!< UART3 interrupt                                          */
  HDL_NVIC_IRQ53_UART4 = 53,                  /*!< UART4 interrupt                                          */
  HDL_NVIC_IRQ54_TIMER5_DAC = 54,             /*!< TIMER5 and DAC0 DAC1 underrun error interrupts           */
  HDL_NVIC_IRQ55_TIMER6 = 55,                 /*!< TIMER6 interrupt                                         */
  HDL_NVIC_IRQ56_DMA1_Channel0 = 56,          /*!< DMA1 channel0 interrupt                                  */
  HDL_NVIC_IRQ57_DMA1_Channel1 = 57,          /*!< DMA1 channel1 interrupt                                  */
  HDL_NVIC_IRQ58_DMA1_Channel2 = 58,          /*!< DMA1 channel2 interrupt                                  */
  HDL_NVIC_IRQ59_DMA1_Channel3 = 59,          /*!< DMA1 channel3 interrupt                                  */
  HDL_NVIC_IRQ60_DMA1_Channel4 = 60,          /*!< DMA1 channel4 interrupt                                  */
  HDL_NVIC_IRQ61_ENET = 61,                   /*!< ENET interrupt                                           */
  HDL_NVIC_IRQ62_ENET_WKUP = 62,              /*!< ENET wakeup through EXTI line interrupt                  */
  HDL_NVIC_IRQ63_CAN1_TX = 63,                /*!< CAN1 TX interrupt                                        */
  HDL_NVIC_IRQ64_CAN1_RX0 = 64,               /*!< CAN1 RX0 interrupt                                       */
  HDL_NVIC_IRQ65_CAN1_RX1 = 65,               /*!< CAN1 RX1 interrupt                                       */
  HDL_NVIC_IRQ66_CAN1_EWMC = 66,              /*!< CAN1 EWMC interrupt                                      */
  HDL_NVIC_IRQ67_USBFS = 67,                  /*!< USBFS interrupt                                          */
  HDL_NVIC_IRQ68_DMA1_Channel5 = 68,          /*!< DMA1 channel5 interrupt                                  */
  HDL_NVIC_IRQ69_DMA1_Channel6 = 69,          /*!< DMA1 channel6 interrupt                                  */
  HDL_NVIC_IRQ70_DMA1_Channel7 = 70,          /*!< DMA1 channel7 interrupt                                  */
  HDL_NVIC_IRQ71_USART5 = 71,                 /*!< USART5 interrupt                                         */
  HDL_NVIC_IRQ72_I2C2_EV = 72,                /*!< I2C2 event interrupt                                     */
  HDL_NVIC_IRQ73_I2C2_ER = 73,                /*!< I2C2 error interrupt                                     */
  HDL_NVIC_IRQ74_USBHS_EP1_Out = 74,          /*!< USBHS endpoint 1 out interrupt                           */
  HDL_NVIC_IRQ75_USBHS_EP1_In = 75,           /*!< USBHS endpoint 1 in interrupt                            */
  HDL_NVIC_IRQ76_USBHS_WKUP = 76,             /*!< USBHS wakeup through EXTI line interrupt                 */
  HDL_NVIC_IRQ77_USBHS = 77,                  /*!< USBHS interrupt                                          */
  HDL_NVIC_IRQ78_DCI = 78,                    /*!< DCI interrupt                                            */
  HDL_NVIC_IRQ79 = 79,                        /* */
  HDL_NVIC_IRQ80_TRNG = 80,                   /*!< TRNG interrupt                                           */
  HDL_NVIC_IRQ81_FPU = 81,                    /*!< FPU interrupt                                            */
  HDL_NVIC_IRQ82 = 82,                        /* */
  HDL_NVIC_IRQ83 = 83,                        /* */
  HDL_NVIC_IRQ84 = 84,                        /* */
  HDL_NVIC_IRQ85 = 85,                        /* */
  HDL_NVIC_IRQ86 = 86,                        /* */
  HDL_NVIC_IRQ87 = 87,                        /* */
  HDL_NVIC_IRQ88 = 88,                        /* */
  HDL_NVIC_IRQ89 = 89,                        /* */
  HDL_NVIC_IRQ90 = 90,                        /* */
#endif                                        /* GD32F407 and GD32F427 */
  HDL_NVIC_IRQ91 = 91,
  HDL_NVIC_IRQ92 = 92,
  HDL_NVIC_IRQ93 = 93,
  HDL_NVIC_IRQ94 = 94,
  HDL_NVIC_IRQ95 = 95,
} hdl_nvic_irq_n_t;

typedef struct {
  hdl_nvic_irq_n_t irq_type;
  uint8_t priority_group;
  uint8_t priority;
  PRIVATE(hw, HDL_INTERRUPT_PRV_SIZE);
} hdl_nvic_interrupt_t;

typedef struct {
  hdl_module_t module;
  uint32_t prio_bits;
  hdl_nvic_interrupt_t **interrupts;
  //uint8_t irq_latency; /* processor ensures that a minimum of irq_latency+1 hclk cycles exist between an interrupt becoming pended */
} hdl_nvic_t;

typedef struct{
  hdl_module_t module;
  uint32_t flash_latency;
} hdl_core_t;

#define hdl_interrupts(...) ((hdl_nvic_interrupt_t *[]){__VA_ARGS__, NULL})

typedef hdl_nvic_t hdl_interrupt_controller_t;
typedef hdl_nvic_interrupt_t hdl_interrupt_t;
typedef hdl_nvic_irq_n_t hdl_irq_n_t;

#endif // PORT_CORE_H_
