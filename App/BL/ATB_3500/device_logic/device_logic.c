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

hdl_button_t power_button = {
    .module.init = &hdl_button,
    .module.dependencies = hdl_module_dependencies(&mod_do_smarc_power_btn.module, &mod_systick_timer_ms.module ),
    .debounce_delay = 50,
    .hold_delay = 0,
};

bldl_smarc_carrier_t mod_smarc = {
    .module.init = &bldl_smarc_carrier,
    .module.dependencies = hdl_module_dependencies(
        &hdl_null_module /* power good */,
        &power_button.module /* power button in */,
        &power_button.module /* power button out */,
        &hdl_null_module /* carrier_power_on */,
        &hdl_null_module /* carrier_stand_by */,
        &mod_do_smarc_reset_in.module, 
        &mod_di_smarc_reset_out.module,
        &mod_systick_timer_ms.module,
        &mod_do_smarc_boot_0.module, 
        &mod_do_smarc_boot_1.module, 
        &mod_do_smarc_boot_2.module),
};

typedef struct {
  uint8_t
     power_domain_3v3_is_stable : 1,
     power_domain_2v5_is_stable : 1,
     power_domain_1v8_is_stable : 1;
} dev_context_t;

void power_domain_rails_stable(dev_context_t *dev) {
    if(dev->power_domain_1v8_is_stable && dev->power_domain_2v5_is_stable && dev->power_domain_3v3_is_stable)
        smarc_carrier_redy(&mod_smarc);
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


void smarc_carrier_event_handler(uint32_t event_trigger, void *sender, void *context) {
    if(event_trigger == SMARC_CARRIER_EVENT_STBY_CIRCUITS) {
        power_domain_set(&mod_power_domain, ATB3500_PD_3V3, HDL_TRUE);
        power_domain_set(&mod_power_domain, ATB3500_PD_2V5, HDL_TRUE);
        power_domain_set(&mod_power_domain, ATB3500_PD_1V8, HDL_TRUE);
        power_domain_event_subscribe(&mod_power_domain, ATB3500_PD_3V3, &power_domain_3v3_rail, context);
        power_domain_event_subscribe(&mod_power_domain, ATB3500_PD_2V5, &power_domain_2v5_rail, context);
        power_domain_event_subscribe(&mod_power_domain, ATB3500_PD_1V8, &power_domain_1v8_rail, context);
        smarc_carrier_boot_select(&mod_smarc, SMARC_CARRIER_BOOT0 | SMARC_CARRIER_BOOT1 | SMARC_CARRIER_BOOT2);
    }
    else if(event_trigger == SMARC_CARRIER_EVENT_RUNTIME_CIRCUITS) {

    }
    else if(event_trigger == SMARC_CARRIER_EVENT_RUNTIME) {
        /* Start runtime */
    }
}

void power_domain_5v_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    if(event_trigger == PD_STATE_STABLE) {
        smarc_carrier_power_good(&mod_smarc);
    }
}

void power_domain_24v_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    if(event_trigger == PD_STATE_STABLE) {
        power_domain_event_subscribe(&mod_power_domain, ATB3500_PD_5V, &power_domain_5v_rail, context);
        power_domain_set(&mod_power_domain, ATB3500_PD_5V, HDL_TRUE);
    }
}

bldl_communication_t smarc_comm = {
    .module.init = communication,
    .module.dependencies = hdl_module_dependencies(&mod_spi_3.module),
};

typedef struct {
    uint8_t x[12]
    /* data */
} test_rx_comm_t;

typedef struct {
    uint8_t x[12]
    /* data */
} test_tx_comm_t;

test_rx_comm_t test_rx;
test_tx_comm_t test_tx;

void device_logic(void) {
    static dev_context_t context;
    hdl_enable(&mod_power_domain.module);

    power_domain_event_subscribe(&mod_power_domain, ATB3500_PD_24V, &power_domain_24v_rail, &context);
    hdl_enable(&mod_smarc.module);
    smarc_carrier_event_subscribe(&mod_smarc, &smarc_carrier_event_handler, &context);

    hdl_enable(&smarc_comm.module);
    communication_mem_map_t map_test_tx = { .offset = 3, .size = sizeof(test_tx_comm_t) };
    communication_mem_map_t map_test_rx = { .offset = 20, .size = sizeof(test_rx_comm_t) };
    communication_map_tx(&smarc_comm, &map_test_tx);
    communication_map_rx(&smarc_comm, &map_test_rx);

    indicator_init();
    connector_init();
    watchdog_init();

    while (1) {
        if(communication_get(&smarc_comm, &map_test_rx, (void*)&test_rx)) {
            __NOP();
        }
        cooperative_scheduler(false);
    }
}

#endif /* ATB_3500 */
