#ifndef HDL_TIMER_EVENT_H_
#define HDL_TIMER_EVENT_H_

#define HDL_TIMER_EVENT_PRV_SIZE         25
typedef enum {
  HDL_TIMER_EVENT_STOP,
  HDL_TIMER_EVENT_RUN,
} hdl_timer_event_state_t;

/* depends on:
  timer
 */
typedef struct {
  hdl_module_t module;
  uint32_t delay;
  hdl_event_t event;
  PRIVATE(hdl, HDL_TIMER_EVENT_PRV_SIZE);
} hdl_timer_event_t;

hdl_module_state_t hdl_timer_event(void *desc, uint8_t enable);

hdl_timer_event_state_t hdl_timer_event_state_get(hdl_timer_event_t *timer);

uint8_t hdl_timer_event_run_once(hdl_timer_event_t *timer);
uint8_t hdl_timer_event_run(hdl_timer_event_t *timer);
uint8_t hdl_timer_event_reset(hdl_timer_event_t *timer);
uint8_t hdl_timer_event_stop(hdl_timer_event_t *timer);
uint32_t hdl_timer_event_get_timer(hdl_timer_event_t *timer);

#endif // HDL_TIMER_EVENT_H_
