#ifndef HDL_TICK_COUNTER_H_
#define HDL_TICK_COUNTER_H_

#include "port_tick_counter.h"

/* depends on:
   hdl_clock_t
 */
typedef struct {
  hdl_module_t module;
  const hdl_tick_counter_config_t config;
} hdl_tick_counter_t;

hdl_module_state_t hdl_tick_counter(void *desc, const uint8_t enable);

uint32_t hdl_tick_counter_get(hdl_tick_counter_t *desc);
void hdl_tick_counter_set(hdl_tick_counter_t *counter, uint32_t value, uint32_t period);
void hdl_tick_counter_stop(hdl_tick_counter_t *desc);

#endif /* HDL_TICK_COUNTER_H_ */ 
