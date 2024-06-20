/*!
    \file device_logic.c
    \author a.bodin
    
    \version 2024-06-04, V1.0.0, This file describe major device logic
 */
#if defined(ATB_3500)

#include "device_logic.h"
#include "app.h"

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
    static hdl_delegate_t power_domain_1v8_delagate;
    static hdl_delegate_t power_domain_2v5_delagate;
    static hdl_delegate_t power_domain_3v3_delagate;
    power_domain_1v8_delagate.context = context;
    power_domain_1v8_delagate.handler = power_domain_1v8_rail;
    power_domain_2v5_delagate.context = context;
    power_domain_2v5_delagate.handler = power_domain_2v5_rail;
    power_domain_3v3_delagate.context = context;
    power_domain_3v3_delagate.handler = power_domain_3v3_rail;
    power_domain_set(ATB3500_PD_3V3, HDL_TRUE);
    power_domain_set(ATB3500_PD_2V5, HDL_TRUE);
    power_domain_set(ATB3500_PD_1V8, HDL_TRUE);
    power_domain_event_subscribe(ATB3500_PD_3V3, &power_domain_3v3_delagate);
    power_domain_event_subscribe(ATB3500_PD_2V5, &power_domain_2v5_delagate);
    power_domain_event_subscribe(ATB3500_PD_1V8, &power_domain_1v8_delagate);
    smarc_boot_select(SMARC_BOOT0 | SMARC_BOOT1 | SMARC_BOOT2);
}

void power_domain_5v_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    if(event_trigger == PD_STATE_STABLE) {
        smarc_power_good();
    }
}

void power_domain_24v_rail(uint32_t event_trigger, void *sender, void *context) {
    static hdl_delegate_t power_domain_5v_delagate;
    dev_context_t *dev = (dev_context_t*)context;
    if(event_trigger == PD_STATE_STABLE) {
        power_domain_5v_delagate.context = context,
        power_domain_5v_delagate.handler = power_domain_5v_rail,
        power_domain_event_subscribe(ATB3500_PD_5V, &power_domain_5v_delagate);
        power_domain_set(ATB3500_PD_5V, HDL_TRUE);
    }
}

void device_logic(void) {
    static dev_context_t context;
    hdl_delegate_t power_domain_24v_delagate = {
        .context = &context,
        .handler = power_domain_24v_rail,
    };
    power_domain_init();
    power_domain_event_subscribe(ATB3500_PD_24V, &power_domain_24v_delagate);
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
