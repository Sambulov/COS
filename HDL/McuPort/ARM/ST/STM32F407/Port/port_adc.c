#include "hdl_iface.h"

static hdl_module_state_t _hdl_adc_common(const void *desc, uint8_t enable) {
  hdl_adc_common_mcu_t *adc = (hdl_adc_common_mcu_t *)desc;
  hdl_adc_common_config_t *adc_cnf = (hdl_adc_common_config_t *)adc->config;
  ADC_Common_TypeDef *phy = (ADC_Common_TypeDef *)adc_cnf->phy;
  if(enable) {
    CL_REG_SET(RCC->APB2ENR, adc_cnf->rcc);
    phy->CCR = adc_cnf->multi_mode | (((adc_cnf->prescaler >> 1) - 1) << 16);
    return HDL_MODULE_ACTIVE;
  }
  CL_REG_CLEAR(RCC->APB2ENR, adc_cnf->rcc);
  return HDL_MODULE_UNLOADED;
}

const hdl_module_base_iface_t hdl_adc_common_iface = {
  .init = &_hdl_adc_common
};

/*=======================================================*/

typedef struct {
  coroutine_t work;
  uint32_t regular_age;
  uint32_t injected_age;
  uint8_t regular_channels;
  uint8_t injected_channels;
} hdl_adc_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_adc_var_t, *((hdl_adc_mcu_t *)0)->obj_var, HDL_ADC_VAR_SIZE, "port_adc.h");

static uint8_t _adc_sw_trigger(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)arg;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)adc->obj_var;
  ADC_TypeDef *phy = (ADC_TypeDef *)adc->config->phy;
  if(phy->CR2 & ADC_CR2_ADON) {
    if(adc_var->regular_channels) {
      uint8_t regular_ready = port_dma_ch_transfer_complete(adc->dependencies[1]) || (phy->SR & ADC_SR_EOC);
      if(adc->config->regular_trg_edge == ADC_EXTERNALTRIGCONVEDGE_NONE)
        if(regular_ready || !(phy->SR & ADC_SR_STRT)) CL_REG_SET(phy->CR2, ADC_CR2_SWSTART);
      if(regular_ready) adc_var->regular_age++;
    }
    if(adc_var->injected_channels) {
      uint8_t injected_ready = (phy->SR & ADC_FLAG_JEOC);
      if(adc->config->injected_trg_edge == ADC_EXTERNALTRIGINJECCONVEDGE_NONE)
        if(injected_ready || !(phy->SR & ADC_SR_JSTRT)) CL_REG_SET(phy->CR2, ADC_CR2_JSWSTART);
      if(injected_ready) adc_var->injected_age++;
    }
    phy->SR = ~(ADC_FLAG_EOC | ADC_FLAG_JEOC | ADC_FLAG_OVR);
  }
  return cancel;
}

static hdl_module_state_t _hdl_adc(const void *desc, uint8_t enable) {
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)desc;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)adc->obj_var;
  hdl_adc_config_t *adc_cnf = (hdl_adc_config_t *)adc->config;
  ADC_TypeDef *phy = (ADC_TypeDef *)adc_cnf->phy;

  if(enable) {
    CL_REG_SET(RCC->APB2ENR, RCC_APB2ENR_ADC1EN);
    CL_REG_SET(RCC->APB2ENR, adc_cnf->rcc);
    phy->SR = 0;
    phy->CR1 = 0;
    phy->CR2 = 0;
    phy->SMPR1 = 0;
    phy->SMPR2 = 0;
    phy->JOFR1 = 0;
    phy->JOFR2 = 0;
    phy->JOFR3 = 0;
    phy->JOFR4 = 0;
    phy->HTR = 0x00000fff;
    phy->LTR = 0;
    phy->SQR1 = 0;
    phy->SQR1 = 0;
    phy->SQR1 = 0;
    phy->JSQR = 0;

    CL_REG_MODIFY(phy->CR1, ADC_CR1_RES, adc_cnf->resolution); /* Set ADC resolution */
    CL_REG_MODIFY(phy->CR2, ADC_CR2_ALIGN, adc_cnf->data_alignment); /* Set ADC data alignment */
    
    CL_REG_MODIFY(phy->CR2, ADC_CR2_EXTSEL, adc_cnf->regular_ext_trg); /* set the regular external trigger */
    CL_REG_MODIFY(phy->CR2, ADC_CR2_EXTEN, adc_cnf->regular_trg_edge); /* set the regular external trigger edge */
    CL_REG_MODIFY(phy->CR2, ADC_CR2_JEXTSEL, adc_cnf->injected_ext_trg);  /* set the injected external trigger */
    CL_REG_MODIFY(phy->CR2, ADC_CR2_JEXTEN, adc_cnf->injected_trg_edge); /* set the regular external trigger edge */

    //CL_REG_CLEAR(phy->CR2, ADC_CR2_CONT); /* Disable ADC continuous conversion mode */
    //CL_REG_CLEAR(phy->CR1, ADC_CR1_DISCEN); /* Disable the selected ADC regular discontinuous mode */
    //CL_REG_CLEAR(phy->CR2, ADC_CR2_EOCS); /* ADC end of conversion selection */

    adc_var->regular_age = 0;
    adc_var->injected_age = 0;
    adc_var->regular_channels = 0;
    adc_var->injected_channels = 0;

    coroutine_add(&adc_var->work, &_adc_sw_trigger, (void *)desc);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&adc_var->work);
  CL_REG_CLEAR(phy->CR2, ADC_CR2_ADON);
  CL_REG_CLEAR(RCC->APB2ENR, adc_cnf->rcc);
  return HDL_MODULE_UNLOADED;
}

const hdl_module_base_iface_t hdl_adc_iface = {
  .init = &_hdl_adc,
};

/*=======================================================*/

static void config_regular_channel(ADC_TypeDef *phy, uint32_t rank, uint32_t channel) { 
  /* For Rank 1 to 6 */
  if (rank < 7U) {
    CL_REG_CLEAR(phy->SQR3, ADC_SQR3_RK(ADC_SQR3_SQ1, rank)); /* Clear the old SQx bits for the selected rank */
    CL_REG_SET(phy->SQR3, ADC_SQR3_RK(channel, rank)); /* Set the SQx bits for the selected rank */
  }
  /* For Rank 7 to 12 */
  else if (rank < 13U) {
    CL_REG_CLEAR(phy->SQR2, ADC_SQR2_RK(ADC_SQR2_SQ7, rank)); /* Clear the old SQx bits for the selected rank */
    CL_REG_SET(phy->SQR2, ADC_SQR2_RK(channel, rank)); /* Set the SQx bits for the selected rank */
  }
  /* For Rank 13 to 16 */
  else {
    CL_REG_CLEAR(phy->SQR1, ADC_SQR1_RK(ADC_SQR1_SQ13, rank)); /* Clear the old SQx bits for the selected rank */
    CL_REG_SET(phy->SQR1, ADC_SQR1_RK(channel, rank)); /* Set the SQx bits for the selected rank */
  }
  /* if ADC1 Channel_18 is selected enable VBAT Channel */
  if ((phy == ADC1) && (channel == ADC_CHANNEL_VBAT)) {
    CL_REG_SET(ADC->CCR, ADC_CCR_VBATE); /* Enable the VBAT channel */
  }
  /* if ADC1 Channel_16 or Channel_17 is selected enable TSVREFE Channel(Temperature sensor and VREFINT) */
  if ((phy == ADC1) && ((channel == ADC_CHANNEL_TEMPSENSOR) || (channel == ADC_CHANNEL_VREFINT))) {
    CL_REG_SET(ADC->CCR, ADC_CCR_TSVREFE); /* Enable the TSVREFE channel*/
    if((channel == ADC_CHANNEL_TEMPSENSOR)) {
      /* Delay for temperature sensor stabilization time */
      /* Compute number of CPU cycles to wait for */
      uint32_t counter = (ADC_TEMPSENSOR_DELAY_US * (SystemCoreClock / 1000000U));
      while(counter != 0U) {
        counter--;
      }
    }
  }
}

static void config_channel_sampling(ADC_TypeDef *phy, uint32_t channel, uint32_t sample_time) { 
  /* if ADC_Channel_10 ... ADC_Channel_18 is selected */
  if (channel > ADC_CHANNEL_9) {
    CL_REG_CLEAR(phy->SMPR1, ADC_SMPR1(ADC_SMPR1_SMP10, channel)); /* Clear the old sample time */
    CL_REG_SET(phy->SMPR1, ADC_SMPR1(sample_time, channel)); /* Set the new sample time */
  }
  else { /* ADC_Channel include in ADC_Channel_[0..9] */
    CL_REG_CLEAR(phy->SMPR2, ADC_SMPR2(ADC_SMPR2_SMP0, channel)); /* Clear the old sample time */
    CL_REG_SET(phy->SMPR2, ADC_SMPR2(sample_time, channel)); /* Set the new sample time */
  }
}

static hdl_module_state_t _hdl_adc_ch(const void *desc, uint8_t enable) {
  hdl_adc_ch_mcu_t *adc_ch = (hdl_adc_ch_mcu_t *)desc;
  //hdl_adc_ch_var_t *adc_ch_var = (hdl_adc_ch_var_t *)adc_ch->obj_var;
  hdl_adc_ch_config_t *adc_ch_cnf = (hdl_adc_ch_config_t *)adc_ch->config;
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)adc_ch->dependencies[0];
  hdl_adc_config_t *adc_cnf = (hdl_adc_config_t *)adc->config;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)adc->obj_var;
  ADC_TypeDef *phy = (ADC_TypeDef *)adc->config->phy;
  if(enable) {
    //todo: stop trigger??
    CL_REG_CLEAR(phy->CR2, ADC_CR2_ADON);
    if(!(adc_ch_cnf->rank & HDL_ADC_CH_RANK_INJECTED_MASK)) {
      adc_cnf->adc_slots[adc_ch_cnf->rank - 1] = HDL_ADC_INVALID_VALUE;
      if(adc_var->regular_channels < adc_ch_cnf->rank) adc_var->regular_channels = adc_ch_cnf->rank;
      config_channel_sampling(phy, adc_ch_cnf->channel, adc_ch_cnf->sample_time);
      config_regular_channel(phy, adc_ch_cnf->rank, adc_ch_cnf->channel);
      hdl_dma_channel_t *dma = (hdl_dma_channel_t *)adc->dependencies[1];
      //if(!hdl_is_null_module(dma) && (adc_cnf->dma_slots != NULL)) {
        CL_REG_SET(phy->CR1, ADC_CR1_SCAN); /* Set ADC scan mode */
        CL_REG_SET(phy->CR2, ADC_CR2_DDS | ADC_CR2_DMA); /* Enable ADC DMA continuous request */
        hdl_dma_channel_stop(dma);
        hdl_dma_channel_run(dma, (void *)&phy->DR, adc->config->adc_slots, adc_var->regular_channels);
      //}
      CL_REG_MODIFY(phy->SQR1, ADC_SQR1_L, ADC_SQR1(adc_var->regular_channels)); /* Set ADC number of conversion */
    }
    else {
      uint8_t rank = adc_ch_cnf->rank & ~HDL_ADC_CH_RANK_INJECTED_MASK;
      config_channel_sampling(phy, adc_ch_cnf->channel, adc_ch_cnf->sample_time);
      CL_REG_CLEAR(phy->JSQR, ADC_JSQR_JSQ1 << (rank - 1)); /* Clear the old JSQx bits for the selected rank */
      CL_REG_SET(phy->JSQR, adc_ch_cnf->channel << (rank - 1)); /* Set the SQx bits for the selected rank */
      if(adc_var->injected_channels < rank) adc_var->injected_channels = rank;
      CL_REG_MODIFY(phy->JSQR, ADC_JSQR_JL, ADC_SQR1(adc_var->injected_channels)); /* Set ADC number of conversion */
    }
    CL_REG_SET(phy->CR2, ADC_CR2_ADON);
    return HDL_MODULE_ACTIVE;
  }
  // todo: deinit
  return HDL_MODULE_UNLOADED;
}

static uint32_t _hdl_adc_age(const void *desc) {
  hdl_adc_ch_mcu_t *adc_ch = (hdl_adc_ch_mcu_t *)desc;
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)adc_ch->dependencies[0];
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)adc->obj_var;
  if(adc_ch->config->rank & HDL_ADC_CH_RANK_INJECTED_MASK) 
    return adc_var->injected_age;
  return adc_var->regular_age;
}

static uint32_t _hdl_adc_get(const void *desc) {
  hdl_adc_ch_mcu_t *adc_ch = (hdl_adc_ch_mcu_t *)desc;
  hdl_adc_ch_config_t *adc_ch_cnf = (hdl_adc_ch_config_t *)adc_ch->config;
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)adc_ch->dependencies[0];
  hdl_adc_config_t *adc_cnf = (hdl_adc_config_t *)adc->config;
  ADC_TypeDef *phy = (ADC_TypeDef *)adc->config->phy;
  if(adc_ch->config->rank & HDL_ADC_CH_RANK_INJECTED_MASK) 
    return (&phy->JDR1)[(adc_ch->config->rank & ~HDL_ADC_CH_RANK_INJECTED_MASK) - 1];
  return adc_cnf->adc_slots[adc_ch_cnf->rank - 1];
}

const hdl_adc_ch_iface_t hdl_adc_ch_iface = {
  .init = &_hdl_adc_ch,
  .value = &_hdl_adc_get,
  .age = &_hdl_adc_age
};
