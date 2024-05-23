#include "hdl_portable.h"
#include "Macros.h"

typedef struct {
  hdl_nvic_irq_n_t irq_type;
  uint8_t priority_group;
  uint8_t priority;
  void *handler_context;
  event_handler_t handler;
} hdl_nvic_interrupt_private_t;

#define hdl_exti_clear_pending(exti_line)       (EXTI_PD |= exti_line)

#define EXTI_LINES_ALL_POSSIBLE   EXTI_0  | EXTI_1  | EXTI_2  |  EXTI_3 | \
                                  EXTI_4  | EXTI_5  | EXTI_6  |  EXTI_7 | \
                                  EXTI_8  | EXTI_9  | EXTI_10 | EXTI_11 | \
                                  EXTI_12 | EXTI_13 | EXTI_14 | EXTI_15 | \
                                  EXTI_16 | EXTI_17 | EXTI_19

#define EXTI_LINES_5_9                      EXTI_5  | EXTI_6  | EXTI_7  | \
                                  EXTI_8  | EXTI_9

#define EXTI_LINES_10_15                              EXTI_10 | EXTI_11 | \
                                  EXTI_12 | EXTI_13 | EXTI_14 | EXTI_15

_Static_assert(sizeof(hdl_nvic_interrupt_private_t) == sizeof(hdl_nvic_interrupt_t), "In hdl_core.h data structure size of hdl_nvic_interrupt_t doesn't match, check HDL_INTERRUPT_PRV_SIZE");

extern void *__stack_end__;
extern void *_sidata, *_sdata, *_edata;
extern void *_sbss, *_ebss;

//void SystemInit();
void __libc_init_array();
void main();
hdl_nvic_t *__ic = NULL;

static void _call_isr(IRQn_Type irq, hdl_nvic_interrupt_t **isrs, uint32_t event) {
  if(isrs != NULL) {
    while (*isrs != NULL) {
      if((*isrs)->irq_type == irq) {
        ((hdl_nvic_interrupt_private_t *)*isrs)->handler(event, __ic, ((hdl_nvic_interrupt_private_t *)*isrs)->handler_context);
        return;
      }
      isrs++;
    }
  }
  //If you get stuck here, your code is missing a handler for some interrupt.
	asm("bkpt 255");
  for(;;) ;
}

void irq_n_handler();

void Reset_Handler();
void NMI_Handler()                      { _call_isr(HDL_NVIC_EXCEPTION_NonMaskableInt, __ic->interrupts, 0); }
void HardFault_Handler()                { _call_isr(HDL_NVIC_EXCEPTION_HardFault, __ic->interrupts, 0); }
void MemManage_Handler()                { _call_isr(HDL_NVIC_EXCEPTION_MemoryManagement, __ic->interrupts, 0); } /* MPU Fault Handler */
void BusFault_Handler()                 { _call_isr(HDL_NVIC_EXCEPTION_BusFault, __ic->interrupts, 0); }
void UsageFault_Handler()               { _call_isr(HDL_NVIC_EXCEPTION_UsageFault, __ic->interrupts, 0); }
void SVC_Handler();                     /* HDL_NVIC_EXCEPTION_SVCall */
void DebugMon_Handler()                 { _call_isr(HDL_NVIC_EXCEPTION_DebugMonitor, __ic->interrupts, 0); }
void PendSV_Handler()                   { _call_isr(HDL_NVIC_EXCEPTION_PendSV, __ic->interrupts, 0); }
void SysTick_Handler()                  { _call_isr(HDL_NVIC_EXCEPTION_SysTick, __ic->interrupts, 0); }


void WWDGT_IRQHandler()  { _call_isr(HDL_NVIC_IRQ0_WWDGT, __ic->interrupts, 0); } 
void LVD_IRQHandler()  { _call_isr(HDL_NVIC_IRQ1_LVD, __ic->interrupts, 0); }    
void TAMPER_STAMP_IRQHandler()  { _call_isr(HDL_NVIC_IRQ2_TAMPER_STAMP, __ic->interrupts, 0); }
void RTC_WKUP_IRQHandler()  { _call_isr( HDL_NVIC_IRQ3_RTC_WKUP, __ic->interrupts, 0); }
void FMC_IRQHandler()  { _call_isr( HDL_NVIC_IRQ4_FMC, __ic->interrupts, 0); }
void RCU_CTC_IRQHandler()  { _call_isr( HDL_NVIC_IRQ5_RCU_CTC, __ic->interrupts, 0); }
void EXTI0_IRQHandler()  { _call_isr( HDL_NVIC_IRQ6_EXTI0, __ic->interrupts, 0); hdl_exti_clear_pending(EXTI_0); }
void EXTI1_IRQHandler()  { _call_isr( HDL_NVIC_IRQ7_EXTI1, __ic->interrupts, 0); hdl_exti_clear_pending(EXTI_1); }
void EXTI2_IRQHandler()  { _call_isr( HDL_NVIC_IRQ8_EXTI2, __ic->interrupts, 0); hdl_exti_clear_pending(EXTI_2); }
void EXTI3_IRQHandler()  { _call_isr( HDL_NVIC_IRQ9_EXTI3, __ic->interrupts, 0); hdl_exti_clear_pending(EXTI_3); }
void EXTI4_IRQHandler()  { _call_isr( HDL_NVIC_IRQ10_EXTI4, __ic->interrupts, 0); hdl_exti_clear_pending(EXTI_4); }
void DMA0_Channel0_IRQHandler()  { _call_isr( HDL_NVIC_IRQ11_DMA0_Channel0, __ic->interrupts, 0); }
void DMA0_Channel1_IRQHandler()  { _call_isr( HDL_NVIC_IRQ12_DMA0_Channel1, __ic->interrupts, 0); }
void DMA0_Channel2_IRQHandler()  { _call_isr( HDL_NVIC_IRQ13_DMA0_Channel2, __ic->interrupts, 0); }
void DMA0_Channel3_IRQHandler()  { _call_isr( HDL_NVIC_IRQ14_DMA0_Channel3, __ic->interrupts, 0); }
void DMA0_Channel4_IRQHandler()  { _call_isr( HDL_NVIC_IRQ15_DMA0_Channel4, __ic->interrupts, 0); }
void DMA0_Channel5_IRQHandler()  { _call_isr( HDL_NVIC_IRQ16_DMA0_Channel5, __ic->interrupts, 0); }
void DMA0_Channel6_IRQHandler()  { _call_isr( HDL_NVIC_IRQ17_DMA0_Channel6, __ic->interrupts, 0); }
void ADC_IRQHandler()  { _call_isr( HDL_NVIC_IRQ18_ADC, __ic->interrupts, 0); }
void CAN0_TX_IRQHandler()  { _call_isr( HDL_NVIC_IRQ19_CAN0_TX, __ic->interrupts, 0); }
void CAN0_RX0_IRQHandler()  { _call_isr( HDL_NVIC_IRQ20_CAN0_RX0, __ic->interrupts, 0); }
void CAN0_RX1_IRQHandler()  { _call_isr( HDL_NVIC_IRQ21_CAN0_RX1, __ic->interrupts, 0); }
void CAN0_EWMC_IRQHandler()  { _call_isr( HDL_NVIC_IRQ22_CAN0_EWMC, __ic->interrupts, 0); }
void EXTI5_9_IRQHandler()  { _call_isr( HDL_NVIC_IRQ23_EXTI5_9, __ic->interrupts, 0); hdl_exti_clear_pending(EXTI_LINES_5_9); }
void TIMER0_BRK_TIMER8_IRQHandler()  { _call_isr( HDL_NVIC_IRQ24_TIMER0_BRK_TIMER8, __ic->interrupts, 0); }
void TIMER0_UP_TIMER9_IRQHandler() {
  _call_isr(HDL_NVIC_IRQ25_TIMER0_UP_TIMER9, __ic->interrupts, 0);
  TIMER_INTF(TIMER0) &= ~TIMER_INTF_UPIF;
  TIMER_INTF(TIMER9) &= ~TIMER_INTF_UPIF;
}
void TIMER0_TRG_CMT_TIMER10_IRQHandler()  { _call_isr( HDL_NVIC_IRQ26_TIMER0_TRG_CMT_TIMER10, __ic->interrupts, 0); }
void TIMER0_Channel_IRQHandler()  { _call_isr( HDL_NVIC_IRQ27_TIMER0_Channel, __ic->interrupts, 0); }
void TIMER1_IRQHandler(){
  _call_isr(HDL_NVIC_IRQ28_TIMER1, __ic->interrupts, 0);
  TIMER_INTF(TIMER1) &= ~TIMER_INTF_UPIF;
}
void TIMER2_IRQHandler()  { _call_isr( HDL_NVIC_IRQ29_TIMER2, __ic->interrupts, 0); }
void TIMER3_IRQHandler()  { _call_isr( HDL_NVIC_IRQ30_TIMER3, __ic->interrupts, 0); }
void I2C0_EV_IRQHandler()  { _call_isr( HDL_NVIC_IRQ31_I2C0_EV, __ic->interrupts, 0); }
void I2C0_ER_IRQHandler()  { _call_isr( HDL_NVIC_IRQ32_I2C0_ER, __ic->interrupts, 0); }
void I2C1_EV_IRQHandler()  { _call_isr( HDL_NVIC_IRQ33_I2C1_EV, __ic->interrupts, 0); }
void I2C1_ER_IRQHandler()  { _call_isr( HDL_NVIC_IRQ34_I2C1_ER, __ic->interrupts, 0); }
void SPI0_IRQHandler()  { _call_isr( HDL_NVIC_IRQ35_SPI0, __ic->interrupts, 0); }
void SPI1_IRQHandler()  { _call_isr( HDL_NVIC_IRQ36_SPI1, __ic->interrupts, 0); }
void USART0_IRQHandler()  { _call_isr( HDL_NVIC_IRQ37_USART0, __ic->interrupts, 0); }
void USART1_IRQHandler()  { _call_isr( HDL_NVIC_IRQ38_USART1, __ic->interrupts, 0); }
void USART2_IRQHandler()  { _call_isr( HDL_NVIC_IRQ39_USART2, __ic->interrupts, 0); }
void EXTI10_15_IRQHandler()  { _call_isr( HDL_NVIC_IRQ40_EXTI10_15, __ic->interrupts, 0); hdl_exti_clear_pending(EXTI_LINES_10_15); }
void RTC_Alarm_IRQHandler()  { _call_isr( HDL_NVIC_IRQ41_RTC_Alarm, __ic->interrupts, 0); }
void USBFS_WKUP_IRQHandler()  { _call_isr( HDL_NVIC_IRQ42_USBFS_WKUP, __ic->interrupts, 0); }
void TIMER7_BRK_TIMER11_IRQHandler()  { _call_isr( HDL_NVIC_IRQ43_TIMER7_BRK_TIMER11, __ic->interrupts, 0); }
void TIMER7_UP_TIMER12_IRQHandler()  { _call_isr( HDL_NVIC_IRQ44_TIMER7_UP_TIMER12, __ic->interrupts, 0); }
void TIMER7_TRG_CMT_TIMER13_IRQHandler()  { _call_isr( HDL_NVIC_IRQ45_TIMER7_TRG_CMT_TIMER13, __ic->interrupts, 0); }
void TIMER7_Channel_IRQHandler()  { _call_isr( HDL_NVIC_IRQ46_TIMER7_Channel, __ic->interrupts, 0); }
void DMA0_Channel7_IRQHandler()  { _call_isr( HDL_NVIC_IRQ47_DMA0_Channel7, __ic->interrupts, 0); }
#if defined (GD32F450) || defined (GD32F470)
void EXMC_IRQHandler()  { _call_isr( HDL_NVIC_IRQ48_EXMC, __ic->interrupts, 0); }
void SDIO_IRQHandler()  { _call_isr( HDL_NVIC_IRQ49_SDIO, __ic->interrupts, 0); }
void TIMER4_IRQHandler()  { _call_isr( HDL_NVIC_IRQ50_TIMER4, __ic->interrupts, 0); }
void SPI2_IRQHandler()  { _call_isr( HDL_NVIC_IRQ51_SPI2, __ic->interrupts, 0); }
void UART3_IRQHandler()  { _call_isr( HDL_NVIC_IRQ52_UART3, __ic->interrupts, 0); }
void UART4_IRQHandler()  { _call_isr( HDL_NVIC_IRQ53_UART4, __ic->interrupts, 0); }
void TIMER5_DAC_IRQHandler()  { _call_isr( HDL_NVIC_IRQ54_TIMER5_DAC, __ic->interrupts, 0); }
void TIMER6_IRQHandler()  { _call_isr( HDL_NVIC_IRQ55_TIMER6, __ic->interrupts, 0); }
void DMA1_Channel0_IRQHandler()  { _call_isr( HDL_NVIC_IRQ56_DMA1_Channel0, __ic->interrupts, 0); }
void DMA1_Channel1_IRQHandler()  { _call_isr( HDL_NVIC_IRQ57_DMA1_Channel1, __ic->interrupts, 0); }
void DMA1_Channel2_IRQHandler()  { _call_isr( HDL_NVIC_IRQ58_DMA1_Channel2, __ic->interrupts, 0); }
void DMA1_Channel3_IRQHandler()  { _call_isr( HDL_NVIC_IRQ59_DMA1_Channel3, __ic->interrupts, 0); }
void DMA1_Channel4_IRQHandler()  { _call_isr( HDL_NVIC_IRQ60_DMA1_Channel4, __ic->interrupts, 0); }
void ENET_IRQHandler()  { _call_isr( HDL_NVIC_IRQ61_ENET, __ic->interrupts, 0); }
void ENET_WKUP_IRQHandler()  { _call_isr( HDL_NVIC_IRQ62_ENET_WKUP, __ic->interrupts, 0); }
void CAN1_TX_IRQHandler()  { _call_isr( HDL_NVIC_IRQ63_CAN1_TX, __ic->interrupts, 0); }
void CAN1_RX0_IRQHandler()  { _call_isr( HDL_NVIC_IRQ64_CAN1_RX0, __ic->interrupts, 0); }
void CAN1_RX1_IRQHandler()  { _call_isr( HDL_NVIC_IRQ65_CAN1_RX1, __ic->interrupts, 0); }
void CAN1_EWMC_IRQHandler()  { _call_isr( HDL_NVIC_IRQ66_CAN1_EWMC, __ic->interrupts, 0); }
void USBFS_IRQHandler()  { _call_isr( HDL_NVIC_IRQ67_USBFS, __ic->interrupts, 0); }
void DMA1_Channel5_IRQHandler()  { _call_isr( HDL_NVIC_IRQ68_DMA1_Channel5, __ic->interrupts, 0); }
void DMA1_Channel6_IRQHandler()  { _call_isr( HDL_NVIC_IRQ69_DMA1_Channel6, __ic->interrupts, 0); }
void DMA1_Channel7_IRQHandler()  { _call_isr( HDL_NVIC_IRQ70_DMA1_Channel7, __ic->interrupts, 0); }
void USART5_IRQHandler()  { _call_isr( HDL_NVIC_IRQ71_USART5, __ic->interrupts, 0); }
void I2C2_EV_IRQHandler()  { _call_isr( HDL_NVIC_IRQ72_I2C2_EV, __ic->interrupts, 0); }
void I2C2_ER_IRQHandler()  { _call_isr( HDL_NVIC_IRQ73_I2C2_ER, __ic->interrupts, 0); }
void USBHS_EP1_Out_IRQHandler()  { _call_isr( HDL_NVIC_IRQ74_USBHS_EP1_Out, __ic->interrupts, 0); }
void USBHS_EP1_In_IRQHandler()  { _call_isr( HDL_NVIC_IRQ75_USBHS_EP1_In, __ic->interrupts, 0); }
void USBHS_WKUP_IRQHandler()  { _call_isr( HDL_NVIC_IRQ76_USBHS_WKUP, __ic->interrupts, 0); }
void USBHS_IRQHandler()  { _call_isr( HDL_NVIC_IRQ77_USBHS, __ic->interrupts, 0); }
void DCI_IRQHandler()  { _call_isr( HDL_NVIC_IRQ78_DCI, __ic->interrupts, 0); }
void TRNG_IRQHandler()  { _call_isr( HDL_NVIC_IRQ80_TRNG, __ic->interrupts, 0); }
void FPU_IRQHandler()  { _call_isr( HDL_NVIC_IRQ81_FPU, __ic->interrupts, 0); }
void UART6_IRQHandler()  { _call_isr( HDL_NVIC_IRQ82_UART6, __ic->interrupts, 0); }
void UART7_IRQHandler()  { _call_isr(HDL_NVIC_IRQ83_UART7 , __ic->interrupts, 0); }
void SPI3_IRQHandler()  { _call_isr( HDL_NVIC_IRQ84_SPI3, __ic->interrupts, 0); }
void SPI4_IRQHandler()  { _call_isr( HDL_NVIC_IRQ85_SPI4, __ic->interrupts, 0); }
void SPI5_IRQHandler()  { _call_isr( HDL_NVIC_IRQ86_SPI5, __ic->interrupts, 0); }
void TLI_IRQHandler()  { _call_isr( HDL_NVIC_IRQ88_TLI, __ic->interrupts, 0); }
void TLI_ER_IRQHandler()  { _call_isr( HDL_NVIC_IRQ89_TLI_ER, __ic->interrupts, 0); }
void IPA_IRQHandler()  { _call_isr( HDL_NVIC_IRQ90_IPA, __ic->interrupts, 0); }
#endif /* GD32F450_470 */
#if defined (GD32F405) || defined (GD32F425)
void SDIO_IRQHandler()  { _call_isr( HDL_NVIC_IRQ49_SDIO, __ic->interrupts, 0); }
void TIMER4_IRQHandler()  { _call_isr( HDL_NVIC_IRQ50_TIMER4, __ic->interrupts, 0); }
void SPI2_IRQHandler()  { _call_isr( HDL_NVIC_IRQ51_SPI2, __ic->interrupts, 0); }
void UART3_IRQHandler()  { _call_isr( HDL_NVIC_IRQ52_UART3, __ic->interrupts, 0); }
void UART4_IRQHandler()  { _call_isr( HDL_NVIC_IRQ53_UART4, __ic->interrupts, 0); }
void TIMER5_DAC_IRQHandler()  { _call_isr( HDL_NVIC_IRQ54_TIMER5_DAC, __ic->interrupts, 0); }
void TIMER6_IRQHandler()  { _call_isr( HDL_NVIC_IRQ55_TIMER6, __ic->interrupts, 0); }
void DMA1_Channel0_IRQHandler()  { _call_isr( HDL_NVIC_IRQ56_DMA1_Channel0, __ic->interrupts, 0); }
void DMA1_Channel1_IRQHandler()  { _call_isr( HDL_NVIC_IRQ57_DMA1_Channel1, __ic->interrupts, 0); }
void DMA1_Channel2_IRQHandler()  { _call_isr( HDL_NVIC_IRQ58_DMA1_Channel2, __ic->interrupts, 0); }
void DMA1_Channel3_IRQHandler()  { _call_isr( HDL_NVIC_IRQ59_DMA1_Channel3, __ic->interrupts, 0); }
void DMA1_Channel4_IRQHandler()  { _call_isr( HDL_NVIC_IRQ60_DMA1_Channel4, __ic->interrupts, 0); }
void CAN1_TX_IRQHandler()  { _call_isr( HDL_NVIC_IRQ63_CAN1_TX, __ic->interrupts, 0); }
void CAN1_RX0_IRQHandler()  { _call_isr( HDL_NVIC_IRQ64_CAN1_RX0, __ic->interrupts, 0); }
void CAN1_RX1_IRQHandler()  { _call_isr( HDL_NVIC_IRQ65_CAN1_RX1, __ic->interrupts, 0); }
void CAN1_EWMC_IRQHandler()  { _call_isr( HDL_NVIC_IRQ66_CAN1_EWMC, __ic->interrupts, 0); }
void USBFS_IRQHandler()  { _call_isr( HDL_NVIC_IRQ67_USBFS, __ic->interrupts, 0); }
void DMA1_Channel5_IRQHandler()  { _call_isr( HDL_NVIC_IRQ68_DMA1_Channel5, __ic->interrupts, 0); }
void DMA1_Channel6_IRQHandler()  { _call_isr( HDL_NVIC_IRQ69_DMA1_Channel6, __ic->interrupts, 0); }
void DMA1_Channel7_IRQHandler()  { _call_isr( HDL_NVIC_IRQ70_DMA1_Channel7, __ic->interrupts, 0); }
void USART5_IRQHandler()  { _call_isr( HDL_NVIC_IRQ71_USART5, __ic->interrupts, 0); }
void I2C2_EV_IRQHandler()  { _call_isr( HDL_NVIC_IRQ72_I2C2_EV, __ic->interrupts, 0); }
void I2C2_ER_IRQHandler()  { _call_isr( HDL_NVIC_IRQ73_I2C2_ER, __ic->interrupts, 0); }
void USBHS_EP1_Out_IRQHandler()  { _call_isr( HDL_NVIC_IRQ74_USBHS_EP1_Out, __ic->interrupts, 0); }
void USBHS_EP1_In_IRQHandler()  { _call_isr( HDL_NVIC_IRQ75_USBHS_EP1_In, __ic->interrupts, 0); }
void USBHS_WKUP_IRQHandler()  { _call_isr( HDL_NVIC_IRQ76_USBHS_WKUP, __ic->interrupts, 0); }
void USBHS_IRQHandler()  { _call_isr( HDL_NVIC_IRQ77_USBHS, __ic->interrupts, 0); }
void DCI_IRQHandler()  { _call_isr( HDL_NVIC_IRQ78_DCI, __ic->interrupts, 0); }
void TRNG_IRQHandler()  { _call_isr( HDL_NVIC_IRQ80_TRNG, __ic->interrupts, 0); }
void FPU_IRQHandler()  { _call_isr( HDL_NVIC_IRQ81_FPU, __ic->interrupts, 0); }
#endif /* GD32F405_425 */
#if defined (GD32F407) || defined (GD32F427)
void EXMC_IRQHandler()  { _call_isr( HDL_NVIC_IRQ48_EXMC, __ic->interrupts, 0); }
void SDIO_IRQHandler()  { _call_isr( HDL_NVIC_IRQ49_SDIO, __ic->interrupts, 0); }
void TIMER4_IRQHandler()  { _call_isr( HDL_NVIC_IRQ50_TIMER4, __ic->interrupts, 0); }
void SPI2_IRQHandler()  { _call_isr( HDL_NVIC_IRQ51_SPI2, __ic->interrupts, 0); }
void UART3_IRQHandler()  { _call_isr( HDL_NVIC_IRQ52_UART3, __ic->interrupts, 0); }
void UART4_IRQHandler()  { _call_isr( HDL_NVIC_IRQ53_UART4, __ic->interrupts, 0); }
void TIMER5_DAC_IRQHandler()  { _call_isr( HDL_NVIC_IRQ54_TIMER5_DAC, __ic->interrupts, 0); }
void TIMER6_IRQHandler()  { _call_isr( HDL_NVIC_IRQ55_TIMER6, __ic->interrupts, 0); }
void DMA1_Channel0_IRQHandler()  { _call_isr( HDL_NVIC_IRQ56_DMA1_Channel0, __ic->interrupts, 0); }
void DMA1_Channel1_IRQHandler()  { _call_isr( HDL_NVIC_IRQ57_DMA1_Channel1, __ic->interrupts, 0); }
void DMA1_Channel2_IRQHandler()  { _call_isr( HDL_NVIC_IRQ58_DMA1_Channel2, __ic->interrupts, 0); }
void DMA1_Channel3_IRQHandler()  { _call_isr( HDL_NVIC_IRQ59_DMA1_Channel3, __ic->interrupts, 0); }
void DMA1_Channel4_IRQHandler()  { _call_isr( HDL_NVIC_IRQ60_DMA1_Channel4, __ic->interrupts, 0); }
void ENET_IRQHandler()  { _call_isr( HDL_NVIC_IRQ61_ENET, __ic->interrupts, 0); }
void ENET_WKUP_IRQHandler()  { _call_isr( HDL_NVIC_IRQ62_ENET_WKUP, __ic->interrupts, 0); }
void CAN1_TX_IRQHandler()  { _call_isr( HDL_NVIC_IRQ63_CAN1_TX, __ic->interrupts, 0); }
void CAN1_RX0_IRQHandler()  { _call_isr( HDL_NVIC_IRQ64_CAN1_RX0, __ic->interrupts, 0); }
void CAN1_RX1_IRQHandler()  { _call_isr( HDL_NVIC_IRQ65_CAN1_RX1, __ic->interrupts, 0); }
void CAN1_EWMC_IRQHandler()  { _call_isr( HDL_NVIC_IRQ66_CAN1_EWMC, __ic->interrupts, 0); }
void USBFS_IRQHandler()  { _call_isr( HDL_NVIC_IRQ67_USBFS, __ic->interrupts, 0); }
void DMA1_Channel5_IRQHandler()  { _call_isr( HDL_NVIC_IRQ68_DMA1_Channel5, __ic->interrupts, 0); }
void DMA1_Channel6_IRQHandler()  { _call_isr( HDL_NVIC_IRQ69_DMA1_Channel6, __ic->interrupts, 0); }
void DMA1_Channel7_IRQHandler()  { _call_isr( HDL_NVIC_IRQ70_DMA1_Channel7, __ic->interrupts, 0); }
void USART5_IRQHandler()  { _call_isr( HDL_NVIC_IRQ71_USART5, __ic->interrupts, 0); }
void I2C2_EV_IRQHandler()  { _call_isr( HDL_NVIC_IRQ72_I2C2_EV, __ic->interrupts, 0); }
void I2C2_ER_IRQHandler()  { _call_isr( HDL_NVIC_IRQ73_I2C2_ER, __ic->interrupts, 0); }
void USBHS_EP1_Out_IRQHandler()  { _call_isr( HDL_NVIC_IRQ74_USBHS_EP1_Out, __ic->interrupts, 0); }
void USBHS_EP1_In_IRQHandler()  { _call_isr( HDL_NVIC_IRQ75_USBHS_EP1_In, __ic->interrupts, 0); }
void USBHS_WKUP_IRQHandler()  { _call_isr( HDL_NVIC_IRQ76_USBHS_WKUP, __ic->interrupts, 0); }
void USBHS_IRQHandler()  { _call_isr( HDL_NVIC_IRQ77_USBHS, __ic->interrupts, 0); }
void DCI_IRQHandler()  { _call_isr( HDL_NVIC_IRQ78_DCI, __ic->interrupts, 0); }
void TRNG_IRQHandler()  { _call_isr( HDL_NVIC_IRQ80_TRNG, __ic->interrupts, 0); }
void FPU_IRQHandler()  { _call_isr( HDL_NVIC_IRQ81_FPU, __ic->interrupts, 0); }
#endif /* GD32F407_427 */

void * g_pfnVectors[0x70] __attribute__ ((section (".isr_vector"), used)) = {
  /* Vector in Cortex - M4 */
  &__stack_end__,                           /* Vector 0 */
  &Reset_Handler,                           /* Vector 1 */
  &NMI_Handler,                             /* Vector 2 IRQ -14 */
  &HardFault_Handler,                       /* Vector 3 IRQ -13 */
  &MemManage_Handler,                       /* Vector 4 IRQ -12 */
  &BusFault_Handler,                        /* Vector 5 IRQ -11 */
  &UsageFault_Handler,                      /* Vector 6 IRQ -10 */
  (void *)NULL,                             /* Vector 7 IRQ -9 */
  (void *)NULL,                             /* Vector 8 IRQ -8 */
  (void *)NULL,                             /* Vector 9 IRQ -7 */
  (void *)NULL,                             /* Vector 10 IRQ -6 */
  &SVC_Handler,                             /* Vector 11 IRQ -5 */
  &DebugMon_Handler,                        /* Vector 12 IRQ -4 */
  (void *)NULL,                             /* Vector 13 IRQ -3 */
  &PendSV_Handler,                          /* Vector 14 IRQ -2 */
  &SysTick_Handler,                         /* Vector 15 IRQ -1 */
  /* Peripheral */
  &WWDGT_IRQHandler,                        /* Vector 16 IRQ 0 */
  &LVD_IRQHandler,                          /* Vector 17 IRQ 1 */
  &TAMPER_STAMP_IRQHandler,                 /* Vector 18 IRQ 2 */
  &RTC_WKUP_IRQHandler,                     /* Vector 19 IRQ 3 */
  &FMC_IRQHandler,                          /* Vector 20 IRQ 4 */
  &RCU_CTC_IRQHandler,                      /* Vector 21 IRQ 5 */
  &EXTI0_IRQHandler,                        /* Vector 22 IRQ 6 */
  &EXTI1_IRQHandler,                        /* Vector 23 IRQ 7 */
  &EXTI2_IRQHandler,                        /* Vector 24 IRQ 8 */
  &EXTI3_IRQHandler,                        /* Vector 25 IRQ 9 */
  &EXTI4_IRQHandler,                        /* Vector 26 IRQ 10 */
  &DMA0_Channel0_IRQHandler,                /* Vector 27 IRQ 11 */
  &DMA0_Channel1_IRQHandler,                /* Vector 28 IRQ 12 */
  &DMA0_Channel2_IRQHandler,                /* Vector 29 IRQ 13 */
  &DMA0_Channel3_IRQHandler,                /* Vector 30 IRQ 14 */
  &DMA0_Channel4_IRQHandler,                /* Vector 31 IRQ 15 */
  &DMA0_Channel5_IRQHandler,                /* Vector 32 IRQ 16 */
  &DMA0_Channel6_IRQHandler,                /* Vector 33 IRQ 17 */
  &ADC_IRQHandler,                          /* Vector 34 IRQ 18 */
  &CAN0_TX_IRQHandler,                      /* Vector 35 IRQ 19 */
  &CAN0_RX0_IRQHandler,                     /* Vector 36 IRQ 20 */
  &CAN0_RX1_IRQHandler,                     /* Vector 37 IRQ 21 */
  &CAN0_EWMC_IRQHandler,                    /* Vector 38 IRQ 22 */
  &EXTI5_9_IRQHandler,                      /* Vector 39 IRQ 23 */
  &TIMER0_BRK_TIMER8_IRQHandler,            /* Vector 40 IRQ 24 */
  &TIMER0_UP_TIMER9_IRQHandler,             /* Vector 41 IRQ 25 */
  &TIMER0_TRG_CMT_TIMER10_IRQHandler,       /* Vector 42 IRQ 26 */
  &TIMER0_Channel_IRQHandler,               /* Vector 43 IRQ 27 */
  &TIMER1_IRQHandler,                       /* Vector 44 IRQ 28 */
  &TIMER2_IRQHandler,                       /* Vector 45 IRQ 29 */
  &TIMER3_IRQHandler,                       /* Vector 46 IRQ 30 */
  &I2C0_EV_IRQHandler,                      /* Vector 47 IRQ 31 */
  &I2C0_ER_IRQHandler,                      /* Vector 48 IRQ 32 */
  &I2C1_EV_IRQHandler,                      /* Vector 49 IRQ 33 */
  &I2C1_ER_IRQHandler,                      /* Vector 50 IRQ 34 */
  &SPI0_IRQHandler,                         /* Vector 51 IRQ 35 */
  &SPI1_IRQHandler,                         /* Vector 52 IRQ 36 */
  &USART0_IRQHandler,                       /* Vector 53 IRQ 37 */
  &USART1_IRQHandler,                       /* Vector 54 IRQ 38 */
  &USART2_IRQHandler,                       /* Vector 55 IRQ 39 */
  &EXTI10_15_IRQHandler,                    /* Vector 56 IRQ 40 */
  &RTC_Alarm_IRQHandler,                    /* Vector 57 IRQ 41 */
  &USBFS_WKUP_IRQHandler,                   /* Vector 58 IRQ 42 */
  &TIMER7_BRK_TIMER11_IRQHandler,           /* Vector 59 IRQ 43 */
  &TIMER7_UP_TIMER12_IRQHandler,            /* Vector 60 IRQ 44 */
  &TIMER7_TRG_CMT_TIMER13_IRQHandler,       /* Vector 61 IRQ 45 */
  &TIMER7_Channel_IRQHandler,               /* Vector 62 IRQ 46 */
  &DMA0_Channel7_IRQHandler,                /* Vector 63 IRQ 47 */
#if defined (GD32F450) || defined (GD32F470)
  &EXMC_IRQHandler,                         /* Vector 64 IRQ 48 */
  &SDIO_IRQHandler,                         /* Vector 65 IRQ 49 */
  &TIMER4_IRQHandler,                       /* Vector 66 IRQ 50 */
  &SPI2_IRQHandler,                         /* Vector 67 IRQ 51 */
  &UART3_IRQHandler,                        /* Vector 68 IRQ 52 */
  &UART4_IRQHandler,                        /* Vector 69 IRQ 53 */
  &TIMER5_DAC_IRQHandler,                   /* Vector 70 IRQ 54 */
  &TIMER6_IRQHandler,                       /* Vector 71 IRQ 55 */
  &DMA1_Channel0_IRQHandler,                /* Vector 72 IRQ 56 */
  &DMA1_Channel1_IRQHandler,                /* Vector 73 IRQ 57 */
  &DMA1_Channel2_IRQHandler,                /* Vector 74 IRQ 58 */
  &DMA1_Channel3_IRQHandler,                /* Vector 75 IRQ 59 */
  &DMA1_Channel4_IRQHandler,                /* Vector 76 IRQ 60 */
  &ENET_IRQHandler,                         /* Vector 77 IRQ 61 */
  &ENET_WKUP_IRQHandler,                    /* Vector 78 IRQ 62 */
  &CAN1_TX_IRQHandler,                      /* Vector 79 IRQ 63 */
  &CAN1_RX0_IRQHandler,                     /* Vector 80 IRQ 64 */
  &CAN1_RX1_IRQHandler,                     /* Vector 81 IRQ 65 */
  &CAN1_EWMC_IRQHandler,                    /* Vector 82 IRQ 66 */
  &USBFS_IRQHandler,                        /* Vector 83 IRQ 67 */
  &DMA1_Channel5_IRQHandler,                /* Vector 84 IRQ 68 */
  &DMA1_Channel6_IRQHandler,                /* Vector 85 IRQ 69 */
  &DMA1_Channel7_IRQHandler,                /* Vector 86 IRQ 70 */
  &USART5_IRQHandler,                       /* Vector 87 IRQ 71 */
  &I2C2_EV_IRQHandler,                      /* Vector 88 IRQ 72 */
  &I2C2_ER_IRQHandler,                      /* Vector 89 IRQ 73 */
  &USBHS_EP1_Out_IRQHandler,                /* Vector 90 IRQ 74 */
  &USBHS_EP1_In_IRQHandler,                 /* Vector 91 IRQ 75 */
  &USBHS_WKUP_IRQHandler,                   /* Vector 92 IRQ 76 */
  &USBHS_IRQHandler,                        /* Vector 93 IRQ 77 */
  &DCI_IRQHandler,                          /* Vector 94 IRQ 78 */
  &irq_n_handler,                           /* Vector 95 IRQ 79 */
  &TRNG_IRQHandler,                         /* Vector 96 IRQ 80 */
  &FPU_IRQHandler,                          /* Vector 97 IRQ 81 */
  &UART6_IRQHandler,                        /* Vector 98 IRQ 82 */
  &UART7_IRQHandler,                        /* Vector 99 IRQ 83 */
  &SPI3_IRQHandler,                         /* Vector 100 IRQ 84 */
  &SPI4_IRQHandler,                         /* Vector 101 IRQ 85 */
  &SPI5_IRQHandler,                         /* Vector 102 IRQ 86 */
  &irq_n_handler,                           /* Vector 103 IRQ 87 */
  &TLI_IRQHandler,                          /* Vector 104 IRQ 88 */
  &TLI_ER_IRQHandler,                       /* Vector 105 IRQ 89 */
  &IPA_IRQHandler,                          /* Vector 106 IRQ 90 */
#endif /* GD32F450_470 */
#if defined (GD32F405) || defined (GD32F425)
  &irq_n_handler,                           /* Vector 64 IRQ 48 */
  &SDIO_IRQHandler,                         /* Vector 65 IRQ 49 */
  &TIMER4_IRQHandler,                       /* Vector 66 IRQ 50 */
  &SPI2_IRQHandler,                         /* Vector 67 IRQ 51 */
  &UART3_IRQHandler,                        /* Vector 68 IRQ 52 */
  &UART4_IRQHandler,                        /* Vector 69 IRQ 53 */
  &TIMER5_DAC_IRQHandler,                   /* Vector 70 IRQ 54 */
  &TIMER6_IRQHandler,                       /* Vector 71 IRQ 55 */
  &DMA1_Channel0_IRQHandler,                /* Vector 72 IRQ 56 */
  &DMA1_Channel1_IRQHandler,                /* Vector 73 IRQ 57 */
  &DMA1_Channel2_IRQHandler,                /* Vector 74 IRQ 58 */
  &DMA1_Channel3_IRQHandler,                /* Vector 75 IRQ 59 */
  &DMA1_Channel4_IRQHandler,                /* Vector 76 IRQ 60 */
  &irq_n_handler,                           /* Vector 77 IRQ 61 */
  &irq_n_handler,                           /* Vector 78 IRQ 62 */
  &CAN1_TX_IRQHandler,                      /* Vector 79 IRQ 63 */
  &CAN1_RX0_IRQHandler,                     /* Vector 80 IRQ 64 */
  &CAN1_RX1_IRQHandler,                     /* Vector 81 IRQ 65 */
  &CAN1_EWMC_IRQHandler,                    /* Vector 82 IRQ 66 */
  &USBFS_IRQHandler,                        /* Vector 83 IRQ 67 */
  &DMA1_Channel5_IRQHandler,                /* Vector 84 IRQ 68 */
  &DMA1_Channel6_IRQHandler,                /* Vector 85 IRQ 69 */
  &DMA1_Channel7_IRQHandler,                /* Vector 86 IRQ 70 */
  &USART5_IRQHandler,                       /* Vector 87 IRQ 71 */
  &I2C2_EV_IRQHandler,                      /* Vector 88 IRQ 72 */
  &I2C2_ER_IRQHandler,                      /* Vector 89 IRQ 73 */
  &USBHS_EP1_Out_IRQHandler,                /* Vector 90 IRQ 74 */
  &USBHS_EP1_In_IRQHandler,                 /* Vector 91 IRQ 75 */
  &USBHS_WKUP_IRQHandler,                   /* Vector 92 IRQ 76 */
  &USBHS_IRQHandler,                        /* Vector 93 IRQ 77 */
  &DCI_IRQHandler,                          /* Vector 94 IRQ 78 */
  &irq_n_handler,                           /* Vector 95 IRQ 79 */
  &TRNG_IRQHandler,                         /* Vector 96 IRQ 80 */
  &FPU_IRQHandler,                          /* Vector 97 IRQ 81 */
  &irq_n_handler,                           /* Vector 98 IRQ 82 */
  &irq_n_handler,                           /* Vector 99 IRQ 83 */
  &irq_n_handler,                           /* Vector 100 IRQ 84 */
  &irq_n_handler,                           /* Vector 101 IRQ 85 */
  &irq_n_handler,                           /* Vector 102 IRQ 86 */
  &irq_n_handler,                           /* Vector 103 IRQ 87 */
  &irq_n_handler,                           /* Vector 104 IRQ 88 */
  &irq_n_handler,                           /* Vector 105 IRQ 89 */
  &irq_n_handler,                           /* Vector 106 IRQ 90 */
#endif /* GD32F405_425 */
#if defined (GD32F407) || defined (GD32F427)
  &EXMC_IRQHandler,                         /* Vector 64 IRQ 48 */
  &SDIO_IRQHandler,                         /* Vector 65 IRQ 49 */
  &TIMER4_IRQHandler,                       /* Vector 66 IRQ 50 */
  &SPI2_IRQHandler,                         /* Vector 67 IRQ 51 */
  &UART3_IRQHandler,                        /* Vector 68 IRQ 52 */
  &UART4_IRQHandler,                        /* Vector 69 IRQ 53 */
  &TIMER5_DAC_IRQHandler,                   /* Vector 70 IRQ 54 */
  &TIMER6_IRQHandler,                       /* Vector 71 IRQ 55 */
  &DMA1_Channel0_IRQHandler,                /* Vector 72 IRQ 56 */
  &DMA1_Channel1_IRQHandler,                /* Vector 73 IRQ 57 */
  &DMA1_Channel2_IRQHandler,                /* Vector 74 IRQ 58 */
  &DMA1_Channel3_IRQHandler,                /* Vector 75 IRQ 59 */
  &DMA1_Channel4_IRQHandler,                /* Vector 76 IRQ 60 */
  &ENET_IRQHandler,                         /* Vector 77 IRQ 61 */
  &ENET_WKUP_IRQHandler,                    /* Vector 78 IRQ 62 */
  &CAN1_TX_IRQHandler,                      /* Vector 79 IRQ 63 */
  &CAN1_RX0_IRQHandler,                     /* Vector 80 IRQ 64 */
  &CAN1_RX1_IRQHandler,                     /* Vector 81 IRQ 65 */
  &CAN1_EWMC_IRQHandler,                    /* Vector 82 IRQ 66 */
  &USBFS_IRQHandler,                        /* Vector 83 IRQ 67 */
  &DMA1_Channel5_IRQHandler,                /* Vector 84 IRQ 68 */
  &DMA1_Channel6_IRQHandler,                /* Vector 85 IRQ 69 */
  &DMA1_Channel7_IRQHandler,                /* Vector 86 IRQ 70 */
  &USART5_IRQHandler,                       /* Vector 87 IRQ 71 */
  &I2C2_EV_IRQHandler,                      /* Vector 88 IRQ 72 */
  &I2C2_ER_IRQHandler,                      /* Vector 89 IRQ 73 */
  &USBHS_EP1_Out_IRQHandler,                /* Vector 90 IRQ 74 */
  &USBHS_EP1_In_IRQHandler,                 /* Vector 91 IRQ 75 */
  &USBHS_WKUP_IRQHandler,                   /* Vector 92 IRQ 76 */
  &USBHS_IRQHandler,                        /* Vector 93 IRQ 77 */
  &DCI_IRQHandler,                          /* Vector 94 IRQ 78 */
  &irq_n_handler,                           /* Vector 95 IRQ 79 */
  &TRNG_IRQHandler,                         /* Vector 96 IRQ 80 */
  &FPU_IRQHandler,                          /* Vector 97 IRQ 81 */
  &irq_n_handler,                           /* Vector 98 IRQ 82 */
  &irq_n_handler,                           /* Vector 99 IRQ 83 */
  &irq_n_handler,                           /* Vector 100 IRQ 84 */
  &irq_n_handler,                           /* Vector 101 IRQ 85 */
  &irq_n_handler,                           /* Vector 102 IRQ 86 */
  &irq_n_handler,                           /* Vector 103 IRQ 87 */
  &irq_n_handler,                           /* Vector 104 IRQ 88 */
  &irq_n_handler,                           /* Vector 105 IRQ 89 */
  &IPA_IRQHandler,                          /* Vector 106 IRQ 90 */  
#endif /* GD32F407_427 */
  &irq_n_handler,                           /* Vector 107 IRQ 91 */
  &irq_n_handler,                           /* Vector 108 IRQ 92 */
  &irq_n_handler,                           /* Vector 109 IRQ 93 */
  &irq_n_handler,                           /* Vector 110 IRQ 94 */
  &irq_n_handler,                           /* Vector 111 IRQ 95 */
};

// __attribute__((naked)) void switch_to_psp(void) {
//   // change PSP
//   asm("LDR R0, =_psp_stack");
//   asm("MSR PSP, R0");
//   // set SPSEL bit
//   asm("MRS R1, CONTROL");
//   asm("ORR R1, R1, #2");
//   asm("MSR CONTROL, R1");
//   // return
//   asm("BX LR");
// }

// __attribute__ ((naked)) void switch_to_unprivileged(void) {
//   // set nPRIV bit
//   asm("MRS R0, CONTROL");
//   asm("ORR R0, R0, #1");
//   asm("MSR CONTROL, r0");
//   // return
//   asm("BX LR");
// }

__attribute__((naked, noreturn)) void Reset_Handler() {
	//asm ("ldr sp, =_estack");
	void **pSource, **pDest;
	for (pSource = &_sidata, pDest = &_sdata; pDest != &_edata; pSource++, pDest++)
	  *pDest = *pSource;

  for (pDest = &_sbss; pDest != &_ebss; pDest++)
    *pDest = 0;
  //TODO: turn off JTAG;
  __libc_init_array();
  //SystemInit();
  main();
  for (;;) ;
}

/* TODO: check sv call, exapmple call: asm("SVC #6"); */
__attribute__ ((naked)) void SVC_Handler(void) {
  //asm("TST LR, 4"); // check LR to know which stack is used
  //asm("ITE EQ"); // 2 next instructions are conditional
  //asm("MRSEQ R0, MSP"); // save MSP if bit 2 is 0
  //asm("MRSNE R0, PSP"); // save PSP if bit 2 is 1

  asm("MRS R0, MSP"); // save MSP stack pointer value

  asm("B svc_handler_main"); // pass R0 as the argument
}

/* TODO: test with __attribute__ ((naked)) */
static void svc_handler_main(uint32_t *sp) {
  /* get the address of the instruction saved in PC */
  uint8_t *instruction = (uint8_t *)(sp[6]);
  /* go back 2 bytes (16-bit opcode) */
  instruction -= 2;
  /* get the opcode, in little endian */
  _call_isr(SVCall_IRQn, __ic->interrupts, (uint8_t)*instruction);
}

void irq_n_handler() {
  uint32_t prio = -1;
  IRQn_Type irq = 0;
  for(uint32_t i = 0; i< sizeof(NVIC->IABR); i++) {
    uint32_t iabr = NVIC->IABR[i];
    while(iabr) {
      uint8_t bit = 31 - __CLZ(iabr);
      IRQn_Type cur_irq = (32 * i) + bit;
      uint32_t cur_prio = NVIC_GetPriority(cur_irq);
      if(cur_prio < prio) {
        irq = cur_irq;
        prio = cur_prio;
      }
      iabr &= ~(1 << bit);
    }
  }
  _call_isr(irq, __ic->interrupts, 0);
}

hdl_module_state_t hdl_core(void *desc, uint8_t enable) {
  /* TODO: */
  if(enable) {
    hdl_core_t *core = (hdl_core_t *)desc;
    rcu_periph_clock_enable(RCU_SYSCFG);
    rcu_periph_clock_enable(RCU_PMU);
    //FMC_WS = (FMC_WS & (~FMC_WC_WSCNT)) | core->flash_latency;
    return HDL_MODULE_INIT_OK;
  }
  rcu_periph_clock_disable(RCU_SYSCFG);
  //FMC_WS = (FMC_WS & (~FMC_WC_WSCNT)) | WS_WSCNT_0;
  return HDL_MODULE_DEINIT_OK;
}

void _hdl_exti_request(hdl_interrupt_controller_t *ic) {
  hdl_nvic_exti_t **extis = ic->exti_lines;
  if(extis != NULL) {
    while (*extis != NULL) {
      uint8_t exti_no = 31 - __CLZ((*extis)->line);
      if(exti_no <= 15) { /* if GPIO exti lines */
        syscfg_exti_line_config((*extis)->source, exti_no);
      } /* other lines from internal modules are fixed */
      if((*extis)->trigger & HDL_EXTI_TRIGGER_FALLING) {
        EXTI_FTEN |= (*extis)->line;
      }
      else {
        EXTI_FTEN &= ~((*extis)->line);
      }
      if((*extis)->trigger & HDL_EXTI_TRIGGER_RISING) {
        EXTI_RTEN |= (*extis)->line;
      }
      else {
        EXTI_RTEN &= ~((*extis)->line);
      }
      EXTI_EVEN |= (*extis)->line;
      extis++;
    }
  }
}

hdl_module_state_t hdl_nvic(void *desc, uint8_t enable) {
  if(enable) {
    hdl_nvic_t *nvic = (hdl_nvic_t *)desc;
    /* TODO: NVIC_SetPriorityGrouping  */
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0);
    //SYSCFG_CPU_IRQ_LAT = nvic->irq_latency;
    __ic = (hdl_nvic_t *)desc;
    /* TODO: fing wokaround to save context for interrupt vector */
    _hdl_exti_request(nvic);
    return HDL_MODULE_INIT_OK;
  }
  else {
    //TODO: disable envic
    HDL_REG_CLEAR(EXTI_INTEN, EXTI_LINES_ALL_POSSIBLE);
    HDL_REG_CLEAR(EXTI_FTEN, EXTI_LINES_ALL_POSSIBLE);
    HDL_REG_CLEAR(EXTI_RTEN, EXTI_LINES_ALL_POSSIBLE);
    HDL_REG_CLEAR(EXTI_EVEN, EXTI_LINES_ALL_POSSIBLE);
    HDL_REG_CLEAR(EXTI_FTEN, EXTI_LINES_ALL_POSSIBLE);
    HDL_REG_CLEAR(EXTI_FTEN, EXTI_LINES_ALL_POSSIBLE);
    HDL_REG_CLEAR(EXTI_FTEN, EXTI_LINES_ALL_POSSIBLE);
    hdl_exti_clear_pending(EXTI_LINES_ALL_POSSIBLE);
  }
  return HDL_MODULE_DEINIT_OK;
}

static void _hdl_nvic_exti_interrupt_enable(hdl_nvic_t *ic, hdl_nvic_interrupt_private_t *isr) {
  hdl_nvic_exti_t **extis = ic->exti_lines;
  if(extis == NULL) return;
  hdl_exti_line_t exti_lines_int_en = 0;
  switch (isr->irq_type) {
    case HDL_NVIC_IRQ6_EXTI0:
      exti_lines_int_en = HDL_EXTI_LINE_0;
      break;
    case HDL_NVIC_IRQ7_EXTI1:
      exti_lines_int_en = HDL_EXTI_LINE_1;
      break;
    case HDL_NVIC_IRQ8_EXTI2:
      exti_lines_int_en = HDL_EXTI_LINE_2;
      break;
    case HDL_NVIC_IRQ9_EXTI3:
      exti_lines_int_en = HDL_EXTI_LINE_3;
      break;
    case HDL_NVIC_IRQ10_EXTI4:
      exti_lines_int_en = HDL_EXTI_LINE_4;
      break;
    case HDL_NVIC_IRQ23_EXTI5_9:
      exti_lines_int_en = EXTI_LINES_5_9;
      break;
    case HDL_NVIC_IRQ40_EXTI10_15:
      exti_lines_int_en = EXTI_LINES_10_15;
      break;    
    default:
      return;
  }
  while ((*extis != NULL) && (exti_lines_int_en)) {
    if((exti_lines_int_en & ((*extis)->line)) &&
       ((*extis)->mode == HDL_EXTI_MODE_INTERRUPT)) {
      EXTI_INTEN |= (*extis)->line;
    }
    exti_lines_int_en &= ~((*extis)->line);
    extis++;
  }
}

uint8_t hdl_interrupt_request(hdl_nvic_t *ic, hdl_nvic_irq_n_t irq, event_handler_t handler, void *context) {
  if((hdl_state(&ic->module) != HDL_MODULE_INIT_OK) || (ic->interrupts == NULL) || (handler == NULL))
    return HDL_FALSE;
  hdl_nvic_interrupt_private_t **isr = (hdl_nvic_interrupt_private_t **)ic->interrupts;
  while ((isr != NULL) && (*isr)->irq_type != irq) isr++;
  if(isr == NULL) return HDL_FALSE;
  (*isr)->handler = handler;
  (*isr)->handler_context = context;
  uint8_t prio = (((*isr)->priority_group << (8U - ic->prio_bits)) | 
                  ((*isr)->priority & (0xFF >> ic->prio_bits)) & 
                  0xFFUL);
  volatile uint8_t *ipr = ((*isr)->irq_type < 0)? &(SCB->SHP[((*isr)->irq_type & 0xFUL) - 4UL]):
                                                    &(NVIC->IP[(*isr)->irq_type]);
  /* set priority for interrupt */
  *ipr = prio;
  /* interrupt enable */
  if((*isr)->irq_type < 0) {
    switch ((*isr)->irq_type) {
      case SysTick_IRQn:
        SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;                  /* Enable SysTick IRQ */
        break;
      case PendSV_IRQn:
        // TODO: enable if possible, ;
        break;
      case SVCall_IRQn:
        // TODO: enable if possible, ;
        break;
      case HardFault_IRQn:
        // TODO: enable if possible, ;
        break;
      case NonMaskableInt_IRQn:
        // TODO: enable if possible, ;
        break;
      default:
        return HDL_FALSE;
    }
  }
  else {
    _hdl_nvic_exti_interrupt_enable(ic, *isr);
    NVIC_EnableIRQ((*isr)->irq_type);
  }
  return HDL_TRUE;
}

void hdl_interrupt_sw_trigger(hdl_nvic_irq_n_t interrupt) {
  NVIC_SetPendingIRQ(interrupt);
}

void hdl_exti_sw_trigger(hdl_interrupt_controller_t *desc, hdl_exti_line_t line) {
  EXTI_SWIEV |= line;
}
