#include "app.h"
#include "CodeLib.h"

typedef struct {
    hdl_module_t module;
    uint32_t adc_scale;
    hdl_adc_source_t *adc_src;
    uint32_t ov_threshold;
    uint32_t uv_threshold;
    uint32_t raise_delay;
    uint32_t stabilization_delay;
    /* private */
    uint32_t timestamp;
    uint32_t adc_age;
    hdl_event_handler_t event;
    void *event_context;
    uint32_t voltage;
    uint32_t voltage_filter[POWER_DOMAIN_ADC_FILTER_LENGH];
    uint8_t state;
    uint8_t filter_values_amount;
    uint8_t filter_value_cursor;
    __linked_list_object__;
} atb3500_power_rail_private_t;

_Static_assert(sizeof(atb3500_power_rail_private_t) == sizeof(atb3500_power_rail_t), "In atb3500_power_domain.h data structure size of atb3500_power_rail_t doesn't match, check ATB3500_POWER_RAIL_PRV_SIZE");

void atb3500_power_rail_set(atb3500_power_rail_t *desc, uint8_t enable) {
    atb3500_power_rail_private_t *rail = (atb3500_power_rail_private_t *)desc;
    if(rail != NULL) {
        hdl_gpio_pin_t *en_pin = (hdl_gpio_pin_t *)rail->module.dependencies[ATB3500_POWER_RAIL_EN_PIN];
        if(HDL_IS_NULL_MODULE(en_pin))
            en_pin = NULL;
        hdl_timer_t *timer = (hdl_timer_t *)rail->module.dependencies[ATB3500_POWER_RAIL_TIMER];
        if(enable) {
            if (rail->state == PD_STATE_OFF) {
                rail->timestamp = hdl_timer_get(timer);
                if(en_pin != NULL) hdl_gpio_set_active(en_pin);   
                rail->state = PD_STATE_ENABLED;
                if(rail->event != NULL)
                    rail->event(rail->state, (void *)rail, rail->event_context);
            }
        }
        else if (rail->state != PD_STATE_OFF) {
            if(en_pin != NULL)
                hdl_gpio_set_inactive(en_pin);
            rail->state = PD_STATE_OFF;
            if(rail->event != NULL)
                rail->event(rail->state, (void *)rail, rail->event_context);
        }
    }
}

uint8_t atb3500_power_rail_event_subscribe(atb3500_power_rail_t *desc, hdl_event_handler_t handler, void *context) {
    atb3500_power_rail_private_t *rail = (atb3500_power_rail_private_t *)desc;
    if(hdl_state(&rail->module) == HDL_MODULE_INIT_OK) {
        rail->event = handler;
        rail->event_context = context;
        if(handler != NULL)
            handler(rail->state, (void *)rail, context);
        return HDL_TRUE;
    }
    return HDL_FALSE;
}

uint8_t atb3500_power_rail_is_stable(atb3500_power_rail_t *desc) {
    atb3500_power_rail_private_t *rail = (atb3500_power_rail_private_t *)desc;
    return (rail != NULL) && 
           (rail->state != PD_STATE_OFF) && 
           (rail->voltage >= rail->uv_threshold) && 
           (rail->voltage <= rail->ov_threshold);
}

static void _feed_rail_filter(atb3500_power_rail_private_t *rail, uint32_t value) {
    if(value == HDL_ADC_INVALID_VALUE)
        return;
    uint8_t *cursor = &rail->filter_value_cursor;
    uint8_t *amount = &rail->filter_values_amount;
    rail->voltage_filter[*cursor] = value * rail->adc_scale * ADC_REFERENCE_VOLTAGE_MV / ADC_RESOLUTION;
    (*cursor)++;
    if(*cursor >= POWER_DOMAIN_ADC_FILTER_LENGH)
        *cursor = 0;
    if(*amount < POWER_DOMAIN_ADC_FILTER_LENGH)
        (*amount)++;
    rail->voltage = array_median(rail->voltage_filter, *amount);
}

static void _power_rail_handler(linked_list_item_t *item, void *arg) {
    atb3500_power_rail_private_t *rail = linked_list_get_object(atb3500_power_rail_private_t, item);
    hdl_adc_t *rail_adc = (hdl_adc_t *)rail->module.dependencies[ATB3500_POWER_RAIL_ADC];
    uint32_t adc_current_age = hdl_adc_get_age(rail_adc);
    atb3500_power_rail_private_t *src_rail = (atb3500_power_rail_private_t *)rail->module.dependencies[ATB3500_POWER_RAIL_SOURSE_RAIL];
    hdl_timer_t *timer = (hdl_timer_t *)rail->module.dependencies[ATB3500_POWER_RAIL_TIMER];
    uint32_t time_now = hdl_timer_get(timer);
    if(HDL_IS_NULL_MODULE(src_rail))
        src_rail = NULL;

    if(rail->adc_age != adc_current_age) {
        _feed_rail_filter(rail, hdl_adc_get_data(rail_adc, rail->adc_src));
    }
    uint8_t state_current = rail->state;
    switch (rail->state) {
        case PD_STATE_OFF:
            break;
        case PD_STATE_ENABLED:
            if((src_rail != NULL) && (src_rail->state != PD_STATE_STABLE)) break;
            if(TIME_ELAPSED(rail->timestamp, rail->raise_delay, time_now)) {
                rail->state = PD_STATE_FAULT;
            }
            else if(atb3500_power_rail_is_stable((atb3500_power_rail_t *)rail)) {
                rail->timestamp = time_now;
                rail->state = PD_STATE_STABLE_DELAY;
            }
            break;
        case PD_STATE_STABLE_DELAY:
            if(TIME_ELAPSED(rail->timestamp, rail->stabilization_delay, time_now)) {
                rail->state = atb3500_power_rail_is_stable((atb3500_power_rail_t*)rail)? PD_STATE_STABLE: PD_STATE_FAULT;
            }
            break;
        case PD_STATE_STABLE:
            if(!atb3500_power_rail_is_stable((atb3500_power_rail_t*)rail)) {
                rail->state = PD_STATE_FAULT;
            }
            break;
        case PD_STATE_FAULT:
            if(atb3500_power_rail_is_stable((atb3500_power_rail_t*)rail)) {
                rail->timestamp = time_now;
                rail->state = PD_STATE_STABLE_DELAY;
            }
            break;
        default:
            break;
    }
    if((state_current != rail->state) && (rail->event != NULL))
        rail->event(rail->state, arg, rail->event_context);
    rail->adc_age = adc_current_age;
}


static uint8_t _power_rail_work(coroutine_desc_t this, uint8_t cancel, void *arg) {
  linked_list_t rails = (linked_list_t)arg;
  linked_list_do_foreach(rails, &_power_rail_handler, NULL);
  return cancel;
}

hdl_module_state_t atb3500_power_rail(void *desc, uint8_t enable) {
    static coroutine_desc_static_t power_task_buf;
    static linked_list_t rails = NULL;
    atb3500_power_rail_private_t *rail = (atb3500_power_rail_private_t*)desc;
    if(enable) {
        rail->state = PD_STATE_OFF;
        rail->filter_value_cursor = 0;
        rail->filter_values_amount = 0;
        rail->adc_age = hdl_adc_get_age((hdl_adc_t *)rail->module.dependencies[ATB3500_POWER_RAIL_ADC]);
        rail->timestamp = hdl_timer_get((hdl_timer_t *)rail->module.dependencies[ATB3500_POWER_RAIL_TIMER]);
        linked_list_insert_last(&rails, linked_list_item(rail));
        coroutine_add_static(&power_task_buf, &_power_rail_work, (void *)rails);
        return HDL_MODULE_INIT_OK;
    }
    linked_list_unlink(linked_list_item(rail));
    return HDL_MODULE_DEINIT_OK;
}


