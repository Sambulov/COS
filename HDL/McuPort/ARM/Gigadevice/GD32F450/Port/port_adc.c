#include "hdl_iface.h"

typedef enum {
  GD_ADC_STATE_MACHINE_INITIAL,
  GD_ADC_STATE_MACHINE_CALIBRATION,
  GD_ADC_STATE_MACHINE_RUN,
  GD_ADC_STATE_MACHINE_WORKING,
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
  adc_var->age++;
}

static void event_adc_start_conversion(uint32_t event, void *sender, void *context) {
  (void)event; (void)sender;
  hdl_adc_mcu_t *adc = (hdl_adc_mcu_t *)context;
  adc_software_trigger_enable(adc->config->phy, ADC_ROUTINE_CHANNEL);
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

  rcu_periph_enum rcu;
  switch (adc->config->phy) {
    case ADC0:
      rcu = RCU_ADC0;
      break;
    case ADC1:
      rcu = RCU_ADC1;
      break;
    case ADC2:
      rcu = RCU_ADC2;
      break;
    default:
      return HDL_MODULE_FAULT;
  }

  /* TODO: SEE ADC_REGULAR_INSERTED_CHANNEL */
  if(enable) {
    switch (adc_var->state_machine){
      case GD_ADC_STATE_MACHINE_INITIAL: {
        rcu_periph_clock_enable(rcu);
        adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
        adc_special_function_config(adc->config->phy, ADC_SCAN_MODE, ENABLE);
        adc_special_function_config(adc->config->phy, ADC_CONTINUOUS_MODE, DISABLE);
        const hdl_adc_source_t * const *adc_source = adc->config->sources;
        adc_var->channels_count = 0;
        if(adc_source != NULL) {
          while (*adc_source != NULL) {
            adc_routine_channel_config(adc->config->phy, adc_var->channels_count, (uint8_t)(*adc_source)->channel, (uint32_t)(*adc_source)->sample_time);
            //adc_regular_channel_config(adc_var->channels_count, (uint8_t)(*adc_source)->channel, (uint32_t)(*adc_source)->sample_time);
            adc_source++;
            adc->config->values[adc_var->channels_count++] = (uint16_t)HDL_ADC_INVALID_VALUE;
          }
        }
        adc_channel_length_config(adc->config->phy, ADC_ROUTINE_CHANNEL, adc_var->channels_count);
        adc_data_alignment_config(adc->config->phy, adc->config->data_alignment);
        adc_resolution_config(adc->config->phy, (uint32_t)adc->config->resolution);
        //adc_external_trigger_config(adc->config->phy, ADC_ROUTINE_CHANNEL, ENABLE);
        //adc_external_trigger_source_config(adc->config->phy, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);
        adc_external_trigger_source_config(adc->config->phy, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T0_CH0); 
        adc_external_trigger_config(adc->config->phy, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);
        adc_dma_request_after_last_enable(adc->config->phy);
        adc_enable(adc->config->phy);
        hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)adc->dependencies[3];
        adc_var->adc_end_of_conversion.context = adc;
        adc_var->adc_end_of_conversion.handler = &event_adc_end_of_conversion;
        hdl_event_subscribe(&adc->config->adc_interrupt->event, &adc_var->adc_end_of_conversion);
        hdl_interrupt_request(ic, adc->config->adc_interrupt);
        ADC_CTL0(adc->config->phy) |= ADC_CTL0_EOCIE;

        /* There must be 14 CK_ADC tact */
        for(uint16_t i = 0; i < 10 * 14; i++)
          __NOP();
        
        ADC_CTL1(adc->config->phy) |= (uint32_t)ADC_CTL1_RSTCLB;
        /* TODO: Should amend */
        while ((ADC_CTL1(adc->config->phy) & ADC_CTL1_RSTCLB));
        ADC_CTL1(adc->config->phy) |= ADC_CTL1_CLB;
        adc_var->age = hdl_time_counter_get(timer);
        adc_var->state_machine = GD_ADC_STATE_MACHINE_CALIBRATION;
        break;
      }
      case GD_ADC_STATE_MACHINE_CALIBRATION:
        if (ADC_CTL1(adc->config->phy) & ADC_CTL1_CLB) {
          if (TIME_ELAPSED(adc_var->age, adc->config->init_timeout, hdl_time_counter_get(timer)))
            return HDL_MODULE_FAULT;
          break;
        }
        adc_var->state_machine = GD_ADC_STATE_MACHINE_RUN;
        break;
      case GD_ADC_STATE_MACHINE_RUN:
        adc_dma_mode_enable(adc->config->phy);
        hdl_dma_channel_run(dma, (uint32_t)&ADC_RDATA(adc->config->phy), (uint32_t)adc->config->values, (uint32_t)adc_var->channels_count);
        adc_var->age = 0;
        adc_var->start_conversion.context = adc;
        adc_var->start_conversion.handler = &event_adc_start_conversion;
        hdl_event_subscribe(&timer->config->reload_interrupt->event, &adc_var->start_conversion);
        //adc_software_trigger_enable(adc->config->phy, ADC_ROUTINE_CHANNEL);
        adc_var->state_machine = GD_ADC_STATE_MACHINE_WORKING;
        break;
      case GD_ADC_STATE_MACHINE_WORKING:
        return HDL_MODULE_ACTIVE;
      default:
        return HDL_MODULE_FAULT;
    }
  }
  else {
    adc_disable(adc->config->phy);
    adc_dma_mode_disable(adc->config->phy);
    rcu_periph_clock_disable(rcu);
    adc_var->state_machine = GD_ADC_STATE_MACHINE_INITIAL;
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
