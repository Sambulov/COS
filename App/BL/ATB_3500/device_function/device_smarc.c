#include "device_logic.h"

// #define MAX_BOOT_BOOT_RETRY              ((int8_t)5)

/* SMARC RESET DELAY */
#define DELAY_MS_SMARC_RESET_AFTER_POWER_ISSUE  ((uint32_t)200)
#define DELAY_MS_SECONDARY_POWER_STABLE         ((uint32_t)5000)

typedef struct {
    hdl_module_t module;
    uint8_t carrier_power_on : 1,
            carrier_stby     : 1,
            reset_out        : 1;
    void *bl_context;
    event_handler_t event;
    void * event_context;
} bldl_smarc_carrier_private_t;

static void _on_power_button_in(uint32_t event_trigger, void *sender, void *context) {
    bldl_smarc_carrier_private_t *carrier = (bldl_smarc_carrier_private_t *)context;
    hdl_button_t *pow_btn_out = (hdl_button_t *)carrier->module.dependencies[2];
    if(!HDL_IS_NULL_MODULE(pow_btn_out)) {
        if(event_trigger == HDL_BTN_EVENT_CLICK)
            hdl_btn_sw_click(pow_btn_out);
        //if(event_trigger == HDL_BTN_EVENT_HOLD)
        //    TODO: force shutdown
    }
}

uint8_t smarc_carrier_event_subscribe(bldl_smarc_carrier_t *desc, event_handler_t handler, void *context) {
    bldl_smarc_carrier_private_t *carrier = (bldl_smarc_carrier_private_t *)desc;
    carrier->event = handler;
    carrier->event_context = context;
}

static void _call_event_hundler(bldl_smarc_carrier_private_t *carrier, smarc_carrier_event_e event) {
    if(carrier->event != NULL) {
        carrier->event(event, (void*)carrier, carrier->event_context);
    }
}

static uint8_t _smarc_worker(coroutine_desc_t this, uint8_t cancel, void *arg) {
    bldl_smarc_carrier_private_t *carrier = (bldl_smarc_carrier_private_t *)arg;
    hdl_gpio_pin_t *carrier_pwr_on = (hdl_gpio_pin_t *)carrier->module.dependencies[3];
    if((HDL_IS_NULL_MODULE(carrier_pwr_on) || HDL_GPIO_IS_ACTIVE(carrier_pwr_on)) && !carrier->carrier_power_on) {
        carrier->carrier_power_on = 1;
        _call_event_hundler(carrier, SMARC_CARRIER_EVENT_STBY_CIRCUITS);
    } 

    hdl_gpio_pin_t *carrier_stby = (hdl_gpio_pin_t *)carrier->module.dependencies[4];
    if((HDL_IS_NULL_MODULE(carrier_stby) || HDL_GPIO_IS_ACTIVE(carrier_stby)) && !carrier->carrier_stby) {
        carrier->carrier_stby = 1;
        _call_event_hundler(carrier, SMARC_CARRIER_EVENT_RUNTIME_CIRCUITS);
    }

    hdl_gpio_pin_t *reset_out = (hdl_gpio_pin_t *)carrier->module.dependencies[6];
    if(!HDL_GPIO_IS_ACTIVE(reset_out) && !carrier->reset_out) {
        carrier->reset_out = 1;
        _call_event_hundler(carrier, SMARC_CARRIER_EVENT_RUNTIME);
    }
    // TODO: other smarc states
    return cancel;
}

hdl_module_state_t bldl_smarc_carrier(void *desc, uint8_t enable) {
    bldl_smarc_carrier_private_t *carrier = (bldl_smarc_carrier_private_t *)desc;
    static coroutine_desc_static_t smarc_task_buf;
    static hdl_delegate_t pow_btn_delegate;
    if(enable) {
        hdl_gpio_pin_t *pow_bad_pin = (hdl_gpio_pin_t *)carrier->module.dependencies[0];
        if(!HDL_IS_NULL_MODULE(pow_bad_pin)) {
            HDL_GPIO_SET_ACTIVE(pow_bad_pin);
        }
        HDL_GPIO_SET_ACTIVE(&mod_do_smarc_reset_in);
        hdl_button_t *pow_btn_in = (hdl_button_t *)carrier->module.dependencies[1];
        if(!HDL_IS_NULL_MODULE(pow_btn_in)) {
            pow_btn_delegate.context = desc;
            pow_btn_delegate.handler = &_on_power_button_in;
            hdl_event_subscribe(&pow_btn_in->event, &pow_btn_delegate);
        }
        
        hdl_gpio_pin_t *reset_out = (hdl_gpio_pin_t *)carrier->module.dependencies[6];
        coroutine_add_static(&smarc_task_buf, &_smarc_worker, desc);
        return HDL_MODULE_INIT_OK;
    }
    return HDL_MODULE_DEINIT_OK;
}

void smarc_carrier_boot_select(bldl_smarc_carrier_t *desc, smarc_carrier_boot_select_e select) {
    bldl_smarc_carrier_private_t *carrier = (bldl_smarc_carrier_private_t *)desc;
    hdl_gpio_pin_t *boot0 = (hdl_gpio_pin_t *)carrier->module.dependencies[8];
    hdl_gpio_pin_t *boot1 = (hdl_gpio_pin_t *)carrier->module.dependencies[9];
    hdl_gpio_pin_t *boot2 = (hdl_gpio_pin_t *)carrier->module.dependencies[10];
    if(!HDL_IS_NULL_MODULE(boot0)) {
        hdl_gpio_write(boot0, (select & SMARC_CARRIER_BOOT0)? !boot0->inactive_default: boot0->inactive_default);
    }
    if(!HDL_IS_NULL_MODULE(boot1)) {
        hdl_gpio_write(boot1, (select & SMARC_CARRIER_BOOT1)? !boot1->inactive_default: boot1->inactive_default);
    }
    if(!HDL_IS_NULL_MODULE(boot2)) {
        hdl_gpio_write(boot2, (select & SMARC_CARRIER_BOOT2)? !boot2->inactive_default: boot2->inactive_default);
    }
}

void smarc_carrier_redy(bldl_smarc_carrier_t *desc) {
    bldl_smarc_carrier_private_t *carrier = (bldl_smarc_carrier_private_t *)desc;
    hdl_gpio_pin_t *reset_in = (hdl_gpio_pin_t *)carrier->module.dependencies[6];
    HDL_GPIO_SET_INACTIVE(reset_in);
}

void smarc_carrier_power_good(bldl_smarc_carrier_t *desc) {
    bldl_smarc_carrier_private_t *carrier = (bldl_smarc_carrier_private_t *)desc;
    hdl_gpio_pin_t *pow_bad_pin = (hdl_gpio_pin_t *)carrier->module.dependencies[0];
    if(!HDL_IS_NULL_MODULE(pow_bad_pin)) {
        HDL_GPIO_SET_INACTIVE(pow_bad_pin);
    }
}
