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

  //uint32_t regular_age;
  //uint32_t injected_age;
  //uint8_t regular_channels;
  //uint8_t injected_channels;

} hdl_adc_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_adc_var_t, *((hdl_adc_mcu_t *)0)->obj_var, HDL_ADC_VAR_SIZE, port_adc.h);

static void event_adc_end_of_conversion(void *event, void *sender, void *context) {
  (void)event; (void)sender;
  hdl_adc_mcu_t *hdl_adc = (hdl_adc_mcu_t *)context;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)hdl_adc->obj_var;
  CL_REG_CLEAR(ADC_STAT(hdl_adc->config->phy), ADC_STAT_EOC);
  adc_var->age++;
}

static void event_adc_start_conversion(void *event, void *sender, void *context) {
  (void)event; (void)sender; (void)context;
  hdl_adc_mcu_t *hdl_adc = (hdl_adc_mcu_t *)context;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)hdl_adc->obj_var;
  if(adc_var->channels_count > 0) adc_software_trigger_enable(hdl_adc->config->phy, ADC_REGULAR_CHANNEL);
}

static hdl_module_state_t _hdl_adc(const void *desc, uint8_t enable) {
  hdl_adc_mcu_t *hdl_adc = (hdl_adc_mcu_t *)desc;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)hdl_adc->obj_var;
  if(!hdl_adc->config->phy || (hdl_adc->dependencies == NULL) || (hdl_adc->dependencies[0] == NULL) ||
    (hdl_adc->dependencies[1] == NULL) || (hdl_adc->dependencies[2] == NULL))
    return HDL_MODULE_FAULT;
  hdl_time_counter_t *timer = (hdl_time_counter_t *)hdl_adc->dependencies[1];
  /* TODO: SEE ADC_REGULAR_INSERTED_CHANNEL */
  if(enable) {
    switch (adc_var->state_machine){
      case GD_ADC_STATE_MACHINE_INITIAL: {
        rcu_periph_clock_enable(hdl_adc->config->rcu);
        adc_special_function_config(hdl_adc->config->phy, ADC_SCAN_MODE, ENABLE);
        adc_special_function_config(hdl_adc->config->phy, ADC_CONTINUOUS_MODE, DISABLE);
        adc_data_alignment_config(hdl_adc->config->phy, hdl_adc->config->data_alignment);
        //adc_resolution_config(hdl_adc->config->phy, (uint32_t)hdl_adc->config->resolution);
        adc_external_trigger_config(hdl_adc->config->phy, ADC_REGULAR_CHANNEL, ENABLE);
        adc_external_trigger_source_config(hdl_adc->config->phy, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
        adc_var->channels_count = 0;
        adc_var->adc_end_of_conversion.context = hdl_adc;
        adc_var->adc_end_of_conversion.handler = &event_adc_end_of_conversion;
        hdl_interrupt_request(hdl_adc->dependencies[3], &adc_var->adc_end_of_conversion);
        //adc_channel_length_config(hdl_adc->config->phy, ADC_REGULAR_CHANNEL, 0);
        adc_enable(hdl_adc->config->phy);
        //         for(uint16_t i = 0; i < adc_short_delay_after_start; i++)
        //     __NOP();
        /* There must be 14 CK_ADC tact */
        ADC_CTL1(hdl_adc->config->phy) |= (uint32_t)ADC_CTL1_RSTCLB;
        while ((ADC_CTL1(hdl_adc->config->phy) & ADC_CTL1_RSTCLB));
        ADC_CTL1(hdl_adc->config->phy) |= ADC_CTL1_CLB;
        adc_var->age = 0;
        adc_var->state_machine = GD_ADC_STATE_MACHINE_CALIBRATION;
        break;
      }
      case GD_ADC_STATE_MACHINE_CALIBRATION:
        if (ADC_CTL1(hdl_adc->config->phy) & ADC_CTL1_CLB) {
          if (CL_TIME_ELAPSED(adc_var->age, hdl_adc->config->init_timeout, hdl_time_counter_get(timer)))
            return HDL_MODULE_FAULT;
          break;
        }
        adc_var->state_machine = GD_ADC_STATE_MACHINE_RUN;
        break;
      case GD_ADC_STATE_MACHINE_RUN:
        adc_dma_mode_enable(hdl_adc->config->phy);
        adc_var->age = 0;
        adc_var->start_conversion.context = hdl_adc;
        adc_var->start_conversion.handler = &event_adc_start_conversion;
        hdl_interrupt_request(timer->dependencies[1], &adc_var->start_conversion);
        //adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
        adc_var->state_machine = GD_ADC_STATE_MACHINE_WORKING;
        ADC_CTL0(hdl_adc->config->phy) |= ADC_CTL0_EOCIE;
        return HDL_MODULE_ACTIVE;
      case GD_ADC_STATE_MACHINE_WORKING:
        return HDL_MODULE_ACTIVE;
      default:
        return HDL_MODULE_FAULT;
    }
  }
  else {
    adc_disable(hdl_adc->config->phy);
    adc_dma_mode_disable(hdl_adc->config->phy);
    rcu_periph_clock_disable(hdl_adc->config->rcu);
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
  /* TODO: SEE ADC_REGULAR_INSERTED_CHANNEL */
  if(enable) {
    adc_disable(adc->config->phy);
    hdl_dma_channel_stop(dma);
    adc->config->adc_slots[adc_var->channels_count++] = HDL_ADC_INVALID_VALUE;
    adc_regular_channel_config(adc->config->phy, adc_ch->config->rank - 1, adc_ch->config->channel, adc_ch->config->sample_time);
    adc_channel_length_config(adc->config->phy, ADC_REGULAR_CHANNEL, adc_var->channels_count);
    adc_enable(adc->config->phy);
    hdl_dma_channel_run(dma, (void *)&ADC_RDATA(adc->config->phy), adc->config->adc_slots, adc_var->channels_count);
    return HDL_MODULE_ACTIVE;
  }
  /* todo unload ch */
  return HDL_MODULE_UNLOADED;
}

static uint32_t _hdl_adc_get(const void *desc) {
  hdl_adc_ch_mcu_t *adc_ch = (hdl_adc_ch_mcu_t *)desc;
  hdl_adc_ch_config_t *adc_ch_cnf = (hdl_adc_ch_config_t *)adc_ch->config;
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)adc_ch->dependencies[0];
  hdl_adc_config_t *adc_cnf = (hdl_adc_config_t *)adc->config;
  if(adc_ch->config->rank & HDL_ADC_CH_RANK_INJECTED_MASK) 
    return ADC_IDATA0(adc->config->phy + ((adc_ch->config->rank & ~HDL_ADC_CH_RANK_INJECTED_MASK) - 1) * 4);
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
