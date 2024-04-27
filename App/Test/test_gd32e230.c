#include "app.h"
#include "CodeLib.h"


#define TEST_CLOCK


#if defined ( GD32E23X )

#ifdef TEST_CLOCK

extern hdl_gpio_pin_t mod_gpo_carrier_pwr_on;
extern hdl_timer_t mod_timer0_ms;
extern hdl_timer_t mod_timer2_ms;

void test() {
  static uint32_t time_stamp_sys_ms = 0;
  static uint32_t time_stamp_timer0_ms = 0;
  static uint32_t time_stamp_timer2_ms = 0;

  hdl_enable(&mod_timer_ms.module);
  hdl_enable(&mod_gpo_carrier_pwr_on.module);
  hdl_enable(&mod_timer0_ms.module);
  hdl_enable(&mod_timer2_ms.module);
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  while (1)
  {
    
    // if (TIME_ELAPSED(time_stamp_sys_ms, 1, hdl_timer_get(&mod_timer_ms))){
    //   time_stamp_sys_ms = hdl_timer_get(&mod_timer_ms);
    //   hdl_gpio_toggle(&mod_gpo_carrier_pwr_on);
    // }

    if (TIME_ELAPSED(time_stamp_timer0_ms, 1, hdl_timer_get(&mod_timer0_ms))){
      time_stamp_timer0_ms = hdl_timer_get(&mod_timer0_ms);
      hdl_gpio_toggle(&mod_gpo_carrier_pwr_on);
    }

    // if (TIME_ELAPSED(time_stamp_timer2_ms, 1, hdl_timer_get(&mod_timer2_ms))){
    //   time_stamp_timer2_ms = hdl_timer_get(&mod_timer2_ms);
    //   hdl_gpio_toggle(&mod_gpo_carrier_pwr_on);
    // }
  }
}

#else

#define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

extern hdl_nvic_t mod_nvic;
extern hdl_core_t mod_sys_core;
extern hdl_adc_t mod_adc;
extern hdl_gpio_pin_t mod_gpio_adc_channel_3v3;
extern hdl_gpio_pin_t mod_gpio_adc_channel_1v5;
extern hdl_timer_t mod_timer_ms;
extern hdl_timer_t mod_timer_ms;


 void SysTick_Event(uint32_t event, void *sender, void *context){
  __NOP();
 }

  void Exti_Event(uint32_t event, void *sender, void *context){
  __NOP();
 }
   void Exti_8_Event(uint32_t event, void *sender, void *context){
  __NOP();
 }


hdl_gpio_port_t hdl_gpio_port_a1 = {
  .init = &hdl_gpio_port,
  .dependencies = NULL,
  .reg = (void *)GPIOA,
};

hdl_gpio_port_t hdl_gpio_port_b1 = {
  .init = &hdl_gpio_port,
  .dependencies = NULL,
  .reg = (void *)GPIOB,
};


hdl_gpio_mode_t gpio_input_np = {
    .type = GPIO_MODE_INPUT,
    .pull = GPIO_PUPD_NONE,
};

hdl_gpio_pin_t pin_pa0 = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a1),
    .module.reg = (void *)GPIO_PIN_0,
    .mode = &gpio_input_np
};

hdl_gpio_pin_t pin_pb8 = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b1),
    .module.reg = (void *)GPIO_PIN_8,
    .mode = &gpio_input_np
};


uint16_t adc_raw[2];
void test() {
  static uint32_t time_stamp_ms = 0;
  hdl_enable(&mod_adc.module);
  hdl_enable(&mod_gpio_adc_channel_3v3.module);
  hdl_enable(&mod_gpio_adc_channel_1v5.module);
  hdl_enable(&mod_timer_ms.module);
  hdl_enable(&mod_nvic.module);
  hdl_enable(&pin_pa0.module);
  hdl_enable(&pin_pb8.module);
  
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  hdl_interrupt_request(&mod_nvic, HDL_NVIC_IRQ5_EXTI0_1, &Exti_Event, NULL);
  hdl_interrupt_request(&mod_nvic, HDL_NVIC_IRQ7_EXTI4_15, &Exti_8_Event, NULL);

  while (1) {
    cooperative_scheduler(false);

    /* This code will be launched ony one time*/
    if(hdl_adc_status(&mod_adc) == HDL_ADC_STATUS_WAITING_START_TRIGGER)
      hdl_adc_start(&mod_adc);

    /* There data is ready */
    if(hdl_adc_status(&mod_adc) == HDL_ADC_STATUS_DATA_READY)
      hdl_adc_start(&mod_adc);

    /* This code checked and launched adc */
    if(hdl_adc_status(&mod_adc) == HDL_ADC_STATUS_DATA_READY || hdl_adc_status(&mod_adc) == HDL_ADC_STATUS_WAITING_START_TRIGGER)
    {
      hdl_adc_start(&mod_adc);
      /* There data is ready */
    }
      
    /* Also you can use only this function */
    if(hdl_adc_start(&mod_adc) == HDL_ADC_STATUS_DATA_READY)
    {
      /* There data is ready */
    }
    hdl_adc_get_data(&mod_adc, HDL_ADC_CHANNEL_3, &adc_raw[0]);
    hdl_adc_get_data(&mod_adc, HDL_ADC_CHANNEL_7, &adc_raw[1]);
    
    if (TIME_ELAPSED(time_stamp_ms, 1000, hdl_timer_get(&mod_timer_ms)))
    {
        __NOP();
    }
  }

}
#endif

#endif
