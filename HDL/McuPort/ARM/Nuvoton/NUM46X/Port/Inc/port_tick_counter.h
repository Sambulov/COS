#ifndef PORT_TICK_COUNTER_H_
#define PORT_TICK_COUNTER_H_

typedef struct {
  uint32_t period;                            /*!< counter auto reload value */
} hdl_tick_counter_systick_config_t;

typedef struct {

} hdl_tick_counter_timer_config_t;

typedef union {
  const hdl_tick_counter_systick_config_t *systick;
  const hdl_tick_counter_timer_config_t *timer;
} hdl_tick_counter_config_t;

#endif /* PORT_CLOCK_COUNTER_H_ */ 


