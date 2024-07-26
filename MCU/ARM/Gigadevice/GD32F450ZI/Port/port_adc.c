#include "hdl_portable.h"
#include "Macros.h"

typedef enum {
  GD_ADC_STATE_MACHINE_INITIAL,
  GD_ADC_STATE_MACHINE_CALIBRATION,
  GD_ADC_STATE_MACHINE_RUN,
  GD_ADC_STATE_MACHINE_WORKING,
} gd_adc_state_machine_e;

typedef struct{
    hdl_module_t module;
    hdl_nvic_irq_n_t adc_iterrupt;
    hdl_delegate_t adc_end_of_conversion;
    hdl_adc_resolution_e resolution;
    hdl_adc_data_alignment_t data_alignment;
    uint32_t init_timeout;
    hdl_adc_source_t **sources;               /* max amount 15 */
    uint16_t *values;
    uint32_t time_stamp;
    gd_adc_state_machine_e state_machine;      /* gd_adc_state_machine_e */
    uint8_t channels_count;
} hdl_adc_private_t;

_Static_assert(sizeof(hdl_adc_private_t) == sizeof(hdl_adc_t), "In hdl_adc.h data structure size of hdl_adc_t doesn't match, check HDL_ADC_PRIVATE_FIELD_SIZE");

static void event_adc_end_of_conversion(uint32_t event, void *sender, void *context) {
    hdl_adc_private_t *hdl_adc = (hdl_adc_private_t *)context;
    hdl_timer_t *timer = (hdl_timer_t *)hdl_adc->module.dependencies[1];
    hdl_adc->time_stamp = hdl_timer_get(timer);
}

hdl_module_state_t hdl_adc(void *desc, uint8_t enable){
  hdl_adc_private_t *hdl_adc = (hdl_adc_private_t *)desc;
  if(hdl_adc->module.reg == NULL || hdl_adc->module.dependencies == NULL || hdl_adc->module.dependencies[0] == NULL ||
    hdl_adc->module.dependencies[1] == NULL || hdl_adc->module.dependencies[2] == NULL || hdl_adc->sources == NULL || 
    hdl_adc->sources[0] == NULL || hdl_adc->module.dependencies[3] == NULL)
      return HDL_MODULE_INIT_FAILED;
  //hdl_clock_t *clock = (hdl_clock_t *)hdl_adc->module.dependencies[0];
  hdl_timer_t *timer = (hdl_timer_t *)hdl_adc->module.dependencies[1];
  hdl_dma_channel_t *dma = (hdl_dma_channel_t *)hdl_adc->module.dependencies[2];

  rcu_periph_enum rcu;
  switch ((uint32_t)hdl_adc->module.reg) {
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
      return HDL_MODULE_INIT_FAILED;
  }

  /* TODO: SEE ADC_REGULAR_INSERTED_CHANNEL */
  if(enable) {
    switch (hdl_adc->state_machine){
      case GD_ADC_STATE_MACHINE_INITIAL: {
        rcu_periph_clock_enable(rcu);
        adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
        adc_special_function_config((uint32_t)hdl_adc->module.reg, ADC_SCAN_MODE, ENABLE);
        adc_special_function_config((uint32_t)hdl_adc->module.reg, ADC_CONTINUOUS_MODE, ENABLE);
        hdl_adc_source_t **adc_source = hdl_adc->sources;
        hdl_adc->channels_count = 0;
        while (*adc_source != NULL) {
          adc_routine_channel_config((uint32_t)hdl_adc->module.reg, hdl_adc->channels_count, (uint8_t)(*adc_source)->channel, (uint32_t)(*adc_source)->sample_time);
          //adc_regular_channel_config(hdl_adc->channels_count, (uint8_t)(*adc_source)->channel, (uint32_t)(*adc_source)->sample_time);
          adc_source++;
          hdl_adc->values[hdl_adc->channels_count++] = (uint16_t)HDL_ADC_INVALID_VALUE;
        }
        adc_channel_length_config((uint32_t)hdl_adc->module.reg, ADC_ROUTINE_CHANNEL, hdl_adc->channels_count);
        adc_data_alignment_config((uint32_t)hdl_adc->module.reg, hdl_adc->data_alignment);
        adc_resolution_config((uint32_t)hdl_adc->module.reg, (uint32_t)hdl_adc->resolution);
        //adc_external_trigger_config((uint32_t)hdl_adc->module.reg, ADC_ROUTINE_CHANNEL, ENABLE);
        //adc_external_trigger_source_config((uint32_t)hdl_adc->module.reg, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);
        adc_external_trigger_source_config((uint32_t)hdl_adc->module.reg, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T0_CH0); 
        adc_external_trigger_config((uint32_t)hdl_adc->module.reg, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);
        adc_dma_request_after_last_enable((uint32_t)hdl_adc->module.reg);
        ADC_CTL0((uint32_t)hdl_adc->module.reg) |= ADC_CTL0_EOCIE;
        adc_enable((uint32_t)hdl_adc->module.reg);
        hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)hdl_adc->module.dependencies[3];
        hdl_adc->adc_end_of_conversion.context = desc;
        hdl_adc->adc_end_of_conversion.handler = &event_adc_end_of_conversion;
        hdl_interrupt_request(ic, hdl_adc->adc_iterrupt, &hdl_adc->adc_end_of_conversion);
        /* There must be 14 CK_ADC tact */
        for(uint16_t i = 0; i < 10 * 14; i++)
          __NOP();
        
        ADC_CTL1((uint32_t)hdl_adc->module.reg) |= (uint32_t)ADC_CTL1_RSTCLB;
        /* TODO: Should amend */
        while ((ADC_CTL1((uint32_t)hdl_adc->module.reg) & ADC_CTL1_RSTCLB));
        ADC_CTL1((uint32_t)hdl_adc->module.reg) |= ADC_CTL1_CLB;
        hdl_adc->time_stamp = hdl_timer_get(timer);
        hdl_adc->state_machine = GD_ADC_STATE_MACHINE_CALIBRATION;
      }
      case GD_ADC_STATE_MACHINE_CALIBRATION:
        if (ADC_CTL1((uint32_t)hdl_adc->module.reg) & ADC_CTL1_CLB) {
          if (TIME_ELAPSED(hdl_adc->time_stamp, hdl_adc->init_timeout, hdl_timer_get(timer)))
            return HDL_MODULE_INIT_FAILED;
          return HDL_MODULE_INIT_ONGOING;
        }
      case GD_ADC_STATE_MACHINE_RUN:
        adc_dma_mode_enable((uint32_t)hdl_adc->module.reg);
        hdl_dma_run(dma, (uint32_t)&ADC_RDATA((uint32_t)hdl_adc->module.reg), (uint32_t)hdl_adc->values, (uint32_t)hdl_adc->channels_count);
        adc_software_trigger_enable((uint32_t)hdl_adc->module.reg, ADC_ROUTINE_CHANNEL);
        hdl_adc->state_machine = GD_ADC_STATE_MACHINE_WORKING;        
      case GD_ADC_STATE_MACHINE_WORKING:
        return HDL_MODULE_INIT_OK;
      default:
        return HDL_MODULE_INIT_FAILED;
    }
  }
  else {
    adc_disable((uint32_t)hdl_adc->module.reg);
    adc_dma_mode_disable((uint32_t)hdl_adc->module.reg);
    rcu_periph_clock_disable(rcu);
    hdl_adc->state_machine = GD_ADC_STATE_MACHINE_INITIAL;
    return HDL_MODULE_DEINIT_OK;
  }
}

uint32_t hdl_adc_get_data(hdl_adc_t *hdl_adc, hdl_adc_source_t *src) {
  if(hdl_adc != NULL) {
    hdl_adc_source_t **adc_channel_source = hdl_adc->sources;
    int16_t channel_element_number = 0;
    while (*adc_channel_source != NULL) {
      if(*adc_channel_source == src)
        return hdl_adc->values[channel_element_number];
      adc_channel_source++;
      channel_element_number++;
    }
  }
  return HDL_ADC_INVALID_VALUE;
}

uint32_t hdl_adc_get_age(hdl_adc_t *hdl_adc) {
  if(hdl_adc != NULL) {
    hdl_adc_private_t *adc = (hdl_adc_private_t *)hdl_adc;
    return adc->state_machine == GD_ADC_STATE_MACHINE_WORKING? adc->time_stamp :0;
  }
}

//TODO: update age