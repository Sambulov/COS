#include "device_logic.h"
#include "CodeLib.h"
// #define MAX_BOOT_BOOT_RETRY              ((int8_t)5)

/* SMARC RESET DELAY */
#define DELAY_MS_SMARC_RESET_AFTER_POWER_ISSUE  ((uint32_t)200)
#define DELAY_MS_SECONDARY_POWER_STABLE         ((uint32_t)5000)

typedef struct {
    hdl_module_t module;
    uint32_t sw_power_btn_trig_delay;
    smarc_carrier_sate_t target_state;
    smarc_carrier_sate_t state;
    smarc_carrier_sate_t old_state;
    hdl_event_handler_t event;
    void * event_context;
} bldl_smarc_carrier_private_t;

_Static_assert(sizeof(bldl_smarc_carrier_private_t) == sizeof(bldl_smarc_carrier_t), "In bldl_smarc_carrier.h data structure size of bldl_smarc_carrier_t doesn't match, check BLDL_SMARC_CARRIER_PRV_SIZE");

#define _call_event_hundler(c, e) { if(c->event != NULL) c->event(e, (void*)carrier, c->event_context); }

static hdl_gpio_pin_t *_smarc_get_ctrl_pin(bldl_smarc_carrier_private_t *carrier, uint32_t pin_no) {
    hdl_gpio_pin_t *pin = (hdl_gpio_pin_t *)carrier->module.dependencies[pin_no];
    return HDL_IS_NULL_MODULE(pin)? NULL: pin;
}

uint8_t smarc_carrier_event_subscribe(bldl_smarc_carrier_t *desc, hdl_event_handler_t handler, void *context) {
    if(hdl_state(&desc->module) != HDL_MODULE_INIT_OK)
        return HDL_FALSE;
    bldl_smarc_carrier_private_t *carrier = (bldl_smarc_carrier_private_t *)desc;
    carrier->event = handler;
    carrier->event_context = context;
    return HDL_TRUE;
}

void smarc_carrier_set_target_state(bldl_smarc_carrier_t *desc, smarc_carrier_sate_t state) {
    if(hdl_state(&desc->module) != HDL_MODULE_INIT_OK)
        return;
    bldl_smarc_carrier_private_t *carrier = (bldl_smarc_carrier_private_t *)desc;
    carrier->target_state = state;
}

smarc_carrier_sate_t smarc_carrier_get_target_state(bldl_smarc_carrier_t *desc) {
    if(hdl_state(&desc->module) != HDL_MODULE_INIT_OK)
        return INITIAL;
    bldl_smarc_carrier_private_t *carrier = (bldl_smarc_carrier_private_t *)desc;
    return carrier->target_state;
}

smarc_carrier_sate_t smarc_carrier_get_current_state(bldl_smarc_carrier_t *desc) {
    if(hdl_state(&desc->module) != HDL_MODULE_INIT_OK)
        return INITIAL;
    bldl_smarc_carrier_private_t *carrier = (bldl_smarc_carrier_private_t *)desc;
    return carrier->state;
}

void smarc_carrier_force_state(bldl_smarc_carrier_t *desc, smarc_carrier_sate_t state, uint8_t active) {
    if(hdl_state(&desc->module) != HDL_MODULE_INIT_OK)
        return;
    bldl_smarc_carrier_private_t *carrier = (bldl_smarc_carrier_private_t *)desc;
    if(active)
        carrier->state |= state;
    else
        carrier->state &= ~state;
}

void smarc_carrier_boot_select(bldl_smarc_carrier_t *desc, smarc_carrier_boot_select_e select) {
    if(hdl_state(&desc->module) != HDL_MODULE_INIT_OK)
        return;
    hdl_gpio_pin_t *boot0 = _smarc_get_ctrl_pin((bldl_smarc_carrier_private_t *)desc, BOOT0_PIN);
    hdl_gpio_pin_t *boot1 = _smarc_get_ctrl_pin((bldl_smarc_carrier_private_t *)desc, BOOT1_PIN);
    hdl_gpio_pin_t *boot2 = _smarc_get_ctrl_pin((bldl_smarc_carrier_private_t *)desc, BOOT2_PIN);
    if(boot0) hdl_gpio_write(boot0, (select & SMARC_CARRIER_BOOT0)? !boot0->inactive_default: boot0->inactive_default);
    if(boot1) hdl_gpio_write(boot1, (select & SMARC_CARRIER_BOOT1)? !boot1->inactive_default: boot1->inactive_default);
    if(boot2) hdl_gpio_write(boot2, (select & SMARC_CARRIER_BOOT2)? !boot2->inactive_default: boot2->inactive_default);
}

static uint8_t _smarc_carrier_work(coroutine_desc_t this, uint8_t cancel, void *arg) {
    bldl_smarc_carrier_private_t *carrier = (bldl_smarc_carrier_private_t *)arg;

    hdl_gpio_pin_t *power_bad = _smarc_get_ctrl_pin(carrier, POWER_BAD_PIN);
    hdl_gpio_pin_t *carrier_pwr_on = _smarc_get_ctrl_pin(carrier, CARRIER_POWER_ON_PIN);
    hdl_gpio_pin_t *carrier_stby = _smarc_get_ctrl_pin(carrier, CARRIER_STBY_PIN);
    hdl_gpio_pin_t *reset_in = _smarc_get_ctrl_pin(carrier, RESET_IN_PIN);
    hdl_gpio_pin_t *reset_out = _smarc_get_ctrl_pin(carrier, RESET_OUT_PIN);

    if(reset_out != NULL) {
        if(HDL_GPIO_IS_INACTIVE(reset_out)) carrier->state |= RESET_OUT;
        else carrier->state &= ~RESET_OUT;
    }

    if(carrier_stby != NULL) {
        if(HDL_GPIO_IS_INACTIVE(carrier_stby)) carrier->state |= CARRIER_STANDBY;
        else carrier->state &= ~CARRIER_STANDBY;
    }

    if(carrier_pwr_on != NULL) {
        if(HDL_GPIO_IS_ACTIVE(carrier_pwr_on)) carrier->state |= CARRIER_POWER_ON;
        else carrier->state &= ~CARRIER_POWER_ON;
    }

    if(carrier->state != carrier->old_state) {
        
        if((carrier->state ^ carrier->old_state) & RUNTIME) {
            if(carrier->state & RUNTIME) {
                if((carrier->target_state >= RUNTIME) && (carrier->old_state & CARRIER_READY)) {
                    _call_event_hundler(carrier, SMARC_EVENT_CARRIER_MODULE_RUNTIME);
                    carrier->old_state |= RUNTIME;
                }
            }
            else {
                _call_event_hundler(carrier, SMARC_EVENT_CARRIER_MODULE_RESET);
                if(carrier->target_state < CARRIER_READY) {
                    carrier->state &= ~RESET_IN;
                }
                carrier->old_state &= ~RUNTIME;
            }
        }

        if((carrier->state ^ carrier->old_state) & CARRIER_READY) {
            if(carrier->state & CARRIER_READY) {
                if((carrier->target_state >= CARRIER_READY) && (carrier->old_state & CARRIER_STANDBY)) {
                    if(reset_in != NULL) HDL_GPIO_SET_ACTIVE(reset_in);
                    _call_event_hundler(carrier, SMARC_EVENT_CARRIER_BOOT_READY);
                    carrier->old_state |= CARRIER_READY;
                }
            }
            else {
                if(reset_in != NULL) HDL_GPIO_SET_INACTIVE(reset_in);
                if(carrier->state < CARRIER_READY) {
                    _call_event_hundler(carrier, SMARC_EVENT_CARRIER_RUNTIME_TO_STBY_CIRCUITS);
                }
                else {
                    _call_event_hundler(carrier, SMARC_EVENT_CARRIER_FAULT);
                }
                carrier->old_state &= ~CARRIER_READY;
            }
        }

        if((carrier->state ^ carrier->old_state) & CARRIER_STANDBY) {
            if(carrier->state & CARRIER_STANDBY) {
                if((carrier->target_state >= CARRIER_STANDBY) && (carrier->old_state & CARRIER_POWER_ON)) {
                    _call_event_hundler(carrier, SMARC_EVENT_CARRIER_STBY_TO_RUNTIME_CIRCUITS);
                    carrier->old_state |= CARRIER_STANDBY;
                }
            }
            else {
                if(carrier->state < CARRIER_STANDBY) { 
                    _call_event_hundler(carrier, SMARC_EVENT_CARRIER_RUNTIME_TO_STBY_CIRCUITS);
                }
                else {
                    carrier->state &= ~RESET_IN;
                    _call_event_hundler(carrier, SMARC_EVENT_CARRIER_MODULE_BAD_STATE);
                }
                carrier->old_state &= ~CARRIER_STANDBY;
            }
        }

        if((carrier->state ^ carrier->old_state) & CARRIER_POWER_ON) {
            if(carrier->state & CARRIER_POWER_ON) {
                if((carrier->target_state >= CARRIER_POWER_ON) && (carrier->old_state & POWER_GOOD)) {
                    _call_event_hundler(carrier, SMARC_EVENT_CARRIER_SLEEP_TO_STBY_CIRCUITS);
                    carrier->old_state |= CARRIER_POWER_ON;
                }
            }
            else {
                if(carrier->state < CARRIER_POWER_ON) { 
                    _call_event_hundler(carrier, SMARC_EVENT_CARRIER_STBY_TO_SLEEP_CIRCUITS);
                }
                else {
                    carrier->state &= ~RESET_IN;
                    _call_event_hundler(carrier, SMARC_EVENT_CARRIER_MODULE_BAD_STATE);
                }
                carrier->old_state &= ~CARRIER_POWER_ON;
            }
        }

        if((carrier->state ^ carrier->old_state) & POWER_GOOD) {
            if(carrier->state & POWER_GOOD) {
                if(power_bad != NULL) HDL_GPIO_SET_INACTIVE(power_bad);
                _call_event_hundler(carrier, SMARC_EVENT_CARRIER_POWER_GOOD);
                carrier->old_state |= POWER_GOOD;
            }
            else {
                if(power_bad != NULL) HDL_GPIO_SET_ACTIVE(power_bad);
                _call_event_hundler(carrier, SMARC_EVENT_CARRIER_POWER_FAULT);
                carrier->old_state &= ~POWER_GOOD;
            }
        }
    }

    return cancel;
}

hdl_module_state_t bldl_smarc_carrier(void *desc, uint8_t enable) {
    bldl_smarc_carrier_private_t *carrier = (bldl_smarc_carrier_private_t *)desc;
    static coroutine_desc_static_t smarc_task_buf;
    if(enable) {
        hdl_gpio_pin_t *pow_bad_pin = (hdl_gpio_pin_t *)carrier->module.dependencies[0];
        hdl_gpio_pin_t *rst_in_pin = (hdl_gpio_pin_t *)carrier->module.dependencies[4];
        if(!HDL_IS_NULL_MODULE(pow_bad_pin)) {
            HDL_GPIO_SET_ACTIVE(pow_bad_pin);
        }
        if(!HDL_IS_NULL_MODULE(rst_in_pin)) {
            HDL_GPIO_SET_INACTIVE(rst_in_pin);
        }
        carrier->old_state = INITIAL;
        carrier->state = INITIAL;
        carrier->target_state = INITIAL;
        coroutine_add_static(&smarc_task_buf, &_smarc_carrier_work, (void *)desc);
        return HDL_MODULE_INIT_OK;
    }
    return HDL_MODULE_DEINIT_OK;
}
