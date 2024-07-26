#include "hdl_portable.h"
#include "CodeLib.h"

#define hdl_exti_clear_pending(exti_line)       (EXTI_PD |= exti_line)

void _hdl_exti_set(hdl_exti_controller_t *ext_ctrl) {
  hdl_exti_t **extis = ext_ctrl->extis;
  if(extis != NULL) {
    while (*extis != NULL) {
      uint8_t exti_no = 31 - __CLZ((*extis)->line);
      if(exti_no <= 15) { /* if GPIO exti lines */
        syscfg_exti_line_config((*extis)->source, exti_no);
      } /* other lines from internal modules are fixed */
      if((*extis)->trigger & HDL_EXTI_TRIGGER_FALLING) {
        EXTI_FTEN |= (*extis)->line;
      }
      else {
        EXTI_FTEN &= ~((*extis)->line);
      }
      if((*extis)->trigger & HDL_EXTI_TRIGGER_RISING) {
        EXTI_RTEN |= (*extis)->line;
      }
      else {
        EXTI_RTEN &= ~((*extis)->line);
      }
      EXTI_EVEN |= (*extis)->line;
      if((*extis)->mode == HDL_EXTI_MODE_INTERRUPT) {
        EXTI_INTEN |= (*extis)->line;
      }
      extis++;
    }
  }
}

hdl_module_state_t hdl_exti(void *desc, uint8_t enable) {
  if(enable) {
    hdl_exti_controller_t *exti = (hdl_exti_controller_t *)desc;
    hdl_nvic_t *ic = (hdl_nvic_t *)exti->module.dependencies[0];
    /* TODO: finde wokaround to save context for interrupt vector */
    _hdl_exti_set(exti);
    return HDL_MODULE_INIT_OK;
  }
  else {
    //TODO: disable nvic
    HDL_REG_CLEAR(EXTI_INTEN, EXTI_LINES_ALL);
    HDL_REG_CLEAR(EXTI_FTEN, EXTI_LINES_ALL);
    HDL_REG_CLEAR(EXTI_RTEN, EXTI_LINES_ALL);
    HDL_REG_CLEAR(EXTI_EVEN, EXTI_LINES_ALL);
    hdl_exti_clear_pending(EXTI_LINES_ALL);
  }
  return HDL_MODULE_DEINIT_OK;
}

void hdl_exti_sw_trigger(hdl_exti_controller_t *desc, hdl_exti_line_t line) {
  EXTI_SWIEV |= line;
}
