#include "app.h"
#include "CodeLib.h"

typedef struct {
  hdl_module_t module;
  const atb3500_power_rail_config_t *config;
  struct {
    uint32_t timestamp;
    uint32_t adc_age;
    hdl_event_handler_t event;
    void *event_context;
    uint32_t voltage;
    uint32_t voltage_filter[POWER_DOMAIN_ADC_FILTER_LENGH];
    uint8_t state;
    uint8_t filter_values_amount;
    uint8_t filter_value_cursor;
    coroutine_t worker;
  } private;
} atb3500_power_rail_private_t;

HDL_ASSERRT_STRUCTURE_CAST(atb3500_power_rail_private_t, atb3500_power_rail_t, ATB3500_POWER_RAIL_PRV_SIZE, atb3500_power_domain.h);

void atb3500_power_rail_set(atb3500_power_rail_t *desc, uint8_t enable) {
    atb3500_power_rail_private_t *rail = (atb3500_power_rail_private_t *)desc;
    if(rail != NULL) {
        hdl_gpio_pin_t *en_pin = (hdl_gpio_pin_t *)rail->module.dependencies[ATB3500_POWER_RAIL_EN_PIN];
        if(HDL_IS_NULL_MODULE(en_pin))
            en_pin = NULL;
        hdl_time_counter_t *timer = (hdl_time_counter_t *)rail->module.dependencies[ATB3500_POWER_RAIL_TIMER];
        if(enable) {
            if (rail->private.state == PD_STATE_OFF) {
                rail->private.timestamp = hdl_time_counter_get(timer);
                if(en_pin != NULL) hdl_gpio_set_active(en_pin);   
                rail->private.state = PD_STATE_ENABLED;
                if(rail->private.event != NULL)
                    rail->private.event(rail->private.state, (void *)rail, rail->private.event_context);
            }
        }
        else if (rail->private.state != PD_STATE_OFF) {
            if(en_pin != NULL)
                hdl_gpio_set_inactive(en_pin);
            rail->private.state = PD_STATE_OFF;
            if(rail->private.event != NULL)
                rail->private.event(rail->private.state, (void *)rail, rail->private.event_context);
        }
    }
}

uint8_t atb3500_power_rail_event_subscribe(atb3500_power_rail_t *desc, hdl_event_handler_t handler, void *context) {
    atb3500_power_rail_private_t *rail = (atb3500_power_rail_private_t *)desc;
    if(hdl_state(&rail->module) == HDL_MODULE_ACTIVE) {
        rail->private.event = handler;
        rail->private.event_context = context;
        if(handler != NULL)
            handler(rail->private.state, (void *)rail, context);
        return HDL_TRUE;
    }
    return HDL_FALSE;
}

static uint8_t _check_feedback(atb3500_power_rail_private_t *desc) {
    hdl_gpio_pin_t *feed_back_power_fault = (hdl_gpio_pin_t *)desc->module.dependencies[ATB3500_POWER_RAIL_FEEDBACK_FAULT];
    hdl_gpio_pin_t *feed_back_power_good = (hdl_gpio_pin_t *)desc->module.dependencies[ATB3500_POWER_RAIL_FEEDBACK_GOOD];
    if((HDL_IS_NULL_MODULE(feed_back_power_fault)) || (HDL_IS_NULL_MODULE(feed_back_power_good)))
        return 1;
    if((desc->private.state == PD_STATE_OFF) || (desc->private.state == PD_STATE_ENABLED) || (desc->private.state == PD_STATE_STABLE_DELAY))
        return 1;
    return ((hdl_gpio_is_active(feed_back_power_fault)) || (hdl_gpio_is_inactive(feed_back_power_good))) ? 0 : 1;
}

uint8_t atb3500_power_rail_is_stable(atb3500_power_rail_t *desc) {
    atb3500_power_rail_private_t *rail = (atb3500_power_rail_private_t *)desc;
    uint8_t feedback_is_ok = _check_feedback(rail);
    return (rail != NULL) && 
           (rail->private.state != PD_STATE_OFF) && 
           (rail->private.voltage >= rail->config->uv_threshold) && 
           (rail->private.voltage <= rail->config->ov_threshold) && 
           (feedback_is_ok);
}

static void _feed_rail_filter(atb3500_power_rail_private_t *rail, uint32_t value) {
    if(value == HDL_ADC_INVALID_VALUE)
        return;
    uint8_t *cursor = &rail->private.filter_value_cursor;
    uint8_t *amount = &rail->private.filter_values_amount;
    rail->private.voltage_filter[*cursor] = value * rail->config->adc_scale * ADC_REFERENCE_VOLTAGE_MV / ADC_RESOLUTION;
    (*cursor)++;
    if(*cursor >= POWER_DOMAIN_ADC_FILTER_LENGH)
        *cursor = 0;
    if(*amount < POWER_DOMAIN_ADC_FILTER_LENGH)
        (*amount)++;
    rail->private.voltage = array_median(rail->private.voltage_filter, *amount);
}

static uint8_t _power_rail_work(coroutine_t *this, uint8_t cancel, void *arg) {
    atb3500_power_rail_private_t *rail = (atb3500_power_rail_private_t *) arg;
    hdl_adc_t *rail_adc = (hdl_adc_t *)rail->module.dependencies[ATB3500_POWER_RAIL_ADC];
    uint32_t adc_current_age = hdl_adc_age(rail_adc);
    atb3500_power_rail_private_t *src_rail = (atb3500_power_rail_private_t *)rail->module.dependencies[ATB3500_POWER_RAIL_SOURSE_RAIL];
    hdl_time_counter_t *timer = (hdl_time_counter_t *)rail->module.dependencies[ATB3500_POWER_RAIL_TIMER];
    uint32_t time_now = hdl_time_counter_get(timer);
    if(HDL_IS_NULL_MODULE(src_rail))
        src_rail = NULL;

    if(rail->private.adc_age != adc_current_age) {
        _feed_rail_filter(rail, hdl_adc_get(rail_adc, rail->config->adc_ch));
    }
    uint8_t state_current = rail->private.state;
    switch (rail->private.state) {
        case PD_STATE_OFF:
            break;
        case PD_STATE_ENABLED:
            if((src_rail != NULL) && (src_rail->private.state != PD_STATE_STABLE)) break;
            if(TIME_ELAPSED(rail->private.timestamp, rail->config->raise_delay, time_now)) {
                rail->private.state = PD_STATE_FAULT;
            }
            else if(atb3500_power_rail_is_stable((atb3500_power_rail_t *)rail)) {
                rail->private.timestamp = time_now;
                rail->private.state = PD_STATE_STABLE_DELAY;
            }
            break;
        case PD_STATE_STABLE_DELAY:
            if(TIME_ELAPSED(rail->private.timestamp, rail->config->stabilization_delay, time_now)) {
                rail->private.state = atb3500_power_rail_is_stable((atb3500_power_rail_t*)rail)? PD_STATE_STABLE: PD_STATE_FAULT;
            }
            break;
        case PD_STATE_STABLE:
            if(!atb3500_power_rail_is_stable((atb3500_power_rail_t*)rail)) {
                rail->private.state = PD_STATE_FAULT;
            }
            break;
        case PD_STATE_FAULT:
            if(atb3500_power_rail_is_stable((atb3500_power_rail_t*)rail)) {
                rail->private.timestamp = time_now;
                rail->private.state = PD_STATE_STABLE_DELAY;
            }
            break;
        default:
            break;
    }
    if((state_current != rail->private.state) && (rail->private.event != NULL))
        rail->private.event(rail->private.state, arg, rail->private.event_context);
    rail->private.adc_age = adc_current_age;
  return cancel;
}

hdl_module_state_t atb3500_power_rail(void *desc, uint8_t enable) {
    atb3500_power_rail_private_t *rail = (atb3500_power_rail_private_t*)desc;
    if(enable) {
        rail->private.state = PD_STATE_OFF;
        rail->private.filter_value_cursor = 0;
        rail->private.filter_values_amount = 0;
        rail->private.adc_age = hdl_adc_age((hdl_adc_t *)rail->module.dependencies[ATB3500_POWER_RAIL_ADC]);
        rail->private.timestamp = hdl_time_counter_get((hdl_time_counter_t *)rail->module.dependencies[ATB3500_POWER_RAIL_TIMER]);
        coroutine_add(&rail->private.worker, &_power_rail_work, desc);
        return HDL_MODULE_ACTIVE;
    }
    coroutine_cancel(&rail->private.worker);
    return HDL_MODULE_UNLOADED;
}


