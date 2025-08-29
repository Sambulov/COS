#include "hdl_iface.h"

typedef enum {
  GD_ADC_STATE_MACHINE_INITIAL,
  GD_ADC_STATE_MACHINE_CALIBRATION,
  GD_ADC_STATE_MACHINE_RUN,
  GD_ADC_STATE_MACHINE_WORKING,
} gd_adc_state_machine_e;

typedef struct{
  hdl_delegate_t start_conversion;
  hdl_delegate_t adc_end_of_conversion;
  uint32_t age;
  gd_adc_state_machine_e state_machine;      /* gd_adc_state_machine_e */
  uint8_t channels_count;    
} hdl_adc_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_adc_var_t, *((hdl_adc_mcu_t *)0)->obj_var, HDL_ADC_VAR_SIZE, port_adc.h);

static void event_adc_end_of_conversion(void *event, void *sender, void *context) {
  (void)event; (void)sender;
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)context;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)adc->obj_var;
  adc_var->age++;
}

static void event_adc_start_conversion(void *event, void *sender, void *context) {
  (void)event; (void)sender; (void)context;
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)context;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)adc->obj_var;
  if(adc_var->channels_count > 0)
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
}

static hdl_module_state_t _hdl_adc(const void *desc, uint8_t enable){
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)desc;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)adc->obj_var;
  if(!adc->config->phy || (adc->dependencies == NULL) || (adc->dependencies[0] == NULL) ||
    (adc->dependencies[1] == NULL) || (adc->dependencies[2] == NULL))
    return HDL_MODULE_FAULT;
  hdl_time_counter_t *timer = (hdl_time_counter_t *)adc->dependencies[1];
  /* TODO: SEE ADC_REGULAR_INSERTED_CHANNEL */
  if(enable) {
    switch (adc_var->state_machine){
      case GD_ADC_STATE_MACHINE_INITIAL: {
        rcu_periph_clock_enable(RCU_ADC);
        adc_special_function_config(ADC_SCAN_MODE, ENABLE);
        adc_special_function_config(ADC_CONTINUOUS_MODE, DISABLE);
        adc_var->channels_count = 0;
        adc_data_alignment_config(adc->config->data_alignment);
        adc_resolution_config((uint32_t)adc->config->resolution);
        adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
        adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);
        adc_enable();
        adc_var->adc_end_of_conversion.context = adc;
        adc_var->adc_end_of_conversion.handler = &event_adc_end_of_conversion;
        hdl_interrupt_request(adc->dependencies[3], &adc_var->adc_end_of_conversion);
        //         for(uint16_t i = 0; i < adc_short_delay_after_start; i++)
        //     __NOP();
        /* There must be 14 CK_ADC tact */
        ADC_CTL1 |= (uint32_t)ADC_CTL1_RSTCLB;
        while ((ADC_CTL1 & ADC_CTL1_RSTCLB));
        ADC_CTL1 |= ADC_CTL1_CLB;
        adc_var->age = hdl_time_counter_get(timer);
        adc_var->state_machine = GD_ADC_STATE_MACHINE_CALIBRATION;
        break;
      }
      case GD_ADC_STATE_MACHINE_CALIBRATION:
        if (ADC_CTL1 & ADC_CTL1_CLB) {
          if (CL_TIME_ELAPSED(adc_var->age, adc->config->init_timeout, hdl_time_counter_get(timer)))
            return HDL_MODULE_FAULT;
          break;
        }
        adc_var->state_machine = GD_ADC_STATE_MACHINE_RUN;
        break;
      case GD_ADC_STATE_MACHINE_RUN:
        adc_dma_mode_enable();
        adc_var->age = 0;
        adc_var->start_conversion.context = adc;
        adc_var->start_conversion.handler = &event_adc_start_conversion;
        hdl_interrupt_request(timer->dependencies[1], &adc_var->start_conversion);
        //adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
        adc_var->state_machine = GD_ADC_STATE_MACHINE_WORKING;
        ADC_CTL0 |= ADC_CTL0_EOCIE;
        return HDL_MODULE_ACTIVE;
      case GD_ADC_STATE_MACHINE_WORKING:
        return HDL_MODULE_ACTIVE;
      default:
        return HDL_MODULE_FAULT;
    }
  }
  else {
    adc_disable();
    adc_dma_mode_disable();
    rcu_periph_clock_disable(RCU_ADC);
    adc_var->state_machine = GD_ADC_STATE_MACHINE_INITIAL;
    return HDL_MODULE_UNLOADED;
  }
  return HDL_MODULE_LOADING;
}

const hdl_module_base_iface_t hdl_adc_iface = {
  .init = &_hdl_adc,
};

static hdl_module_state_t _hdl_adc_ch(const void *desc, uint8_t enable){
  hdl_adc_ch_mcu_t *adc_ch = (hdl_adc_ch_mcu_t *)desc;
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)adc_ch->dependencies[0];
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)adc->obj_var;
  hdl_dma_channel_mcu_t *dma = (hdl_dma_channel_mcu_t *)adc->dependencies[2];
  if(enable) {
    adc_disable();
    hdl_dma_channel_stop(dma);
    adc->config->adc_slots[adc_var->channels_count++] = HDL_ADC_INVALID_VALUE;
    adc_regular_channel_config(adc_ch->config->rank - 1, adc_ch->config->channel, adc_ch->config->sample_time);
    adc_channel_length_config(ADC_REGULAR_CHANNEL, adc_var->channels_count);
    adc_enable();
    hdl_dma_channel_run(dma, (void *)&ADC_RDATA, adc->config->adc_slots, adc_var->channels_count);
    return HDL_MODULE_ACTIVE;
  }
  /* todo */
  return HDL_MODULE_UNLOADED;
}

static uint32_t _hdl_adc_get(const void *desc) {
  hdl_adc_ch_mcu_t *adc_ch = (hdl_adc_ch_mcu_t *)desc;
  hdl_adc_ch_config_t *adc_ch_cnf = (hdl_adc_ch_config_t *)adc_ch->config;
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)adc_ch->dependencies[0];
  hdl_adc_config_t *adc_cnf = (hdl_adc_config_t *)adc->config;
  if(adc_ch->config->rank & HDL_ADC_CH_RANK_INJECTED_MASK) 
    return REG32(ADC + 0x3CU +(adc->config->phy + ((adc_ch->config->rank & ~HDL_ADC_CH_RANK_INJECTED_MASK) - 1) * 4));
  return adc_cnf->adc_slots[adc_ch_cnf->rank - 1];
}

static uint32_t _hdl_adc_age(const void *desc) {
  hdl_adc_ch_mcu_t *adc_ch = (hdl_adc_ch_mcu_t *)desc;
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)adc_ch->dependencies[0];
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)adc->obj_var;
  //if(adc_ch->config->rank & HDL_ADC_CH_RANK_INJECTED_MASK) 
  //  return adc_var->age;
  return adc_var->age;
}

const hdl_adc_ch_iface_t hdl_adc_ch_iface = {
  .init = &_hdl_adc_ch,
  .value = &_hdl_adc_get,
  .age = &_hdl_adc_age
};
