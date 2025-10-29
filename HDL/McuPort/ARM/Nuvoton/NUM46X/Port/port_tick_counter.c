#include "hdl_iface.h"

static hdl_module_state_t _hdl_tick_counter(const void *desc, const uint8_t enable) {
  (void)desc;
  //hdl_timertick_counter_t *counter = (hdl_timertick_counter_t *)desc;
  //uint32_t periph = (uint32_t)counter->config->phy;
  //const hdl_tick_counter_timer_config_t *config = counter->config->type.timer;
  if(enable) {
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
    // timer_disable(periph);
    // rcu_periph_clock_disable(config->rcu);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_tick_counter_get(const void *desc, uint32_t *value, uint32_t *period) {
  (void)desc;(void)value;(void)period;
  //hdl_timertick_counter_t *counter = (hdl_timertick_counter_t *)desc;
  //uint32_t periph = (uint32_t)counter->config->phy;
  //const hdl_tick_counter_timer_config_t *config = counter->config.timer;
  // if(config->counterdirection == TIMER_COUNTER_UP)
  //   return TIMER_CNT(periph);
  // return (TIMER_CAR(periph) - TIMER_CNT(periph));
  return HDL_FALSE;
}

static uint8_t _hdl_tick_counter_set(const void *counter, uint32_t *value, uint32_t *period) {
  (void)counter;(void)value;(void)period;
  //hdl_timertick_counter_t *tick_cnt = (hdl_timertick_counter_t *)counter;
  //uint32_t periph = (uint32_t)tick_cnt->config->phy;
  //const hdl_tick_counter_timer_config_t *config = tick_cnt->config->type.timer;
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
  return HDL_FALSE;
}

static uint8_t _hdl_tick_counter_stop(const void *counter) {
  (void)counter;
  //hdl_timertick_counter_t *tick_cnt = (hdl_timertick_counter_t *)counter;
  //uint32_t periph = (uint32_t)tick_cnt->config->phy;
  //  timer_disable(periph);
  //  timer_interrupt_flag_clear(periph, TIMER_INT_FLAG_UP);
  return HDL_FALSE;
}

const hdl_tick_counter_iface_t hdl_timertick_counter_iface = {
  .init = &_hdl_tick_counter,
  .get = &_hdl_tick_counter_get,
  .set = &_hdl_tick_counter_set,
  .stop = &_hdl_tick_counter_stop
};
