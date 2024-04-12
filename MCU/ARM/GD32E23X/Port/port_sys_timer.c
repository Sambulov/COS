#include "hdl_portable.h"

void event_sys_timer_isr(uint32_t event, void *sender, void *context) {
  hdl_sys_timer_t *desc = (hdl_sys_timer_t *)context;
  desc->val++;
}

/*=============================================*/
/* TODO: Interrupt arc */
static hdl_sys_timer_t *__desc = NULL;

void SysTick_Handler() {
  event_sys_timer_isr(0, NULL, (void*)__desc);
}
/*=============================================*/

hdl_init_state_t hdl_sys_timer(void *desc, const uint8_t enable) {
  if(enable) {
    /*Enable systick interrupt*/
    hdl_sys_timer_t *timer = (hdl_sys_timer_t *)desc;
    hdl_clock_counter_t *counter = (hdl_clock_counter_t *)timer->hw.dependencies[0];
    hdl_clock_t *clock = (hdl_clock_t *)counter->hw.dependencies[0];
    __desc = desc;
    /* TODO */
    NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    return HDL_HW_INIT_OK;
  }
  else {

  }
}

// uint32_t hdl_sys_timer_get(hdl_sys_timer_t *desc) {
//   return desc->val;
// }
