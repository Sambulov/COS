#include "hdl.h"
#include "CodeLib.h"

typedef struct {
    hdl_module_t module;
    uint32_t sw_power_btn_trig_delay;
    hdl_smarc_carrier_sate_t target_state;
    hdl_smarc_carrier_sate_t state;
    hdl_smarc_carrier_sate_t old_state;
    hdl_event_handler_t event;
    void * event_context;
    coroutine_t worker;
} hdl_smarc_carrier_private_t;

_Static_assert(sizeof(hdl_smarc_carrier_private_t) == sizeof(hdl_smarc_carrier_t), "In bldl_smarc_carrier.h data structure size of hdl_smarc_carrier_t doesn't match, check BLDL_SMARC_CARRIER_PRV_SIZE");

#define _call_event_hundler(c, e) { if(c->event != NULL) c->event(e, (void*)carrier, c->event_context); }

static hdl_gpio_pin_t *_smarc_get_ctrl_pin(hdl_smarc_carrier_private_t *carrier, uint32_t pin_no) {
    hdl_gpio_pin_t *pin = (hdl_gpio_pin_t *)carrier->module.dependencies[pin_no];
    return HDL_IS_NULL_MODULE(pin)? NULL: pin;
}

uint8_t hdl_smarc_carrier_event_subscribe(hdl_smarc_carrier_t *desc, hdl_event_handler_t handler, void *context) {
    if(hdl_state(&desc->module) != HDL_MODULE_ACTIVE)
        return HDL_FALSE;
    hdl_smarc_carrier_private_t *carrier = (hdl_smarc_carrier_private_t *)desc;
    carrier->event = handler;
    carrier->event_context = context;
    return HDL_TRUE;
}

void hdl_smarc_carrier_set_target_state(hdl_smarc_carrier_t *desc, hdl_smarc_carrier_sate_t state) {
    if(hdl_state(&desc->module) != HDL_MODULE_ACTIVE)
        return;
    hdl_smarc_carrier_private_t *carrier = (hdl_smarc_carrier_private_t *)desc;
    carrier->target_state = state;
}

hdl_smarc_carrier_sate_t hdl_smarc_carrier_get_target_state(hdl_smarc_carrier_t *desc) {
    if(hdl_state(&desc->module) != HDL_MODULE_ACTIVE)
        return HDL_SMARC_CARRIER_STATE_INITIAL;
    hdl_smarc_carrier_private_t *carrier = (hdl_smarc_carrier_private_t *)desc;
    return carrier->target_state;
}

hdl_smarc_carrier_sate_t hdl_smarc_carrier_get_current_state(hdl_smarc_carrier_t *desc) {
    if(hdl_state(&desc->module) != HDL_MODULE_ACTIVE)
        return HDL_SMARC_CARRIER_STATE_INITIAL;
    hdl_smarc_carrier_private_t *carrier = (hdl_smarc_carrier_private_t *)desc;
    return carrier->state;
}

void hdl_smarc_carrier_force_state(hdl_smarc_carrier_t *desc, hdl_smarc_carrier_sate_t state, uint8_t active) {
    if(hdl_state(&desc->module) != HDL_MODULE_ACTIVE)
        return;
    hdl_smarc_carrier_private_t *carrier = (hdl_smarc_carrier_private_t *)desc;
    if(active)
        carrier->state |= state;
    else
        carrier->state &= ~state;
}

void hdl_smarc_carrier_boot_select(hdl_smarc_carrier_t *desc, hdl_smarc_carrier_boot_select_t select) {
    if(hdl_state(&desc->module) != HDL_MODULE_ACTIVE)
        return;
    hdl_gpio_pin_t *boot0 = _smarc_get_ctrl_pin((hdl_smarc_carrier_private_t *)desc, HDL_SMARC_CARRIER_DEPENDENCY_BOOT0_PIN);
    hdl_gpio_pin_t *boot1 = _smarc_get_ctrl_pin((hdl_smarc_carrier_private_t *)desc, HDL_SMARC_CARRIER_DEPENDENCY_BOOT1_PIN);
    hdl_gpio_pin_t *boot2 = _smarc_get_ctrl_pin((hdl_smarc_carrier_private_t *)desc, HDL_SMARC_CARRIER_DEPENDENCY_BOOT2_PIN);
    if(boot0) hdl_gpio_write(boot0, (select & HDL_SMARC_CARRIER_BOOT0)? !boot0->config->inactive_default: boot0->config->inactive_default);
    if(boot1) hdl_gpio_write(boot1, (select & HDL_SMARC_CARRIER_BOOT1)? !boot1->config->inactive_default: boot1->config->inactive_default);
    if(boot2) hdl_gpio_write(boot2, (select & HDL_SMARC_CARRIER_BOOT2)? !boot2->config->inactive_default: boot2->config->inactive_default);
}

static uint8_t _smarc_carrier_work(coroutine_t *this, uint8_t cancel, void *arg) {
    hdl_smarc_carrier_private_t *carrier = (hdl_smarc_carrier_private_t *)arg;

    hdl_gpio_pin_t *power_bad = _smarc_get_ctrl_pin(carrier, HDL_SMARC_CARRIER_DEPENDENCY_POWER_BAD_PIN);
    hdl_gpio_pin_t *carrier_pwr_on = _smarc_get_ctrl_pin(carrier, HDL_SMARC_CARRIER_DEPENDENCY_CARRIER_POWER_ON_PIN);
    hdl_gpio_pin_t *carrier_stby = _smarc_get_ctrl_pin(carrier, HDL_SMARC_CARRIER_DEPENDENCY_CARRIER_STBY_PIN);
    hdl_gpio_pin_t *reset_in = _smarc_get_ctrl_pin(carrier, HDL_SMARC_CARRIER_DEPENDENCY_RESET_IN_PIN);
    hdl_gpio_pin_t *reset_out = _smarc_get_ctrl_pin(carrier, HDL_SMARC_CARRIER_DEPENDENCY_RESET_OUT_PIN);

    if(reset_out != NULL) {
        if(hdl_gpio_is_inactive(reset_out)) carrier->state |= HDL_SMARC_CARRIER_STATE_RESET_OUT;
        else carrier->state &= ~HDL_SMARC_CARRIER_STATE_RESET_OUT;
    }

    if(carrier_stby != NULL) {
        if(hdl_gpio_is_inactive(carrier_stby)) carrier->state |= HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY;
        else carrier->state &= ~HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY;
    }

    if(carrier_pwr_on != NULL) {
        if(hdl_gpio_is_active(carrier_pwr_on)) carrier->state |= HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON;
        else carrier->state &= ~HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON;
    }

    if(carrier->state != carrier->old_state) {
        
        if((carrier->state ^ carrier->old_state) & HDL_SMARC_CARRIER_STATE_RUNTIME) {
            if(carrier->state & HDL_SMARC_CARRIER_STATE_RUNTIME) {
                if((carrier->target_state >= HDL_SMARC_CARRIER_STATE_RUNTIME) && (carrier->old_state & HDL_SMARC_CARRIER_STATE_CARRIER_READY)) {
                    _call_event_hundler(carrier, HDL_SMARC_CARRIER_EVENT_MODULE_RUNTIME);
                    carrier->old_state |= HDL_SMARC_CARRIER_STATE_RUNTIME;
                }
            }
            else {
                _call_event_hundler(carrier, HDL_SMARC_CARRIER_EVENT_MODULE_RESET);
                if(carrier->target_state < HDL_SMARC_CARRIER_STATE_CARRIER_READY) {
                    carrier->state &= ~HDL_SMARC_CARRIER_STATE_RESET_IN;
                }
                carrier->old_state &= ~HDL_SMARC_CARRIER_STATE_RUNTIME;
            }
        }

        if((carrier->state ^ carrier->old_state) & HDL_SMARC_CARRIER_STATE_CARRIER_READY) {
            if(carrier->state & HDL_SMARC_CARRIER_STATE_CARRIER_READY) {
                if((carrier->target_state >= HDL_SMARC_CARRIER_STATE_CARRIER_READY) && 
                   (carrier->old_state & HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY)) {
                    if(reset_in != NULL) hdl_gpio_set_active(reset_in);
                    _call_event_hundler(carrier, HDL_SMARC_CARRIER_EVENT_BOOT_READY);
                    carrier->old_state |= HDL_SMARC_CARRIER_STATE_CARRIER_READY;
                }
            }
            else {
                if(reset_in != NULL) hdl_gpio_set_inactive(reset_in);
                if(carrier->state < HDL_SMARC_CARRIER_STATE_CARRIER_READY) {
                    _call_event_hundler(carrier, HDL_SMARC_CARRIER_EVENT_RUNTIME_TO_STBY_CIRCUITS);
                }
                else {
                    _call_event_hundler(carrier, HDL_SMARC_CARRIER_EVENT_FAULT);
                }
                carrier->old_state &= ~HDL_SMARC_CARRIER_STATE_CARRIER_READY;
            }
        }

        if((carrier->state ^ carrier->old_state) & HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY) {
            if(carrier->state & HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY) {
                if((carrier->target_state >= HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY) && 
                   (carrier->old_state & HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON)) {
                    _call_event_hundler(carrier, HDL_SMARC_CARRIER_EVENT_STBY_TO_RUNTIME_CIRCUITS);
                    carrier->old_state |= HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY;
                }
            }
            else {
                if(carrier->state < HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY) { 
                    _call_event_hundler(carrier, HDL_SMARC_CARRIER_EVENT_RUNTIME_TO_STBY_CIRCUITS);
                }
                else {
                    carrier->state &= ~HDL_SMARC_CARRIER_STATE_RESET_IN;
                    _call_event_hundler(carrier, HDL_SMARC_CARRIER_EVENT_MODULE_BAD_STATE);
                }
                carrier->old_state &= ~HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY;
            }
        }

        if((carrier->state ^ carrier->old_state) & HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON) {
            if(carrier->state & HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON) {
                if((carrier->target_state >= HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON) && 
                   (carrier->old_state & HDL_SMARC_CARRIER_STATE_POWER_GOOD)) {
                    _call_event_hundler(carrier, HDL_SMARC_CARRIER_EVENT_SLEEP_TO_STBY_CIRCUITS);
                    carrier->old_state |= HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON;
                }
            }
            else {
                if(carrier->state < HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON) { 
                    _call_event_hundler(carrier, HDL_SMARC_CARRIER_EVENT_STBY_TO_SLEEP_CIRCUITS);
                }
                else {
                    carrier->state &= ~HDL_SMARC_CARRIER_STATE_RESET_IN;
                    _call_event_hundler(carrier, HDL_SMARC_CARRIER_EVENT_MODULE_BAD_STATE);
                }
                carrier->old_state &= ~HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON;
            }
        }

        if((carrier->state ^ carrier->old_state) & HDL_SMARC_CARRIER_STATE_POWER_GOOD) {
            if(carrier->state & HDL_SMARC_CARRIER_STATE_POWER_GOOD) {
                if(power_bad != NULL) hdl_gpio_set_inactive(power_bad);
                _call_event_hundler(carrier, HDL_SMARC_CARRIER_EVENT_POWER_GOOD);
                carrier->old_state |= HDL_SMARC_CARRIER_STATE_POWER_GOOD;
            }
            else {
                if(power_bad != NULL) hdl_gpio_set_active(power_bad);
                _call_event_hundler(carrier, HDL_SMARC_CARRIER_EVENT_POWER_FAULT);
                carrier->old_state &= ~HDL_SMARC_CARRIER_STATE_POWER_GOOD;
            }
        }
    }

    return cancel;
}

hdl_module_state_t hdl_smarc_carrier(void *desc, uint8_t enable) {
    hdl_smarc_carrier_private_t *carrier = (hdl_smarc_carrier_private_t *)desc;
    if(enable) {
        hdl_gpio_pin_t *pow_bad_pin = _smarc_get_ctrl_pin(carrier, HDL_SMARC_CARRIER_DEPENDENCY_POWER_BAD_PIN);
        hdl_gpio_pin_t *rst_in_pin = _smarc_get_ctrl_pin(carrier, HDL_SMARC_CARRIER_DEPENDENCY_RESET_IN_PIN);
        if(pow_bad_pin != NULL) hdl_gpio_set_active(pow_bad_pin);
        if(rst_in_pin != NULL) hdl_gpio_set_inactive(rst_in_pin);
        carrier->old_state = HDL_SMARC_CARRIER_STATE_INITIAL;
        carrier->state = HDL_SMARC_CARRIER_STATE_INITIAL;
        carrier->target_state = HDL_SMARC_CARRIER_STATE_INITIAL;
        coroutine_add(&carrier->worker, &_smarc_carrier_work, (void *)desc);
        return HDL_MODULE_ACTIVE;
    }
    return HDL_MODULE_UNLOADED;
}
