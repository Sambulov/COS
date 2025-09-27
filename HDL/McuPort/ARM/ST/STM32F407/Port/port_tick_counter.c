#include "hdl_iface.h"

#define IS_TIM_APB1(INSTANCE) (((INSTANCE) == TIM2)   || \
                              ((INSTANCE) == TIM3)   || \
                              ((INSTANCE) == TIM4)   || \
                              ((INSTANCE) == TIM5)   || \
                              ((INSTANCE) == TIM6)   || \
                              ((INSTANCE) == TIM7)   || \
                              ((INSTANCE) == TIM12)  || \
                              ((INSTANCE) == TIM13)  || \
                              ((INSTANCE) == TIM14))

#define IS_TIM_APB2(INSTANCE) (((INSTANCE) == TIM1)   || \
                              ((INSTANCE) == TIM8)   || \
                              ((INSTANCE) == TIM9)   || \
                              ((INSTANCE) == TIM10)  || \
                              ((INSTANCE) == TIM11))

static hdl_module_state_t _hdl_tick_counter(const void *desc, const uint8_t enable) {
  hdl_timertick_counter_t *counter = (hdl_timertick_counter_t *)desc;
  TIM_TypeDef *TIMx = (TIM_TypeDef *)counter->config->phy;
  const hdl_timertick_counter_config_t *config = counter->config;
  if(IS_TIM_APB1(TIMx)) RCC->APB1RSTR |= config->rcc;
  else RCC->APB2RSTR |= config->rcc;
  if(enable) {
    if(!IS_TIM_INSTANCE(TIMx)) return HDL_MODULE_FAULT;
    if(IS_TIM_APB1(TIMx)) {
      RCC->APB1RSTR &= ~config->rcc;
      RCC->APB1ENR |= config->rcc;
    }
    else {
      RCC->APB2RSTR &= ~config->rcc;
      RCC->APB2ENR |= config->rcc;
    }
    uint32_t tmpcr1 = 0U;
    if(IS_TIM_CC3_INSTANCE(TIMx)) tmpcr1 |= config->counter_mode;
    if(IS_TIM_CC1_INSTANCE(TIMx)) tmpcr1 |= (uint32_t)config->clock_div;
    TIMx->CR1 = tmpcr1;
    TIMx->ARR = (uint32_t)config->period; /* Set the Auto-reload value */
    TIMx->PSC = (uint32_t)config->prescaler; /* Set the Prescaler value */
    if(IS_TIM_ADVANCED_INSTANCE(TIMx)) TIMx->RCR = config->repetition_counter; /* Set the Repetition Counter value */
    /* Generate an update event to reload the Prescaler and the repetition counter(only for TIM1 and TIM8) value immediately */
    TIMx->EGR = TIM_EGR_UG;
    TIMx->CR2 = config->trgo_mode;
    TIMx->CR1 |= TIM_CR1_CEN;
    return HDL_MODULE_ACTIVE;
  }
  if(IS_TIM_APB1(TIMx)) {
    RCC->AHB1RSTR &= ~config->rcc;
    RCC->APB1ENR &= ~config->rcc;
  }
  else {
    RCC->AHB2RSTR &= ~config->rcc;
    RCC->APB2ENR &= ~config->rcc;
  }
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_tick_counter_get(const void *counter, uint32_t *value, uint32_t *period) {
  hdl_timertick_counter_t *tick_cnt = (hdl_timertick_counter_t *)counter;
  TIM_TypeDef *TIMx = (TIM_TypeDef *)tick_cnt->config->phy;
  if(!IS_TIM_INSTANCE(TIMx)) return HDL_FALSE;
  if(value != NULL) {
    if(TIMx->CR1 & TIM_CR1_DIR) *value = (TIMx->ARR - TIMx->CNT);
    else *value = TIMx->CNT;
  }
  if(period != NULL) *period = TIMx->ARR + 1;
  return HDL_TRUE;
}

static uint8_t _hdl_tick_counter_clock_get(const void *counter, hdl_clock_freq_t *freq) {
  hdl_timertick_counter_t *tick_cnt = (hdl_timertick_counter_t *)counter;
  const hdl_timertick_counter_config_t *config = tick_cnt->config;
  hdl_clock_t *src_clk = (hdl_clock_t *)tick_cnt->dependencies[0];
  int32_t presc = config->period;
  switch (config->clock_div) {
    case TIM_CLOCKDIVISION_DIV4:
      presc <<= 1;
    /* fall through */
    case TIM_CLOCKDIVISION_DIV2:
      presc <<= 1;
    /* fall through */
    case TIM_CLOCKDIVISION_DIV1:
    default:
      break;
  }
  /* if center alingned mode */
  if((config->counter_mode != TIM_COUNTERMODE_UP) &&  (config->counter_mode != TIM_COUNTERMODE_DOWN))
    presc <<= 1;
  hdl_clock_freq_t src_freq;
  hdl_clock_get(src_clk, &src_freq);
  hdl_clock_calc_div(&src_freq, presc, freq);
  return HDL_TRUE;
}

static uint8_t _hdl_tick_counter_set(const void *counter, uint32_t *value, uint32_t *period) {
  hdl_timertick_counter_t *tick_cnt = (hdl_timertick_counter_t *)counter;
  TIM_TypeDef *TIMx = (TIM_TypeDef *)tick_cnt->config->phy;
  if(!IS_TIM_INSTANCE(TIMx)) return HDL_FALSE;
  TIMx->CR1 &= ~TIM_CR1_CEN;
  if(value != NULL) {
    if(TIMx->CR1 & TIM_CR1_DIR) TIMx->CNT = (TIMx->ARR - *value);
    else TIMx->CNT = *value;
  }
  if(period != NULL) TIMx->ARR = *period;
  TIMx->CR1 |= TIM_CR1_CEN;
  return HDL_TRUE;
}

static uint8_t _hdl_tick_counter_stop(const void *counter) {
  hdl_timertick_counter_t *tick_cnt = (hdl_timertick_counter_t *)counter;
  TIM_TypeDef *TIMx = (TIM_TypeDef *)tick_cnt->config->phy;
  if(!IS_TIM_INSTANCE(TIMx)) return HDL_FALSE;
  TIMx->CR1 &= ~TIM_CR1_CEN;
  return HDL_TRUE;
}

const hdl_tick_counter_iface_t hdl_timertick_counter_iface = {
  .init = &_hdl_tick_counter,
  .clock_get = &_hdl_tick_counter_clock_get,
  .get = &_hdl_tick_counter_get,
  .set = &_hdl_tick_counter_set,
  .stop = &_hdl_tick_counter_stop
};
