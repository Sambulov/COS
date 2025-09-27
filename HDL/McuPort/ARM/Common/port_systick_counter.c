#include "hdl_iface.h"

static hdl_module_state_t _hdl_tick_counter(const void *desc, const uint8_t enable) {
  hdl_systick_counter_t *counter = (hdl_systick_counter_t *)desc;
  uint32_t periph = counter->config->phy;
  if(enable) {
    if(periph == (uint32_t)SysTick) {
      SysTick->LOAD  = counter->config->period & SysTick_LOAD_RELOAD_Msk;        /* set reload register */
      SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
      uint32_t ctrl = SysTick_CTRL_ENABLE_Msk;
      if(counter->config->clock_src) ctrl |= SysTick_CTRL_CLKSOURCE_Msk;
      else ctrl &= ~SysTick_CTRL_CLKSOURCE_Msk;
      SysTick->CTRL = ctrl;
      return HDL_MODULE_ACTIVE;
    }
  }
  if(periph == (uint32_t)SysTick) {
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  }
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_tick_counter_get(const void *counter, uint32_t *value, uint32_t *period) {
  hdl_systick_counter_t *tick_cnt = (hdl_systick_counter_t *)counter;
  uint32_t periph = tick_cnt->config->phy;
  if(periph == (uint32_t)SysTick) return HDL_FALSE;
  if(value != NULL) *value = (SysTick->LOAD - SysTick->VAL);
  if(period != NULL) *period = SysTick->LOAD + 1;
  return HDL_TRUE;
}

static uint8_t _hdl_tick_counter_set(const void *counter, uint32_t *value, uint32_t *period) {
  hdl_systick_counter_t *tick_cnt = (hdl_systick_counter_t *)counter;
  uint32_t periph = tick_cnt->config->phy;
  if(periph == (uint32_t)SysTick)  return HDL_FALSE;
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  if(value != NULL) SysTick->VAL = (SysTick->LOAD - *value);
  if(period != NULL) SysTick->LOAD = *period & SysTick_LOAD_RELOAD_Msk;
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
  return HDL_TRUE;
}

static uint8_t _hdl_tick_counter_stop(const void *counter) {
  hdl_systick_counter_t *tick_cnt = (hdl_systick_counter_t *)counter;
  uint32_t periph = tick_cnt->config->phy;
  if(periph != (uint32_t)SysTick) return HDL_FALSE;
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  return HDL_TRUE;
}

const hdl_tick_counter_iface_t hdl_systick_counter_iface = {
  .init = &_hdl_tick_counter,
  .get = &_hdl_tick_counter_get,
  .set = &_hdl_tick_counter_set,
  .stop = &_hdl_tick_counter_stop
};
