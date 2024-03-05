#include "hdl.h"

#define HDL_BUTTON_VALIDATION_MARKER    ((uint32_t)(0xAB620D7F))

typedef enum {
  HDL_BTN_O_DEFAULT = 0,
  HDL_BTN_O_RELEASE,
  HDL_BTN_O_PRESS,
  HDL_BTN_O_CLICK_PRESS,
  HDL_BTN_O_CLICK_DEBOUNCE,
} hdl_btn_output_state_t;

typedef struct {
  const hdl_callback_t *on_button_down_cb;
  const hdl_callback_t *on_button_up_cb;
  const hdl_callback_t *on_button_click_cb;
  const hdl_callback_t *on_button_hold_cb;
  uint32_t validation;
  const hdl_button_hw_t *hw;
  hdl_btn_state_t input_state;
  hdl_btn_output_state_t output_state;
  uint32_t output_change_time;
  uint32_t input_change_time;
} hdl_button_private_t;

_Static_assert(sizeof(hdl_button_private_t) == sizeof(hdl_button_t), "In hdl_button.h data structure size of hdl_button_t doesn't match, check HDL_BUTTON_DESC_SIZE");

#define _hdl_btn_valid(desc) ((desc != NULL) && (desc->validation == HDL_BUTTON_VALIDATION_MARKER))

void hdl_btn_init(hdl_button_t *desc, const hdl_button_hw_t *hw) {
  if((desc != NULL) && (hw != NULL) && (hw->btn_gpio != NULL)) {
    hdl_button_private_t *pdesc = (hdl_button_private_t *)desc;
    pdesc->hw = hw;
    pdesc->validation = HDL_BUTTON_VALIDATION_MARKER;
    pdesc->input_state = HDL_BTN_RELEASED;
    pdesc->output_state = HDL_BTN_O_DEFAULT;
    hdl_gpio_init(pdesc->hw->btn_gpio);
  }
}

hdl_btn_state_t hdl_btn_state_get(hdl_button_t *desc) {
  hdl_button_private_t *pdesc = (hdl_button_private_t *)desc;
  return _hdl_btn_valid(pdesc)? pdesc->input_state: HDL_BTN_UNKNOWN;
}

uint8_t hdl_btn_press(hdl_button_t *desc) {
  hdl_button_private_t *pdesc = (hdl_button_private_t *)desc;
  if(!_hdl_btn_valid(pdesc) || (pdesc->output_state != HDL_BTN_O_DEFAULT))
    return HDL_FALSE;
  pdesc->output_state = HDL_BTN_O_PRESS;
  return HDL_TRUE;
}

uint8_t hdl_btn_click(hdl_button_t *desc) {
  hdl_button_private_t *pdesc = (hdl_button_private_t *)desc;
  if(!_hdl_btn_valid(pdesc) || (pdesc->output_state != HDL_BTN_O_DEFAULT))
    return HDL_FALSE;
  pdesc->output_state = HDL_BTN_O_CLICK_PRESS;
  return HDL_TRUE;
}

uint8_t hdl_btn_release(hdl_button_t *desc) {
  hdl_button_private_t *pdesc = (hdl_button_private_t *)desc;
  if(!_hdl_btn_valid(pdesc) || (pdesc->output_state != HDL_BTN_O_PRESS))
    return HDL_FALSE;
  pdesc->output_state = HDL_BTN_O_RELEASE;
  return HDL_TRUE;
}


void hdl_btn_process(hdl_button_t *desc) {
  hdl_button_private_t *pdesc = (hdl_button_private_t *)desc;
  if(!_hdl_btn_valid(pdesc))
    return;
  switch (pdesc->output_state) {
    case HDL_BTN_O_CLICK_PRESS:
      pdesc->output_state = HDL_BTN_O_CLICK_DEBOUNCE;
      pdesc->output_change_time = hdl_millis();
    case HDL_BTN_O_PRESS:
      hdl_gpio_write(pdesc->hw->btn_gpio, pdesc->hw->active_state);
      break;
    case HDL_BTN_O_CLICK_DEBOUNCE: {
      uint8_t debounce_timeout = (hdl_millis() - pdesc->output_change_time) >= pdesc->hw->debounce_delay;
      if(debounce_timeout) {
        pdesc->output_state = HDL_BTN_O_RELEASE;
      }
      break;
    }
    case HDL_BTN_O_RELEASE:
      hdl_gpio_write(pdesc->hw->btn_gpio, !pdesc->hw->active_state);
      pdesc->output_state = HDL_BTN_O_DEFAULT;
    case HDL_BTN_O_DEFAULT:
    default:
      break;
  }

  uint8_t btn_active = 0;
  btn_active = (hdl_gpio_read(pdesc->hw->btn_gpio) == pdesc->hw->active_state);
  switch (pdesc->input_state) {
    case HDL_BTN_RELEASED:
      if(!btn_active) 
        break;
      pdesc->input_change_time = hdl_millis();
      pdesc->input_state = HDL_BTN_PRESS;
    case HDL_BTN_PRESS: {
      if(!btn_active) {
        pdesc->input_state = HDL_BTN_RELEASED;
        break;
      }
      uint8_t debounce_timeout = (hdl_millis() - pdesc->input_change_time) >= pdesc->hw->debounce_delay;
      if(debounce_timeout) {
        pdesc->input_state = HDL_BTN_PRESSED;
        if((pdesc->on_button_down_cb != NULL) && (pdesc->on_button_down_cb->handler != NULL))
          pdesc->on_button_down_cb->handler(pdesc, pdesc->on_button_down_cb->context);
      }
    }
    case HDL_BTN_PRESSED:
      if(btn_active) {
        uint8_t hold_timeout = pdesc->hw->hold_delay && ((hdl_millis() - pdesc->input_change_time) >= pdesc->hw->hold_delay);
        if(hold_timeout) {
          pdesc->input_state = HDL_BTN_HOLDING;
          if((pdesc->on_button_hold_cb != NULL) && (pdesc->on_button_hold_cb->handler != NULL))
            pdesc->on_button_hold_cb->handler(pdesc, pdesc->on_button_hold_cb->context);
        }
      }
      else {
        pdesc->input_state = HDL_BTN_RELEASED;
        if((pdesc->on_button_up_cb != NULL) && (pdesc->on_button_up_cb->handler != NULL))
          pdesc->on_button_up_cb->handler(pdesc, pdesc->on_button_up_cb->context);
        if((pdesc->on_button_click_cb != NULL) && (pdesc->on_button_click_cb->handler != NULL))
          pdesc->on_button_click_cb->handler(pdesc, pdesc->on_button_click_cb->context);
      }
      break;
    case HDL_BTN_HOLDING:
    default:
      if(!btn_active) {
        pdesc->input_state = HDL_BTN_RELEASED;
        if((pdesc->on_button_up_cb != NULL) && (pdesc->on_button_up_cb->handler != NULL))
          pdesc->on_button_up_cb->handler(pdesc, pdesc->on_button_up_cb->context);
      }
      break;
  }
}
