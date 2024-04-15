#include "hdl_portable.h"

hdl_init_state_t hdl_clock_counter(void *desc, const uint8_t enable) {
  hdl_clock_counter_t *counter = (hdl_clock_counter_t *)desc;
  if((uint32_t)counter->hw.periph == (uint32_t)SysTick) {
    SysTick->LOAD  = (uint32_t)(counter->counter_reload & SysTick_LOAD_RELOAD_Msk); /* set reload register */
    SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
    /* if clock AHB set SysTick_CTRL_CLKSOURCE_Msk */
    /* TODO: check if clock source valid */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |                     /* Set AHB clock source*/
                     SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick Timer */
    return HDL_HW_INIT_OK;
  }
  return HDL_HW_INIT_FAILED;
  /* TODO init timers */
}

uint32_t hdl_clock_counter_get_count(hdl_clock_counter_t *desc) {
  hdl_clock_counter_t *counter = (hdl_clock_counter_t *)desc;
  if((uint32_t)counter->hw.periph == (uint32_t)SysTick) {
    return (SysTick->LOAD - SysTick->VAL);
  }
  /* TODO get timers count */
  return 0;
}
