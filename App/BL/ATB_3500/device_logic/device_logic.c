/*!
    \file device_logic.c
    \author a.bodin
    
    \version 2024-06-04, V1.0.0, This file describe major device logic
 */
#if defined(ATB_3500)

#include "device_logic.h"
#include "app.h"

/* ADC threshold */
#define ADC_24V_LOW_TRHESHOLD_VALUE_MV          ((uint16_t)21600)  /* -10% from 24v */
#define ADC_24V_HIGH_TRHESHOLD_VALUE_MV         ((uint16_t)26400)  /* +10% from 24v */
#define ADC_5V_LOW_TRHESHOLD_VALUE_MV           ((uint16_t)4500)   /* -10% from 5v */
#define ADC_5V_HIGH_TRHESHOLD_VALUE_MV          ((uint16_t)5500)   /* +10% from 5v */
#define ADC_3V3_LOW_TRHESHOLD_VALUE_MV          ((uint16_t)2970)   /* -10% from 3.3v */
#define ADC_3V3_HIGH_TRHESHOLD_VALUE_MV         ((uint16_t)3630)   /* +10% from 3.3v */
#define ADC_2V5_LOW_TRHESHOLD_VALUE_MV          ((uint16_t)2250)   /* -10% from 2.5v */
#define ADC_2V5_HIGH_TRHESHOLD_VALUE_MV         ((uint16_t)2750)   /* +10% from 2.5v */
#define ADC_1V8_LOW_TRHESHOLD_VALUE_MV          ((uint16_t)1620)   /* -10% from 1.8v */
#define ADC_1V8_HIGH_TRHESHOLD_VALUE_MV         ((uint16_t)1980)   /* +10% from 1.8v */
/* delay for stable */
#define DELAY_MS_SECONDARY_POWER_STABLE         ((uint32_t)5000)
/* SMARC RESET DELAY */
#define DELAY_MS_SMARC_RESET_AFTER_POWER_ISSUE  ((uint32_t)200)

/* Main global struvt */
object_dictionary_t od;
/* State machine behavior */
static void state_initial(void);
static void state_smarc_power_up(void);
static void state_power_monitor(void);
static void state_power_reset(void);
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
                state_smarc_power_up();
                break;
            }
            case DL_STATE_MACHINE_POWER_MONITOR: {
                state_power_monitor();
                break;
            }
            case DL_STATE_MACHINE_POWER_RESET: {
                state_power_reset();
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
    /* Every 1 ms */
    if(get_ms_time_from(time_stamp_1_ms) >= 1) {
        time_stamp_1_ms = get_ms_time();
        device_adc_proc(&od);
        device_check_power_status(&od);
        device_relay_control(&od);


    }
}

void state_initial(void) {
    device_logic_state_machine_t *sm = &od.state_machine;
    switch (sm->sub_state) {
        /* Init major strcut */
        case 0: {
            device_object_distonary_init_default(&od);
            sm->sub_state = 1;
            break;
        }
        /* Hardware init */
        case 1: {
            if(device_hardware_init(&od) == DL_STATUS_SUCCESS) {
                sm->sub_state = 2;
                od.sb.sb_all_hdl_module_is_init = 1;
            }
            break;
        }
        /* Reset SMARC and other pin */
        case 2: {
            od.sb.sb_24v_poe_is_enable = 0;


            hdl_gpio_write(get_object_do_smarc_reset(), HDL_GPIO_ON_WRAP(get_object_do_smarc_reset()));
            hdl_gpio_write(get_object_do_lte_reset(), HDL_GPIO_ON_WRAP(get_object_do_lte_reset()));

            hdl_gpio_write(get_object_do_smarc_irq_1(), HDL_GPIO_OFF_WRAP(get_object_do_smarc_irq_1()));
            hdl_gpio_write(get_object_do_smarc_irq_2(), HDL_GPIO_OFF_WRAP(get_object_do_smarc_irq_2()));
            hdl_gpio_write(get_object_do_smarc_button(),HDL_GPIO_OFF_WRAP(get_object_do_smarc_button()));
            hdl_gpio_write(get_object_do_smarc_boot_0(), HDL_GPIO_OFF_WRAP(get_object_do_smarc_boot_0()));
            hdl_gpio_write(get_object_do_smarc_boot_1(), HDL_GPIO_OFF_WRAP(get_object_do_smarc_boot_1()));
            hdl_gpio_write(get_object_do_smarc_boot_2(), HDL_GPIO_OFF_WRAP(get_object_do_smarc_boot_2()));

            hdl_gpio_write(get_object_do_24v_power_on(),HDL_GPIO_OFF_WRAP(get_object_do_24v_power_on()));
            hdl_gpio_write(get_object_do_5v_power_on(),HDL_GPIO_OFF_WRAP(get_object_do_5v_power_on()));
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
            uint16_t adc_value = filter_mean_get(&od.adc_filter[ATB3500_ADC_24V]);
            if( (adc_value >= ADC_24V_LOW_TRHESHOLD_VALUE_MV) && (adc_value <= ADC_24V_HIGH_TRHESHOLD_VALUE_MV) ){
                hdl_gpio_write(get_object_do_5v_power_on(), HDL_GPIO_ON_WRAP(get_object_do_5v_power_on()));
                sm->sub_state = 5;
            }
            break;
        }
        /* Wait 5v stable */
        case 5: {
            uint16_t adc_value = filter_mean_get(&od.adc_filter[ATB3500_ADC_5V]);
            if( (adc_value >= ADC_5V_LOW_TRHESHOLD_VALUE_MV) && (adc_value <= ADC_5V_HIGH_TRHESHOLD_VALUE_MV) ){
                _state_machine_switch(DL_STATE_MACHINE_SMARC_POWER_UP, 0);
            }
            break;
        }
        default:
            break;
    }
}

void state_smarc_power_up(void) {
    static uint32_t time_stmap_ms = 0;
    device_logic_state_machine_t *sm = &od.state_machine;
    switch (sm->sub_state) {
        /* Choose smarc boot option */
        case 0: {
            device_choose_smarc_boot(&od);
            sm->sub_state = 1;
            break;
        }
        case 1: {
            /* Here we have to check CARRIER_POWER_ON (adc_ain_3) */
            hdl_gpio_write(get_object_do_smarc_reset(), HDL_GPIO_OFF_WRAP(get_object_do_smarc_reset()));
            sm->sub_state = 2;
            time_stmap_ms = get_ms_time();
            break;
        }
        case 2: {
            if(get_ms_time_from(time_stmap_ms) >= DELAY_MS_SECONDARY_POWER_STABLE) {
                /* TODO: Here we cycling  */
            }
            else {
                uint8_t power_is_stable = 1;
                uint16_t adc_value = filter_mean_get(&od.adc_filter[ATB3500_ADC_3V3]);
                if( !((adc_value >= ADC_3V3_LOW_TRHESHOLD_VALUE_MV) && (adc_value <= ADC_3V3_HIGH_TRHESHOLD_VALUE_MV)) ){
                    power_is_stable = 0;
                }
                adc_value = filter_mean_get(&od.adc_filter[ATB3500_ADC_2V5]);
                if( !((adc_value >= ADC_2V5_LOW_TRHESHOLD_VALUE_MV) && (adc_value <= ADC_2V5_HIGH_TRHESHOLD_VALUE_MV)) ){
                    power_is_stable = 0;
                }
                adc_value = filter_mean_get(&od.adc_filter[ATB3500_ADC_1V8]);
                if( !((adc_value >= ADC_1V8_LOW_TRHESHOLD_VALUE_MV) && (adc_value <= ADC_1V8_HIGH_TRHESHOLD_VALUE_MV)) ){
                    power_is_stable = 0;
                }
                if(power_is_stable)
                    sm->sub_state = 3;
            }
            break;
        }
        case 3: {
            hdl_gpio_write(get_object_do_24v_power_on(), HDL_GPIO_ON_WRAP(get_object_do_24v_power_on()));
            hdl_gpio_write(get_object_do_lte_reset(), HDL_GPIO_OFF_WRAP(get_object_do_lte_reset()));
            od.sb.sb_24v_poe_is_enable = 1;
            _state_machine_switch(DL_STATE_MACHINE_POWER_MONITOR, 0);
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
            if( (hdl_gpio_read(get_object_di_smarc_reset_feedback())) == HDL_GPIO_ON_WRAP(get_object_di_smarc_reset_feedback()) ) {
                _state_machine_switch(DL_STATE_MACHINE_POWER_RESET, 0);
            }
            if(od.error.dl_error_poe_fault || od.error.dl_error_poe_not_good) {
                _state_machine_switch(DL_STATE_MACHINE_POWER_RESET, 0);
            }
            break;
        }
        default:
            break;
    }
}

void state_power_reset(void) {
    static uint32_t smarc_reset_time_stamp_ms;
    device_logic_state_machine_t *sm = &od.state_machine;
    switch (sm->sub_state) {
        /* Init major strcut */
        case 0: {
            if(od.sb.sb_24v_poe_is_enable) {
                if(od.counter_of_boot_retry > 0) {
                    od.counter_of_boot_retry--;
                    hdl_gpio_write(get_object_do_smarc_reset(), HDL_GPIO_ON_WRAP(get_object_do_smarc_reset()));
                    smarc_reset_time_stamp_ms = get_ms_time();
                    sm->sub_state = 1;
                }
                else {
                    /* TODO: Here we cycling */
                }
            }
            else {
                sm->sub_state = 2;
            }
            break;
        }
        case 1: {
            if(get_ms_time_from(smarc_reset_time_stamp_ms) >= DELAY_MS_SMARC_RESET_AFTER_POWER_ISSUE) {
                sm->sub_state = 2;
            }
            break;
        }
        case 2: {
            _state_machine_switch(DL_STATE_MACHINE_SMARC_POWER_UP, 0);
            break;
        }
        default:
            break;
    }
}

#endif /* ATB_3500 */
