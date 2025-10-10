#include "hdl_iface.h"


static hdl_module_state_t _hdl_pwm(const void *desc, uint8_t enable) {
  hdl_pwm_mcu_t *pwm = (hdl_pwm_mcu_t *)desc;
  hdl_timertick_counter_t *timer = (hdl_timertick_counter_t *)pwm->dependencies[0];
  TIM_TypeDef *TIMx = (TIM_TypeDef *)timer->config->phy;
  hdl_pwm_mcu_config_t *cnf = (hdl_pwm_mcu_config_t *)pwm->config;
  if(enable) {
    uint32_t r32_ind = cnf->channel >> 2;
    uint32_t r16_ind = cnf->channel >> 3;
    uint32_t b8_offset = (8U * (r32_ind & 1));
    uint32_t b4_offset = (4U * r32_ind);
    uint32_t b2_offset = (2U * r32_ind);
    uint32_t tmpccmrx = 0U;
    uint32_t tmpccer = 0U;
    uint32_t tmpcr2 = 0U;
    TIMx->CCER &= ~(TIM_CCER_CC1E << b4_offset); /* Disable the Channel x: Reset the CCxE Bit */
    tmpccer = TIMx->CCER; /* Get the TIMx CCER register value */
    tmpcr2 = TIMx->CR2; /* Get the TIMx CR2 register value */
    tmpccmrx = (&TIMx->CCMR1)[r16_ind]; /* Get the TIMx CCMRx register value */
    /* Reset the Output Compare mode and Capture/Compare selection Bits */
    tmpccmrx &= ~(TIM_CCMR1_OC1M << b8_offset);

    tmpccmrx &= ~(TIM_CCMR1_CC1S << b8_offset);
    tmpccmrx |= (cnf->mode << b8_offset); /* Select the Output Compare Mode */
    tmpccer &= ~(TIM_CCER_CC1P << b4_offset); /* Reset the Output Polarity level */
    tmpccer |= (cnf->polarity << b4_offset); /* Set the Output Compare Polarity */
    if(IS_TIM_ADVANCED_INSTANCE(TIMx)) { /* if((TIMx == TIM1) || (TIMx == TIM8)) */
      tmpcr2 &= ~(TIM_CR2_OIS1 << b2_offset); /* Reset the Output Compare IDLE State */
      tmpcr2 |= (cnf->idle_state << b2_offset); /* Set the Output Idle state */
    }
    TIMx->CR2 = tmpcr2; /* Write to TIMx CR2 */
    (&TIMx->CCMR1)[r16_ind] = tmpccmrx; /* Write to TIMx CCMRx */
    (&TIMx->CCR1)[r32_ind] = cnf->pulse; /* Set the Capture Compare Register value */
    TIMx->CCER = tmpccer; /* Write to TIMx CCER */
    (&TIMx->CCMR1)[r16_ind] |= (TIM_CCMR1_OC1PE << b8_offset);
    (&TIMx->CCMR1)[r16_ind] &= ~(TIM_CCMR1_OC1FE << b8_offset);
    (&TIMx->CCMR1)[r16_ind] |= (cnf->fast_mode << b4_offset);
    TIMx->CCER |= (TIM_CCER_CC1E << b4_offset); /* Enable the Channel x */
    return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_pwm_get(void const *desc, uint32_t *threshold) {
  if(threshold == NULL) return HDL_FALSE;
  hdl_pwm_mcu_t *pwm = (hdl_pwm_mcu_t *)desc;
  hdl_timertick_counter_t *timer = (hdl_timertick_counter_t *)pwm->dependencies[0];
  TIM_TypeDef *TIMx = (TIM_TypeDef *)timer->config->phy;
  *threshold = (&TIMx->CCR1)[pwm->config->channel >> 2];
  return HDL_TRUE;
}

static uint8_t _hdl_pwm_set(void const *desc, uint32_t *threshold) {
  if(threshold == NULL) return HDL_FALSE;
  hdl_pwm_mcu_t *pwm = (hdl_pwm_mcu_t *)desc;
  hdl_timertick_counter_t *timer = (hdl_timertick_counter_t *)pwm->dependencies[0];
  TIM_TypeDef *TIMx = (TIM_TypeDef *)timer->config->phy;
  (&TIMx->CCR1)[pwm->config->channel >> 2] = *threshold;
  return HDL_TRUE;
}

static hdl_tick_counter_t *_hdl_pwm_get_tickcounter(void const *desc) {
  hdl_pwm_mcu_t *pwm = (hdl_pwm_mcu_t *)desc;
  return (hdl_tick_counter_t *)pwm->dependencies[0];
}

const hdl_pwm_iface_t hdl_pwm_mcu_iface = {
  .init = &_hdl_pwm,
  .get = &_hdl_pwm_get,
  .set = &_hdl_pwm_set,
  .get_tickcounter = &_hdl_pwm_get_tickcounter
};
