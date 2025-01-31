#include "hdl_portable.h"

void wwdgt_handler()                 { call_isr((hdl_nvic_irq_n_t)WWDGT_IRQn, 0); }
void lvd_handler()                   { call_isr((hdl_nvic_irq_n_t)LVD_IRQn, 0); }
void rtc_handler()                   { call_isr((hdl_nvic_irq_n_t)RTC_IRQn, 0); }
void fmc_handler()                   { call_isr((hdl_nvic_irq_n_t)FMC_IRQn, 0); }
void rcu_handler()                   { call_isr((hdl_nvic_irq_n_t)RCU_IRQn, 0); }
void exti0_1_handler()               { call_isr((hdl_nvic_irq_n_t)EXTI0_1_IRQn, 0); EXTI_PD |= (EXTI_0 | EXTI_1); }
void exti2_3_IRQHandler()            { call_isr((hdl_nvic_irq_n_t)EXTI2_3_IRQn, 0); EXTI_PD |= (EXTI_2 | EXTI_3); }
void exti4_15_handler()              { call_isr((hdl_nvic_irq_n_t)EXTI4_15_IRQn, 0); EXTI_PD |= EXTI_LINES_4_15; }
void dma_channel0_handler()          { call_isr((hdl_nvic_irq_n_t)DMA_Channel0_IRQn, 0); }
void dma_channel1_2_handler()        { call_isr((hdl_nvic_irq_n_t)DMA_Channel1_2_IRQn, 0); }
void dma_channel3_4_handler()        { call_isr((hdl_nvic_irq_n_t)DMA_Channel3_4_IRQn, 0); }
void adc_cmp_handler()               { call_isr((hdl_nvic_irq_n_t)ADC_CMP_IRQn, 0); }
void timer0_brk_up_trg_com_handler() { call_isr((hdl_nvic_irq_n_t)TIMER0_BRK_UP_TRG_COM_IRQn, 0); TIMER_INTF(TIMER0) &= ~TIMER_INTF_UPIF; }
void timer0_channel_handler()        { call_isr((hdl_nvic_irq_n_t)TIMER0_Channel_IRQn, 0); }
void timer2_handler()                { call_isr((hdl_nvic_irq_n_t)TIMER2_IRQn, 0); TIMER_INTF(TIMER2) &= ~TIMER_INTF_UPIF; }
void timer5_handler()                { call_isr((hdl_nvic_irq_n_t)TIMER5_IRQn, 0); }
void timer13_handler()               { call_isr((hdl_nvic_irq_n_t)TIMER13_IRQn, 0); }
void timer14_handler()               { call_isr((hdl_nvic_irq_n_t)TIMER14_IRQn, 0); }
void timer15_handler()               { call_isr((hdl_nvic_irq_n_t)TIMER15_IRQn, 0); }
void timer16_handler()               { call_isr((hdl_nvic_irq_n_t)TIMER16_IRQn, 0); }
void i2c0_ev_handler()               { call_isr((hdl_nvic_irq_n_t)I2C0_EV_IRQn, 0); }
void i2c1_ev_handler()               { call_isr((hdl_nvic_irq_n_t)I2C1_EV_IRQn, 0); }
void spi0_handler()                  { call_isr((hdl_nvic_irq_n_t)SPI0_IRQn, 0); }
void spi1_handler()                  { call_isr((hdl_nvic_irq_n_t)SPI1_IRQn, 0); }
void usart0_handler()                { call_isr((hdl_nvic_irq_n_t)USART0_IRQn, 0); }
void usart1_handler()                { call_isr((hdl_nvic_irq_n_t)USART1_IRQn, 0); }
void i2c0_er_handler()               { call_isr((hdl_nvic_irq_n_t)I2C0_ER_IRQn, 0); }
void i2c1_er_handler()               { call_isr((hdl_nvic_irq_n_t)I2C1_ER_IRQn, 0); }

hdl_module_state_t hdl_core(void *desc, uint8_t enable) {
  if(enable) {
    hdl_core_t *core = (hdl_core_t *)desc;
    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | core->config->flash_latency;
    rcu_periph_clock_enable(RCU_CFGCMP);
    return HDL_MODULE_ACTIVE;
  }
  rcu_periph_clock_disable(RCU_CFGCMP);
  FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT_0;
  return HDL_MODULE_UNLOADED;
}
