#include "app.h"
#include "CodeLib.h"

#if defined ( GD32F103VG )

#include "mig_uspd.h"

extern hdl_nvic_t mod_nvic;
extern hdl_timer_t mod_timer_ms;
extern hdl_clock_counter_t mod_timer0_counter;
extern hdl_dma_channel_t mod_m2m_dma_ch;

extern hdl_adc_source_t mod_adc_source_0;
extern hdl_adc_source_t mod_adc_source_1;
extern hdl_adc_source_t mod_adc_source_2;
extern hdl_adc_source_t mod_adc_source_3;
extern hdl_adc_t mod_adc; 

hdl_module_t my_module = {
  .init = NULL,
  .dependencies = hdl_module_dependencies(&mod_nvic.module, &mod_timer_ms.module, &mod_gpio_a15_led1.module,
                                          &mod_gpio_c10_led2.module, &mod_gpio_c11_led3.module,
                                          &mod_gpio_c12_led4.module, &mod_gpio_d0_led5.module,
                                          &mod_gpio_d1_led6.module, &mod_gpio_d2_led7.module,
                                          &mod_gpio_d3_led8.module, &mod_gpio_d4_led9.module,
                                          &mod_gpio_d5_led10.module, &mod_gpio_d6_led11.module,
                                          &mod_gpio_d7_led12.module, &mod_uspd_od_port1.module,
                                          &mod_uspd_od_port2.module, &mod_uspd_od_port3.module,
                                          &mod_uspd_od_port4.module, &mod_uspd_od_port5.module,
                                          &mod_uspd_od_port6.module, &mod_uspd_od_port7.module,
                                          &mod_uspd_od_port8.module, &mod_uspd_ai_port0.module,
                                          &mod_uspd_ai_port1.module, &mod_uspd_ai_port2.module,
                                          &mod_uspd_ai_port3.module),
  .reg = NULL
};



uint32_t arr[10]={1,2,3,4,5,0,0,0,0,0};

void test() {
  hdl_enable(&my_module);
  hdl_enable(&mod_m2m_dma_ch.module);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  hdl_dma_run(&mod_m2m_dma_ch, (uint32_t)&arr[0], (uint32_t)&arr[5], 5);

  uint32_t i = 0;
  uint32_t timer = hdl_timer_get(&mod_timer_ms);
  uint32_t led_switch_mask = -1;

  while (1) {
    for(int i = 0; i < 4; i++) {
      arr[i] = mod_adc.values[i];
    }
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