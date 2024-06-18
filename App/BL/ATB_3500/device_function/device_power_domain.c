#include "device_logic.h"

#define TIMEOUT_MS_POE_24                       ((uint32_t) 5000)

#define ADC_REFERENCE_VOLTAGE_MV      ((uint32_t)2500)
#define ADC_RESOLUTION                ((uint32_t)4096)
#define ADC_CHANNEL_0_VOLTAGE_DIVIDER ((uint32_t)11)
#define ADC_CHANNEL_1_VOLTAGE_DIVIDER ((uint32_t)11)
#define ADC_CHANNEL_2_VOLTAGE_DIVIDER ((uint32_t)3)
#define ADC_CHANNEL_3_VOLTAGE_DIVIDER ((uint32_t)2)
#define ADC_CHANNEL_4_VOLTAGE_DIVIDER ((uint32_t)2)
#define ADC_CHANNEL_5_VOLTAGE_DIVIDER ((uint32_t)1)

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

#define POWER_RAIL_24V_RAISE_DELAY              ((uint32_t)(5000))
#define POWER_RAIL_24V_STAB_DELAY               ((uint32_t)(100))

#define MAX_FILTER_MEAN_LENGH 		            7

typedef struct {
    hdl_module_t module;
    uint32_t timestamps[ADC_CHANNEL_AMOUNT];
    uint8_t state[ADC_CHANNEL_AMOUNT];
    hdl_event_t power_event[ADC_CHANNEL_AMOUNT];
    uint32_t adc_scale[ADC_CHANNEL_AMOUNT];
    hdl_adc_source_t *adc_src[ADC_CHANNEL_AMOUNT];
    uint8_t ch_filter_values_amount[ADC_CHANNEL_AMOUNT];
    uint8_t ch_filter_value_cursor[ADC_CHANNEL_AMOUNT];
    uint32_t ch_filter_values[ADC_CHANNEL_AMOUNT][MAX_FILTER_MEAN_LENGH];
    uint32_t adc_age;
    uint32_t ch_values[ADC_CHANNEL_AMOUNT];
} dev_power_domain_private_t;

hdl_module_state_t power_domain(void *desc, uint8_t enable);

dev_power_domain_private_t mod_power_domain = {
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

static void _power_domain_set(atb3500_power_domain_e domain, hdl_gpio_pin_t *en_pin, uint8_t enable) {
    if(enable) {
        if (mod_power_domain.state[domain] == PD_STATE_OFF) {
            mod_power_domain.timestamps[domain] = get_ms_time();
            if(en_pin != NULL)
                HDL_GPIO_SET_ACTIVE(en_pin);
            mod_power_domain.state[domain] = PD_STATE_ENABLE;
            hdl_event_raise(&mod_power_domain.power_event[domain], &mod_power_domain, mod_power_domain.state[domain]);
        }
    }
    else if (mod_power_domain.state[domain] != PD_STATE_OFF) {
        if(en_pin != NULL)
            HDL_GPIO_SET_INACTIVE(en_pin);
        mod_power_domain.state[domain] = PD_STATE_OFF;
        if(domain == ATB3500_PD_5V) {
            _power_domain_set(ATB3500_PD_3V3, NULL, HDL_FALSE);
            _power_domain_set(ATB3500_PD_2V5, NULL, HDL_FALSE);
            _power_domain_set(ATB3500_PD_1V8, NULL, HDL_FALSE);
        }
        hdl_event_raise(&mod_power_domain.power_event[domain], &mod_power_domain, mod_power_domain.state[domain]);
    }
}

void power_domain_set(atb3500_power_domain_e domain, uint8_t enable) {
    hdl_gpio_pin_t *en_pin = NULL;
    switch (domain) {
        case ATB3500_PD_24V_POE:
            en_pin = &mod_do_24v_poe_power_on;
            break;
        case ATB3500_PD_5V:
            en_pin = &mod_do_5v_power_on;
            break;
        case ATB3500_PD_24V:
        case ATB3500_PD_3V3:
        case ATB3500_PD_2V5:
        case ATB3500_PD_1V8:
        default:
            return;
    }
    _power_domain_set(domain, en_pin, enable);
}

uint8_t power_domain_is_stable(atb3500_power_domain_e domain) {
    uint16_t ch_value = mod_power_domain.ch_values[domain];
    switch (domain) {
        case ATB3500_PD_24V:
        case ATB3500_PD_24V_POE:
        return (ch_value >= ADC_24V_LOW_TRHESHOLD_VALUE_MV) && (ch_value <= ADC_24V_HIGH_TRHESHOLD_VALUE_MV) &&
                !HDL_GPIO_IS_ACTIVE(&mod_di_24v_poe_power_fault) && HDL_GPIO_IS_ACTIVE(&mod_di_24v_poe_power_good) ;
        case ATB3500_PD_5V:
        return (ch_value >= ADC_5V_LOW_TRHESHOLD_VALUE_MV) && (ch_value <= ADC_5V_HIGH_TRHESHOLD_VALUE_MV);
        case ATB3500_PD_3V3:
        return (ch_value >= ADC_3V3_LOW_TRHESHOLD_VALUE_MV) && (ch_value <= ADC_3V3_HIGH_TRHESHOLD_VALUE_MV);
        case ATB3500_PD_2V5:
        return (ch_value >= ADC_2V5_LOW_TRHESHOLD_VALUE_MV) && (ch_value <= ADC_2V5_HIGH_TRHESHOLD_VALUE_MV);
        case ATB3500_PD_1V8:
        return (ch_value >= ADC_1V8_LOW_TRHESHOLD_VALUE_MV) && (ch_value <= ADC_1V8_HIGH_TRHESHOLD_VALUE_MV);
        default:
            return HDL_FALSE;
    }
}

static void _feed_ch_filter(dev_power_domain_private_t *pd, atb3500_power_domain_e ch, uint32_t value) {
    if(value == HDL_ADC_INVALID_VALUE)
        return;
    uint8_t *cursor = &pd->ch_filter_value_cursor[ch];
    uint8_t *amount = &pd->ch_filter_values_amount[ch];
    pd->ch_filter_values[ch][*cursor] = value * pd->adc_scale[ch] * ADC_REFERENCE_VOLTAGE_MV / ADC_RESOLUTION;
    *cursor++;
    if(*cursor >= MAX_FILTER_MEAN_LENGH)
        *cursor = 0;
    if(*amount < MAX_FILTER_MEAN_LENGH)
        *amount++;
    pd->ch_values[ch] = array_median(pd->ch_filter_values[ch], MAX_FILTER_MEAN_LENGH);
}

static uint8_t _power_domain_work(coroutine_desc_t this, uint8_t cancel, void *arg) {
    dev_power_domain_private_t *pd = (dev_power_domain_private_t*)arg;
    uint32_t time_now = get_ms_time();
    for(int i = 0; i < ADC_CHANNEL_AMOUNT; i++) {
        uint32_t adc_current_age = hdl_adc_get_age((hdl_adc_t *)pd->module.dependencies[4]);
        if(pd->adc_age != adc_current_age) { //TODO: !!!if it is new value!!! update driver
            _feed_ch_filter(pd, i, hdl_adc_get_data(&mod_adc, pd->adc_src[i]));
        }
        uint8_t state_current = pd->state[i];
        switch (pd->state[i]) {
            case PD_STATE_OFF:
                break;
            case PD_STATE_ENABLE:
                if(TIME_ELAPSED(pd->timestamps[i], POWER_RAIL_24V_RAISE_DELAY, time_now)) {
                    pd->state[i] = PD_STATE_FAULT;
                }
                else if(power_domain_is_stable(i)) {
                    pd->timestamps[i] = time_now;
                    pd->state[i] = PD_STATE_STABLE_DELAY;
                }
                break;
            case PD_STATE_STABLE_DELAY:
                if(TIME_ELAPSED(pd->timestamps[i], POWER_RAIL_24V_STAB_DELAY, time_now)) {
                    pd->state[i] = power_domain_is_stable(i)? PD_STATE_STABLE: PD_STATE_FAULT;
                    if(i == ATB3500_PD_5V) {
                        _power_domain_set(ATB3500_PD_3V3, NULL, HDL_TRUE);
                        _power_domain_set(ATB3500_PD_2V5, NULL, HDL_TRUE);
                        _power_domain_set(ATB3500_PD_1V8, NULL, HDL_TRUE);
                    }
                }
                break;
            case PD_STATE_STABLE:
                if(!power_domain_is_stable(i)) {
                    pd->state[i] = PD_STATE_FAULT;
                }
                break;
            case PD_STATE_FAULT:
                if(power_domain_is_stable(i)) {
                    pd->timestamps[i] = time_now;
                    pd->state[i] = PD_STATE_STABLE_DELAY;
                }
                break;
            default:
                break;
        }
        if(state_current != pd->state[i])
            hdl_event_raise(&pd->power_event[i], pd, pd->state[i]);
    }

    return cancel;
}

hdl_module_state_t power_domain(void *desc, uint8_t enable) {
    static coroutine_desc_static_t power_task_buf;
    if(enable) {
        for(int i = 0; i < ADC_CHANNEL_AMOUNT; i++) {
            mod_power_domain.state[i] = PD_STATE_OFF;
            mod_power_domain.ch_filter_value_cursor[i] = 0;
            mod_power_domain.ch_filter_values_amount[i] = 0;
        }
        mod_power_domain.adc_age = hdl_adc_get_age((hdl_adc_t *)mod_power_domain.module.dependencies[4]);
        mod_power_domain.state[ATB3500_PD_24V] = PD_STATE_ENABLE;
        mod_power_domain.timestamps[ATB3500_PD_24V] = get_ms_time();
        coroutine_add_static(&power_task_buf, &_power_domain_work, (void *)desc);
        return HDL_MODULE_INIT_OK;
    }
    return HDL_MODULE_DEINIT_OK;
}
