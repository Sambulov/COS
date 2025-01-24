#include "hdl_portable.h"

void wwdgt_handler()                 { call_isr(WWDGT_IRQn, 0); }
void lvd_handler()                   { call_isr(LVD_IRQn, 0); }
void rtc_handler()                   { call_isr(RTC_IRQn, 0); }
void fmc_handler()                   { call_isr(FMC_IRQn, 0); }
void rcu_handler()                   { call_isr(RCU_IRQn, 0); }
void exti0_1_handler()               { call_isr(EXTI0_1_IRQn, 0); EXTI_PD |= (EXTI_0 | EXTI_1); }
void exti2_3_IRQHandler()            { call_isr(EXTI2_3_IRQn, 0); EXTI_PD |= (EXTI_2 | EXTI_3); }
void exti4_15_handler()              { call_isr(EXTI4_15_IRQn, 0); EXTI_PD |= EXTI_LINES_4_15; }
void dma_channel0_handler()          { call_isr(DMA_Channel0_IRQn, 0); }
void dma_channel1_2_handler()        { call_isr(DMA_Channel1_2_IRQn, 0); }
void dma_channel3_4_handler()        { call_isr(DMA_Channel3_4_IRQn, 0); }
void adc_cmp_handler()               { call_isr(ADC_CMP_IRQn, 0); }
void timer0_brk_up_trg_com_handler() { call_isr(TIMER0_BRK_UP_TRG_COM_IRQn, 0); TIMER_INTF(TIMER0) &= ~TIMER_INTF_UPIF; }
void timer0_channel_handler()        { call_isr(TIMER0_Channel_IRQn, 0); }
void timer2_handler()                { call_isr(TIMER2_IRQn, 0); TIMER_INTF(TIMER2) &= ~TIMER_INTF_UPIF; }
void timer5_handler()                { call_isr(TIMER5_IRQn, 0); }
void timer13_handler()               { call_isr(TIMER13_IRQn, 0); }
void timer14_handler()               { call_isr(TIMER14_IRQn, 0); }
void timer15_handler()               { call_isr(TIMER15_IRQn, 0); }
void timer16_handler()               { call_isr(TIMER16_IRQn, 0); }
void i2c0_ev_handler()               { call_isr(I2C0_EV_IRQn, 0); }
void i2c1_ev_handler()               { call_isr(I2C1_EV_IRQn, 0); }
void spi0_handler()                  { call_isr(SPI0_IRQn, 0); }
void spi1_handler()                  { call_isr(SPI1_IRQn, 0); }
void usart0_handler()                { call_isr(USART0_IRQn, 0); }
void usart1_handler()                { call_isr(USART1_IRQn, 0); }
void i2c0_er_handler()               { call_isr(I2C0_ER_IRQn, 0); }
void i2c1_er_handler()               { call_isr(I2C1_ER_IRQn, 0); }

static hdl_module_state_t _hdl_core(void *desc, uint8_t enable) {
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

hdl_module_base_iface_t hdl_core_iface = {
  .init = _hdl_core
};
