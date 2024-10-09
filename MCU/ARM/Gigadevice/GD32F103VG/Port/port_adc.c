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
    const hdl_adc_config_t *config;
    hdl_delegate_t adc_end_of_conversion;
    uint32_t time_stamp;
    gd_adc_state_machine_e state_machine;      /* gd_adc_state_machine_e */
    uint8_t channels_count;
} hdl_adc_private_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_adc_private_t, hdl_adc_t, HDL_ADC_PRV_SIZE, hdl_adc.h);

static void event_adc_end_of_conversion(uint32_t event, void *sender, void *context) {
    hdl_adc_private_t *hdl_adc = (hdl_adc_private_t *)context;
    hdl_time_counter_t *timer = (hdl_time_counter_t *)hdl_adc->module.dependencies[1];
    hdl_adc->time_stamp = hdl_time_counter_get(timer);
}

hdl_module_state_t hdl_adc(void *desc, uint8_t enable){
  hdl_adc_private_t *hdl_adc = (hdl_adc_private_t *)desc;
  if(hdl_adc->module.reg == NULL || hdl_adc->module.dependencies == NULL || hdl_adc->module.dependencies[0] == NULL ||
    hdl_adc->module.dependencies[1] == NULL || hdl_adc->module.dependencies[2] == NULL)
      return HDL_MODULE_FAULT;
  //hdl_clock_t *clock = (hdl_clock_t *)hdl_adc->module.dependencies[0];
  hdl_time_counter_t *timer = (hdl_time_counter_t *)hdl_adc->module.dependencies[1];
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
      return HDL_MODULE_FAULT;
  }
  /* TODO: SEE ADC_REGULAR_INSERTED_CHANNEL */
  if(enable) {
    switch (hdl_adc->state_machine){
      case GD_ADC_STATE_MACHINE_INITIAL: {
        rcu_periph_clock_enable(rcu);
        adc_special_function_config((uint32_t)hdl_adc->module.reg, ADC_SCAN_MODE, ENABLE);
        adc_special_function_config((uint32_t)hdl_adc->module.reg, ADC_CONTINUOUS_MODE, ENABLE);
        adc_dma_mode_enable((uint32_t)hdl_adc->module.reg);
        hdl_adc_source_t **adc_source = hdl_adc->config->sources;
        hdl_adc->channels_count = 0;
        if(adc_source != NULL) {
          while (*adc_source != NULL) {
            adc_regular_channel_config((uint32_t)hdl_adc->module.reg, hdl_adc->channels_count, (uint8_t)(*adc_source)->channel, (uint32_t)(*adc_source)->sample_time);
            adc_source++;
            hdl_adc->config->values[hdl_adc->channels_count++] = HDL_ADC_INVALID_VALUE;
          }
        }
        adc_channel_length_config((uint32_t)hdl_adc->module.reg, ADC_REGULAR_CHANNEL, hdl_adc->channels_count);
        adc_data_alignment_config((uint32_t)hdl_adc->module.reg, hdl_adc->config->data_alignment);
        //adc_resolution_config((uint32_t)hdl_adc->module.reg, (uint32_t)hdl_adc->resolution);
        adc_external_trigger_config((uint32_t)hdl_adc->module.reg, ADC_REGULAR_CHANNEL, ENABLE);
        adc_external_trigger_source_config((uint32_t)hdl_adc->module.reg, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
        adc_enable((uint32_t)hdl_adc->module.reg);
        hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)hdl_adc->module.dependencies[3];
        hdl_adc->adc_end_of_conversion.context = desc;
        hdl_adc->adc_end_of_conversion.handler = &event_adc_end_of_conversion;
        hdl_interrupt_request(ic, hdl_adc->config->adc_interrupt, &hdl_adc->adc_end_of_conversion);
        //         for(uint16_t i = 0; i < adc_short_delay_after_start; i++)
        //     __NOP();
        /* There must be 14 CK_ADC tact */
        ADC_CTL1((uint32_t)hdl_adc->module.reg) |= (uint32_t)ADC_CTL1_RSTCLB;
        while ((ADC_CTL1((uint32_t)hdl_adc->module.reg) & ADC_CTL1_RSTCLB));
        ADC_CTL1((uint32_t)hdl_adc->module.reg) |= ADC_CTL1_CLB;
        hdl_adc->time_stamp = hdl_time_counter_get(timer);
        hdl_adc->state_machine = GD_ADC_STATE_MACHINE_CALIBRATION;
      }
      case GD_ADC_STATE_MACHINE_CALIBRATION:
        if (ADC_CTL1((uint32_t)hdl_adc->module.reg) & ADC_CTL1_CLB) {
          if (TIME_ELAPSED(hdl_adc->time_stamp, hdl_adc->config->init_timeout, hdl_time_counter_get(timer)))
            return HDL_MODULE_FAULT;
          return HDL_MODULE_LOADING;
        }
      case GD_ADC_STATE_MACHINE_RUN:
        hdl_dma_run(dma, (uint32_t)&ADC_RDATA((uint32_t)hdl_adc->module.reg), (uint32_t)hdl_adc->config->values, (uint32_t)hdl_adc->channels_count);
        adc_software_trigger_enable((uint32_t)hdl_adc->module.reg, ADC_REGULAR_CHANNEL);
        hdl_adc->state_machine = GD_ADC_STATE_MACHINE_WORKING;        
      case GD_ADC_STATE_MACHINE_WORKING:
        return HDL_MODULE_ACTIVE;
      default:
        return HDL_MODULE_FAULT;
    }
  }
  else {
    adc_disable((uint32_t)hdl_adc->module.reg);
    adc_dma_mode_disable((uint32_t)hdl_adc->module.reg);
    rcu_periph_clock_disable(rcu);
    hdl_adc->state_machine = GD_ADC_STATE_MACHINE_INITIAL;
    return HDL_MODULE_UNLOADED;
  }
}

uint32_t hdl_adc_get(hdl_adc_t *hdl_adc, uint32_t src) {
  if((hdl_adc != NULL) && (hdl_state(&hdl_adc->module) == HDL_MODULE_ACTIVE) && 
     (((hdl_adc_private_t *)hdl_adc)->channels_count > src)) {
    return hdl_adc->config->values[src];
  }
  return HDL_ADC_INVALID_VALUE;
}

uint32_t hdl_adc_age(hdl_adc_t *hdl_adc) {
  if((hdl_adc != NULL) && (hdl_state(&hdl_adc->module) == HDL_MODULE_ACTIVE)) {
    return ((hdl_adc_private_t *)hdl_adc)->time_stamp;
  }
  return 0;
}
