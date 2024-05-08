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

extern hdl_gpio_pin_t mod_do_led_0_0;
extern hdl_gpio_pin_t mod_do_led_0_1;
extern hdl_gpio_pin_t mod_do_led_0_2;
extern hdl_gpio_pin_t mod_do_led_1_0;
extern hdl_gpio_pin_t mod_do_led_1_1;
extern hdl_gpio_pin_t mod_do_led_1_2;
extern hdl_gpio_pin_t mod_do_led_2_0;
extern hdl_gpio_pin_t mod_do_led_2_1;
extern hdl_gpio_pin_t mod_do_led_2_2;

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

extern bldl_atb3500_led_port_t mod_atb3500_led_0_0;
extern bldl_atb3500_led_port_t mod_atb3500_led_0_1;
extern bldl_atb3500_led_port_t mod_atb3500_led_0_2;
extern bldl_atb3500_led_port_t mod_atb3500_led_1_0;
extern bldl_atb3500_led_port_t mod_atb3500_led_1_1;
extern bldl_atb3500_led_port_t mod_atb3500_led_1_2;
extern bldl_atb3500_led_port_t mod_atb3500_led_2_0;
extern bldl_atb3500_led_port_t mod_atb3500_led_2_1;
extern bldl_atb3500_led_port_t mod_atb3500_led_2_2;

extern bldl_atb3500_ain_port_t mod_atb3500_ai_ch0;
extern bldl_atb3500_ain_port_t mod_atb3500_ai_ch1;
extern bldl_atb3500_ain_port_t mod_atb3500_ai_ch2;
extern bldl_atb3500_ain_port_t mod_atb3500_ai_ch3;
extern bldl_atb3500_ain_port_t mod_atb3500_ai_ch4;
extern bldl_atb3500_ain_port_t mod_atb3500_ai_ch5;

extern bldl_atb3500_port_expander_t mod_atb3500_port_expander;


hdl_module_t my_module = {
  .init = NULL,
  .dependencies = hdl_module_dependencies(&mod_sys_core.module, &mod_nvic.module, &mod_systick_timer_ms.module,
  &mod_timer0_ms.module, &mod_timer1_ms.module, &mod_clock_ahb.module, &mod_clock_apb1.module, 
  &mod_clock_apb2.module, &mod_do_led_0_0.module, &mod_do_led_0_1.module, &mod_do_led_0_2.module, &mod_do_led_1_0.module, 
  &mod_do_led_1_1.module, &mod_do_led_1_2.module, &mod_do_led_2_0.module, &mod_do_led_2_1.module, &mod_do_led_2_2.module,
  &mod_adc_in0.module, &mod_adc_in1.module, &mod_adc_in2.module, &mod_adc_in3.module, &mod_adc_in4.module,
   &mod_adc_in5.module, &mod_dma.module, &mod_adc.module),
  .reg = NULL
};

static uint32_t time_stamp_ms = 0;
static uint32_t adc_raw[6];

void test() {
  hdl_enable(&my_module);
  hdl_enable(&mod_atb3500_port_expander);
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  while (1)
  {
    cooperative_scheduler(false);
    if(TIME_ELAPSED(time_stamp_ms, 1000, hdl_timer_get(&mod_systick_timer_ms))){
      time_stamp_ms += 1000;

      mod_atb3500_led_0_0.output ^=1;
      mod_atb3500_led_0_1.output ^=1;
      mod_atb3500_led_0_2.output ^=1;

      mod_atb3500_led_1_0.output ^=1;
      mod_atb3500_led_1_1.output ^=1;
      mod_atb3500_led_1_2.output ^=1;

      mod_atb3500_led_2_0.output ^=1;
      mod_atb3500_led_2_1.output ^=1;
      mod_atb3500_led_2_2.output ^=1;


      adc_raw[0] = mod_atb3500_ai_ch0.input;
      adc_raw[1] = mod_atb3500_ai_ch1.input;
      adc_raw[2] = mod_atb3500_ai_ch2.input;
      adc_raw[3] = mod_atb3500_ai_ch3.input;
      adc_raw[4] = mod_atb3500_ai_ch4.input;
      adc_raw[5] = mod_atb3500_ai_ch5.input;
    }
  }
}


#endif