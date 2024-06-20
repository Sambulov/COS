#include "device_logic.h"

typedef struct {
    hdl_module_t module;
    uint32_t adc_scale[POWER_DOMAIN_CHANNELS_AMOUNT];
    hdl_adc_source_t *adc_src[POWER_DOMAIN_CHANNELS_AMOUNT];
    /* private */
    uint32_t timestamps[POWER_DOMAIN_CHANNELS_AMOUNT];
    event_handler_t power_event[POWER_DOMAIN_CHANNELS_AMOUNT];
    void *power_event_context[POWER_DOMAIN_CHANNELS_AMOUNT];
    uint32_t ch_values[POWER_DOMAIN_CHANNELS_AMOUNT];

    uint32_t ch_filter_values[POWER_DOMAIN_CHANNELS_AMOUNT][POWER_DOMAIN_ADC_FILTER_LENGH];
    uint32_t adc_age;
    uint8_t state[POWER_DOMAIN_CHANNELS_AMOUNT];
    uint8_t ch_filter_values_amount[POWER_DOMAIN_CHANNELS_AMOUNT];
    uint8_t ch_filter_value_cursor[POWER_DOMAIN_CHANNELS_AMOUNT];
} bldl_power_domain_private_t;


_Static_assert(sizeof(bldl_power_domain_private_t) == sizeof(bldl_power_domain_t), "In bldl_power_domain.h data structure size of bldl_power_domain_t doesn't match, check BLDL_POWER_DOMAIN_PRV_SIZE");

static void _power_domain_set(bldl_power_domain_private_t *mod_power_domain, atb3500_power_domain_e domain, hdl_gpio_pin_t *en_pin, uint8_t enable) {
    if(enable) {
        if (mod_power_domain->state[domain] == PD_STATE_OFF) {
            mod_power_domain->timestamps[domain] = get_ms_time();
            if(en_pin != NULL)
                HDL_GPIO_SET_ACTIVE(en_pin);
            mod_power_domain->state[domain] = PD_STATE_ENABLE;
            if(mod_power_domain->power_event[domain] != NULL)
                mod_power_domain->power_event[domain](mod_power_domain->state[domain], (void *)mod_power_domain, mod_power_domain->power_event_context[domain]);
        }
    }
    else if (mod_power_domain->state[domain] != PD_STATE_OFF) {
        if(en_pin != NULL)
            HDL_GPIO_SET_INACTIVE(en_pin);
        mod_power_domain->state[domain] = PD_STATE_OFF;
        if(domain == ATB3500_PD_5V) {
            _power_domain_set(mod_power_domain, ATB3500_PD_3V3, NULL, HDL_FALSE);
            _power_domain_set(mod_power_domain, ATB3500_PD_2V5, NULL, HDL_FALSE);
            _power_domain_set(mod_power_domain, ATB3500_PD_1V8, NULL, HDL_FALSE);
        }
        if(mod_power_domain->power_event[domain] != NULL)
            mod_power_domain->power_event[domain](mod_power_domain->state[domain], (void *)mod_power_domain, mod_power_domain->power_event_context[domain]);
    }
}

void power_domain_set(bldl_power_domain_t *desc, atb3500_power_domain_e domain, uint8_t enable) {
    bldl_power_domain_private_t *pd = (bldl_power_domain_private_t *)desc;
    hdl_gpio_pin_t *en_pin = NULL;
    switch (domain) {
        case ATB3500_PD_24V_POE:
            en_pin = &mod_do_24v_poe_power_on;
            break;
        case ATB3500_PD_5V:
            en_pin = &mod_do_5v_power_on;
            break;
        case ATB3500_PD_3V3:
        case ATB3500_PD_2V5:
        case ATB3500_PD_1V8:
            break;
        case ATB3500_PD_24V:
        default:
            return;
    }
    _power_domain_set(pd, domain, en_pin, enable);
}

uint8_t power_domain_is_stable(bldl_power_domain_t *desc, atb3500_power_domain_e domain) {
    bldl_power_domain_private_t *mod_power_domain = (bldl_power_domain_private_t *)desc;
    uint16_t ch_value = mod_power_domain->ch_values[domain];
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

static void _feed_ch_filter(bldl_power_domain_private_t *pd, atb3500_power_domain_e ch, uint32_t value) {
    if(value == HDL_ADC_INVALID_VALUE)
        return;
    uint8_t *cursor = &pd->ch_filter_value_cursor[ch];
    uint8_t *amount = &pd->ch_filter_values_amount[ch];
    pd->ch_filter_values[ch][*cursor] = value * pd->adc_scale[ch] * ADC_REFERENCE_VOLTAGE_MV / ADC_RESOLUTION;
    (*cursor)++;
    if(*cursor >= POWER_DOMAIN_ADC_FILTER_LENGH)
        *cursor = 0;
    if(*amount < POWER_DOMAIN_ADC_FILTER_LENGH)
        (*amount)++;
    pd->ch_values[ch] = array_median(pd->ch_filter_values[ch], *amount);
}

static uint8_t _power_domain_work(coroutine_desc_t this, uint8_t cancel, void *arg) {
    bldl_power_domain_private_t *pd = (bldl_power_domain_private_t*)arg;
    uint32_t time_now = get_ms_time();
    uint32_t adc_current_age = hdl_adc_get_age((hdl_adc_t *)pd->module.dependencies[4]);
    for(int i = 0; i < POWER_DOMAIN_CHANNELS_AMOUNT; i++) {
        if(pd->adc_age != adc_current_age) {
            _feed_ch_filter(pd, i, hdl_adc_get_data(&mod_adc, pd->adc_src[i]));
        }
        uint8_t state_current = pd->state[i];
        switch (pd->state[i]) {
            case PD_STATE_OFF:
                break;
            case PD_STATE_ENABLE:
                if(i == ATB3500_PD_3V3 || i == ATB3500_PD_2V5 || i == ATB3500_PD_1V8) {
                    if(pd->state[ATB3500_PD_5V] != PD_STATE_STABLE) {
                        break;
                    }
                }
                if(TIME_ELAPSED(pd->timestamps[i], POWER_RAIL_24V_RAISE_DELAY, time_now)) {
                    pd->state[i] = PD_STATE_FAULT;
                }
                else if(power_domain_is_stable((bldl_power_domain_t*)pd, i)) {
                    pd->timestamps[i] = time_now;
                    pd->state[i] = PD_STATE_STABLE_DELAY;
                }
                break;
            case PD_STATE_STABLE_DELAY:
                if(TIME_ELAPSED(pd->timestamps[i], POWER_RAIL_24V_STAB_DELAY, time_now)) {
                    pd->state[i] = power_domain_is_stable((bldl_power_domain_t*)pd, i)? PD_STATE_STABLE: PD_STATE_FAULT;
                }
                break;
            case PD_STATE_STABLE:
                if(!power_domain_is_stable((bldl_power_domain_t*)pd, i)) {
                    pd->state[i] = PD_STATE_FAULT;
                }
                break;
            case PD_STATE_FAULT:
                if(power_domain_is_stable((bldl_power_domain_t*)pd, i)) {
                    pd->timestamps[i] = time_now;
                    pd->state[i] = PD_STATE_STABLE_DELAY;
                }
                break;
            default:
                break;
        }
        if((state_current != pd->state[i]) && (pd->power_event[i] != NULL))
            pd->power_event[i](pd->state[i], arg, pd->power_event_context[i]);
    }
    pd->adc_age = adc_current_age;
    return cancel;
}

hdl_module_state_t power_domain(void *desc, uint8_t enable) {
    bldl_power_domain_private_t *mod_power_domain = (bldl_power_domain_private_t*)desc;
    static coroutine_desc_static_t power_task_buf;
    if(enable) {
        for(int i = 0; i < POWER_DOMAIN_CHANNELS_AMOUNT; i++) {
            mod_power_domain->state[i] = PD_STATE_OFF;
            mod_power_domain->ch_filter_value_cursor[i] = 0;
            mod_power_domain->ch_filter_values_amount[i] = 0;
        }
        mod_power_domain->adc_age = hdl_adc_get_age((hdl_adc_t *)mod_power_domain->module.dependencies[4]);
        mod_power_domain->state[ATB3500_PD_24V] = PD_STATE_ENABLE;
        mod_power_domain->timestamps[ATB3500_PD_24V] = get_ms_time();
        coroutine_add_static(&power_task_buf, &_power_domain_work, (void *)desc);
        return HDL_MODULE_INIT_OK;
    }
    return HDL_MODULE_DEINIT_OK;
}

uint8_t power_domain_event_subscribe(bldl_power_domain_t *desc, atb3500_power_domain_e domain, event_handler_t handler, void *context) {
    bldl_power_domain_private_t *mod_power_domain = (bldl_power_domain_private_t*)desc;
    if(domain <= ATB3500_PD_1V8) {
        mod_power_domain->power_event[domain] = handler;
        mod_power_domain->power_event_context[domain] = context;
        return HDL_TRUE;
    }
    return HDL_FALSE;
}
