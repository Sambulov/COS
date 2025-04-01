#include "hdl_iface.h"

typedef struct {
  hdl_smarc_carrier_sate_t target_state;
  hdl_smarc_carrier_sate_t state;
  hdl_smarc_carrier_sate_t old_state;
  hdl_event_handler_t event;
  void *event_context;
  coroutine_t worker;
} hdl_smarc_carrier_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_smarc_carrier_var_t, *((hdl_smarc_carrier_t *)0)->obj_var, HDL_SMARC_CARRIER_VAR_SIZE, hdl_smarc_carrier.h);

#define _call_event_hundler(c, e) { if(c->event != NULL) c->event(e, (void*)c, c->event_context); }

static hdl_gpio_pin_t *_smarc_get_ctrl_pin(hdl_smarc_carrier_t *carrier, uint32_t pin_no) {
  hdl_gpio_pin_t *pin = (hdl_gpio_pin_t *)carrier->dependencies[pin_no];
  return hdl_is_null_module(pin)? NULL: pin;
}

static uint8_t _hdl_smarc_carrier_event_subscribe(const void *desc, hdl_event_handler_t handler, void *context) {
  hdl_smarc_carrier_t *carrier = (hdl_smarc_carrier_t *)desc;
  hdl_smarc_carrier_var_t *carrier_var = (hdl_smarc_carrier_var_t *)carrier->obj_var;
  carrier_var->event = handler;
  carrier_var->event_context = context;
  return HDL_TRUE;
}

static void _hdl_smarc_carrier_set_target_state(const void *desc, hdl_smarc_carrier_sate_t state) {
  hdl_smarc_carrier_t *carrier = (hdl_smarc_carrier_t *)desc;
  hdl_smarc_carrier_var_t *carrier_var = (hdl_smarc_carrier_var_t *)carrier->obj_var;
  carrier_var->target_state = state;
}

static hdl_smarc_carrier_sate_t _hdl_smarc_carrier_get_target_state(const void *desc) {
  hdl_smarc_carrier_t *carrier = (hdl_smarc_carrier_t *)desc;
  hdl_smarc_carrier_var_t *carrier_var = (hdl_smarc_carrier_var_t *)carrier->obj_var;
  return carrier_var->target_state;
}

static hdl_smarc_carrier_sate_t _hdl_smarc_carrier_get_current_state(const void *desc) {
  hdl_smarc_carrier_t *carrier = (hdl_smarc_carrier_t *)desc;
  hdl_smarc_carrier_var_t *carrier_var = (hdl_smarc_carrier_var_t *)carrier->obj_var;
  return carrier_var->state;
}

static void _hdl_smarc_carrier_force_state(const void *desc, hdl_smarc_carrier_sate_t state, uint8_t active) {
  hdl_smarc_carrier_t *carrier = (hdl_smarc_carrier_t *)desc;
  hdl_smarc_carrier_var_t *carrier_var = (hdl_smarc_carrier_var_t *)carrier->obj_var;
  if(active)
    carrier_var->state |= state;
  else
    carrier_var->state &= ~state;
}

static void _hdl_smarc_carrier_boot_select(const void *desc, hdl_smarc_carrier_boot_select_t select) {
  hdl_gpio_pin_t *boot0 = _smarc_get_ctrl_pin((hdl_smarc_carrier_t *)desc, HDL_SMARC_CARRIER_DEPENDENCY_BOOT0_PIN);
  hdl_gpio_pin_t *boot1 = _smarc_get_ctrl_pin((hdl_smarc_carrier_t *)desc, HDL_SMARC_CARRIER_DEPENDENCY_BOOT1_PIN);
  hdl_gpio_pin_t *boot2 = _smarc_get_ctrl_pin((hdl_smarc_carrier_t *)desc, HDL_SMARC_CARRIER_DEPENDENCY_BOOT2_PIN);
  if(boot0) hdl_gpio_write(boot0, (select & HDL_SMARC_CARRIER_BOOT0)? !boot0->config->inactive_default: boot0->config->inactive_default);
  if(boot1) hdl_gpio_write(boot1, (select & HDL_SMARC_CARRIER_BOOT1)? !boot1->config->inactive_default: boot1->config->inactive_default);
  if(boot2) hdl_gpio_write(boot2, (select & HDL_SMARC_CARRIER_BOOT2)? !boot2->config->inactive_default: boot2->config->inactive_default);
}

static uint8_t _smarc_carrier_work(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_smarc_carrier_t *carrier = (hdl_smarc_carrier_t *)arg;
  hdl_smarc_carrier_var_t *carrier_var = (hdl_smarc_carrier_var_t *)carrier->obj_var;

  hdl_gpio_pin_t *power_bad = _smarc_get_ctrl_pin(carrier, HDL_SMARC_CARRIER_DEPENDENCY_POWER_BAD_PIN);
  hdl_gpio_pin_t *carrier_pwr_on = _smarc_get_ctrl_pin(carrier, HDL_SMARC_CARRIER_DEPENDENCY_CARRIER_POWER_ON_PIN);
  hdl_gpio_pin_t *carrier_stby = _smarc_get_ctrl_pin(carrier, HDL_SMARC_CARRIER_DEPENDENCY_CARRIER_STBY_PIN);
  hdl_gpio_pin_t *reset_in = _smarc_get_ctrl_pin(carrier, HDL_SMARC_CARRIER_DEPENDENCY_RESET_IN_PIN);
  hdl_gpio_pin_t *reset_out = _smarc_get_ctrl_pin(carrier, HDL_SMARC_CARRIER_DEPENDENCY_RESET_OUT_PIN);

  if(reset_out != NULL) {
    if(hdl_gpio_is_inactive(reset_out)) carrier_var->state |= HDL_SMARC_CARRIER_STATE_RESET_OUT;
    else carrier_var->state &= ~HDL_SMARC_CARRIER_STATE_RESET_OUT;
  }

  if(carrier_stby != NULL) {
    if(hdl_gpio_is_inactive(carrier_stby)) carrier_var->state |= HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY;
    else carrier_var->state &= ~HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY;
  }

  if(carrier_pwr_on != NULL) {
    if(hdl_gpio_is_active(carrier_pwr_on)) carrier_var->state |= HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON;
    else carrier_var->state &= ~HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON;
  }

  if(carrier_var->state != carrier_var->old_state) {
    
    if((carrier_var->state ^ carrier_var->old_state) & HDL_SMARC_CARRIER_STATE_RUNTIME) {
      if(carrier_var->state & HDL_SMARC_CARRIER_STATE_RUNTIME) {
        if((carrier_var->target_state >= HDL_SMARC_CARRIER_STATE_RUNTIME) && (carrier_var->old_state & HDL_SMARC_CARRIER_STATE_CARRIER_READY)) {
          _call_event_hundler(carrier_var, HDL_SMARC_CARRIER_EVENT_MODULE_RUNTIME);
          carrier_var->old_state |= HDL_SMARC_CARRIER_STATE_RUNTIME;
        }
      }
      else {
        _call_event_hundler(carrier_var, HDL_SMARC_CARRIER_EVENT_MODULE_RESET);
        if(carrier_var->target_state < HDL_SMARC_CARRIER_STATE_CARRIER_READY) {
          carrier_var->state &= ~HDL_SMARC_CARRIER_STATE_RESET_IN;
        }
        carrier_var->old_state &= ~HDL_SMARC_CARRIER_STATE_RUNTIME;
      }
    }

    if((carrier_var->state ^ carrier_var->old_state) & HDL_SMARC_CARRIER_STATE_CARRIER_READY) {
      if(carrier_var->state & HDL_SMARC_CARRIER_STATE_CARRIER_READY) {
        if((carrier_var->target_state >= HDL_SMARC_CARRIER_STATE_CARRIER_READY) && 
          (carrier_var->old_state & HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY)) {
          if(reset_in != NULL) hdl_gpio_set_active(reset_in);
          _call_event_hundler(carrier_var, HDL_SMARC_CARRIER_EVENT_BOOT_READY);
          carrier_var->old_state |= HDL_SMARC_CARRIER_STATE_CARRIER_READY;
        }
      }
      else {
        if(reset_in != NULL) hdl_gpio_set_inactive(reset_in);
        if(carrier_var->state < HDL_SMARC_CARRIER_STATE_CARRIER_READY) {
          _call_event_hundler(carrier_var, HDL_SMARC_CARRIER_EVENT_RUNTIME_TO_STBY_CIRCUITS);
        }
        else {
          _call_event_hundler(carrier_var, HDL_SMARC_CARRIER_EVENT_FAULT);
        }
        carrier_var->old_state &= ~HDL_SMARC_CARRIER_STATE_CARRIER_READY;
      }
    }

    if((carrier_var->state ^ carrier_var->old_state) & HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY) {
      if(carrier_var->state & HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY) {
        if((carrier_var->target_state >= HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY) && 
          (carrier_var->old_state & HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON)) {
          _call_event_hundler(carrier_var, HDL_SMARC_CARRIER_EVENT_STBY_TO_RUNTIME_CIRCUITS);
          carrier_var->old_state |= HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY;
        }
      }
      else {
        if(carrier_var->state < HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY) { 
          _call_event_hundler(carrier_var, HDL_SMARC_CARRIER_EVENT_RUNTIME_TO_STBY_CIRCUITS);
        }
        else {
          carrier_var->state &= ~HDL_SMARC_CARRIER_STATE_RESET_IN;
          _call_event_hundler(carrier_var, HDL_SMARC_CARRIER_EVENT_MODULE_BAD_STATE);
        }
        carrier_var->old_state &= ~HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY;
      }
    }

    if((carrier_var->state ^ carrier_var->old_state) & HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON) {
      if(carrier_var->state & HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON) {
        if((carrier_var->target_state >= HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON) && 
          (carrier_var->old_state & HDL_SMARC_CARRIER_STATE_POWER_GOOD)) {
          _call_event_hundler(carrier_var, HDL_SMARC_CARRIER_EVENT_SLEEP_TO_STBY_CIRCUITS);
          carrier_var->old_state |= HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON;
        }
      }
      else {
        if(carrier_var->state < HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON) { 
          _call_event_hundler(carrier_var, HDL_SMARC_CARRIER_EVENT_STBY_TO_SLEEP_CIRCUITS);
        }
        else {
          carrier_var->state &= ~HDL_SMARC_CARRIER_STATE_RESET_IN;
          _call_event_hundler(carrier_var, HDL_SMARC_CARRIER_EVENT_MODULE_BAD_STATE);
        }
        carrier_var->old_state &= ~HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON;
      }
    }

    if((carrier_var->state ^ carrier_var->old_state) & HDL_SMARC_CARRIER_STATE_POWER_GOOD) {
      if(carrier_var->state & HDL_SMARC_CARRIER_STATE_POWER_GOOD) {
        if(power_bad != NULL) hdl_gpio_set_inactive(power_bad);
        _call_event_hundler(carrier_var, HDL_SMARC_CARRIER_EVENT_POWER_GOOD);
        carrier_var->old_state |= HDL_SMARC_CARRIER_STATE_POWER_GOOD;
      }
      else {
        if(power_bad != NULL) hdl_gpio_set_active(power_bad);
        _call_event_hundler(carrier_var, HDL_SMARC_CARRIER_EVENT_POWER_FAULT);
        carrier_var->old_state &= ~HDL_SMARC_CARRIER_STATE_POWER_GOOD;
      }
    }
  }

  return cancel;
}

static hdl_module_state_t _hdl_smarc_carrier(const void *desc, uint8_t enable) {
  hdl_smarc_carrier_t *carrier = (hdl_smarc_carrier_t *)desc;
  hdl_smarc_carrier_var_t *carrier_var = (hdl_smarc_carrier_var_t *)carrier->obj_var;
  if(enable) {
    hdl_gpio_pin_t *pow_bad_pin = _smarc_get_ctrl_pin(carrier, HDL_SMARC_CARRIER_DEPENDENCY_POWER_BAD_PIN);
    hdl_gpio_pin_t *rst_in_pin = _smarc_get_ctrl_pin(carrier, HDL_SMARC_CARRIER_DEPENDENCY_RESET_IN_PIN);
    if(pow_bad_pin != NULL) hdl_gpio_set_active(pow_bad_pin);
    if(rst_in_pin != NULL) hdl_gpio_set_inactive(rst_in_pin);
    carrier_var->old_state = HDL_SMARC_CARRIER_STATE_INITIAL;
    carrier_var->state = HDL_SMARC_CARRIER_STATE_INITIAL;
    carrier_var->target_state = HDL_SMARC_CARRIER_STATE_INITIAL;
    coroutine_add(&carrier_var->worker, &_smarc_carrier_work, carrier);
    return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_UNLOADED;
}

const hdl_smarc_carrier_iface_t hdl_smarc_carrier_iface = {
.init = &_hdl_smarc_carrier,
.event_subscribe = &_hdl_smarc_carrier_event_subscribe,
.set_target_state = &_hdl_smarc_carrier_set_target_state,
.get_target_state = &_hdl_smarc_carrier_get_target_state,
.get_current_state = &_hdl_smarc_carrier_get_current_state,
.force_state = &_hdl_smarc_carrier_force_state,
.boot_select = &_hdl_smarc_carrier_boot_select
};

