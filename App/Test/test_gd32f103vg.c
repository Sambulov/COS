#include "app.h"
#include "CodeLib.h"

#if defined ( GD32F103VG )

extern hdl_nvic_t mod_nvic;
extern hdl_timer_t mod_timer_ms;
extern hdl_gpio_pin_t mod_gpio_a15_led1;
extern hdl_gpio_pin_t mod_gpio_c10_led2;
extern hdl_gpio_pin_t mod_gpio_c11_led3;
extern hdl_gpio_pin_t mod_gpio_c12_led4;
extern hdl_gpio_pin_t mod_gpio_d0_led5;
extern hdl_gpio_pin_t mod_gpio_d1_led6;
extern hdl_gpio_pin_t mod_gpio_d2_led7;
extern hdl_gpio_pin_t mod_gpio_d3_led8;
extern hdl_gpio_pin_t mod_gpio_d4_led9;
extern hdl_gpio_pin_t mod_gpio_d5_led10;
extern hdl_gpio_pin_t mod_gpio_d6_led11;
extern hdl_gpio_pin_t mod_gpio_d7_led12;
extern hdl_clock_counter_t mod_timer0_counter;

hdl_module_t my_module = {
  .init = NULL,
  .dependencies = hdl_module_dependencies(&mod_nvic.module, &mod_timer_ms.module, &mod_gpio_a15_led1.module,
                                          &mod_gpio_c10_led2.module, &mod_gpio_c11_led3.module,
                                          &mod_gpio_c12_led4.module, &mod_gpio_d0_led5.module,
                                          &mod_gpio_d1_led6.module, &mod_gpio_d2_led7.module,
                                          &mod_gpio_d3_led8.module, &mod_gpio_d4_led9.module,
                                          &mod_gpio_d5_led10.module, &mod_gpio_d6_led11.module,
                                          &mod_gpio_d7_led12.module),
  .reg = NULL
};

void test() {
  hdl_enable(&my_module);
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  uint32_t i = 0;
  uint32_t timer = hdl_timer_get(&mod_timer_ms);
  uint32_t led_switch_mask = -1;

  hdl_gpio_write(&mod_gpio_a15_led1, HDL_GPIO_HIGH);
  hdl_gpio_write(&mod_gpio_c10_led2, HDL_GPIO_HIGH);
  hdl_gpio_write(&mod_gpio_c11_led3, HDL_GPIO_HIGH);
  hdl_gpio_write(&mod_gpio_c12_led4, HDL_GPIO_HIGH);
  hdl_gpio_write(&mod_gpio_d0_led5, HDL_GPIO_HIGH);
  hdl_gpio_write(&mod_gpio_d1_led6, HDL_GPIO_HIGH);
  hdl_gpio_write(&mod_gpio_d2_led7, HDL_GPIO_HIGH);
  hdl_gpio_write(&mod_gpio_d3_led8, HDL_GPIO_HIGH);
  hdl_gpio_write(&mod_gpio_d4_led9, HDL_GPIO_HIGH);
  hdl_gpio_write(&mod_gpio_d5_led10, HDL_GPIO_HIGH);
  hdl_gpio_write(&mod_gpio_d6_led11, HDL_GPIO_HIGH);
  hdl_gpio_write(&mod_gpio_d7_led12, HDL_GPIO_HIGH);

  while (1) {
    volatile uint32_t x = hdl_clock_counter_get_count(&mod_timer0_counter);
    cooperative_scheduler(false);
    if(((timer + 50) == hdl_timer_get(&mod_timer_ms)) && (led_switch_mask & 1)) {
      hdl_gpio_toggle(&mod_gpio_a15_led1);
      hdl_gpio_toggle(&mod_gpio_c10_led2);
      led_switch_mask &= ~1;
    }
    if(((timer + 100) == hdl_timer_get(&mod_timer_ms)) && (led_switch_mask & 2)) {
      hdl_gpio_toggle(&mod_gpio_c11_led3);
      led_switch_mask &= ~2;
    }
    if(((timer + 150) == hdl_timer_get(&mod_timer_ms)) && (led_switch_mask & 4)) {
      hdl_gpio_toggle(&mod_gpio_c12_led4);
      led_switch_mask &= ~4;
    }
    if(((timer + 200) == hdl_timer_get(&mod_timer_ms)) && (led_switch_mask & 8)) {
      hdl_gpio_toggle(&mod_gpio_d0_led5);
      led_switch_mask &= ~8;
    }
    if(((timer + 250) == hdl_timer_get(&mod_timer_ms)) && (led_switch_mask & 16)) {
      hdl_gpio_toggle(&mod_gpio_d1_led6);
      led_switch_mask &= ~16;
    }
    if(((timer + 300) == hdl_timer_get(&mod_timer_ms)) && (led_switch_mask & 32)) {
      hdl_gpio_toggle(&mod_gpio_d2_led7);
      led_switch_mask &= ~32;
    }
    if(((timer + 350) == hdl_timer_get(&mod_timer_ms)) && (led_switch_mask & 64)) {
      hdl_gpio_toggle(&mod_gpio_d3_led8);
      led_switch_mask &= ~64;
    }
    if(((timer + 400) == hdl_timer_get(&mod_timer_ms)) && (led_switch_mask & 128)) {
      hdl_gpio_toggle(&mod_gpio_d4_led9);
      led_switch_mask &= ~128;
    }
    if(((timer + 450) == hdl_timer_get(&mod_timer_ms)) && (led_switch_mask & 256)) {
      hdl_gpio_toggle(&mod_gpio_d5_led10);
      led_switch_mask &= ~256;
    }
    if(((timer + 500) == hdl_timer_get(&mod_timer_ms)) && (led_switch_mask & 512)) {
      hdl_gpio_toggle(&mod_gpio_d6_led11);
      hdl_gpio_toggle(&mod_gpio_d7_led12);
      timer += 500;
      led_switch_mask = -1;
    }
  }
}

#endif