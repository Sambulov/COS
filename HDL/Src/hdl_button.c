#include "hdl.h"

#define HDL_BUTTON_VALIDATION_MARKER    ((uint32_t)(0xAB620D7F))

typedef struct {
  const hdl_button_hw_t *hw;
  const hdl_callback_t *on_button_down_cb;
  const hdl_callback_t *on_button_up_cb;
  const hdl_callback_t *on_button_click_cb;
  const hdl_callback_t *on_button_hold_cb;
  uint32_t validation;
  hdl_btn_state_t state;
  uint32_t change_time;
} hdl_button_private_t;

_Static_assert(sizeof(hdl_button_private_t) == sizeof(hdl_button_t), "In hdl_button.h data structure size of hdl_button_t doesn't match, check HDL_BUTTON_DESC_SIZE");

#define _hdl_btn_valid(desc) ((desc != NULL) && (desc->validation == HDL_BUTTON_VALIDATION_MARKER))

void hdl_btn_init(hdl_button_t *desc) {
  if((desc != NULL) && (desc->hw != NULL) && (desc->hw->btn_gpio != NULL)) {
    hdl_button_private_t *pdesc = (hdl_button_private_t *)desc;
    pdesc->validation = HDL_BUTTON_VALIDATION_MARKER;
    pdesc->state = HDL_BTN_RELEASED;
    hdl_gpio_init(pdesc->hw->btn_gpio);
  }
}

hdl_btn_state_t hdl_btn_state_get(hdl_button_t *desc) {
  hdl_button_private_t *pdesc = (hdl_button_private_t *)desc;
  return _hdl_btn_valid(pdesc)? pdesc->state: HDL_BTN_UNKNOWN;
}

void hdl_btn_process(hdl_button_t *desc) {
  hdl_button_private_t *pdesc = (hdl_button_private_t *)desc;
  if(!_hdl_btn_valid(pdesc))
    return;
  uint8_t btn_active = 0;
  btn_active = (hdl_gpio_read(pdesc->hw->btn_gpio) == pdesc->hw->active_state);
  switch (pdesc->state) {
    case HDL_BTN_RELEASED:
      if(!btn_active) 
        break;
        pdesc->change_time = hdl_millis();
        pdesc->state = HDL_BTN_PRESS;
    case HDL_BTN_PRESS:
      if(!btn_active) {
        pdesc->state = HDL_BTN_RELEASED;
        break;
      }
      uint8_t debounce_timeout = (hdl_millis() - pdesc->change_time) >= pdesc->hw->debounce_delay;
      if(debounce_timeout) {
        pdesc->state = HDL_BTN_PRESSED;
        if((pdesc->on_button_down_cb != NULL) && (pdesc->on_button_down_cb->handler != NULL))
          pdesc->on_button_down_cb->handler(pdesc, pdesc->on_button_down_cb->context);
      }
    case HDL_BTN_PRESSED:
      if(btn_active) {
        uint8_t hold_timeout = ((hdl_millis() - pdesc->change_time) > (pdesc->hw->hold_delay - 1));
        if(hold_timeout) {
          pdesc->state = HDL_BTN_HOLDING;
          if((pdesc->on_button_hold_cb != NULL) && (pdesc->on_button_hold_cb->handler != NULL))
            pdesc->on_button_hold_cb->handler(pdesc, pdesc->on_button_hold_cb->context);
        }
      }
      else {
        pdesc->state = HDL_BTN_RELEASED;
        if((pdesc->on_button_up_cb != NULL) && (pdesc->on_button_up_cb->handler != NULL))
          pdesc->on_button_up_cb->handler(pdesc, pdesc->on_button_up_cb->context);
        if((pdesc->on_button_click_cb != NULL) && (pdesc->on_button_click_cb->handler != NULL))
          pdesc->on_button_click_cb->handler(pdesc, pdesc->on_button_click_cb->context);
      }
      break;
    case HDL_BTN_HOLDING:
    default:
      if(!btn_active) {
        pdesc->state = HDL_BTN_RELEASED;
        if((pdesc->on_button_up_cb != NULL) && (pdesc->on_button_up_cb->handler != NULL))
          pdesc->on_button_up_cb->handler(pdesc, pdesc->on_button_up_cb->context);
      }
      break;
  }
}
