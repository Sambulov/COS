#ifndef HDL_TIMER_H_
#define HDL_TIMER_H_

#define HDL_TIMER_EVENT_PRV_SIZE         44

typedef enum {
  HDL_TIMER_EVENT_IDLE,
  HDL_TIMER_EVENT_LOOP,
  HDL_TIMER_EVENT_SINGLE,
} hdl_timer_mode_t;

/* depends on:
  time_counter
 */
typedef struct {
  hdl_module_t module;
  PRIVATE(hdl, HDL_TIMER_EVENT_PRV_SIZE);
} hdl_timer_t;

hdl_module_state_t hdl_timer(void *desc, uint8_t enable);

uint8_t hdl_timer_set(hdl_timer_t *timer, uint32_t delay, hdl_timer_mode_t mode);
uint8_t hdl_timer_subscribe(hdl_timer_t *timer, hdl_delegate_t *delegate);
hdl_timer_mode_t hdl_timer_mode(hdl_timer_t *timer);
uint32_t hdl_timer_left(hdl_timer_t *timer);

#endif /* HDL_TIMER_H_ */
