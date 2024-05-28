#include "app.h"
#include "CodeLib.h"
//#define TEST_CLOCK


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
    
    if (TIME_ELAPSED(time_stamp_sys_ms, 1000, hdl_timer_get(&mod_timer_ms))){
      time_stamp_sys_ms += 1000;
      hdl_gpio_toggle(&mod_gpo_carrier_pwr_on);
    }

    // if (TIME_ELAPSED(time_stamp_timer0_ms, 1000, hdl_timer_get(&mod_timer0_ms))){
    //   time_stamp_timer0_ms += 1000;
    //   hdl_gpio_toggle(&mod_gpo_carrier_pwr_on);
    // }

    // if (TIME_ELAPSED(time_stamp_timer2_ms, 1000, hdl_timer_get(&mod_timer2_ms))){
    //   time_stamp_timer2_ms += 1000;
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
extern hdl_adc_source_t mod_adc_source_0;
extern hdl_adc_source_t mod_adc_source_1;
extern hdl_clock_prescaler_t mod_clock_apb2;

extern  hdl_dma_channel_t mod_m2m_dma_ch;

 void SysTick_Event(uint32_t event, void *sender, void *context){
  __NOP();
 }

  void Exti_Event(uint32_t event, void *sender, void *context){
  __NOP();
 }
 void Exti_8_Event(uint32_t event, void *sender, void *context) {
   static int8_t cnt = 0;
   static uint32_t a;
   if (event & GPIO_PIN_15) {
    gpio_bit_toggle(GPIOB, GPIO_PIN_1);
     for(uint16_t i = 0; i < 10000; i++);
     if (gpio_input_bit_get(GPIOA, GPIO_PIN_15) == RESET)
       cnt++;
     else
       cnt--;
   }
   a = SysTick->VAL;
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

hdl_gpio_mode_t gpio_spi_mode = {
    .type = GPIO_MODE_AF,
    .pull = GPIO_PUPD_NONE,
    .ospeed = GPIO_OSPEED_50MHZ,
    .af = GPIO_AF_0,
    .otype = GPIO_OTYPE_PP,
};

hdl_gpio_pin_t gpio_pin_spi_mosi = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b1),
    .module.reg = (void *)GPIO_PIN_5,
    .mode = &gpio_spi_mode
};

hdl_gpio_pin_t gpio_pin_spi_miso = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b1),
    .module.reg = (void *)GPIO_PIN_4,
    .mode = &gpio_spi_mode
};

hdl_gpio_pin_t gpio_pin_spi_sck = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b1),
    .module.reg = (void *)GPIO_PIN_3,
    .mode = &gpio_spi_mode
};

hdl_gpio_pin_t gpio_pin_spi_cs = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a1),
    .module.reg = (void *)GPIO_PIN_15,
    .mode = &gpio_spi_mode,
    .inactive_default = HDL_GPIO_LOW,
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

hdl_gpio_mode_t hdl_gpio_mode_uart_0_tx = {
  .af = GPIO_AF_1,
  .pull = GPIO_PUPD_NONE,
  .type = GPIO_MODE_AF,
  .otype = GPIO_OTYPE_PP,
  .ospeed = GPIO_OSPEED_10MHZ
};
hdl_gpio_mode_t hdl_gpio_mode_uart_0_rx = {
  .af = GPIO_AF_1,
  .pull = GPIO_PUPD_NONE,
  .type = GPIO_MODE_AF,
  .otype = GPIO_OTYPE_PP,
  .ospeed = GPIO_OSPEED_10MHZ
};


hdl_gpio_pin_t hdl_gpio_pin_uart_0_tx = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a1),
  .module.reg = (void *)GPIO_PIN_9,
  .mode = &hdl_gpio_mode_uart_0_tx,
};
hdl_gpio_pin_t hdl_gpio_pin_uart_0_rx = {
  .module.init = &hdl_gpio_pin,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a1),
  .module.reg = (void *)GPIO_PIN_10,
  .mode = &hdl_gpio_mode_uart_0_rx,
};

hdl_uart_t hdl_uart_0 = {
  .module.init = &hdl_uart,
  .module.reg = (void *)USART0,
  .module.dependencies = hdl_module_dependencies(&hdl_gpio_pin_uart_0_rx.module, &hdl_gpio_pin_uart_0_tx.module,
   &mod_clock_apb2.module, &mod_nvic.module, &mod_timer_ms.module),
  .iterrupt = HDL_NVIC_IRQ27_USART0,
  .baudrate = 115200,
  .parity = HDL_UART_PARITY_NONE,
  .stop_bits = HDL_UART_STOP_1BIT,
  .word_len = HDL_UART_WORD_LEN_8BIT,
};

hdl_spi_server_config_t spi_server_config = {
  .endian = HDL_SPI_ENDIAN_MSB,
  .polarity = SPI_CK_PL_LOW_PH_2EDGE,
  .prescale = HDL_SPI_PSC_2,
};

hdl_spi_server_t mod_spi_slave = {
  .module.dependencies = hdl_module_dependencies(&gpio_pin_spi_mosi.module, &gpio_pin_spi_miso.module, &gpio_pin_spi_sck.module,
                                                  &gpio_pin_spi_cs.module, &mod_clock_apb2.module, &mod_nvic.module, &mod_timer_ms.module),
  .module.reg = (void*)SPI0,
  .module.init = &hdl_spi_server,
  .config = &spi_server_config,
  .spi_iterrupt = HDL_NVIC_IRQ25_SPI0,
  .nss_iterrupt = HDL_NVIC_IRQ7_EXTI4_15,
};

hdl_isr_buffer_t uart_isr_buffer;
uint8_t uart_tx_buff[32];
uint8_t uart_rx_buff[32];




uint32_t arr1[10]={1,2,3,4,5};


uint32_t adc_raw[2];
void test() {
  static uint32_t time_stamp_ms = 0;
  hdl_enable(&mod_adc.module);
  //hdl_enable(&mod_gpio_adc_channel_3v3.module);
  //hdl_enable(&mod_gpio_adc_channel_1v5.module);
  hdl_enable(&mod_timer_ms.module);
  hdl_enable(&mod_nvic.module);
  hdl_enable(&mod_gpo_carrier_pwr_on.module);
  //hdl_enable(&pin_pa0.module);
  //hdl_enable(&pin_pb8.module);
  //hdl_enable(&mod_m2m_dma_ch.module);
  //hdl_enable(&hdl_uart_0.module);
  hdl_enable(&mod_spi_slave.module);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  //hdl_interrupt_request(&mod_nvic, HDL_NVIC_IRQ5_EXTI0_1, &Exti_Event, NULL);
  //hdl_interrupt_request(&mod_nvic, HDL_NVIC_IRQ7_EXTI4_15, &Exti_8_Event, NULL);

  //hdl_dma_run(&mod_m2m_dma_ch, (uint32_t)&arr1[0], (uint32_t)&arr1[5], 20);

  hdl_isr_buffer_congig_t usart_isr_buffer_config = {
  .rx_buffer = uart_rx_buff,
  .tx_buffer = uart_tx_buff,
  .tx_buffer_size = sizeof(uart_tx_buff),
  .rx_buffer_size = sizeof(uart_rx_buff),
  };
  //hdl_uart_set_transceiver(&hdl_uart_0, hdl_get_isr_transceiver_handler(&uart_isr_buffer, &usart_isr_buffer_config));
  hdl_spi_server_set_transceiver(&mod_spi_slave, hdl_get_isr_transceiver_handler(&uart_isr_buffer, &usart_isr_buffer_config));

  while (1) {
    cooperative_scheduler(false);

    // uint8_t data;
    // if (hdl_isr_buffer_read(&uart_isr_buffer, &data, 1)) {
    //   hdl_isr_buffer_write(&uart_isr_buffer, &data, 1);
    // }
    //cl_protocol_transceiver_worker(&spi_slave);
    /* This code will be launched ony one time*/
    // if(hdl_adc_status(&mod_adc) == HDL_ADC_STATUS_WAITING_START_TRIGGER)
    //   hdl_adc_start(&mod_adc);

    /* There data is ready */
    // if(hdl_adc_status(&mod_adc) == HDL_ADC_STATUS_DATA_READY)
    //   hdl_adc_start(&mod_adc);

    /* This code checked and launched adc */
    // if(hdl_adc_status(&mod_adc) == HDL_ADC_STATUS_DATA_READY || hdl_adc_status(&mod_adc) == HDL_ADC_STATUS_WAITING_START_TRIGGER)
    // {
    //   hdl_adc_start(&mod_adc);
    //   /* There data is ready */
    // }
      
    /* Also you can use only this function */
    // if(hdl_adc_start(&mod_adc) == HDL_ADC_STATUS_DATA_READY)
    // {
    //   /* There data is ready */
    // }
    //adc_raw[0] = hdl_adc_get_data(&mod_adc, &mod_adc_source_0);
    //adc_raw[1] = hdl_adc_get_data(&mod_adc, &mod_adc_source_1);
    
    if (TIME_ELAPSED(time_stamp_ms, 1000, hdl_timer_get(&mod_timer_ms)))
    {
        __NOP();
    }
  }
}
#endif

#endif
