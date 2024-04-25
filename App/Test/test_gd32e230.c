#include "app.h"
#include "CodeLib.h"

#if defined ( GD32E23X )

#define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

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



  hdl_nvic_interrupt_t _mod_irq_exti_0_1 = {
    .irq_type = HDL_NVIC_IRQ5_EXTI0_1,
    .priority = 0,
    .priority_group = 1,
  };

  hdl_nvic_exti_t _mod_nvic_exti_line_0 = {
    .line = HDL_EXTI_LINE_0,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PA,
    .trigger = HDL_EXTI_TRIGGER_FALLING
  };

  hdl_nvic_t mod_nvic1 = {
    .module.init = &hdl_nvic,
    //.module.dependencies = hdl_module_dependencies(&mod_sys_core.module),
    .module.reg = NVIC,
    .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
    .irq_latency = 0, /* TODO: define static assert */
    .interrupts = hdl_interrupts(&_mod_irq_exti_0_1),
    .exti_lines = hdl_exti_lines(&_mod_nvic_exti_line_0)
  };

hdl_gpio_port_t hdl_gpio_port_a1 = {
  .init = &hdl_gpio_port,
  .dependencies = NULL,
  .reg = (void *)GPIOA,
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

void test() {
    //SystemInit();

    //hdl_enable(&mod_sys_core.module);
    hdl_enable(&mod_nvic1.module);
    hdl_enable(&pin_pa0.module);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  
  hdl_interrupt_request(&mod_nvic1, HDL_NVIC_IRQ5_EXTI0_1, &Exti_Event, NULL);
  while (1)
  {
    // if(hdl_gpio_read(&pin_pe13) == HDL_GPIO_LOW)
    //   __NOP();

    cooperative_scheduler(false);
  }
}

//uint16_t adc_raw[2];
// void test() {
//   static uint32_t time_stamp_ms = 0;

//   hdl_enable(&mod_gpio_adc_channel_3v3.module);
//   hdl_enable(&mod_gpio_adc_channel_1v5.module);
//   hdl_enable(&mod_adc.module);
//   hdl_enable(&mod_timer_ms.module);
  
//   // while (hdl_state(&mod_adc.module) != HDL_MODULE_INIT_OK ||
//   //         hdl_state(&mod_gpio_adc_channel_3v3.module) != HDL_MODULE_INIT_OK ||
//   //           hdl_state(&mod_gpio_adc_channel_1v5.module) != HDL_MODULE_INIT_OK)
//   // {
//   //   cooperative_scheduler(false);
//   // }
  
//   while (!hdl_init_complete()) {
//     cooperative_scheduler(false);
//   }

  

//   while (1) {
//     cooperative_scheduler(false);

//     /* This code will be launched ony one time*/
//     if(hdl_adc_status(&mod_adc) == HDL_ADC_STATUS_WAITING_START_TRIGGER)
//       hdl_adc_start(&mod_adc, adc_raw);

//     /* There data is ready */
//     if(hdl_adc_status(&mod_adc) == HDL_ADC_STATUS_DATA_READY)
//       hdl_adc_start(&mod_adc, adc_raw);

//     /* This code checked and launched adc */
//     if(hdl_adc_status(&mod_adc) == HDL_ADC_STATUS_DATA_READY || hdl_adc_status(&mod_adc) == HDL_ADC_STATUS_WAITING_START_TRIGGER)
//     {
//       hdl_adc_start(&mod_adc, adc_raw);
//       /* There data is ready */
//     }
      
//     /* Also you can use only this function */
//     if(hdl_adc_start(&mod_adc, adc_raw) == HDL_ADC_STATUS_DATA_READY)
//     {
//       /* There data is ready */
//     }

    
//     if (TIME_ELAPSED(time_stamp_ms, 1000, hdl_timer_get(&mod_timer_ms)))
//     {
//         __NOP();
//     }
//   }

// }

#endif
