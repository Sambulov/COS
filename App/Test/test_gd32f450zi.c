#include "app.h"
#include "CodeLib.h"

#if defined( GD32F450 )

extern hdl_core_t mod_sys_core;
extern hdl_nvic_t mod_nvic;
extern hdl_timer_t mod_systick_timer_ms;

extern hdl_timer_t mod_systick_timer_ms;
extern hdl_timer_t mod_timer0_ms;
extern hdl_timer_t mod_timer1_ms;

extern hdl_clock_prescaler_t mod_clock_ahb;

extern hdl_gpio_pin_t mod_led_0_0;
extern hdl_gpio_pin_t mod_led_0_1;
extern hdl_gpio_pin_t mod_led_0_2;
extern hdl_gpio_pin_t mod_led_1_0;
extern hdl_gpio_pin_t mod_led_1_1;
extern hdl_gpio_pin_t mod_led_1_2;
extern hdl_gpio_pin_t mod_led_2_0;
extern hdl_gpio_pin_t mod_led_2_1;
extern hdl_gpio_pin_t mod_led_2_2;



void test() {
  //SystemInit();

  hdl_enable(&mod_sys_core.module);
  hdl_enable(&mod_nvic.module);
  
  hdl_enable(&mod_clock_ahb.module);
  hdl_enable(&mod_systick_timer_ms.module);
  hdl_enable(&mod_timer0_ms.module);
  hdl_enable(&mod_timer1_ms.module);
  
  hdl_enable(&mod_led_0_0.module);
  hdl_enable(&mod_led_0_1.module);
  hdl_enable(&mod_led_0_2.module);
  hdl_enable(&mod_led_1_0.module);
  hdl_enable(&mod_led_1_1.module);
  hdl_enable(&mod_led_1_2.module);
  hdl_enable(&mod_led_2_0.module);
  hdl_enable(&mod_led_2_1.module);
  hdl_enable(&mod_led_2_2.module);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  while (1)
  {
    static uint32_t time_stamp_ms = 0;
    if(TIME_ELAPSED(time_stamp_ms, 1000, hdl_timer_get(&mod_systick_timer_ms))){
      time_stamp_ms += 1000;
      hdl_gpio_toggle(&mod_led_0_0);
      hdl_gpio_toggle(&mod_led_0_1);
      hdl_gpio_toggle(&mod_led_0_2);
      hdl_gpio_toggle(&mod_led_1_0);
      hdl_gpio_toggle(&mod_led_1_1);
      hdl_gpio_toggle(&mod_led_1_2);
      hdl_gpio_toggle(&mod_led_2_0);
      hdl_gpio_toggle(&mod_led_2_1);
      hdl_gpio_toggle(&mod_led_2_2);
    }
  }
  
}


#endif