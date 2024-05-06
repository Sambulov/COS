#include "app.h"
#include "CodeLib.h"

#if defined( GD32F450 )

extern hdl_core_t mod_sys_core;
extern hdl_nvic_t mod_nvic;

extern hdl_timer_t mod_systick_timer_ms;
extern hdl_timer_t mod_timer0_ms;
extern hdl_timer_t mod_timer1_ms;

extern hdl_clock_prescaler_t mod_clock_ahb;
extern hdl_clock_prescaler_t mod_clock_apb1;
extern hdl_clock_prescaler_t mod_clock_apb2;

extern hdl_gpio_pin_t mod_led_0_0;
extern hdl_gpio_pin_t mod_led_0_1;
extern hdl_gpio_pin_t mod_led_0_2;
extern hdl_gpio_pin_t mod_led_1_0;
extern hdl_gpio_pin_t mod_led_1_1;
extern hdl_gpio_pin_t mod_led_1_2;
extern hdl_gpio_pin_t mod_led_2_0;
extern hdl_gpio_pin_t mod_led_2_1;
extern hdl_gpio_pin_t mod_led_2_2;

extern hdl_gpio_pin_t mod_adc_in0;
extern hdl_gpio_pin_t mod_adc_in1;
extern hdl_gpio_pin_t mod_adc_in2;
extern hdl_gpio_pin_t mod_adc_in3;
extern hdl_gpio_pin_t mod_adc_in4;
extern hdl_gpio_pin_t mod_adc_in5;

extern hdl_dma_t mod_dma;
extern hdl_dma_channel_t mod_adc_dma_ch;
extern hdl_adc_source_t mod_adc_source_0;
extern hdl_adc_source_t mod_adc_source_1;
extern hdl_adc_source_t mod_adc_source_2;
extern hdl_adc_source_t mod_adc_source_3;
extern hdl_adc_source_t mod_adc_source_4;
extern hdl_adc_source_t mod_adc_source_5;
extern hdl_adc_t mod_adc;

hdl_module_t my_module = {
  .init = NULL,
  .dependencies = hdl_module_dependencies(&mod_sys_core.module, &mod_nvic.module, &mod_systick_timer_ms.module,
  &mod_timer0_ms.module, &mod_timer1_ms.module, &mod_clock_ahb.module, &mod_clock_apb1.module, 
  &mod_clock_apb2.module, &mod_led_0_0.module, &mod_led_0_1.module, &mod_led_0_2.module, &mod_led_1_0.module, 
  &mod_led_1_1.module, &mod_led_1_2.module, &mod_led_2_0.module, &mod_led_2_1.module, &mod_led_2_2.module,
  &mod_adc_in0.module, &mod_adc_in1.module, &mod_adc_in2.module, &mod_adc_in3.module, &mod_adc_in4.module,
   &mod_adc_in5.module, &mod_dma.module, &mod_adc.module),
  .reg = NULL
};


void test() {
  hdl_enable(&my_module);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  while (1)
  {
    static uint32_t time_stamp_ms = 0;
    static uint32_t adc_raw[6];

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

      adc_raw[0] = hdl_adc_get_data(&mod_adc, &mod_adc_source_0);
      adc_raw[1] = hdl_adc_get_data(&mod_adc, &mod_adc_source_1);
      adc_raw[2] = hdl_adc_get_data(&mod_adc, &mod_adc_source_2);
      adc_raw[3] = hdl_adc_get_data(&mod_adc, &mod_adc_source_3);
      adc_raw[4] = hdl_adc_get_data(&mod_adc, &mod_adc_source_4);
      adc_raw[5] = hdl_adc_get_data(&mod_adc, &mod_adc_source_5);
    }
  }
}


#endif