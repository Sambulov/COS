#include "hdl.h"

#define HDL_BUTTON_OVN    ((uint32_t)(0xAB620D7F))

typedef enum {
  HDL_BTN_O_DEFAULT = 0,
  HDL_BTN_O_RELEASE,
  HDL_BTN_O_PRESS,
  HDL_BTN_O_CLICK_PRESS,
  HDL_BTN_O_CLICK_DEBOUNCE,
} hdl_btn_output_state_t;

typedef struct {
  uint32_t ovn;
  hdl_btn_state_t input_state;
  hdl_btn_output_state_t output_state;
  uint32_t output_change_time;
  uint32_t input_change_time;
  const hdl_button_hw_t *hw;
  hdl_btn_event_t event_mask;
  event_handler_t button_event_cb;
  void *context;
} hdl_button_private_t;

_Static_assert(sizeof(hdl_button_private_t) == sizeof(hdl_button_t), "In hdl_button.h data structure size of hdl_button_t doesn't match, check HDL_BUTTON_DESC_SIZE");
_Static_assert(offsetof(hdl_button_private_t, button_event_cb) == offsetof(hdl_button_t, button_event_cb), "In hdl_button.h hdl_button_t properties order doesn't match");

#define _hdl_btn_valid(desc) ((desc != NULL) && (desc->ovn == HDL_BUTTON_OVN))

hdl_module_state_t hdl_button(void *desc, uint8_t enable) {
  hdl_button_t *btn = (hdl_button_t *)desc;
  if(desc != NULL) {
    hdl_button_private_t *pdesc = (hdl_button_private_t *)desc;
    pdesc->ovn = HDL_BUTTON_OVN;
    pdesc->input_state = HDL_BTN_RELEASED;
    pdesc->output_state = HDL_BTN_O_DEFAULT;
    //return hdl_gpio((void *)btn->hw->btn_gpio, enable);
  }
  return HDL_FALSE;
}

uint8_t _hdl_btn_init() {
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

static void _btn_rise_event(hdl_button_private_t *pdesc, hdl_btn_event_t event) {
  if((pdesc->button_event_cb != NULL) && (pdesc->event_mask & event))
    pdesc->button_event_cb(event, pdesc, pdesc->context);
}

void hdl_btn_work(hdl_button_t *desc) {
  // hdl_button_private_t *pdesc = (hdl_button_private_t *)desc;
  // if(!_hdl_btn_valid(pdesc) && (_hdl_btn_init(desc) != HDL_TRUE))
  //   return;
  // switch (pdesc->output_state) {
  //   case HDL_BTN_O_CLICK_PRESS:
  //     pdesc->output_state = HDL_BTN_O_CLICK_DEBOUNCE;
  //     pdesc->output_change_time = hdl_millis();
  //   case HDL_BTN_O_PRESS:
  //     hdl_gpio_write(pdesc->hw->btn_gpio, pdesc->hw->active_state);
  //     break;
  //   case HDL_BTN_O_CLICK_DEBOUNCE: {
  //     uint8_t debounce_timeout = (hdl_millis() - pdesc->output_change_time) >= pdesc->hw->debounce_delay;
  //     if(debounce_timeout) {
  //       pdesc->output_state = HDL_BTN_O_RELEASE;
  //     }
  //     break;
  //   }
  //   case HDL_BTN_O_RELEASE:
  //     hdl_gpio_write(pdesc->hw->btn_gpio, !pdesc->hw->active_state);
  //     pdesc->output_state = HDL_BTN_O_DEFAULT;
  //   case HDL_BTN_O_DEFAULT:
  //   default:
  //     break;
  // }

  // uint8_t btn_active = 0;
  // btn_active = (hdl_gpio_read(pdesc->hw->btn_gpio) == pdesc->hw->active_state);
  // switch (pdesc->input_state) {
  //   case HDL_BTN_RELEASED:
  //     if(!btn_active) 
  //       break;
  //     pdesc->input_change_time = hdl_millis();
  //     pdesc->input_state = HDL_BTN_PRESS_DEBOUNCE;
  //   case HDL_BTN_PRESS_DEBOUNCE: {
  //     if(!btn_active) {
  //       pdesc->input_state = HDL_BTN_RELEASED;
  //       break;
  //     }
  //     uint8_t debounce_timeout = (hdl_millis() - pdesc->input_change_time) >= pdesc->hw->debounce_delay;
  //     if(debounce_timeout) {
  //       pdesc->input_state = HDL_BTN_PRESSED;
  //       _btn_rise_event(pdesc, HDL_BTN_EVENT_PRESS);
  //     }
  //   }
  //   case HDL_BTN_PRESSED:
  //     if(btn_active) {
  //       uint8_t hold_timeout = pdesc->hw->hold_delay && ((hdl_millis() - pdesc->input_change_time) >= pdesc->hw->hold_delay);
  //       if(hold_timeout) {
  //         pdesc->input_state = HDL_BTN_HOLDING;
  //         _btn_rise_event(pdesc, HDL_BTN_EVENT_HOLD);
  //       }
  //     }
  //     else {
  //       pdesc->input_state = HDL_BTN_RELEASED;
  //       _btn_rise_event(pdesc, HDL_BTN_EVENT_RELEASE);
  //       _btn_rise_event(pdesc, HDL_BTN_EVENT_CLICK);
  //     }
  //     break;
  //   case HDL_BTN_HOLDING:
  //   default:
  //     if(!btn_active) {
  //       pdesc->input_state = HDL_BTN_RELEASED;
  //       _btn_rise_event(pdesc, HDL_BTN_EVENT_RELEASE);
  //     }
  //     break;
  // }
}
