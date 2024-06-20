/*!
    \file device_logic.c
    \author a.bodin
    
    \version 2024-06-04, V1.0.0, This file describe major device logic
 */
#if defined(ATB_3500)

#include "device_logic.h"
#include "app.h"


/* Main global struvt */
//object_dictionary_t od;
/* State machine behavior */
//static void state_initial(void);


/* Function common for all state machine */
//static void state_common(void);
/* Support function */
//static void _state_machine_switch(device_logic_state_machine_e state, uint8_t substate);
/* Callback */
//void power_domain_24v_rail(uint32_t event_trigger, void *sender, void *context);

/* Major cycle */
// void device_logic(void) {
//     while (1) {    
//         state_common();
//         switch (od.state_machine.state) {
//             case DL_STATE_MACHINE_INITIAL: {
//                 state_initial();
//                 break;
//             }
//             case DL_STATE_MACHINE_SMARC_POWER_UP: {
//                 //state_smarc_power_up();
//                 break;
//             }
//             case DL_STATE_MACHINE_POWER_MONITOR: {
//                 //state_power_monitor();
//                 break;
//             }
//             case DL_STATE_MACHINE_POWER_RESET: {
//                 //state_power_reset();
//                 break;
//             }
//             default:
//                 break;
//         }
//     }
// }

typedef struct {
  uint8_t lv_stable;
} dev_context_t;

void power_domain_lv_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    if(event_trigger == PD_STATE_STABLE) {
        dev->lv_stable++;
        if(dev->lv_stable == 3) {
            smarc_carrier_redy();
        }
    }
}

void smarc_standby_circuits_cb(void *context) {
    static hdl_delegate_t power_domain_lv_delagate;
    power_domain_lv_delagate.context = context,
    power_domain_lv_delagate.handler = power_domain_lv_rail,
    power_domain_set(ATB3500_PD_3V3, HDL_TRUE);
    power_domain_set(ATB3500_PD_2V5, HDL_TRUE);
    power_domain_set(ATB3500_PD_1V8, HDL_TRUE);
    power_domain_event_subscribe(ATB3500_PD_3V3, &power_domain_lv_delagate);
    power_domain_event_subscribe(ATB3500_PD_2V5, &power_domain_lv_delagate);
    power_domain_event_subscribe(ATB3500_PD_1V8, &power_domain_lv_delagate);
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
    dev_context_t context;
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

// void state_common(void) {
//     static uint32_t time_stamp_1_ms = 0;
//     /* Wait until hdl module will have initialized */
//     if(!od.sb.sb_all_hdl_module_is_init)
//         return;
//     cooperative_scheduler(false);
//     /* Every 1 ms */
//     if(get_ms_time_from(time_stamp_1_ms) >= 1) {
//         time_stamp_1_ms = get_ms_time();
//         //device_adc_proc(&od);
//         //device_check_power_status(&od);
//         device_relay_control(&od);
//     }
// }   


// void state_initial(void) {
//     device_logic_state_machine_t *sm = &od.state_machine;
//     switch (sm->sub_state) {
//         /* Init major strcut */
//         case 0: {
//             // void device_object_distonary_init_default(object_dictionary_t *od) {
//             //     memset(od, 0, sizeof(object_dictionary_t));
//             //     /* Intit filter for adc */
//             //     od->counter_of_boot_retry = MAX_BOOT_BOOT_RETRY;
//             //     for(uint16_t i = 0; i < ADC_CHANNEL_AMOUNT; i++) {
//             //         filter_mean_init(&od->adc_filter[i], ADC_FILTER_LEN, ADC_FILTER_SAMPLE_TIME_MS, &get_ms_time_from, &get_ms_time);
//             //     }     
//             // }
//             //device_object_distonary_init_default(&od);
//             sm->sub_state = 1;
//             break;
//         }
//         /* Hardware init */
//         case 1: {
//             power_domain_init();
//             power_domain_event_subscribe(ATB3500_PD_24V, &power_domain_24v_delagate);
//             smarc_init();
//             indicator_init();
//             connector_init();
//             watchdog_init();
//             communication_init();
//             while (!hdl_init_complete()) {
//                 cooperative_scheduler(false);
//             }
//             sm->sub_state = 2;
//             od.sb.sb_all_hdl_module_is_init = 1;
//             break;
//         }
//         /* Reset SMARC and other pin */
//         case 2: {
//             sm->sub_state = 3;
//             break;
//         }
//         /* Wait ADC stable */
//         case 3: {
//             if(od.sb.sb_adc_include_valid_data)
//                 sm->sub_state = 4;
//             break;
//         }
//         /* Wait 24v stable */
//         case 4: {
//             if(power_domain_is_stable(ATB3500_PD_24V)) {
//                 power_domain_set(ATB3500_PD_5V, HDL_TRUE);
//                 sm->sub_state = 5;
//             }
//             break;
//         }
//         /* Wait 5v stable */
//         case 5: {
//             if(power_domain_is_stable(ATB3500_PD_5V)) {
//                 _state_machine_switch(DL_STATE_MACHINE_SMARC_POWER_UP, 0);
//             }
//             break;
//         }
//         default:
//             break;
//     }
// }



#endif /* ATB_3500 */
