#include "hdl_portable.h"

#define hdl_exti_clear_pending(exti_line)       (EXTI->PR |= exti_line)

static hdl_module_state_t _hdl_exti(const void *desc, uint8_t enable) {
  if(enable) {
    hdl_exti_controller_t *exti = (hdl_exti_controller_t *)desc;
    const hdl_exti_t * const *extis = exti->config->extis;
    if(extis != NULL) {
      while (*extis != NULL) {
        CL_REG_SET(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
        uint8_t exti_no = 31 - __CLZ((*extis)->line);
        if(exti_no <= 15) { /* if GPIO exti lines */
          uint8_t ind = exti_no >> 2;
          uint8_t off = (exti_no & 0x3) << 2;
          CL_REG_MODIFY(SYSCFG->EXTICR[ind], (SYSCFG_EXTICR1_EXTI0 << off), ((*extis)->source << off));
        } /* other lines from internal modules are fixed */
        if((*extis)->trigger & HDL_EXTI_TRIGGER_FALLING) EXTI->FTSR |= (*extis)->line;
        else EXTI->FTSR &= ~((*extis)->line);
        if((*extis)->trigger & HDL_EXTI_TRIGGER_RISING) EXTI->RTSR |= (*extis)->line;
        else EXTI->RTSR &= ~((*extis)->line);
        EXTI->EMR |= (*extis)->line;
        if((*extis)->mode == HDL_EXTI_MODE_INTERRUPT) {
          EXTI->IMR |= (*extis)->line;
        }
        extis++;
      }
    }
    return HDL_MODULE_ACTIVE;
  }
  else {
    //TODO: disable nvic
    CL_REG_CLEAR(EXTI->IMR, EXTI_LINES_ALL);
    CL_REG_CLEAR(EXTI->FTSR, EXTI_LINES_ALL);
    CL_REG_CLEAR(EXTI->RTSR, EXTI_LINES_ALL);
    CL_REG_CLEAR(EXTI->EMR, EXTI_LINES_ALL);
    hdl_exti_clear_pending(EXTI_LINES_ALL);
  }
  return HDL_MODULE_UNLOADED;
}

static void _hdl_exti_sw_trigger(const void *desc, hdl_exti_line_t line) {
  (void)desc;
  EXTI->SWIER |= line;
}

const hdl_exti_controller_iface_t hdl_exti_controller_iface = {
  .init = &_hdl_exti,
  .trigger = &_hdl_exti_sw_trigger
};
