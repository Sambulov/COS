#include "hdl_iface.h"

typedef enum {
  ADC_STATE_MACHINE_INITIAL,
  ADC_STATE_MACHINE_CALIBRATION,
  ADC_STATE_MACHINE_RUN,
  ADC_STATE_MACHINE_WORKING,
} gd_adc_state_machine_e;

typedef struct{
  hdl_delegate_t adc_end_of_conversion;
  hdl_delegate_t start_conversion;
  uint32_t age;
  gd_adc_state_machine_e state_machine;      /* gd_adc_state_machine_e */
  uint8_t channels_count;
} hdl_adc_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_adc_mcu_t, hdl_adc_mcu_t, HDL_ADC_PRV_SIZE, port_adc.h);

static void event_adc_end_of_conversion(uint32_t event, void *sender, void *context) {
  (void)event; (void)sender;
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)context;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)adc->obj_var;
  hdl_adc_config_t *adc_cnf = (hdl_adc_config_t *)adc->config;
  ADC_TypeDef *phy = (ADC_TypeDef *)adc_cnf->phy;
  adc_var->age++;
  CL_REG_CLEAR(phy->SR, ADC_SR_EOC);
}

static void event_adc_start_conversion(uint32_t event, void *sender, void *context) {
  (void)event; (void)sender;
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)context;
  ADC_TypeDef *phy = (ADC_TypeDef *)adc->config->phy;
  CL_REG_SET(phy->CR2, ADC_CR2_ADON);
  phy->SR = ~(ADC_FLAG_EOC | ADC_FLAG_OVR);
  CL_REG_SET(phy->CR2, ADC_CR2_SWSTART);
}

static void config_channel(ADC_TypeDef *phy, uint32_t rank, uint32_t channel, uint32_t sample_time) { 
  /* if ADC_Channel_10 ... ADC_Channel_18 is selected */
  if (channel > ADC_CHANNEL_9) {
    CL_REG_CLEAR(phy->SMPR1, ADC_SMPR1(ADC_SMPR1_SMP10, channel)); /* Clear the old sample time */
    CL_REG_SET(phy->SMPR1, ADC_SMPR1(sample_time, channel)); /* Set the new sample time */
  }
  else { /* ADC_Channel include in ADC_Channel_[0..9] */
    CL_REG_CLEAR(phy->SMPR2, ADC_SMPR2(ADC_SMPR2_SMP0, channel)); /* Clear the old sample time */
    CL_REG_SET(phy->SMPR2, ADC_SMPR2(sample_time, channel)); /* Set the new sample time */
  }
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

static hdl_module_state_t _hdl_adc(const void *desc, uint8_t enable){
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)desc;
  if(!adc->config->phy || (adc->dependencies == NULL) || (adc->dependencies[0] == NULL) ||
    (adc->dependencies[1] == NULL) || (adc->dependencies[2] == NULL) || (adc->dependencies[3] == NULL))
      return HDL_MODULE_FAULT;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)adc->obj_var;
  //hdl_clock_t *clock = (hdl_clock_t *)adc->dependencies[0];
  hdl_time_counter_t *timer = (hdl_time_counter_t *)adc->dependencies[1];
  hdl_dma_channel_t *dma = (hdl_dma_channel_t *)adc->dependencies[2];
  hdl_adc_config_t *adc_cnf = (hdl_adc_config_t *)adc->config;
  ADC_TypeDef *phy = (ADC_TypeDef *)adc_cnf->phy;
  if(enable) {
    switch (adc_var->state_machine) {
      case ADC_STATE_MACHINE_INITIAL: {
        CL_REG_SET(RCC->APB2ENR, adc_cnf->rcc);
        /* Set ADC parameters */
        ADC->CCR &= ~(ADC_CCR_ADCPRE);
        ADC->CCR |=  (((adc_cnf->prescaler >> 1) - 1) << 16); /* Set the ADC clock prescaler */
        CL_REG_SET(phy->CR1, ADC_CR1_SCAN); /* Set ADC scan mode */
        CL_REG_MODIFY(phy->CR1, ADC_CR1_RES, adc_cnf->resolution); /* Set ADC resolution */
        CL_REG_MODIFY(phy->CR2, ADC_CR2_ALIGN, adc_cnf->data_alignment); /* Set ADC data alignment */
        CL_REG_CLEAR(phy->CR2,(ADC_CR2_EXTSEL | ADC_CR2_EXTEN)); /* Reset the external trigger */
        CL_REG_CLEAR(phy->CR2, ADC_CR2_CONT); /* Enable or disable ADC continuous conversion mode */
        CL_REG_CLEAR(phy->CR1, ADC_CR1_DISCEN); /* Disable the selected ADC regular discontinuous mode */
        const hdl_adc_source_t * const *adc_source = adc->config->sources;
        adc_var->channels_count = 0;
        if(adc_source != NULL) {
          while (*adc_source != NULL) {
            config_channel(phy, adc_var->channels_count, (uint8_t)(*adc_source)->channel, (uint32_t)(*adc_source)->sample_time);
            adc_source++;
            adc->config->values[adc_var->channels_count++] = HDL_ADC_INVALID_VALUE;
          }
        }
        CL_REG_MODIFY(phy->SQR1, ADC_SQR1_L, ADC_SQR1(adc_var->channels_count)); /* Set ADC number of conversion */
        CL_REG_SET(phy->CR2, ADC_CR2_DDS); /* Enable ADC DMA continuous request */
        CL_REG_CLEAR(phy->CR2, ADC_CR2_EOCS); /* ADC end of conversion selection */
        CL_REG_SET(phy->CR2, ADC_CR2_ADON); 

        hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)adc->dependencies[3];
        adc_var->adc_end_of_conversion.context = adc;
        adc_var->adc_end_of_conversion.handler = &event_adc_end_of_conversion;
        hdl_event_subscribe(&adc->config->adc_interrupt->event, &adc_var->adc_end_of_conversion);
        hdl_interrupt_request(ic, adc->config->adc_interrupt);        
        CL_REG_SET(phy->CR1, ADC_CR1_EOCIE); 
        adc_var->age = hdl_time_counter_get(timer);
        adc_var->state_machine = ADC_STATE_MACHINE_CALIBRATION;
      }
      /* fall through */
      case ADC_STATE_MACHINE_CALIBRATION:
          if((phy->CR2 & ADC_CR2_ADON) != ADC_CR2_ADON){
            if (CL_TIME_ELAPSED(adc_var->age, adc->config->init_timeout, hdl_time_counter_get(timer)))
              return HDL_MODULE_FAULT;
            break;
          }
          adc_var->state_machine = ADC_STATE_MACHINE_RUN;
      /* fall through */
      case ADC_STATE_MACHINE_RUN:
        /* Clear regular group conversion flag and overrun flag */
        /* (To ensure of no unknown state from potential previous ADC operations) */
        CL_REG_CLEAR(phy->SR, ADC_FLAG_EOC | ADC_FLAG_OVR);
        phy->CR2 |= ADC_CR2_DMA; /* Enable ADC DMA mode */
        hdl_dma_channel_run(dma, (void *)&phy->DR, adc->config->values, adc_var->channels_count);
        adc_var->age = 0;
        adc_var->start_conversion.context = adc;
        adc_var->start_conversion.handler = &event_adc_start_conversion;
        hdl_event_subscribe(&timer->config->reload_interrupt->event, &adc_var->start_conversion);
        adc_var->state_machine = ADC_STATE_MACHINE_WORKING;
        break;
      case ADC_STATE_MACHINE_WORKING:
        return HDL_MODULE_ACTIVE;
      default:
        return HDL_MODULE_FAULT;
    }
  }
  else {
    CL_REG_CLEAR(phy->CR2, ADC_CR2_ADON);
    CL_REG_CLEAR(RCC->APB2ENR, adc_cnf->rcc);
    adc_var->state_machine = ADC_STATE_MACHINE_INITIAL;
    return HDL_MODULE_UNLOADED;
  }
  return HDL_MODULE_LOADING;
}

static uint32_t _hdl_adc_age(const void *desc) {
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)desc;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)adc->obj_var;
  return adc_var->age;
}

static uint32_t _hdl_adc_get(const void *desc, uint32_t src) {
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)desc;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)adc->obj_var;
  if(adc_var->channels_count > src) {
    return adc->config->values[src];
  }
  return HDL_ADC_INVALID_VALUE;
}

const hdl_adc_iface_t hdl_adc_iface = {
  .init = &_hdl_adc,
  .get = &_hdl_adc_get,
  .age = &_hdl_adc_age
};
