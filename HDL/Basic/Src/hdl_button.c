#include "hdl_iface.h"


typedef enum {
  HDL_BTN_O_DEFAULT = 0,
  HDL_BTN_O_RELEASE,
  HDL_BTN_O_PRESS,
  HDL_BTN_O_CLICK_PRESS,
  HDL_BTN_O_CLICK_DEBOUNCE,
} hdl_btn_output_state_t;

typedef struct {
  hdl_event_t event;
  hdl_btn_state_t input_state;
  hdl_btn_output_state_t output_state;
  uint32_t output_change_time;
  uint32_t input_change_time;
  coroutine_t button_worker;
} hdl_button_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_button_var_t, *((hdl_button_t *)0)->obj_var, HDL_BUTTON_VAR_SIZE, hdl_button.h);

static uint8_t _button_handler(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_button_t *btn = (hdl_button_t *)arg;
  hdl_button_var_t *btn_var = (hdl_button_var_t *)btn->obj_var;
  hdl_gpio_pin_t *btn_gpio = (hdl_gpio_pin_t *)btn->dependencies[0];
  hdl_time_counter_t *btn_timer = (hdl_time_counter_t *)btn->dependencies[1];
  switch (btn_var->output_state) {
    case HDL_BTN_O_CLICK_PRESS:
      btn_var->output_state = HDL_BTN_O_CLICK_DEBOUNCE;
      btn_var->output_change_time = hdl_time_counter_get((hdl_module_base_t *)btn_timer);
      break;
    case HDL_BTN_O_PRESS:
      hdl_gpio_set_active(btn_gpio);
      break;
    case HDL_BTN_O_CLICK_DEBOUNCE: {
      uint8_t debounce_timeout = CL_TIME_ELAPSED(btn_var->output_change_time, btn->config->debounce_delay, hdl_time_counter_get((hdl_module_base_t *)btn_timer));
      if(debounce_timeout) {
        btn_var->output_state = HDL_BTN_O_RELEASE;
      }
      break;
    }
    case HDL_BTN_O_RELEASE:
      hdl_gpio_set_inactive(btn_gpio);
      btn_var->output_state = HDL_BTN_O_DEFAULT;
    case HDL_BTN_O_DEFAULT:
    default:
      break;
  }

  uint8_t btn_active = hdl_gpio_is_active(btn_gpio);
  switch (btn_var->input_state) {
    case HDL_BTN_RELEASED:
      if(!btn_active) 
        break;
      btn_var->input_change_time = hdl_time_counter_get((hdl_module_base_t *)btn_timer);
      btn_var->input_state = HDL_BTN_PRESS_DEBOUNCE;
      break;
    case HDL_BTN_PRESS_DEBOUNCE: {
      if(!btn_active) {
        btn_var->input_state = HDL_BTN_RELEASED;
        break;
      }
      uint8_t debounce_timeout = CL_TIME_ELAPSED(btn_var->input_change_time, btn->config->debounce_delay, hdl_time_counter_get((hdl_module_base_t *)btn_timer));
      if(debounce_timeout) {
        btn_var->input_state = HDL_BTN_PRESSED;
        hdl_event_raise(&btn_var->event, (void *)btn, HDL_BTN_EVENT_PRESS);
      }
      break;
    }
    case HDL_BTN_PRESSED:
      if(btn_active) {
        uint8_t hold_timeout = btn->config->hold_delay && CL_TIME_ELAPSED(btn_var->input_change_time, btn->config->hold_delay, hdl_time_counter_get((hdl_module_base_t *)btn_timer));
        if(hold_timeout) {
          btn_var->input_state = HDL_BTN_HOLDING;
          hdl_event_raise(&btn_var->event, (void *)btn, HDL_BTN_EVENT_HOLD);
        }
      }
      else {
        btn_var->input_state = HDL_BTN_RELEASED;
        hdl_event_raise(&btn_var->event, (void *)btn, HDL_BTN_EVENT_RELEASE);
        hdl_event_raise(&btn_var->event, (void *)btn, HDL_BTN_EVENT_CLICK);
      }
      break;
    case HDL_BTN_HOLDING:
    default:
      if(!btn_active) {
        btn_var->input_state = HDL_BTN_RELEASED;
        hdl_event_raise(&btn_var->event, (void *)btn, HDL_BTN_EVENT_RELEASE);
      }
      break;
  }
  return cancel;
}

static hdl_module_state_t _hdl_button(const void *desc, uint8_t enable) {
  hdl_button_t *btn = (hdl_button_t *)desc;
  hdl_button_var_t *btn_var = (hdl_button_var_t *)btn->obj_var;
  if(enable) {
    btn_var->input_state = HDL_BTN_RELEASED;
    btn_var->output_state = HDL_BTN_O_DEFAULT;
    coroutine_add(&btn_var->button_worker, &_button_handler, (void *)btn);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&btn_var->button_worker);
  return HDL_MODULE_UNLOADED;
}

static hdl_btn_state_t _hdl_btn_state_get(const void *btn) {
  hdl_button_var_t *btn_var = (hdl_button_var_t *)(((hdl_button_t *)btn)->obj_var);
  return btn_var->input_state;
}

static uint8_t _hdl_btn_sw_event(hdl_button_var_t *btn, hdl_btn_output_state_t check_state, hdl_btn_output_state_t set_state) {
  if((btn == NULL) || (btn->output_state != check_state))
    return HDL_FALSE;
  btn->output_state = set_state;
  return HDL_TRUE;
}

static uint8_t _hdl_btn_sw_press(const void *btn) {
  return _hdl_btn_sw_event((hdl_button_var_t *)(((hdl_button_t *)btn)->obj_var), HDL_BTN_O_DEFAULT, HDL_BTN_O_PRESS);
}

static uint8_t _hdl_btn_sw_click(const void *btn) {
  return _hdl_btn_sw_event((hdl_button_var_t *)(((hdl_button_t *)btn)->obj_var), HDL_BTN_O_DEFAULT, HDL_BTN_O_CLICK_PRESS);
}

static uint8_t _hdl_btn_sw_release(const void *btn) {
  return _hdl_btn_sw_event((hdl_button_var_t *)(((hdl_button_t *)btn)->obj_var), HDL_BTN_O_PRESS, HDL_BTN_O_RELEASE);
}

static void _hdl_button_subscribe(const void *btn, hdl_delegate_t *delegate) {
  hdl_event_subscribe(&((hdl_button_var_t *)((hdl_button_t *)btn)->obj_var)->event, delegate);  
}

const hdl_button_iface_t hdl_button_iface = {
  .init = &_hdl_button,
  .subscribe = &_hdl_button_subscribe,
  .state_get = &_hdl_btn_state_get,
  .press = &_hdl_btn_sw_press,
  .click = &_hdl_btn_sw_click,
  .release = &_hdl_btn_sw_release
};
