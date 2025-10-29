#ifndef PORT_PWM_H_
#define PORT_PWM_H_

#include "hdl_pwm.h"

#define HDL_PWM_MCU_VAR_SIZE 0

typedef struct {
  uint32_t mode;         /* TIM_OCMODE_TIMING, TIM_OCMODE_ACTIVE, TIM_OCMODE_INACTIVE, TIM_OCMODE_TOGGLE, TIM_OCMODE_PWM1, TIM_OCMODE_PWM2, TIM_OCMODE_FORCED_ACTIVE, TIM_OCMODE_FORCED_INACTIVE */
  uint32_t pulse;        /* Min_Data = 0x0000U and Max_Data = 0xFFFFU */
  uint32_t fast_mode;    /* TIM_OCFAST_DISABLE, TIM_OCFAST_ENABLE only in PWM1 and PWM2 mode. */
  uint32_t polarity;     /* TIM_OCPOLARITY_HIGH, TIM_OCPOLARITY_LOW */
  uint32_t polarity_n;   /* TIM_OCNPOLARITY_HIGH, TIM_OCNPOLARITY_LOW only for TIM1 and TIM8. */
  uint32_t idle_state;   /* TIM_OCIDLESTATE_SET, TIM_OCIDLESTATE_RESET only for TIM1 and TIM8. */
  uint32_t idle_state_n; /* TIM_OCNIDLESTATE_SET, TIM_OCNIDLESTATE_RESET only for TIM1 and TIM8. */
  uint32_t channel;      /* TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3, TIM_CHANNEL_4 */
} hdl_pwm_mcu_config_t;

/* depends on:
   hdl_tick_counter
   hdl_gpio
 */
hdl_module_new_t(hdl_pwm_mcu_t, HDL_PWM_MCU_VAR_SIZE, hdl_pwm_mcu_config_t*, hdl_pwm_iface_t);

extern const hdl_pwm_iface_t hdl_pwm_mcu_iface;

#endif /* PORT_PWM_H_ */ 
