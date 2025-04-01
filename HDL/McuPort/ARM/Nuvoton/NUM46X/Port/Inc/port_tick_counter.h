#ifndef PORT_TICK_COUNTER_H_
#define PORT_TICK_COUNTER_H_

#include "hdl_tick_counter.h"

typedef struct {
  uint32_t period;                            /*!< counter auto reload value */
} hdl_tick_counter_systick_config_t;

typedef struct {

} hdl_tick_counter_timer_config_t;

typedef struct {
  void *phy;
  union {
      const hdl_tick_counter_timer_config_t *timer;
      const hdl_tick_counter_systick_config_t *systick;
  } type;
} hdl_tick_counter_config_t;

/* depends on:
   hdl_clock_t
 */
hdl_module_new_t(hdl_tick_counter_mcu_t, 0, hdl_tick_counter_config_t, hdl_tick_counter_iface_t);

extern const hdl_tick_counter_iface_t hdl_tick_counter_iface;

#endif /* PORT_CLOCK_COUNTER_H_ */ 


