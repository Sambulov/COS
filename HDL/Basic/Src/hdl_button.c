#include "hdl.h"
#include "CodeLib.h"

typedef enum {
  HDL_BTN_O_DEFAULT = 0,
  HDL_BTN_O_RELEASE,
  HDL_BTN_O_PRESS,
  HDL_BTN_O_CLICK_PRESS,
  HDL_BTN_O_CLICK_DEBOUNCE,
} hdl_btn_output_state_t;

typedef struct {
  hdl_module_t module;
  const hdl_button_config_t *config;
  hdl_event_t event;
  struct {
    hdl_btn_state_t input_state;
    hdl_btn_output_state_t output_state;
    uint32_t output_change_time;
    uint32_t input_change_time;
    coroutine_t button_worker;
  } private;  
} hdl_button_private_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_button_private_t, hdl_button_t, HDL_BUTTON_PRV_SIZE, hdl_button.h);
_Static_assert(offsetof(hdl_button_private_t, event) == offsetof(hdl_button_t, event), "In hdl_button.h hdl_button_t properties order doesn't match");

static uint8_t _button_handler(coroutine_t *this, uint8_t cancel, void *arg) {
  hdl_button_private_t *btn = (hdl_button_private_t *)arg;
  hdl_gpio_pin_t *btn_gpio = (hdl_gpio_pin_t *)btn->module.dependencies[0];
  hdl_time_counter_t *btn_timer = (hdl_time_counter_t *)btn->module.dependencies[1];
  switch (btn->private.output_state) {
    case HDL_BTN_O_CLICK_PRESS:
      btn->private.output_state = HDL_BTN_O_CLICK_DEBOUNCE;
      btn->private.output_change_time = hdl_time_counter_get(btn_timer);
    case HDL_BTN_O_PRESS:
      hdl_gpio_set_active(btn_gpio);
      break;
    case HDL_BTN_O_CLICK_DEBOUNCE: {
      uint8_t debounce_timeout = TIME_ELAPSED(btn->private.output_change_time, btn->config->debounce_delay, hdl_time_counter_get(btn_timer));
      if(debounce_timeout) {
        btn->private.output_state = HDL_BTN_O_RELEASE;
      }
      break;
    }
    case HDL_BTN_O_RELEASE:
      hdl_gpio_set_inactive(btn_gpio);
      btn->private.output_state = HDL_BTN_O_DEFAULT;
    case HDL_BTN_O_DEFAULT:
    default:
      break;
  }

  uint8_t btn_active = hdl_gpio_is_active(btn_gpio);
  switch (btn->private.input_state) {
    case HDL_BTN_RELEASED:
      if(!btn_active) 
        break;
      btn->private.input_change_time = hdl_time_counter_get(btn_timer);
      btn->private.input_state = HDL_BTN_PRESS_DEBOUNCE;
    case HDL_BTN_PRESS_DEBOUNCE: {
      if(!btn_active) {
        btn->private.input_state = HDL_BTN_RELEASED;
        break;
      }
      uint8_t debounce_timeout = TIME_ELAPSED(btn->private.input_change_time, btn->config->debounce_delay, hdl_time_counter_get(btn_timer));
      if(debounce_timeout) {
        btn->private.input_state = HDL_BTN_PRESSED;
        hdl_event_raise(&btn->event, (void *)btn, HDL_BTN_EVENT_PRESS);
      }
    }
    case HDL_BTN_PRESSED:
      if(btn_active) {
        uint8_t hold_timeout = btn->config->hold_delay && TIME_ELAPSED(btn->private.input_change_time, btn->config->hold_delay, hdl_time_counter_get(btn_timer));
        if(hold_timeout) {
          btn->private.input_state = HDL_BTN_HOLDING;
          hdl_event_raise(&btn->event, (void *)btn, HDL_BTN_EVENT_HOLD);
        }
      }
      else {
        btn->private.input_state = HDL_BTN_RELEASED;
        hdl_event_raise(&btn->event, (void *)btn, HDL_BTN_EVENT_RELEASE);
        hdl_event_raise(&btn->event, (void *)btn, HDL_BTN_EVENT_CLICK);
      }
      break;
    case HDL_BTN_HOLDING:
    default:
      if(!btn_active) {
        btn->private.input_state = HDL_BTN_RELEASED;
        hdl_event_raise(&btn->event, (void *)btn, HDL_BTN_EVENT_RELEASE);
      }
      break;
  }
  return cancel;
}

hdl_module_state_t hdl_button(void *desc, uint8_t enable) {
  hdl_button_private_t *btn = (hdl_button_private_t *)desc;
  if(desc != NULL) {
    if(enable) {
      btn->private.input_state = HDL_BTN_RELEASED;
      btn->private.output_state = HDL_BTN_O_DEFAULT;
      coroutine_add(&btn->private.button_worker, &_button_handler, desc);
      return HDL_MODULE_ACTIVE;
    }
    coroutine_cancel(&btn->private.button_worker);
  }
  return HDL_MODULE_UNLOADED;
}

hdl_btn_state_t hdl_btn_state_get(hdl_button_t *desc) {
  hdl_button_private_t *btn = (hdl_button_private_t *)desc;
  return (desc != NULL)? btn->private.input_state: HDL_BTN_UNKNOWN;
}

static uint8_t _hdl_btn_sw_event(hdl_button_private_t *btn, hdl_btn_output_state_t check_state, hdl_btn_output_state_t set_state) {
  if((btn == NULL) || (btn->private.output_state != check_state))
    return HDL_FALSE;
  btn->private.output_state = set_state;
  return HDL_TRUE;
}

uint8_t hdl_btn_sw_press(hdl_button_t *desc) {
  return _hdl_btn_sw_event((hdl_button_private_t *)desc, HDL_BTN_O_DEFAULT, HDL_BTN_O_PRESS);
}

uint8_t hdl_btn_sw_click(hdl_button_t *desc) {
  return _hdl_btn_sw_event((hdl_button_private_t *)desc, HDL_BTN_O_DEFAULT, HDL_BTN_O_CLICK_PRESS);
}

uint8_t hdl_btn_sw_release(hdl_button_t *desc) {
  return _hdl_btn_sw_event((hdl_button_private_t *)desc, HDL_BTN_O_PRESS, HDL_BTN_O_RELEASE);
}
