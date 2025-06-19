#ifndef PORT_SYSTICK_COUNTER_H_
#define PORT_SYSTICK_COUNTER_H_

#include "hdl_tick_counter.h"

typedef struct {
  uint32_t phy;
  uint32_t period;                            /*!< counter auto reload value */
  uint32_t clock_src_mask;                    /* clock source switch: SysTick_CTRL_CLKSOURCE_Msk */
} hdl_systick_counter_config_t;

/* depends on:
   hdl_clock_t
 */
hdl_module_new_t(hdl_systick_counter_t, 0, hdl_systick_counter_config_t*, hdl_tick_counter_iface_t);

extern const hdl_tick_counter_iface_t hdl_systick_counter_iface;

#endif /* PORT_SYSTICK_COUNTER_H_ */ 
