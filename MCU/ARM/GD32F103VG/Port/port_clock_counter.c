#include "hdl_portable.h"

static rcu_periph_enum _hdl_timer_get_rcu(hdl_clock_counter_t *counter) {
  switch((uint32_t)counter->module.reg) {
    case TIMER0:
      return RCU_TIMER0;
    case TIMER1:
      return RCU_TIMER1;
    case TIMER2:
      return RCU_TIMER2;
    case TIMER3:
      return RCU_TIMER3;
    case TIMER4:
      return RCU_TIMER4;
    case TIMER5:
      return RCU_TIMER5;
    case TIMER6:
      return RCU_TIMER6;
    case TIMER7:
      return RCU_TIMER7;
    case TIMER8:
      return RCU_TIMER8;
    case TIMER9:
      return RCU_TIMER9;
    case TIMER10:
      return RCU_TIMER10;
    case TIMER11:
      return RCU_TIMER11;
    case TIMER12:
      return RCU_TIMER12;
    case TIMER13:
      return RCU_TIMER13;
    default:
      return -1;
  }
}

hdl_module_state_t hdl_clock_counter(void *desc, const uint8_t enable) {
  hdl_clock_counter_t *counter = (hdl_clock_counter_t *)desc;
  if(enable) {
    if((uint32_t)counter->module.reg == (uint32_t)SysTick) {
      SysTick->LOAD  = (uint32_t)(counter->counter_reload & SysTick_LOAD_RELOAD_Msk); /* set reload register */
      SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
      /* if clock AHB set SysTick_CTRL_CLKSOURCE_Msk */
      /* TODO: check if clock source valid */
      SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |                     /* Set AHB clock source*/
                      SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick Timer */
      return HDL_MODULE_INIT_OK;
    }
    rcu_periph_enum rcu = _hdl_timer_get_rcu(counter);
    if(rcu == -1) return HDL_MODULE_INIT_FAILED;
    //hdl_clock_prescaler_t *clock_prescaler = (hdl_clock_prescaler_t *)counter->module.dependencies[0];
    timer_parameter_struct config;
    config.prescaler = 0;
    config.alignedmode = TIMER_COUNTER_EDGE;
    config.counterdirection = (counter->diction == HDL_UP_COUNTER) ? TIMER_COUNTER_UP : TIMER_COUNTER_DOWN;
    config.period = counter->counter_reload;
    config.clockdivision = TIMER_CKDIV_DIV1;
    config.repetitioncounter = 0;
    rcu_periph_clock_enable(rcu);
    timer_init((uint32_t)counter->module.reg, &config);
    TIMER_DMAINTEN((uint32_t)counter->module.reg) |= TIMER_DMAINTEN_UPIE;
    timer_enable((uint32_t)counter->module.reg);
    return HDL_MODULE_INIT_OK; 
  }
  if((uint32_t)counter->module.reg == (uint32_t)SysTick) {
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  }
  else {
    rcu_periph_enum rcu = _hdl_timer_get_rcu(counter);
    if(rcu == -1) return HDL_MODULE_INIT_FAILED;
    timer_disable((uint32_t)counter->module.reg);
    rcu_periph_clock_disable(rcu);
  }
  return HDL_MODULE_DEINIT_OK;
  /* TODO init timers */
}

uint32_t hdl_clock_counter_get_count(hdl_clock_counter_t *desc) {
  hdl_clock_counter_t *counter = (hdl_clock_counter_t *)desc;
  if(hdl_state(&counter->module) == HDL_MODULE_INIT_OK) {
    if((uint32_t)counter->module.reg == (uint32_t)SysTick) {
      return (SysTick->LOAD - SysTick->VAL);
    }
    if(counter->diction == HDL_UP_COUNTER)
      return TIMER_CNT((uint32_t)counter->module.reg);
    return (TIMER_CAR((uint32_t)counter->module.reg) - TIMER_CNT((uint32_t)counter->module.reg));
  }
  return 0;
}
