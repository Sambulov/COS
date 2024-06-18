/*!
    \file device_logic.c
    \author a.bodin
    
    \version 2024-06-04, V1.0.0, This file describe major device logic
 */
#if defined(ATB_3500)

#include "device_logic.h"
#include "app.h"


/* Main global struvt */
object_dictionary_t od;
/* State machine behavior */
static void state_initial(void);
static void state_power_monitor(void);

/* Function common for all state machine */
static void state_common(void);
/* Support function */
static void _state_machine_switch(device_logic_state_machine_e state, uint8_t substate);

/* Major cycle */
void device_logic(void) {
    while (1) {    
        state_common();
        switch (od.state_machine.state) {
            case DL_STATE_MACHINE_INITIAL: {
                state_initial();
                break;
            }
            case DL_STATE_MACHINE_SMARC_POWER_UP: {
                //state_smarc_power_up();
                break;
            }
            case DL_STATE_MACHINE_POWER_MONITOR: {
                state_power_monitor();
                break;
            }
            case DL_STATE_MACHINE_POWER_RESET: {
                //state_power_reset();
                break;
            }
            default:
                break;
        }
    }
}

static void _state_machine_switch(device_logic_state_machine_e state, uint8_t substate) {
    od.state_machine.state = state;
    od.state_machine.sub_state = substate;
}

void state_common(void) {
    static uint32_t time_stamp_1_ms = 0;
    /* Wait until hdl module will have initialized */
    if(!od.sb.sb_all_hdl_module_is_init)
        return;
    cooperative_scheduler(false);
    /* Every 1 ms */
    if(get_ms_time_from(time_stamp_1_ms) >= 1) {
        time_stamp_1_ms = get_ms_time();
        //device_adc_proc(&od);
        //device_check_power_status(&od);
        device_relay_control(&od);
    }
}   

void state_initial(void) {
    device_logic_state_machine_t *sm = &od.state_machine;
    switch (sm->sub_state) {
        /* Init major strcut */
        case 0: {
            // void device_object_distonary_init_default(object_dictionary_t *od) {
            //     memset(od, 0, sizeof(object_dictionary_t));
            //     /* Intit filter for adc */
            //     od->counter_of_boot_retry = MAX_BOOT_BOOT_RETRY;
            //     for(uint16_t i = 0; i < ADC_CHANNEL_AMOUNT; i++) {
            //         filter_mean_init(&od->adc_filter[i], ADC_FILTER_LEN, ADC_FILTER_SAMPLE_TIME_MS, &get_ms_time_from, &get_ms_time);
            //     }     
            // }
            //device_object_distonary_init_default(&od);
            sm->sub_state = 1;
            break;
        }
        /* Hardware init */
        case 1: {
            smarc_init();
            indicator_init();
            connector_init();
            watchdog_init();
            communication_init();
            while (!hdl_init_complete()) {
                cooperative_scheduler(false);
            }
            sm->sub_state = 2;
            od.sb.sb_all_hdl_module_is_init = 1;
            break;
        }
        /* Reset SMARC and other pin */
        case 2: {
            sm->sub_state = 3;
            break;
        }
        /* Wait ADC stable */
        case 3: {
            if(od.sb.sb_adc_include_valid_data)
                sm->sub_state = 4;
            break;
        }
        /* Wait 24v stable */
        case 4: {
            if(power_domain_is_stable(ATB3500_PD_24V)) {
                power_domain_set(ATB3500_PD_5V, HDL_TRUE);
                sm->sub_state = 5;
            }
            break;
        }
        /* Wait 5v stable */
        case 5: {
            if(power_domain_is_stable(ATB3500_PD_5V)) {
                _state_machine_switch(DL_STATE_MACHINE_SMARC_POWER_UP, 0);
            }
            break;
        }
        default:
            break;
    }
}


void state_power_monitor(void) {
    device_logic_state_machine_t *sm = &od.state_machine;
    switch (sm->sub_state) {
        /* Init major strcut */
        case 0: {
            if( hdl_gpio_read(&mod_di_smarc_reset_feedback) == HDL_GPIO_ON_WRAP(mod_di_smarc_reset_feedback) ) {
                //_state_machine_switch(DL_STATE_MACHINE_POWER_RESET, 0);
            }
            if(od.error.dl_error_poe_fault || od.error.dl_error_poe_not_good) {
                //_state_machine_switch(DL_STATE_MACHINE_POWER_RESET, 0);
            }
            break;
        }
        default:
            break;
    }
}



#endif /* ATB_3500 */
