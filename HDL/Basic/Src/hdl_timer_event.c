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
  uint32_t delay;
  hdl_event_t event;
  uint8_t raise_once : 1;
  uint8_t running : 1;
  uint32_t time_stamp;
  uint32_t current_delay;
  __linked_list_object__;
} hdl_timer_event_private_t;

_Static_assert(sizeof(hdl_timer_event_private_t) == sizeof(hdl_timer_event_t), "In hdl_timer_event.h data structure size of hdl_button_t doesn't match, check HDL_TIMER_EVENT_PRV_SIZE");
_Static_assert(offsetof(hdl_timer_event_private_t, event) == offsetof(hdl_timer_event_t, event), "In hdl_timer_event.h hdl_timer_event_t properties order doesn't match");


static void _timer_event_handler(LinkedListItem_t *item, void *arg) {
  hdl_timer_event_private_t *timer_event = linked_list_get_object(hdl_timer_event_private_t, item);
  hdl_timer_t *timer = (hdl_timer_t *)btn->module.dependencies[0];
  if(!timer_event->running)
    return;
  if(TIME_ELAPSED(timer_event->time_stamp, timer_event->current_delay,hdl_timer_get(timer))) {
    timer_event->current_delay = timer_event->delay;
    timer_event->time_stamp += timer_event->current_delay;
    hdl_event_raise(&timer_event->event, (void *)timer_event, 0);
    if(timer_event->raise_once) {
      timer_event->running = 0;
    }
  }
}

static uint8_t _timer_events_handler(coroutine_desc_t this, uint8_t cancel, void *arg) {
  linked_list_t timer_events_list = (linked_list_t)arg;
  linked_list_do_foreach(timer_events_list, &_timer_event_handler, NULL);
  return cancel;
}

hdl_module_state_t hdl_timer_event(void *desc, uint8_t enable) {
  static coroutine_desc_static_t timer_events_worker;
  static linked_list_t timer_events;
  hdl_timer_event_private_t *timer_event = (hdl_timer_event_private_t *)desc;
  if(desc != NULL) {
    if(enable) {
      timer_event->running = 0;
      linked_list_insert_last(&timer_events, linked_list_item(timer_event));
      coroutine_add_static(&timer_events_worker, &_timer_events_handler, (void*)timer_events);
      return HDL_MODULE_INIT_OK;
    }
    linked_list_unlink(linked_list_item(timer_event));
  }
  return HDL_MODULE_DEINIT_OK;
}

hdl_timer_event_state_t hdl_timer_event_state_get(hdl_timer_event_t *timer) {
  return ((hdl_timer_event_private_t *)timer)->running? HDL_TIMER_EVENT_RUN: HDL_TIMER_EVENT_STOP;
}

static uint8_t _hdl_timer_event_turn(hdl_timer_event_private_t *timer_event, uint8_t run, uint8_t once) {
  if((timer_event != NULL) && (timer_event->running != run)) {
    timer_event->raise_once = once;
    timer_event->running = run;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

uint8_t hdl_timer_event_run_once(hdl_timer_event_t *timer) {
  return _hdl_timer_event_turn((hdl_timer_event_private_t *)timer, 1, 1);
}

uint8_t hdl_timer_event_run(hdl_timer_event_t *timer) {
  return _hdl_timer_event_turn((hdl_timer_event_private_t *)timer, 1, 0);
}

uint8_t hdl_timer_event_stop(hdl_timer_event_t *timer) {
  return _hdl_timer_event_turn((hdl_timer_event_private_t *)timer, 0, 0);
}

uint8_t hdl_timer_event_reset(hdl_timer_event_t *timer) {
  hdl_timer_event_private_t *timer_event = (hdl_timer_event_private_t *)timer;
  if((timer_event != NULL) && (timer_event->running)) {
    hdl_timer_t *timer = (hdl_timer_t *)btn->module.dependencies[0];
    timer_event->time_stamp = hdl_timer_get(timer);
    return HDL_TRUE;
  }
  return HDL_FALSE;
}
