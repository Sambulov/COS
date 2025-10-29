#ifndef PORT_TICK_COUNTER_H_
#define PORT_TICK_COUNTER_H_

#include "hdl_tick_counter.h"

typedef struct {
  void *phy;
} hdl_timertick_counter_config_t;

/* depends on:
   hdl_clock_t
 */
hdl_module_new_t(hdl_timertick_counter_t, 0, hdl_timertick_counter_config_t*, hdl_tick_counter_iface_t);

extern const hdl_tick_counter_iface_t hdl_timertick_counter_iface;

#endif /* PORT_CLOCK_COUNTER_H_ */ 


