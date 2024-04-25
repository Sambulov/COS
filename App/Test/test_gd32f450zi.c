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

  hdl_nvic_exti_t mod_nvic_exti_line_13 = {
    .line = HDL_EXTI_LINE_13,
    .mode = HDL_EXTI_MODE_INTERRUPT,
    .source = HDL_EXTI_SOURCE_PE,
    .trigger = HDL_EXTI_TRIGGER_RISING
  };

  hdl_nvic_t mod_nvic = {
    .module.init = &hdl_nvic,
    //.module.dependencies = hdl_module_dependencies(&mod_sys_core.module),
    .module.reg = NVIC,
    .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
    .irq_latency = 0, /* TODO: define static assert */
    .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_exti_10_15),
    .exti_lines = hdl_exti_lines(&mod_nvic_exti_line_13)
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