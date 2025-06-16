#ifndef PORT_TICK_COUNTER_H_
#define PORT_TICK_COUNTER_H_

#include "hdl_tick_counter.h"

typedef struct {
  uint32_t phy;
  uint32_t period;                            /*!< counter auto reload value, 0~65535 */
  uint16_t prescaler;                         /*!< prescaler value of the counter clock, 0~65535 */
  uint16_t alignedmode;                       /*!< TIMER_COUNTER_EDGE TIMER_COUNTER_CENTER_DOWN TIMER_COUNTER_CENTER_UP TIMER_COUNTER_CENTER_BOTH */
  uint16_t counterdirection;                  /*!< TIMER_COUNTER_UP TIMER_COUNTER_DOWN */
  uint16_t clockdivision;                     /*!< TIMER_CKDIV_DIV1 TIMER_CKDIV_DIV2 TIMER_CKDIV_DIV4 */
  uint8_t  repetitioncounter;                 /*!< counter repetition value, 0~255 */
  rcu_periph_enum rcu;                        /*!< RCU_TIMER0 RCU_TIMER2 RCU_TIMER5 RCU_TIMER13 RCU_TIMER14 RCU_TIMER15 RCU_TIMER16 */
} hdl_timertick_counter_config_t;

/* depends on:
   hdl_clock_t
 */
hdl_module_new_t(hdl_timertick_counter_t, 0, hdl_timertick_counter_config_t, hdl_tick_counter_iface_t);

extern const hdl_tick_counter_iface_t hdl_timertick_counter_iface;

#endif /* PORT_CLOCK_COUNTER_H_ */ 


