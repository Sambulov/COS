#include "hdl_portable.h"

hdl_module_state_t hdl_tick_counter(void *desc, const uint8_t enable) {
  hdl_tick_counter_t *counter = (hdl_tick_counter_t *)desc;
  uint32_t periph = (uint32_t)counter->module.reg;
  hdl_tick_counter_hw_config_t *config = (hdl_tick_counter_hw_config_t *)counter->config;
  if(enable) {
    if(periph == (uint32_t)SysTick) {
      hdl_tick_counter_systick_config_t *config = (hdl_tick_counter_systick_config_t *)counter->config;
      SysTick->LOAD  = config->period & SysTick_LOAD_RELOAD_Msk;        /* set reload register */
      SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
      /* if clock AHB set SysTick_CTRL_CLKSOURCE_Msk */
      /* TODO: check if clock source valid */
      SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |                     /* Set AHB clock source*/
                      SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick Timer */
      return HDL_MODULE_INIT_OK;
    }
    rcu_periph_clock_enable(config->rcu);
    timer_parameter_struct init_p = {
      .alignedmode = config->alignedmode,
      .clockdivision = config->clockdivision,
      .counterdirection = config->counterdirection,
      .period = config->period,
      .prescaler = config->prescaler,
      .repetitioncounter = config->repetitioncounter
    };
    timer_init(periph, &init_p);
    timer_enable(periph);
    return HDL_MODULE_INIT_OK; 
  }
  if(periph == (uint32_t)SysTick) {
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  }
  else {
    if(periph == (uint32_t)SysTick) {
      SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    }
    else {
      timer_disable(periph);
      rcu_periph_clock_disable(config->rcu);
    }
  }
  return HDL_MODULE_DEINIT_OK;
  /* TODO init timers */
}

uint32_t hdl_tick_counter_get(hdl_tick_counter_t *desc) {
  hdl_tick_counter_t *counter = (hdl_tick_counter_t *)desc;
  uint32_t periph = (uint32_t)counter->module.reg;
  hdl_tick_counter_hw_config_t *config = (hdl_tick_counter_hw_config_t *)counter->config;
  if(hdl_state(&counter->module) == HDL_MODULE_INIT_OK) {
    if(periph == (uint32_t)SysTick) {
      return (SysTick->LOAD - SysTick->VAL);
    }
    if(config->counterdirection == TIMER_COUNTER_UP)
      return TIMER_CNT(periph);
    return (TIMER_CAR(periph) - TIMER_CNT(periph));
  }
  return 0;
}