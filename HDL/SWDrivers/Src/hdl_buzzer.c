#include "hdl_iface.h"

typedef struct {
  coroutine_t work;
  uint8_t busy;
  uint32_t duration;
  uint32_t timer;
} hdl_buzzer_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_buzzer_var_t, *((hdl_buzzer_t *)0)->obj_var, HDL_BUZZER_VAR_SIZE, "hdl_buzzer.h");

static uint8_t _buzzer_work(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_buzzer_t *buzz = (hdl_buzzer_t *)arg;
  hdl_buzzer_var_t *buzz_var = (hdl_buzzer_var_t *)buzz->obj_var;
  if(buzz_var->busy) {
    hdl_time_counter_t *tc = (hdl_time_counter_t *)buzz->dependencies[2];
    uint32_t now = hdl_time_counter_get(tc);
    if(CL_TIME_ELAPSED(buzz_var->timer, buzz_var->duration, now)) {
      hdl_pwm_t *pwm = (hdl_pwm_t *)buzz->dependencies[0];
      hdl_pwm_set(pwm, 0);
      hdl_gpio_pin_t *pin = (hdl_gpio_pin_t *)buzz->dependencies[1];
      hdl_gpio_set_inactive(pin);
      buzz_var->busy = HDL_FALSE;
    }
  }
  return cancel;
}

static hdl_module_state_t _hdl_buzzer(const void *desc, uint8_t enable) {
  hdl_buzzer_t *buzz = (hdl_buzzer_t *)desc;
  hdl_buzzer_var_t *buzz_var = (hdl_buzzer_var_t *)buzz->obj_var;
  if(enable) {
    buzz_var->busy = HDL_FALSE;
    coroutine_add(&buzz_var->work, &_buzzer_work, (void *)desc);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&buzz_var->work);
  return HDL_MODULE_UNLOADED;
}

uint8_t _hdl_buzzer_set(const void *desc, uint32_t duration, uint8_t volume) {
  hdl_buzzer_t *buzz = (hdl_buzzer_t *)desc;
  hdl_buzzer_var_t *buzz_var = (hdl_buzzer_var_t *)buzz->obj_var;
  if(buzz_var->busy) return HDL_FALSE;
  buzz_var->duration = duration;
  hdl_time_counter_t *tc = (hdl_time_counter_t *)buzz->dependencies[2];
  buzz_var->timer = hdl_time_counter_get(tc);
  hdl_pwm_t *pwm = (hdl_pwm_t *)buzz->dependencies[0];
  uint32_t buzz_p = hdl_pwm_period(pwm);
  hdl_pwm_set(pwm, (((buzz_p + 1) * (volume + 1)) >> 9));
  hdl_gpio_pin_t *pin = (hdl_gpio_pin_t *)buzz->dependencies[1];
  hdl_gpio_set_active(pin);
  buzz_var->busy = HDL_TRUE;
  return HDL_TRUE;
}

const hdl_buzzer_iface_t hdl_buzzer_iface = {
    .init = &_hdl_buzzer,
    .set = &_hdl_buzzer_set
};
