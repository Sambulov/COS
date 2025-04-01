#include "hdl_iface.h"

static hdl_module_state_t _hdl_tick_counter(const void *desc, const uint8_t enable) {
  hdl_tick_counter_mcu_t *counter = (hdl_tick_counter_mcu_t *)desc;
  uint32_t periph = (uint32_t)counter->config->phy;
  //const hdl_tick_counter_timer_config_t *config = counter->config->type.timer;
  if(enable) {
    if(periph == (uint32_t)SysTick) {
      const hdl_tick_counter_systick_config_t *config = counter->config->type.systick;
      SysTick->LOAD  = config->period & SysTick_LOAD_RELOAD_Msk;        /* set reload register */
      SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
      /* if clock AHB set SysTick_CTRL_CLKSOURCE_Msk */
      /* TODO: check if clock source valid */
      SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |                     /* Set AHB clock source*/
                      SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick Timer */
      return HDL_MODULE_ACTIVE;
    }
    // rcu_periph_clock_enable(config->rcu);
    // timer_parameter_struct init_p = {
    //   .alignedmode = config->alignedmode,
    //   .clockdivision = config->clockdivision,
    //   .counterdirection = config->counterdirection,
    //   .period = config->period,
    //   .prescaler = config->prescaler,
    //   .repetitioncounter = config->repetitioncounter
    // };
    // timer_init(periph, &init_p);
    // timer_enable(periph);
    // return HDL_MODULE_ACTIVE; 
    return HDL_MODULE_FAULT;
  }
  if(periph == (uint32_t)SysTick) {
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  }
  else {
    // timer_disable(periph);
    // rcu_periph_clock_disable(config->rcu);
  }
  return HDL_MODULE_UNLOADED;
  /* TODO init timers */
}

static uint32_t _hdl_tick_counter_get(const void *desc) {
  hdl_tick_counter_mcu_t *counter = (hdl_tick_counter_mcu_t *)desc;
  uint32_t periph = (uint32_t)counter->config->phy;
  //const hdl_tick_counter_timer_config_t *config = counter->config.timer;
  if(periph == (uint32_t)SysTick) {
    return (SysTick->LOAD - SysTick->VAL);
  }
  // if(config->counterdirection == TIMER_COUNTER_UP)
  //   return TIMER_CNT(periph);
  // return (TIMER_CAR(periph) - TIMER_CNT(periph));
  return 0;
}

void _hdl_tick_counter_set(const void *counter, uint32_t value, uint32_t period) {
  hdl_tick_counter_mcu_t *tick_cnt = (hdl_tick_counter_mcu_t *)counter;
  uint32_t periph = (uint32_t)tick_cnt->config->phy;
  //const hdl_tick_counter_timer_config_t *config = tick_cnt->config->type.timer;
  if(periph == (uint32_t)SysTick) {
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = (SysTick->LOAD - value);
    SysTick->LOAD = period & SysTick_LOAD_RELOAD_Msk;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
  }
  // else {
  //   timer_disable(periph);
  //   timer_interrupt_flag_clear(periph, TIMER_INT_FLAG_UP);
  //   if(config->counterdirection == TIMER_COUNTER_UP) {
  //     TIMER_CNT(periph) = value;
  //   }
  //   else {
  //     TIMER_CNT(periph) = (TIMER_CAR(periph) - value);
  //   }
  //   TIMER_CAR(periph) = period;
  //   timer_enable(periph);
  // }
}

void _hdl_tick_counter_stop(const void *counter) {
  hdl_tick_counter_mcu_t *tick_cnt = (hdl_tick_counter_mcu_t *)counter;
  uint32_t periph = (uint32_t)tick_cnt->config->phy;
  if(periph == (uint32_t)SysTick) {
      SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  }
  //else {
  //  timer_disable(periph);
  //  timer_interrupt_flag_clear(periph, TIMER_INT_FLAG_UP);
  //}
}

const hdl_tick_counter_iface_t hdl_tick_counter_iface = {
  .init = &_hdl_tick_counter,
  .get = &_hdl_tick_counter_get,
  .set = &_hdl_tick_counter_set,
  .stop = &_hdl_tick_counter_stop
};
