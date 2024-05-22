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

extern void *_estack;
extern void *_sidata, *_sdata, *_edata;
extern void *_sbss, *_ebss;

//void SystemInit();
void __libc_init_array();
void main();
hdl_nvic_t *__ic = NULL;

static void _call_isr(IRQn_Type irq, hdl_nvic_interrupt_private_t **isrs, uint32_t event) {
  if(isrs != NULL) {
    while (*isrs != NULL) {
      if((*isrs)->irq_type == irq) {
        (*isrs)->handler(event, __ic, (*isrs)->handler_context);
        return;
      }
      isrs++;
    }
  }
  //If you get stuck here, your code is missing a handler for some interrupt.
	//asm("bkpt 255");
  //for(;;) ;
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

void WWDGT_IRQHandler()                 { _call_isr(HDL_NVIC_IRQ0_WWDGT, __ic->interrupts, 0); }
void LVD_IRQHandler()                   { _call_isr(HDL_NVIC_IRQ1_LVD, __ic->interrupts, 0); }
void TAMPER_IRQHandler()                { _call_isr(HDL_NVIC_IRQ2_TAMPER, __ic->interrupts, 0); }   
void RTC_IRQHandler()                   { _call_isr(HDL_NVIC_IRQ3_RTC, __ic->interrupts, 0); }
void FMC_IRQHandler()                   { _call_isr(HDL_NVIC_IRQ4_FMC, __ic->interrupts, 0); }
void RCU_IRQHandler()                   { _call_isr(HDL_NVIC_IRQ5_RCU_CTC, __ic->interrupts, 0); }
void EXTI0_IRQHandler()                 { _call_isr(HDL_NVIC_IRQ6_EXTI0, __ic->interrupts, EXTI_0); hdl_exti_clear_pending(EXTI_0); }
void EXTI1_IRQHandler()                 { _call_isr(HDL_NVIC_IRQ7_EXTI1, __ic->interrupts, EXTI_1); hdl_exti_clear_pending(EXTI_1); }
void EXTI2_IRQHandler()                 { _call_isr(HDL_NVIC_IRQ8_EXTI2, __ic->interrupts, EXTI_2); hdl_exti_clear_pending(EXTI_2); }
void EXTI3_IRQHandler()                 { _call_isr(HDL_NVIC_IRQ9_EXTI3, __ic->interrupts, EXTI_3); hdl_exti_clear_pending(EXTI_3); }
void EXTI4_IRQHandler()                 { _call_isr(HDL_NVIC_IRQ10_EXTI4, __ic->interrupts, EXTI_4); hdl_exti_clear_pending(EXTI_4); }
void DMA0_Channel0_IRQHandler()         { _call_isr(HDL_NVIC_IRQ11_DMA0_Channel0, __ic->interrupts, 0); }
void DMA0_Channel1_IRQHandler()         { _call_isr(HDL_NVIC_IRQ12_DMA0_Channel1, __ic->interrupts, 0); }
void DMA0_Channel2_IRQHandler()         { _call_isr(HDL_NVIC_IRQ13_DMA0_Channel2, __ic->interrupts, 0); }
void DMA0_Channel3_IRQHandler()         { _call_isr(HDL_NVIC_IRQ14_DMA0_Channel3, __ic->interrupts, 0); }
void DMA0_Channel4_IRQHandler()         { _call_isr(HDL_NVIC_IRQ15_DMA0_Channel4, __ic->interrupts, 0); }
void DMA0_Channel5_IRQHandler()         { _call_isr(HDL_NVIC_IRQ16_DMA0_Channel5, __ic->interrupts, 0); } 
void DMA0_Channel6_IRQHandler()         { _call_isr(HDL_NVIC_IRQ17_DMA0_Channel6, __ic->interrupts, 0); }
void ADC0_1_IRQHandler()                { _call_isr(HDL_NVIC_IRQ18_ADC0_1, __ic->interrupts, 0); }

#ifdef GD32F10X_MD
void USBD_HP_CAN0_TX_IRQHandler         { _call_isr(HDL_NVIC_IRQ19_USBD_HP_CAN0_TX, __ic->interrupts, 0); } /* IRQ19 */
void USBD_LP_CAN0_RX0_IRQHandler()      { _call_isr(HDL_NVIC_IRQ20_USBD_LP_CAN0_RX0, __ic->interrupts, 0); } /* IRQ20 */
void CAN0_RX1_IRQHandler()              { _call_isr(HDL_NVIC_IRQ21_CAN0_RX1, __ic->interrupts, 0); } /* IRQ21 */
void CAN0_EWMC_IRQHandler()             { _call_isr(HDL_NVIC_IRQ22_CAN0_EWMC, __ic->interrupts, 0); } /* IRQ22 */
void EXTI5_9_IRQHandler()               { _call_isr(HDL_NVIC_IRQ23_EXTI5_9, __ic->interrupts, 0); } /* IRQ23 */
void TIMER0_BRK_IRQHandler()            { _call_isr(HDL_NVIC_IRQ24_TIMER0_BRK, __ic->interrupts, 0); } /* IRQ24 */
void TIMER0_UP_IRQHandler()             { _call_isr(HDL_NVIC_IRQ25_TIMER0_UP, __ic->interrupts, 0); } /* IRQ25 */
void TIMER0_TRG_CMT_IRQHandler()        { _call_isr(HDL_NVIC_IRQ26_TIMER0_TRG_CMT, __ic->interrupts, 0); } /* IRQ26 */
void TIMER0_Channel_IRQHandler()        { _call_isr(HDL_NVIC_IRQ27_TIMER0_Channel, __ic->interrupts, 0); } /* IRQ27 */
void TIMER1_IRQHandler()                { _call_isr(HDL_NVIC_IRQ28_TIMER1, __ic->interrupts, 0); } /* IRQ28 */
void TIMER2_IRQHandler()                { _call_isr(HDL_NVIC_IRQ29_TIMER2, __ic->interrupts, 0); } /* IRQ29 */
void TIMER3_IRQHandler()                { _call_isr(HDL_NVIC_IRQ30_TIMER3, __ic->interrupts, 0); } /* IRQ30 */
void I2C0_EV_IRQHandler()               { _call_isr(HDL_NVIC_IRQ31_I2C0_EV, __ic->interrupts, 0); } /* IRQ31 */
void I2C0_ER_IRQHandler()               { _call_isr(HDL_NVIC_IRQ32_I2C0_ER, __ic->interrupts, 0); } /* IRQ32 */
void I2C1_EV_IRQHandler()               { _call_isr(HDL_NVIC_IRQ33_I2C1_EV, __ic->interrupts, 0); } /* IRQ33 */
void I2C1_ER_IRQHandler()               { _call_isr(HDL_NVIC_IRQ34_I2C1_ER, __ic->interrupts, 0); } /* IRQ34 */
void SPI0_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ35_SPI0, __ic->interrupts, 0); } /* IRQ35 */
void SPI1_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ36_SPI1, __ic->interrupts, 0); } /* IRQ36 */
void USART0_IRQHandler()                { _call_isr(HDL_NVIC_IRQ37_USART0, __ic->interrupts, 0); } /* IRQ37 */
void USART1_IRQHandler()                { _call_isr(HDL_NVIC_IRQ38_USART1, __ic->interrupts, 0); } /* IRQ38 */
void USART2_IRQHandler()                { _call_isr(HDL_NVIC_IRQ39_USART2, __ic->interrupts, 0); } /* IRQ39 */
void EXTI10_15_IRQHandler()             { _call_isr(HDL_NVIC_IRQ40_EXTI10_15, __ic->interrupts, 0); } /* IRQ40 */
void RTC_Alarm_IRQHandler()             { _call_isr(HDL_NVIC_IRQ41_RTC_Alarm, __ic->interrupts, 0); } /* IRQ41 */
void USBD_WKUP_IRQHandler()             { _call_isr(HDL_NVIC_IRQ42_USBD_WKUP, __ic->interrupts, 0); } /* IRQ42 */
void EXMC_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ48_EXMC, __ic->interrupts, 0); } /* IRQ48 */
#endif /* GD32F10X_MD */
#ifdef GD32F10X_HD
void USBD_HP_CAN0_TX_IRQHandler()       { _call_isr(HDL_NVIC_IRQ19_USBD_HP_CAN0_TX, __ic->interrupts, 0); } /* IRQ19 */
void USBD_LP_CAN0_RX0_IRQHandler()      { _call_isr(HDL_NVIC_IRQ20_USBD_LP_CAN0_RX0, __ic->interrupts, 0); } /* IRQ20 */
void CAN0_RX1_IRQHandler()              { _call_isr(HDL_NVIC_IRQ21_CAN0_RX1, __ic->interrupts, 0); } /* IRQ21 */
void CAN0_EWMC_IRQHandler()             { _call_isr(HDL_NVIC_IRQ22_CAN0_EWMC, __ic->interrupts, 0); } /* IRQ22 */
void EXTI5_9_IRQHandler()               { _call_isr(HDL_NVIC_IRQ23_EXTI5_9, __ic->interrupts, 0); } /* IRQ23 */
void TIMER0_BRK_IRQHandler()            { _call_isr(HDL_NVIC_IRQ24_TIMER0_BRK, __ic->interrupts, 0); } /* IRQ24 */
void TIMER0_UP_IRQHandler()             { _call_isr(HDL_NVIC_IRQ25_TIMER0_UP, __ic->interrupts, 0); } /* IRQ25 */
void TIMER0_TRG_CMT_IRQHandler()        { _call_isr(HDL_NVIC_IRQ26_TIMER0_TRG_CMT, __ic->interrupts, 0); } /* IRQ26 */
void TIMER0_Channel_IRQHandler()        { _call_isr(HDL_NVIC_IRQ27_TIMER0_Channel, __ic->interrupts, 0); } /* IRQ27 */
void TIMER1_IRQHandler()                { _call_isr(HDL_NVIC_IRQ28_TIMER1, __ic->interrupts, 0); } /* IRQ28 */
void TIMER2_IRQHandler()                { _call_isr(HDL_NVIC_IRQ29_TIMER2, __ic->interrupts, 0); } /* IRQ29 */
void TIMER3_IRQHandler()                { _call_isr(HDL_NVIC_IRQ30_TIMER3, __ic->interrupts, 0); } /* IRQ30 */
void I2C0_EV_IRQHandler()               { _call_isr(HDL_NVIC_IRQ31_I2C0_EV, __ic->interrupts, 0); } /* IRQ31 */
void I2C0_ER_IRQHandler()               { _call_isr(HDL_NVIC_IRQ32_I2C0_ER, __ic->interrupts, 0); } /* IRQ32 */
void I2C1_EV_IRQHandler()               { _call_isr(HDL_NVIC_IRQ33_I2C1_EV, __ic->interrupts, 0); } /* IRQ33 */
void I2C1_ER_IRQHandler()               { _call_isr(HDL_NVIC_IRQ34_I2C1_ER, __ic->interrupts, 0); } /* IRQ34 */
void SPI0_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ35_SPI0, __ic->interrupts, 0); } /* IRQ35 */
void SPI1_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ36_SPI1, __ic->interrupts, 0); } /* IRQ36 */
void USART0_IRQHandler()                { _call_isr(HDL_NVIC_IRQ37_USART0, __ic->interrupts, 0); } /* IRQ37 */
void USART1_IRQHandler()                { _call_isr(HDL_NVIC_IRQ38_USART1, __ic->interrupts, 0); } /* IRQ38 */
void USART2_IRQHandler()                { _call_isr(HDL_NVIC_IRQ39_USART2, __ic->interrupts, 0); } /* IRQ39 */
void EXTI10_15_IRQHandler()             { _call_isr(HDL_NVIC_IRQ40_EXTI10_15, __ic->interrupts, 0); } /* IRQ40 */
void RTC_Alarm_IRQHandler()             { _call_isr(HDL_NVIC_IRQ41_RTC_Alarm, __ic->interrupts, 0); } /* IRQ41 */
void USBD_WKUP_IRQHandler()             { _call_isr(HDL_NVIC_IRQ42_USBD_WKUP, __ic->interrupts, 0); } /* IRQ42 */
void TIMER7_BRK_IRQHandler()            { _call_isr(HDL_NVIC_IRQ43_TIMER7_BRK, __ic->interrupts, 0); } /* IRQ43 */
void TIMER7_UP_IRQHandler()             { _call_isr(HDL_NVIC_IRQ44_TIMER7_UP, __ic->interrupts, 0); } /* IRQ44 */
void TIMER7_TRG_CMT_IRQHandler()        { _call_isr(HDL_NVIC_IRQ45_TIMER7_TRG_CMT, __ic->interrupts, 0); } /* IRQ45 */
void TIMER7_Channel_IRQHandler()        { _call_isr(HDL_NVIC_IRQ46_TIMER7_Channel, __ic->interrupts, 0); } /* IRQ46 */
void ADC2_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ47_ADC2, __ic->interrupts, 0); } /* IRQ47 */
void EXMC_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ48_EXMC, __ic->interrupts, 0); } /* IRQ48 */
void SDIO_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ49_SDIO, __ic->interrupts, 0); } /* IRQ49 */
void TIMER4_IRQHandler()                { _call_isr(HDL_NVIC_IRQ50_TIMER4, __ic->interrupts, 0); } /* IRQ50 */
void SPI2_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ51_SPI2, __ic->interrupts, 0); } /* IRQ51 */
void UART3_IRQHandler()                 { _call_isr(HDL_NVIC_IRQ52_UART3, __ic->interrupts, 0); } /* IRQ52 */
void UART4_IRQHandler()                 { _call_isr(HDL_NVIC_IRQ53_UART4, __ic->interrupts, 0); } /* IRQ53 */
void TIMER5_IRQHandler()                { _call_isr(HDL_NVIC_IRQ54_TIMER5, __ic->interrupts, 0); } /* IRQ54 */
void TIMER6_IRQHandler()                { _call_isr(HDL_NVIC_IRQ55_TIMER6, __ic->interrupts, 0); } /* IRQ55 */
void DMA1_Channel0_IRQHandler()         { _call_isr(HDL_NVIC_IRQ56_DMA1_Channel0, __ic->interrupts, 0); } /* IRQ56 */
void DMA1_Channel1_IRQHandler()         { _call_isr(HDL_NVIC_IRQ57_DMA1_Channel1, __ic->interrupts, 0); } /* IRQ57 */
void DMA1_Channel2_IRQHandler()         { _call_isr(HDL_NVIC_IRQ58_DMA1_Channel2, __ic->interrupts, 0); } /* IRQ58 */
void DMA1_Channel3_4_IRQHandler()       { _call_isr(HDL_NVIC_IRQ59_DMA1_Channel3_Channel4, __ic->interrupts, 0); } /* IRQ59 */
#endif /* GD32F10X_HD */
#ifdef GD32F10X_XD
void USBD_HP_CAN0_TX_IRQHandler()       { _call_isr(HDL_NVIC_IRQ19_USBD_HP_CAN0_TX, __ic->interrupts, 0); } /* IRQ19 */
void USBD_LP_CAN0_RX0_IRQHandler()      { _call_isr(HDL_NVIC_IRQ20_USBD_LP_CAN0_RX0, __ic->interrupts, 0); } /* IRQ20 */
void CAN0_RX1_IRQHandler()              { _call_isr(HDL_NVIC_IRQ21_CAN0_RX1, __ic->interrupts, 0); } /* IRQ21 */
void CAN0_EWMC_IRQHandler()             { _call_isr(HDL_NVIC_IRQ22_CAN0_EWMC, __ic->interrupts, 0); }  /* IRQ22 */
void EXTI5_9_IRQHandler()               { _call_isr(HDL_NVIC_IRQ23_EXTI5_9, __ic->interrupts, EXTI_PD & EXTI_LINES_5_9); hdl_exti_clear_pending(EXTI_LINES_5_9); } /* IRQ23 */
void TIMER0_BRK_TIMER8_IRQHandler()     { _call_isr(HDL_NVIC_IRQ24_TIMER0_BRK_TIMER8, __ic->interrupts, 0); } /* IRQ24 */
void TIMER0_UP_TIMER9_IRQHandler() { 
  _call_isr(HDL_NVIC_IRQ25_TIMER0_UP_TIMER9, __ic->interrupts, 0);
  TIMER_INTF(TIMER0) &= ~TIMER_INTF_UPIF;
  TIMER_INTF(TIMER9) &= ~TIMER_INTF_UPIF;
} /* IRQ25 */
void TIMER0_TRG_CMT_TIMER10_IRQHandler(){ _call_isr(HDL_NVIC_IRQ26_TIMER0_TRG_CMT_TIMER10, __ic->interrupts, 0); } /* IRQ26 */
void TIMER0_Channel_IRQHandler()        { _call_isr(HDL_NVIC_IRQ27_TIMER0_Channel, __ic->interrupts, 0); } /* IRQ27 */
void TIMER1_IRQHandler() {
  _call_isr(HDL_NVIC_IRQ28_TIMER1, __ic->interrupts, 0);
  TIMER_INTF(TIMER1) &= ~TIMER_INTF_UPIF;
} /* IRQ28 */
void TIMER2_IRQHandler()                { _call_isr(HDL_NVIC_IRQ29_TIMER2, __ic->interrupts, 0); } /* IRQ29 */
void TIMER3_IRQHandler()                { _call_isr(HDL_NVIC_IRQ30_TIMER3, __ic->interrupts, 0); } /* IRQ30 */
void I2C0_EV_IRQHandler()               { _call_isr(HDL_NVIC_IRQ31_I2C0_EV, __ic->interrupts, 0); } /* IRQ31 */
void I2C0_ER_IRQHandler()               { _call_isr(HDL_NVIC_IRQ32_I2C0_ER, __ic->interrupts, 0); } /* IRQ32 */
void I2C1_EV_IRQHandler()               { _call_isr(HDL_NVIC_IRQ33_I2C1_EV, __ic->interrupts, 0); } /* IRQ33 */
void I2C1_ER_IRQHandler()               { _call_isr(HDL_NVIC_IRQ34_I2C1_ER, __ic->interrupts, 0); } /* IRQ34 */
void SPI0_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ35_SPI0, __ic->interrupts, 0); } /* IRQ35 */
void SPI1_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ36_SPI1, __ic->interrupts, 0); } /* IRQ36 */
void USART0_IRQHandler()                { _call_isr(HDL_NVIC_IRQ37_USART0, __ic->interrupts, 0); } /* IRQ37 */
void USART1_IRQHandler()                { _call_isr(HDL_NVIC_IRQ38_USART1, __ic->interrupts, 0); } /* IRQ38 */
void USART2_IRQHandler()                { _call_isr(HDL_NVIC_IRQ39_USART2, __ic->interrupts, 0); } /* IRQ39 */
void EXTI10_15_IRQHandler()             { _call_isr(HDL_NVIC_IRQ40_EXTI10_15, __ic->interrupts, EXTI_PD & EXTI_LINES_10_15); hdl_exti_clear_pending(EXTI_LINES_10_15); } /* IRQ40 */
void RTC_Alarm_IRQHandler()             { _call_isr(HDL_NVIC_IRQ41_RTC_Alarm, __ic->interrupts, 0); } /* IRQ41 */
void USBD_WKUP_IRQHandler()             { _call_isr(HDL_NVIC_IRQ42_USBD_WKUP, __ic->interrupts, 0); } /* IRQ42 */
void TIMER7_BRK_TIMER11_IRQHandler()    { _call_isr(HDL_NVIC_IRQ43_TIMER7_BRK_TIMER11, __ic->interrupts, 0); } /* IRQ43 */
void TIMER7_UP_TIMER12_IRQHandler()     { _call_isr(HDL_NVIC_IRQ44_TIMER7_UP_TIMER12, __ic->interrupts, 0); } /* IRQ44 */
void TIMER7_TRG_CMT_TIMER13_IRQHandler(){ _call_isr(HDL_NVIC_IRQ45_TIMER7_TRG_CMT_TIMER13, __ic->interrupts, 0); } /* IRQ45 */
void TIMER7_Channel_IRQHandler()        { _call_isr(HDL_NVIC_IRQ46_TIMER7_Channel, __ic->interrupts, 0); }  /* IRQ46 */
void ADC2_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ47_ADC2, __ic->interrupts, 0); } /* IRQ47 */
void EXMC_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ48_EXMC, __ic->interrupts, 0); } /* IRQ48 */
void SDIO_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ49_SDIO, __ic->interrupts, 0); } /* IRQ49 */
void TIMER4_IRQHandler()                { _call_isr(HDL_NVIC_IRQ50_TIMER4, __ic->interrupts, 0); } /* IRQ50 */
void SPI2_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ51_SPI2, __ic->interrupts, 0); } /* IRQ51 */
void UART3_IRQHandler()                 { _call_isr(HDL_NVIC_IRQ52_UART3, __ic->interrupts, 0); } /* IRQ52 */
void UART4_IRQHandler()                 { _call_isr(HDL_NVIC_IRQ53_UART4, __ic->interrupts, 0); } /* IRQ53 */
void TIMER5_IRQHandler()                { _call_isr(HDL_NVIC_IRQ54_TIMER5, __ic->interrupts, 0); } /* IRQ54 */
void TIMER6_IRQHandler()                { _call_isr(HDL_NVIC_IRQ55_TIMER6, __ic->interrupts, 0); } /* IRQ55 */
void DMA1_Channel0_IRQHandler()         { _call_isr(HDL_NVIC_IRQ56_DMA1_Channel0, __ic->interrupts, 0); } /* IRQ56 */
void DMA1_Channel1_IRQHandler()         { _call_isr(HDL_NVIC_IRQ57_DMA1_Channel1, __ic->interrupts, 0); } /* IRQ57 */
void DMA1_Channel2_IRQHandler()         { _call_isr(HDL_NVIC_IRQ58_DMA1_Channel2, __ic->interrupts, 0); } /* IRQ58 */
void DMA1_Channel3_4_IRQHandler()       { _call_isr(HDL_NVIC_IRQ59_DMA1_Channel3_Channel4, __ic->interrupts, 0); } /* IRQ59 */
#endif /* GD32F10X_XD */
#ifdef GD32F10X_CL
void CAN0_TX_IRQHandler()               { _call_isr(HDL_NVIC_IRQ19_CAN0_TX, __ic->interrupts, 0); } /* IRQ19 */
void CAN0_RX0_IRQHandler()              { _call_isr(HDL_NVIC_IRQ20_CAN0_RX0, __ic->interrupts, 0); } /* IRQ20 */
void CAN0_RX1_IRQHandler()              { _call_isr(HDL_NVIC_IRQ21_CAN0_RX1, __ic->interrupts, 0); } /* IRQ21 */
void CAN0_EWMC_IRQHandler()             { _call_isr(HDL_NVIC_IRQ22_CAN0_EWMC, __ic->interrupts, 0); } /* IRQ22 */
void EXTI5_9_IRQHandler()               { _call_isr(HDL_NVIC_IRQ23_EXTI5_9, __ic->interrupts, 0); } /* IRQ23 */
void TIMER0_BRK_IRQHandler()            { _call_isr(HDL_NVIC_IRQ24_TIMER0_BRK, __ic->interrupts, 0); } /* IRQ24 */
void TIMER0_UP_IRQHandler()             { _call_isr(HDL_NVIC_IRQ25_TIMER0_UP, __ic->interrupts, 0); } /* IRQ25 */
void TIMER0_TRG_CMT_IRQHandler()        { _call_isr(HDL_NVIC_IRQ26_TIMER0_TRG_CMT, __ic->interrupts, 0); } /* IRQ26 */
void TIMER0_Channel_IRQHandler()        { _call_isr(HDL_NVIC_IRQ27_TIMER0_Channel, __ic->interrupts, 0); } /* IRQ27 */
void TIMER1_IRQHandler()                { _call_isr(HDL_NVIC_IRQ28_TIMER1, __ic->interrupts, 0); } /* IRQ28 */
void TIMER2_IRQHandler()                { _call_isr(HDL_NVIC_IRQ29_TIMER2, __ic->interrupts, 0); } /* IRQ29 */
void TIMER3_IRQHandler()                { _call_isr(HDL_NVIC_IRQ30_TIMER3, __ic->interrupts, 0); } /* IRQ30 */
void I2C0_EV_IRQHandler()               { _call_isr(HDL_NVIC_IRQ31_I2C0_EV, __ic->interrupts, 0); } /* IRQ31 */
void I2C0_ER_IRQHandler()               { _call_isr(HDL_NVIC_IRQ32_I2C0_ER, __ic->interrupts, 0); } /* IRQ32 */
void I2C1_EV_IRQHandler()               { _call_isr(HDL_NVIC_IRQ33_I2C1_EV, __ic->interrupts, 0); } /* IRQ33 */
void I2C1_ER_IRQHandler()               { _call_isr(HDL_NVIC_IRQ34_I2C1_ER, __ic->interrupts, 0); } /* IRQ34 */
void SPI0_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ35_SPI0, __ic->interrupts, 0); } /* IRQ35 */
void SPI1_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ36_SPI1, __ic->interrupts, 0); } /* IRQ36 */
void USART0_IRQHandler()                { _call_isr(HDL_NVIC_IRQ37_USART0, __ic->interrupts, 0); } /* IRQ37 */
void USART1_IRQHandler()                { _call_isr(HDL_NVIC_IRQ38_USART1, __ic->interrupts, 0); } /* IRQ38 */
void USART2_IRQHandler()                { _call_isr(HDL_NVIC_IRQ39_USART2, __ic->interrupts, 0); } /* IRQ39 */
void EXTI10_15_IRQHandler()             { _call_isr(HDL_NVIC_IRQ40_EXTI10_15, __ic->interrupts, 0); } /* IRQ40 */
void RTC_Alarm_IRQHandler()             { _call_isr(HDL_NVIC_IRQ41_RTC_ALARM, __ic->interrupts, 0); } /* IRQ41 */
void USBFS_WKUP_IRQHandler()            { _call_isr(HDL_NVIC_IRQ42_USBFS_WKUP, __ic->interrupts, 0); } /* IRQ42 */
void TIMER7_BRK_IRQHandler()            { _call_isr(HDL_NVIC_IRQ43_TIMER7_BRK, __ic->interrupts, 0); } /* IRQ43 */
void TIMER7_UP_IRQHandler()             { _call_isr(HDL_NVIC_IRQ44_TIMER7_UP, __ic->interrupts, 0); } /* IRQ44 */
void TIMER7_TRG_CMT_IRQHandler()        { _call_isr(HDL_NVIC_IRQ45_TIMER7_TRG_CMT, __ic->interrupts, 0); } /* IRQ45 */
void TIMER7_Channel_IRQHandler()        { _call_isr(HDL_NVIC_IRQ46_TIMER7_Channel, __ic->interrupts, 0); } /* IRQ46 */ 
void EXMC_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ48_EXMC, __ic->interrupts, 0); } /* IRQ48 */
void TIMER4_IRQHandler()                { _call_isr(HDL_NVIC_IRQ50_TIMER4, __ic->interrupts, 0); } /* IRQ50 */
void SPI2_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ51_SPI2, __ic->interrupts, 0); } /* IRQ51 */
void UART3_IRQHandler()                 { _call_isr(HDL_NVIC_IRQ52_UART3, __ic->interrupts, 0); } /* IRQ52 */
void UART4_IRQHandler()                 { _call_isr(HDL_NVIC_IRQ53_UART4, __ic->interrupts, 0); } /* IRQ53 */
void TIMER5_IRQHandler()                { _call_isr(HDL_NVIC_IRQ54_TIMER5, __ic->interrupts, 0); } /* IRQ54 */
void TIMER6_IRQHandler()                { _call_isr(HDL_NVIC_IRQ55_TIMER6, __ic->interrupts, 0); } /* IRQ55 */
void DMA1_Channel0_IRQHandler()         { _call_isr(HDL_NVIC_IRQ56_DMA1_Channel0, __ic->interrupts, 0); } /* IRQ56 */
void DMA1_Channel1_IRQHandler()         { _call_isr(HDL_NVIC_IRQ57_DMA1_Channel1, __ic->interrupts, 0); } /* IRQ57 */
void DMA1_Channel2_IRQHandler()         { _call_isr(HDL_NVIC_IRQ58_DMA1_Channel2, __ic->interrupts, 0); } /* IRQ58 */
void DMA1_Channel3_IRQHandler()         { _call_isr(HDL_NVIC_IRQ59_DMA1_Channel3, __ic->interrupts, 0); } /* IRQ59 */
void DMA1_Channel4_IRQHandler()         { _call_isr(HDL_NVIC_IRQ60_DMA1_Channel4, __ic->interrupts, 0); } /* IRQ60 */
void ENET_IRQHandler()                  { _call_isr(HDL_NVIC_IRQ61_ENET, __ic->interrupts, 0); } /* IRQ61 */
void ENET_WKUP_IRQHandler()             { _call_isr(HDL_NVIC_IRQ62_ENET_WKUP, __ic->interrupts, 0); } /* IRQ62 */
void CAN1_TX_IRQHandler()               { _call_isr(HDL_NVIC_IRQ63_CAN1_TX, __ic->interrupts, 0); } /* IRQ63 */
void CAN1_RX0_IRQHandler()              { _call_isr(HDL_NVIC_IRQ64_CAN1_RX0, __ic->interrupts, 0); } /* IRQ64 */
void CAN1_RX1_IRQHandler()              { _call_isr(HDL_NVIC_IRQ65_CAN1_RX1, __ic->interrupts, 0); } /* IRQ65 */
void CAN1_EWMC_IRQHandler()             { _call_isr(HDL_NVIC_IRQ66_CAN1_EWMC, __ic->interrupts, 0); } /* IRQ66 */
void USBFS_IRQHandler()                 { _call_isr(HDL_NVIC_IRQ67_USBFS, __ic->interrupts, 0); } /* IRQ67 */
#endif /* GD32F10X_CL */

void * g_pfnVectors[0x60] __attribute__ ((section (".isr_vector"), used)) = {
  &_estack,
  &Reset_Handler,
  &NMI_Handler,
  &HardFault_Handler,
  &MemManage_Handler,
  &BusFault_Handler,
  &UsageFault_Handler,
  (void *)NULL,
  (void *)NULL,
  (void *)NULL,
  (void *)NULL,
  &SVC_Handler,
  &DebugMon_Handler,
  (void *)NULL,
  &PendSV_Handler,
  &SysTick_Handler,

  &WWDGT_IRQHandler,                    /* IRQ0 */
  &LVD_IRQHandler,                      /* IRQ1 */
  &TAMPER_IRQHandler,                   /* IRQ2 */
  &RTC_IRQHandler,                      /* IRQ3 */
  &FMC_IRQHandler,                      /* IRQ4 */
  &RCU_IRQHandler,                      /* IRQ5 */
  &EXTI0_IRQHandler,                    /* IRQ6 */
  &EXTI1_IRQHandler,                    /* IRQ7 */
  &EXTI2_IRQHandler,                    /* IRQ8 */
  &EXTI3_IRQHandler,                    /* IRQ9 */
  &EXTI4_IRQHandler,                    /* IRQ10 */
  &DMA0_Channel0_IRQHandler,            /* IRQ11 */
  &DMA0_Channel1_IRQHandler,            /* IRQ12 */
  &DMA0_Channel2_IRQHandler,            /* IRQ13 */
  &DMA0_Channel3_IRQHandler,            /* IRQ14 */
  &DMA0_Channel4_IRQHandler,            /* IRQ15 */
  &DMA0_Channel5_IRQHandler,            /* IRQ16 */
  &DMA0_Channel6_IRQHandler,            /* IRQ17 */
  &ADC0_1_IRQHandler,                   /* IRQ18 */
#ifdef GD32F10X_MD
  &USBD_HP_CAN0_TX_IRQHandler,          /* IRQ19 */
  &USBD_LP_CAN0_RX0_IRQHandler,         /* IRQ20 */
  &CAN0_RX1_IRQHandler,                 /* IRQ21 */
  &CAN0_EWMC_IRQHandler,                /* IRQ22 */
  &EXTI5_9_IRQHandler,                  /* IRQ23 */
  &TIMER0_BRK_IRQHandler,               /* IRQ24 */
  &TIMER0_UP_IRQHandler,                /* IRQ25 */
  &TIMER0_TRG_CMT_IRQHandler,           /* IRQ26 */
  &TIMER0_Channel_IRQHandler,           /* IRQ27 */
  &TIMER1_IRQHandler,                   /* IRQ28 */
  &TIMER2_IRQHandler,                   /* IRQ29 */
  &TIMER3_IRQHandler,                   /* IRQ30 */
  &I2C0_EV_IRQHandler,                  /* IRQ31 */
  &I2C0_ER_IRQHandler,                  /* IRQ32 */
  &I2C1_EV_IRQHandler,                  /* IRQ33 */
  &I2C1_ER_IRQHandler,                  /* IRQ34 */
  &SPI0_IRQHandler,                     /* IRQ35 */
  &SPI1_IRQHandler,                     /* IRQ36 */
  &USART0_IRQHandler,                   /* IRQ37 */
  &USART1_IRQHandler,                   /* IRQ38 */
  &USART2_IRQHandler,                   /* IRQ39 */
  &EXTI10_15_IRQHandler,                /* IRQ40 */
  &RTC_Alarm_IRQHandler,                /* IRQ41 */
  &USBD_WKUP_IRQHandler,                /* IRQ42 */
  &irq_n_handler,                       /* IRQ43 */
  &irq_n_handler,                       /* IRQ44 */
  &irq_n_handler,                       /* IRQ45 */
  &irq_n_handler,                       /* IRQ46 */
  &irq_n_handler,                       /* IRQ47 */
  &EXMC_IRQHandler,                     /* IRQ48 */
  &irq_n_handler,                       /* IRQ49 */
  &irq_n_handler,                       /* IRQ50 */
  &irq_n_handler,                       /* IRQ51 */
  &irq_n_handler,                       /* IRQ52 */
  &irq_n_handler,                       /* IRQ53 */
  &irq_n_handler,                       /* IRQ54 */
  &irq_n_handler,                       /* IRQ55 */
  &irq_n_handler,                       /* IRQ56 */
  &irq_n_handler,                       /* IRQ57 */
  &irq_n_handler,                       /* IRQ58 */
  &irq_n_handler,                       /* IRQ59 */
  &irq_n_handler,                       /* IRQ60 */
  &irq_n_handler,                       /* IRQ61 */
  &irq_n_handler,                       /* IRQ62 */
  &irq_n_handler,                       /* IRQ63 */
  &irq_n_handler,                       /* IRQ64 */
  &irq_n_handler,                       /* IRQ65 */
  &irq_n_handler,                       /* IRQ66 */
  &irq_n_handler,                       /* IRQ67 */
#endif /* GD32F10X_MD */
#ifdef GD32F10X_HD
  &USBD_HP_CAN0_TX_IRQHandler,          /* IRQ19 */
  &USBD_LP_CAN0_RX0_IRQHandler,         /* IRQ20 */
  &CAN0_RX1_IRQHandler,                 /* IRQ21 */
  &CAN0_EWMC_IRQHandler,                /* IRQ22 */
  &EXTI5_9_IRQHandler,                  /* IRQ23 */
  &TIMER0_BRK_IRQHandler,               /* IRQ24 */
  &TIMER0_UP_IRQHandler,                /* IRQ25 */
  &TIMER0_TRG_CMT_IRQHandler,           /* IRQ26 */
  &TIMER0_Channel_IRQHandler,           /* IRQ27 */
  &TIMER1_IRQHandler,                   /* IRQ28 */
  &TIMER2_IRQHandler,                   /* IRQ29 */
  &TIMER3_IRQHandler,                   /* IRQ30 */
  &I2C0_EV_IRQHandler,                  /* IRQ31 */
  &I2C0_ER_IRQHandler,                  /* IRQ32 */
  &I2C1_EV_IRQHandler,                  /* IRQ33 */
  &I2C1_ER_IRQHandler,                  /* IRQ34 */
  &SPI0_IRQHandler,                     /* IRQ35 */
  &SPI1_IRQHandler,                     /* IRQ36 */
  &USART0_IRQHandler,                   /* IRQ37 */
  &USART1_IRQHandler,                   /* IRQ38 */
  &USART2_IRQHandler,                   /* IRQ39 */
  &EXTI10_15_IRQHandler,                /* IRQ40 */
  &RTC_Alarm_IRQHandler,                /* IRQ41 */
  &USBD_WKUP_IRQHandler,                /* IRQ42 */
  &TIMER7_BRK_IRQHandler,               /* IRQ43 */
  &TIMER7_UP_IRQHandler,                /* IRQ44 */
  &TIMER7_TRG_CMT_IRQHandler,           /* IRQ45 */
  &TIMER7_Channel_IRQHandler,           /* IRQ46 */
  &ADC2_IRQHandler,                     /* IRQ47 */
  &EXMC_IRQHandler,                     /* IRQ48 */
  &SDIO_IRQHandler,                     /* IRQ49 */
  &TIMER4_IRQHandler,                   /* IRQ50 */
  &SPI2_IRQHandler,                     /* IRQ51 */
  &UART3_IRQHandler,                    /* IRQ52 */
  &UART4_IRQHandler,                    /* IRQ53 */
  &TIMER5_IRQHandler,                   /* IRQ54 */
  &TIMER6_IRQHandler,                   /* IRQ55 */
  &DMA1_Channel0_IRQHandler,            /* IRQ56 */
  &DMA1_Channel1_IRQHandler,            /* IRQ57 */
  &DMA1_Channel2_IRQHandler,            /* IRQ58 */
  &DMA1_Channel3_4_IRQHandler,          /* IRQ59 */
  &irq_n_handler,                       /* IRQ60 */
  &irq_n_handler,                       /* IRQ61 */
  &irq_n_handler,                       /* IRQ62 */
  &irq_n_handler,                       /* IRQ63 */
  &irq_n_handler,                       /* IRQ64 */
  &irq_n_handler,                       /* IRQ65 */
  &irq_n_handler,                       /* IRQ66 */
  &irq_n_handler,                       /* IRQ67 */
#endif /* GD32F10X_HD */
#ifdef GD32F10X_XD
  &USBD_HP_CAN0_TX_IRQHandler,          /* IRQ19 */
  &USBD_LP_CAN0_RX0_IRQHandler,         /* IRQ20 */
  &CAN0_RX1_IRQHandler,                 /* IRQ21 */
  &CAN0_EWMC_IRQHandler,                /* IRQ22 */
  &EXTI5_9_IRQHandler,                  /* IRQ23 */
  &TIMER0_BRK_TIMER8_IRQHandler,        /* IRQ24 */
  &TIMER0_UP_TIMER9_IRQHandler,         /* IRQ25 */
  &TIMER0_TRG_CMT_TIMER10_IRQHandler,   /* IRQ26 */
  &TIMER0_Channel_IRQHandler,           /* IRQ27 */
  &TIMER1_IRQHandler,                   /* IRQ28 */
  &TIMER2_IRQHandler,                   /* IRQ29 */
  &TIMER3_IRQHandler,                   /* IRQ30 */
  &I2C0_EV_IRQHandler,                  /* IRQ31 */
  &I2C0_ER_IRQHandler,                  /* IRQ32 */
  &I2C1_EV_IRQHandler,                  /* IRQ33 */
  &I2C1_ER_IRQHandler,                  /* IRQ34 */
  &SPI0_IRQHandler,                     /* IRQ35 */
  &SPI1_IRQHandler,                     /* IRQ36 */
  &USART0_IRQHandler,                   /* IRQ37 */
  &USART1_IRQHandler,                   /* IRQ38 */
  &USART2_IRQHandler,                   /* IRQ39 */
  &EXTI10_15_IRQHandler,                /* IRQ40 */
  &RTC_Alarm_IRQHandler,                /* IRQ41 */
  &USBD_WKUP_IRQHandler,                /* IRQ42 */
  &TIMER7_BRK_TIMER11_IRQHandler,       /* IRQ43 */
  &TIMER7_UP_TIMER12_IRQHandler,        /* IRQ44 */
  &TIMER7_TRG_CMT_TIMER13_IRQHandler,   /* IRQ45 */
  &TIMER7_Channel_IRQHandler,           /* IRQ46 */
  &ADC2_IRQHandler,                     /* IRQ47 */
  &EXMC_IRQHandler,                     /* IRQ48 */
  &SDIO_IRQHandler,                     /* IRQ49 */
  &TIMER4_IRQHandler,                   /* IRQ50 */
  &SPI2_IRQHandler,                     /* IRQ51 */
  &UART3_IRQHandler,                    /* IRQ52 */
  &UART4_IRQHandler,                    /* IRQ53 */
  &TIMER5_IRQHandler,                   /* IRQ54 */
  &TIMER6_IRQHandler,                   /* IRQ55 */
  &DMA1_Channel0_IRQHandler,            /* IRQ56 */
  &DMA1_Channel1_IRQHandler,            /* IRQ57 */
  &DMA1_Channel2_IRQHandler,            /* IRQ58 */
  &DMA1_Channel3_4_IRQHandler,          /* IRQ59 */
  &irq_n_handler,                       /* IRQ60 */
  &irq_n_handler,                       /* IRQ61 */
  &irq_n_handler,                       /* IRQ62 */
  &irq_n_handler,                       /* IRQ63 */
  &irq_n_handler,                       /* IRQ64 */
  &irq_n_handler,                       /* IRQ65 */
  &irq_n_handler,                       /* IRQ66 */
  &irq_n_handler,                       /* IRQ67 */
#endif /* GD32F10X_XD */
#ifdef GD32F10X_CL
  &CAN0_TX_IRQHandler,                  /* IRQ19 */
  &CAN0_RX0_IRQHandler,                 /* IRQ20 */
  &CAN0_RX1_IRQHandler,                 /* IRQ21 */
  &CAN0_EWMC_IRQHandler,                /* IRQ22 */
  &EXTI5_9_IRQHandler,                  /* IRQ23 */
  &TIMER0_BRK_IRQHandler,               /* IRQ24 */
  &TIMER0_UP_IRQHandler,                /* IRQ25 */
  &TIMER0_TRG_CMT_IRQHandler,           /* IRQ26 */
  &TIMER0_Channel_IRQHandler,           /* IRQ27 */
  &TIMER1_IRQHandler,                   /* IRQ28 */
  &TIMER2_IRQHandler,                   /* IRQ29 */
  &TIMER3_IRQHandler,                   /* IRQ30 */
  &I2C0_EV_IRQHandler,                  /* IRQ31 */
  &I2C0_ER_IRQHandler,                  /* IRQ32 */
  &I2C1_EV_IRQHandler,                  /* IRQ33 */
  &I2C1_ER_IRQHandler,                  /* IRQ34 */
  &SPI0_IRQHandler,                     /* IRQ35 */
  &SPI1_IRQHandler,                     /* IRQ36 */
  &USART0_IRQHandler,                   /* IRQ37 */
  &USART1_IRQHandler,                   /* IRQ38 */
  &USART2_IRQHandler,                   /* IRQ39 */
  &EXTI10_15_IRQHandler,                /* IRQ40 */
  &RTC_Alarm_IRQHandler,                /* IRQ41 */
  &USBFS_WKUP_IRQHandler,               /* IRQ42 */
  &TIMER7_BRK_IRQHandler,               /* IRQ43 */
  &TIMER7_UP_IRQHandler,                /* IRQ44 */
  &TIMER7_TRG_CMT_IRQHandler,           /* IRQ45 */
  &TIMER7_Channel_IRQHandler,           /* IRQ46 */ 
  &irq_n_handler,                       /* IRQ47 */
  &EXMC_IRQHandler,                     /* IRQ48 */
  &irq_n_handler,                       /* IRQ49 */
  &TIMER4_IRQHandler,                   /* IRQ50 */
  &SPI2_IRQHandler,                     /* IRQ51 */
  &UART3_IRQHandler,                    /* IRQ52 */
  &UART4_IRQHandler,                    /* IRQ53 */
  &TIMER5_IRQHandler,                   /* IRQ54 */
  &TIMER6_IRQHandler,                   /* IRQ55 */
  &DMA1_Channel0_IRQHandler,            /* IRQ56 */
  &DMA1_Channel1_IRQHandler,            /* IRQ57 */
  &DMA1_Channel2_IRQHandler,            /* IRQ58 */
  &DMA1_Channel3_IRQHandler,            /* IRQ59 */
  &DMA1_Channel4_IRQHandler,            /* IRQ60 */
  &ENET_IRQHandler,                     /* IRQ61 */
  &ENET_WKUP_IRQHandler,                /* IRQ62 */
  &CAN1_TX_IRQHandler,                  /* IRQ63 */
  &CAN1_RX0_IRQHandler,                 /* IRQ64 */
  &CAN1_RX1_IRQHandler,                 /* IRQ65 */
  &CAN1_EWMC_IRQHandler,                /* IRQ66 */
  &USBFS_IRQHandler,                    /* IRQ67 */
#endif /* GD32F10X_CL */
  &irq_n_handler,                       /* IRQ68 */
  &irq_n_handler,                       /* IRQ69 */
  &irq_n_handler,                       /* IRQ70 */
  &irq_n_handler,                       /* IRQ71 */
  &irq_n_handler,                       /* IRQ72 */
  &irq_n_handler,                       /* IRQ73 */
  &irq_n_handler,                       /* IRQ74 */
  &irq_n_handler,                       /* IRQ75 */
  &irq_n_handler,                       /* IRQ76 */
  &irq_n_handler,                       /* IRQ77 */
  &irq_n_handler,                       /* IRQ78 */
  &irq_n_handler,                       /* IRQ79 */
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
    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | core->flash_latency;
    rcu_periph_clock_enable(RCU_AF);
    return HDL_MODULE_INIT_OK;
  }
  rcu_periph_clock_disable(RCU_AF);
  FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT_0;
  return HDL_MODULE_DEINIT_OK;
}

void _hdl_exti_request(hdl_interrupt_controller_t *ic) {
  if((hdl_state(&ic->module) == HDL_MODULE_INIT_OK)) {
    hdl_nvic_exti_t **extis = ic->exti_lines;
    if(extis != NULL) {
      while (*extis != NULL) {
        uint8_t exti_no = 31 - __CLZ((*extis)->line);
        if(exti_no <= 15) { /* if GPIO exti lines */
          gpio_exti_source_select((*extis)->source, exti_no);
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

static void _hdl_nvic_exti_interrupt_enable(hdl_nvic_t *ic, hdl_nvic_interrupt_private_t **isr) {
  hdl_nvic_exti_t **extis = ic->exti_lines;
  if(extis == NULL) return;
  hdl_exti_line_t exti_lines_int_en = 0;
  switch ((*isr)->irq_type) {
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
    _hdl_nvic_exti_interrupt_enable(ic, isr);
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
