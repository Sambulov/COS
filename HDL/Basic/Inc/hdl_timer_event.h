#ifndef HDL_TIMER_EVENT_H_
#define HDL_TIMER_EVENT_H_

#define HDL_TIMER_EVENT_PRV_SIZE         40
typedef enum {
  HDL_TIMER_EVENT_IDLE,
  HDL_TIMER_EVENT_LOOP,
  HDL_TIMER_EVENT_SINGLE,
} hdl_timer_event_mode_t;

/* depends on:
  timer
 */
typedef struct {
  hdl_module_t module;
  hdl_event_t event;
  PRIVATE(hdl, HDL_TIMER_EVENT_PRV_SIZE);
} hdl_timer_event_t;

hdl_module_state_t hdl_timer_event(void *desc, uint8_t enable);

uint8_t hdl_timer_event_set(hdl_timer_event_t *timer, uint32_t delay, hdl_timer_event_mode_t mode);
hdl_timer_event_mode_t hdl_timer_event_mode(hdl_timer_event_t *timer);
uint32_t hdl_timer_event_time_left(hdl_timer_event_t *timer);

#endif // HDL_TIMER_EVENT_H_
