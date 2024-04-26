#include "hdl_portable.h"

hdl_module_state_t hdl_clock_counter(void *desc, const uint8_t enable) {
  hdl_clock_counter_t *counter = (hdl_clock_counter_t *)desc;
  if((uint32_t)counter->module.reg == (uint32_t)SysTick) {
    SysTick->LOAD  = (uint32_t)(counter->counter_reload & SysTick_LOAD_RELOAD_Msk); /* set reload register */
    SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
    /* if clock AHB set SysTick_CTRL_CLKSOURCE_Msk */
    /* TODO: check if clock source valid */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |                     /* Set AHB clock source*/
                     SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick Timer */
    return HDL_MODULE_INIT_OK;
  }
  else if((uint32_t)counter->module.reg == (uint32_t)TIMER0){
    hdl_clock_prescaler_t *clock_prescaler = (hdl_clock_prescaler_t *)counter->module.dependencies[0];
    timer_parameter_struct config;
    config.prescaler = clock_prescaler->muldiv_factor;
    config.alignedmode = TIMER_COUNTER_EDGE;
    config.counterdirection = (counter->diction == HDL_UP_COUNTER) ? TIMER_COUNTER_UP : TIMER_COUNTER_DOWN;
    config.period = counter->counter_reload;
    config.clockdivision = TIMER_CKDIV_DIV1;
    config.repetitioncounter = 0;
    rcu_periph_clock_enable(RCU_TIMER0);
    timer_init(TIMER0, &config);
    timer_enable(TIMER0);
    return HDL_MODULE_INIT_OK;
  }
  return HDL_MODULE_INIT_FAILED;
  /* TODO init timers */
}

uint32_t hdl_clock_counter_get_count(hdl_clock_counter_t *desc) {
  hdl_clock_counter_t *counter = (hdl_clock_counter_t *)desc;
  if((uint32_t)counter->module.reg == (uint32_t)SysTick) {
    return (SysTick->LOAD - SysTick->VAL);
  }
  /* TODO get timers count */
  return 0;
}
