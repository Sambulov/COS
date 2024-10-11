#ifndef PORT_TIME_COUNTER_H_
#define PORT_TIME_COUNTER_H_

#define HDL_TIME_COUNTER_PRV_SIZE         28

typedef struct {
  hdl_interrupt_t *reload_interrupt;
} hdl_time_counter_config_t;

#endif // PORT_TIME_COUNTER_H_