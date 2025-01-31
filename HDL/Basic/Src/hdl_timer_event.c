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
  hdl_event_t event;
  struct {
    uint32_t time_stamp;
    uint32_t delay;
    hdl_timer_mode_t mode;
    coroutine_t timer_events_worker;
  } private;
} hdl_timer_private_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_timer_private_t, hdl_timer_t, HDL_TIMER_EVENT_PRV_SIZE, hdl_timer_event.h);

static uint8_t _timer_events_handler(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_timer_private_t *timer_event = (hdl_timer_private_t *)arg;
  hdl_time_counter_t *timer = (hdl_time_counter_t *)timer_event->module.dependencies[0];
  if(timer_event->private.mode != HDL_TIMER_EVENT_IDLE) {
    uint32_t now = hdl_time_counter_get(timer);
    if(TIME_ELAPSED(timer_event->private.time_stamp, timer_event->private.delay, now)) {
      timer_event->private.time_stamp += timer_event->private.delay;
      if(timer_event->private.mode == HDL_TIMER_EVENT_SINGLE) {
        timer_event->private.mode = HDL_TIMER_EVENT_IDLE;
      }
      hdl_event_raise(&timer_event->event, (void *)timer_event, 0);
    }
  }
  return cancel;
}

hdl_module_state_t hdl_timer(void *desc, uint8_t enable) {
  hdl_timer_private_t *timer_event = (hdl_timer_private_t *)desc;
  if(desc != NULL) {
    if(enable) {
      timer_event->private.mode = HDL_TIMER_EVENT_IDLE;
      coroutine_add(&timer_event->private.timer_events_worker, &_timer_events_handler, (void*)timer_event);
      return HDL_MODULE_ACTIVE;
    }
    coroutine_cancel(&timer_event->private.timer_events_worker);
  }
  return HDL_MODULE_UNLOADED;
}

uint8_t hdl_timer_set(hdl_timer_t *timer, uint32_t delay, hdl_timer_mode_t mode) {
  hdl_timer_private_t *timer_event = (hdl_timer_private_t *)timer;
  if((timer_event != NULL) && (hdl_state(&timer_event->module) == HDL_MODULE_ACTIVE)) {
    hdl_time_counter_t *timer = (hdl_time_counter_t *)timer_event->module.dependencies[0];
    timer_event->private.time_stamp = hdl_time_counter_get(timer);
    timer_event->private.mode = mode;
    timer_event->private.delay = delay;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

hdl_timer_mode_t hdl_timer_mode(hdl_timer_t *timer) {
  hdl_timer_private_t *timer_event = (hdl_timer_private_t *)timer;
  if((timer_event != NULL) && (hdl_state(&timer_event->module) == HDL_MODULE_ACTIVE)) {
    return timer_event->private.mode;
  }
  return HDL_TIMER_EVENT_IDLE;
}

uint32_t hdl_timer_left(hdl_timer_t *timer) {
  hdl_timer_private_t *timer_event = (hdl_timer_private_t *)timer;
  if((timer_event != NULL) && (hdl_state(&timer_event->module) == HDL_MODULE_ACTIVE)) {
    hdl_time_counter_t *timer = (hdl_time_counter_t *)timer_event->module.dependencies[0];
    return timer_event->private.delay - (timer_event->private.mode != HDL_TIMER_EVENT_IDLE)? (hdl_time_counter_get(timer) - timer_event->private.time_stamp): 0;
  }
  return 0;
}
