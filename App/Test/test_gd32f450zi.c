#include "app.h"
#include "CodeLib.h"

#if defined( GD32F450 )

  #define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

 void SysTick_Event(uint32_t event, void *sender, void *context){
  __NOP();
 }

  void Exti_Event(uint32_t event, void *sender, void *context){
  __NOP();
 }


  hdl_core_t mod_sys_core = {
    .module.init = &hdl_core,
    .module.dependencies = NULL,
    .module.reg = (void *)SCB_BASE,
    .flash_latency = WS_WSCNT_2 /* WS_WSCNT_0: sys_clock <= 24MHz, WS_WSCNT_1: sys_clock <= 48MHz, WS_WSCNT_2: sys_clock <= 72MHz */
    /* TODO: ... */
  };

  hdl_nvic_interrupt_t mod_irq_systick = {
    .irq_type = HDL_NVIC_EXCEPTION_SysTick,
    .priority = 0,
    .priority_group = 0,
  };

  hdl_nvic_interrupt_t mod_irq_exti_10_15 = {
    .irq_type = HDL_NVIC_IRQ40_EXTI10_15,
    .priority = 0,
    .priority_group = 1,
  };

/***********************************************************
 * EXTI TEST
************************************************************/
  hdl_nvic_exti_t mod_nvic_exti_line_0 = {
    .line = HDL_EXTI_LINE_0,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

  hdl_nvic_exti_t mod_nvic_exti_line_1 = {
    .line = HDL_EXTI_LINE_1,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

  hdl_nvic_exti_t mod_nvic_exti_line_2 = {
    .line = HDL_EXTI_LINE_2,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

  hdl_nvic_exti_t mod_nvic_exti_line_3 = {
    .line = HDL_EXTI_LINE_3,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

  hdl_nvic_exti_t mod_nvic_exti_line_4 = {
    .line = HDL_EXTI_LINE_4,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

  hdl_nvic_exti_t mod_nvic_exti_line_5 = {
    .line = HDL_EXTI_LINE_5,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

  hdl_nvic_exti_t mod_nvic_exti_line_6 = {
    .line = HDL_EXTI_LINE_6,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

    hdl_nvic_exti_t mod_nvic_exti_line_7 = {
    .line = HDL_EXTI_LINE_7,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

  hdl_nvic_exti_t mod_nvic_exti_line_8 = {
    .line = HDL_EXTI_LINE_8,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

  hdl_nvic_exti_t mod_nvic_exti_line_9 = {
    .line = HDL_EXTI_LINE_9,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

  hdl_nvic_exti_t mod_nvic_exti_line_10 = {
    .line = HDL_EXTI_LINE_10,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

  hdl_nvic_exti_t mod_nvic_exti_line_11 = {
    .line = HDL_EXTI_LINE_11,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

  hdl_nvic_exti_t mod_nvic_exti_line_12 = {
    .line = HDL_EXTI_LINE_12,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

  hdl_nvic_exti_t mod_nvic_exti_line_13 = {
    .line = HDL_EXTI_LINE_13,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

    hdl_nvic_exti_t mod_nvic_exti_line_14 = {
    .line = HDL_EXTI_LINE_14,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

  hdl_nvic_exti_t mod_nvic_exti_line_15 = {
    .line = HDL_EXTI_LINE_15,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

  hdl_nvic_t mod_nvic = {
    .module.init = &hdl_nvic,
    .module.dependencies = hdl_module_dependencies(&mod_sys_core.module),
    .module.reg = NVIC,
    .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
    .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_exti_10_15),
    .exti_lines = hdl_exti_lines(&mod_nvic_exti_line_0, &mod_nvic_exti_line_1, &mod_nvic_exti_line_2,
    &mod_nvic_exti_line_3, &mod_nvic_exti_line_4, &mod_nvic_exti_line_5, &mod_nvic_exti_line_6, &mod_nvic_exti_line_7,
    &mod_nvic_exti_line_8, &mod_nvic_exti_line_9, &mod_nvic_exti_line_10,&mod_nvic_exti_line_11, &mod_nvic_exti_line_12,
    &mod_nvic_exti_line_13, &mod_nvic_exti_line_14, &mod_nvic_exti_line_15)
  };

hdl_gpio_port_t hdl_gpio_port_e = {
  .init = &hdl_gpio_port,
  .dependencies = NULL,
  .reg = (void *)GPIOE,
};

hdl_gpio_mode_t gpio_input_np = {
    .type = GPIO_MODE_INPUT,
    .pull = GPIO_PUPD_NONE,
};

hdl_gpio_pin_t pin_pe13 = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_e),
    .module.reg = (void *)GPIO_PIN_13,
    .mode = &gpio_input_np
};



//  void test() {
//           rcu_periph_clock_enable(RCU_GPIOE);
//     rcu_periph_clock_enable(RCU_SYSCFG);

//     /* configure button pin as input */
//     gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_14);


//         /* enable and set key EXTI interrupt to the lowest priority */
//         nvic_irq_enable(EXTI10_15_IRQn, 2U, 0U);

//         /* connect key EXTI line to key GPIO pin */
//         syscfg_exti_line_config(EXTI_SOURCE_GPIOE, EXTI_SOURCE_PIN14);

//         /* configure key EXTI line */
//         exti_init(EXTI_14, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
//         exti_interrupt_flag_clear(EXTI_14);
    

//     while(1) {
//     }
//   }

void test() {
    //SystemInit();

    hdl_enable(&mod_sys_core.module);
    hdl_enable(&mod_nvic.module);
    hdl_enable(&pin_pe13.module);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  //SysTick_Config(200000);

  //hdl_interrupt_request(&mod_nvic, HDL_NVIC_EXCEPTION_SysTick, &SysTick_Event, NULL);
  hdl_interrupt_request(&mod_nvic, HDL_NVIC_IRQ40_EXTI10_15, &Exti_Event, NULL);
  //hdl_exti_sw_trigger(&mod_nvic, HDL_NVIC_IRQ40_EXTI10_15);
  while (1)
  {
    if(hdl_gpio_read(&pin_pe13) == HDL_GPIO_LOW)
       __NOP();

    cooperative_scheduler(false);
  }
  

    // rcu_periph_clock_enable(RCU_GPIOF);
    // gpio_af_set(GPIOF, GPIO_AF_0, GPIO_PIN_13);
    // gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_13);
    // gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_13);
    // gpio_bit_write(GPIOF, GPIO_PIN_13, 0);
    // gpio_bit_write(GPIOF, GPIO_PIN_13, 1);
    while (1)
    {
      __NOP();
    }
    // rcu_periph_clock_enable(RCU_GPIOC);
    //  gpio_mode_set(RCU_GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO_PIN_3);
    //  gpio_output_options_set(RCU_GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ,GPIO_PIN_3);
    //  gpio_mode_set(RCU_GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO_PIN_6);
    //  gpio_output_options_set(RCU_GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ,GPIO_PIN_6);
     
    //  gpio_bit_set(RCU_GPIOC, GPIO_PIN_3);
    //  gpio_bit_set(RCU_GPIOC, GPIO_PIN_6);
}
#endif