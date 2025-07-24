#ifndef PORT_TICK_COUNTER_H_
#define PORT_TICK_COUNTER_H_

#include "hdl_tick_counter.h"

typedef struct {
  uint32_t phy;
  uint32_t rcc;
  uint32_t period;                         /* Min_Data = 0x0000U and Max_Data = 0xFFFFU for 16bit timers */
  uint16_t prescaler;                      /* Min_Data = 0x0000U and Max_Data = 0xFFFFU */
  uint16_t counter_mode;                   /* TIM_COUNTERMODE_UP TIM_COUNTERMODE_DOWN TIM_COUNTERMODE_CENTERALIGNED1 TIM_COUNTERMODE_CENTERALIGNED2 TIM_COUNTERMODE_CENTERALIGNED3 */
  uint16_t clock_div;                      /* TIM_CLOCKDIVISION_DIV1 TIM_CLOCKDIVISION_DIV2 TIM_CLOCKDIVISION_DIV4 */
  uint16_t trgo_mode;                      /* TIM_TRGO_RESET, TIM_TRGO_ENABLE, TIM_TRGO_UPDATE, TIM_TRGO_OC1, TIM_TRGO_OC1REF, TIM_TRGO_OC2REF, TIM_TRGO_OC3REF, TIM_TRGO_OC4REF */
  uint8_t  repetition_counter;             /* counter repetition value, 0~255 */
} hdl_timertick_counter_config_t;

/* depends on:
   hdl_clock_t
 */
hdl_module_new_t(hdl_timertick_counter_t, 0, hdl_timertick_counter_config_t*, hdl_tick_counter_iface_t);

extern const hdl_tick_counter_iface_t hdl_timertick_counter_iface;

#endif /* PORT_CLOCK_COUNTER_H_ */ 
