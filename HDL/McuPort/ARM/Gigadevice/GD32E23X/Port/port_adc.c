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

static void event_adc_end_of_conversion(uint32_t event, void *sender, void *context) {
  (void)event; (void)sender;
  hdl_adc_mcu_t *hdl_adc = (hdl_adc_mcu_t *)context;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)hdl_adc->obj_var;
  adc_var->age++;
}

static void event_adc_start_conversion(uint32_t event, void *sender, void *context) {
  (void)event; (void)sender; (void)context;
  adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
}

static hdl_module_state_t _hdl_adc(const void *desc, uint8_t enable){
  hdl_adc_mcu_t *hdl_adc = (hdl_adc_mcu_t *)desc;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)hdl_adc->obj_var;
  if(!hdl_adc->config->phy || (hdl_adc->dependencies == NULL) || (hdl_adc->dependencies[0] == NULL) ||
    (hdl_adc->dependencies[1] == NULL) || (hdl_adc->dependencies[2] == NULL))
    return HDL_MODULE_FAULT;
  //hdl_clock_t *clock = (hdl_clock_t *)hdl_adc->dependencies[0];
  hdl_time_counter_t *timer = (hdl_time_counter_t *)hdl_adc->dependencies[1];
  hdl_dma_channel_mcu_t *dma = (hdl_dma_channel_mcu_t *)hdl_adc->dependencies[2];
  /* TODO: SEE ADC_REGULAR_INSERTED_CHANNEL */
  if(enable) {
    switch (adc_var->state_machine){
      case GD_ADC_STATE_MACHINE_INITIAL: {
        rcu_periph_clock_enable(RCU_ADC);
        adc_special_function_config(ADC_SCAN_MODE, ENABLE);
        adc_special_function_config(ADC_CONTINUOUS_MODE, DISABLE);
        hdl_adc_source_t **adc_source = hdl_adc->config->sources;
        adc_var->channels_count = 0;
        if(adc_source != NULL) {
          while (*adc_source != NULL) {
            adc_regular_channel_config(adc_var->channels_count, (uint8_t)(*adc_source)->channel, (uint32_t)(*adc_source)->sample_time);
            adc_source++;
            hdl_adc->config->values[adc_var->channels_count++] = HDL_ADC_INVALID_VALUE;
          }
        }
        adc_channel_length_config(ADC_REGULAR_CHANNEL, adc_var->channels_count);
        adc_data_alignment_config(hdl_adc->config->data_alignment);
        adc_resolution_config((uint32_t)hdl_adc->config->resolution);
        adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
        adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);
        adc_enable();
        hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)hdl_adc->dependencies[3];
        adc_var->adc_end_of_conversion.context = hdl_adc;
        adc_var->adc_end_of_conversion.handler = &event_adc_end_of_conversion;
        hdl_event_subscribe(&hdl_adc->config->adc_interrupt->event, &adc_var->adc_end_of_conversion);
        hdl_interrupt_request(ic, hdl_adc->config->adc_interrupt);
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
          if (TIME_ELAPSED(adc_var->age, hdl_adc->config->init_timeout, hdl_time_counter_get(timer)))
            return HDL_MODULE_FAULT;
          break;
        }
        adc_var->state_machine = GD_ADC_STATE_MACHINE_RUN;
        break;
      case GD_ADC_STATE_MACHINE_RUN:
        adc_dma_mode_enable();
        hdl_dma_channel_run(dma, (uint32_t)&ADC_RDATA, (uint32_t)hdl_adc->config->values, (uint32_t)adc_var->channels_count);
        adc_var->age = 0;
        adc_var->start_conversion.context = hdl_adc;
        adc_var->start_conversion.handler = &event_adc_start_conversion;
        hdl_event_subscribe(&timer->config->reload_interrupt->event, &adc_var->start_conversion);
        //adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
        adc_var->state_machine = GD_ADC_STATE_MACHINE_WORKING;
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

static uint32_t _hdl_adc_get(const void *desc, uint32_t src) {
  hdl_adc_mcu_t *hdl_adc = (hdl_adc_mcu_t *)desc;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)hdl_adc->obj_var;
  if((hdl_state(hdl_adc) == HDL_MODULE_ACTIVE) && (adc_var->channels_count > src))
    return hdl_adc->config->values[src];
  return HDL_ADC_INVALID_VALUE;
}

static uint32_t _hdl_adc_age(const void *desc) {
  hdl_adc_mcu_t *hdl_adc = (hdl_adc_mcu_t *)desc;
  hdl_adc_var_t *adc_var = (hdl_adc_var_t *)hdl_adc->obj_var;
  if(hdl_state(hdl_adc) == HDL_MODULE_ACTIVE) return adc_var->age;
  return 0;
}

const hdl_adc_iface_t hdl_adc_iface = {
  .init = &_hdl_adc,
  .get = &_hdl_adc_get,
  .age = &_hdl_adc_age
};
