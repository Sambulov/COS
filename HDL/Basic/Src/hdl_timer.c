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
  uint32_t time_stamp;
  uint32_t delay;
  hdl_timer_mode_t mode;
  coroutine_t timer_events_worker;
} hdl_timer_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_timer_var_t, *((hdl_timer_t *)0)->obj_var, HDL_TIMER_VAR_SIZE, hdl_timer.h);

static uint8_t _timer_work_handler(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_timer_t *timer = (hdl_timer_t *)arg;
  hdl_time_counter_t *time = (hdl_time_counter_t *)timer->dependencies[0];
  hdl_timer_var_t *timer_var = (hdl_timer_var_t *)timer->obj_var;
  if(timer_var->mode != HDL_TIMER_MODE_IDLE) {
    uint32_t now = hdl_time_counter_get(time);
    if(TIME_ELAPSED(timer_var->time_stamp, timer_var->delay, now)) {
      timer_var->time_stamp += timer_var->delay;
      if(timer_var->mode == HDL_TIMER_MODE_SINGLE) {
        timer_var->mode = HDL_TIMER_MODE_IDLE;
      }
      hdl_event_raise(&timer_var->event, arg, 0);
    }
  }
  return cancel;
}

hdl_module_state_t hdl_timer(const void *desc, uint8_t enable) {
  hdl_timer_t *timer = (hdl_timer_t *)desc;
  hdl_timer_var_t *timer_var = (hdl_timer_var_t *)timer->obj_var;
  if(enable) {
    timer_var->mode = timer->config->mode;
    timer_var->delay = timer->config->delay;
    coroutine_add(&timer_var->timer_events_worker, &_timer_work_handler, (void*)timer);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&timer_var->timer_events_worker);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_timer_reset(const void *desc, uint32_t delay, hdl_timer_mode_t mode) {
  hdl_timer_t *timer = (hdl_timer_t *)desc;
  if((hdl_state(timer) != HDL_MODULE_UNLOADED)) {
    hdl_time_counter_t *time = (hdl_time_counter_t *)timer->dependencies[0];
    hdl_timer_var_t *timer_var = (hdl_timer_var_t *)timer->obj_var;
    timer_var->time_stamp = hdl_time_counter_get(time);
    timer_var->mode = mode;
    timer_var->delay = delay;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

// hdl_timer_mode_t hdl_timer_mode(hdl_timer_t *timer) {
//   hdl_timer_private_t *timer_event = (hdl_timer_private_t *)timer;
//   if((timer_event != NULL) && (hdl_state(&timer_event->module) == HDL_MODULE_ACTIVE)) {
//     return timer_event->private.mode;
//   }
//   return HDL_TIMER_EVENT_IDLE;
// }

// uint32_t hdl_timer_left(hdl_timer_t *timer) {
//   hdl_timer_private_t *timer_event = (hdl_timer_private_t *)timer;
//   if((timer_event != NULL) && (hdl_state(&timer_event->module) == HDL_MODULE_ACTIVE)) {
//     hdl_time_counter_t *timer = (hdl_time_counter_t *)timer_event->module.dependencies[0];
//     return timer_event->private.delay - (timer_event->private.mode != HDL_TIMER_EVENT_IDLE)? (hdl_time_counter_get(timer) - timer_event->private.time_stamp): 0;
//   }
//   return 0;
// }

static void _hdl_timer_subscribe(const void *timer, hdl_delegate_t *delegate) {
  hdl_event_subscribe(&((hdl_timer_var_t *)((hdl_timer_t *)timer)->obj_var)->event, delegate);  
}

const hdl_timer_iface_t hdl_timer_iface = {
  .init = &hdl_timer,
  .subscribe = &_hdl_timer_subscribe,
  .reset = &_hdl_timer_reset
};
