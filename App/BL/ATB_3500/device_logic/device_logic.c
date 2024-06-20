/*!
    \file device_logic.c
    \author a.bodin
    
    \version 2024-06-04, V1.0.0, This file describe major device logic
 */
#if defined(ATB_3500)

#include "device_logic.h"
#include "app.h"


bldl_power_domain_t mod_power_domain = {
    .module.init = &power_domain,
    .module.dependencies = hdl_module_dependencies(
    /***********************************************************
     *                  POWER ADJUST
     ***********************************************************/
        &mod_do_24v_poe_power_on.module, &mod_do_5v_power_on.module, &mod_di_24v_poe_power_fault.module, &mod_di_24v_poe_power_good.module, 
    /***********************************************************
     *                  ADC
     ***********************************************************/        
        &mod_adc.module, &mod_adc_pin_24v.module, &mod_adc_pin_24v_poe.module, &mod_adc_pin_5v.module, &mod_adc_pin_3v3.module, 
        &mod_adc_pin_2v5.module, &mod_adc_pin_1v8.module),
    .adc_scale = (uint32_t[]) { ADC_CHANNEL_0_VOLTAGE_DIVIDER, ADC_CHANNEL_1_VOLTAGE_DIVIDER,
        ADC_CHANNEL_2_VOLTAGE_DIVIDER, ADC_CHANNEL_3_VOLTAGE_DIVIDER,
        ADC_CHANNEL_4_VOLTAGE_DIVIDER, ADC_CHANNEL_5_VOLTAGE_DIVIDER },

    .adc_src = (hdl_adc_source_t *[]) { &mod_adc_source_0_adc_24v, &mod_adc_source_1_adc_24v_poe,
    &mod_adc_source_2_adc_5v, &mod_adc_source_3_adc_3v3, &mod_adc_source_4_adc_2v5,
    &mod_adc_source_5_adc_1v8 },

};




typedef struct {
  uint8_t
     power_domain_3v3_is_stable : 1,
     power_domain_2v5_is_stable : 1,
     power_domain_1v8_is_stable : 1;
} dev_context_t;

void smarc_runtime_cb(void *context) {
    /* Start runtime */
}

void power_domain_rails_stable(dev_context_t *dev) {
    if(dev->power_domain_1v8_is_stable && dev->power_domain_2v5_is_stable && dev->power_domain_3v3_is_stable)
        smarc_carrier_redy();
}

void power_domain_1v8_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    if(event_trigger == PD_STATE_STABLE) {
        dev->power_domain_1v8_is_stable = 1;
    }
    power_domain_rails_stable(dev);
}
void power_domain_3v3_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    if(event_trigger == PD_STATE_STABLE) {
        dev->power_domain_3v3_is_stable = 1;
    }
    power_domain_rails_stable(dev);
}
void power_domain_2v5_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    if(event_trigger == PD_STATE_STABLE) {
        dev->power_domain_2v5_is_stable = 1;
    }
    power_domain_rails_stable(dev);
}


void smarc_standby_circuits_cb(void *context) {
    power_domain_set(&mod_power_domain, ATB3500_PD_3V3, HDL_TRUE);
    power_domain_set(&mod_power_domain, ATB3500_PD_2V5, HDL_TRUE);
    power_domain_set(&mod_power_domain, ATB3500_PD_1V8, HDL_TRUE);
    power_domain_event_subscribe(&mod_power_domain, ATB3500_PD_3V3, &power_domain_3v3_rail, context);
    power_domain_event_subscribe(&mod_power_domain, ATB3500_PD_2V5, &power_domain_2v5_rail, context);
    power_domain_event_subscribe(&mod_power_domain, ATB3500_PD_1V8, &power_domain_1v8_rail, context);
    smarc_boot_select(SMARC_BOOT0 | SMARC_BOOT1 | SMARC_BOOT2);
}

void power_domain_5v_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    if(event_trigger == PD_STATE_STABLE) {
        smarc_power_good();
    }
}

void power_domain_24v_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    if(event_trigger == PD_STATE_STABLE) {
        power_domain_event_subscribe(&mod_power_domain, ATB3500_PD_5V, &power_domain_5v_rail, context);
        power_domain_set(&mod_power_domain, ATB3500_PD_5V, HDL_TRUE);
    }
}

void device_logic(void) {
    static dev_context_t context;
    hdl_enable(&mod_power_domain.module);

    power_domain_event_subscribe(&mod_power_domain, ATB3500_PD_24V, &power_domain_24v_rail, &context);
    smarc_init(&context);
    indicator_init();
    connector_init();
    watchdog_init();
    communication_init();
    while (1) {
        cooperative_scheduler(false);
    }
}

#endif /* ATB_3500 */
