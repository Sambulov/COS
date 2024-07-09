#include "app.h"
#include "CodeLib.h"

extern hdl_timer_t mod_timer_ms;
extern hdl_gpio_pin_t mod_gpo_led;
extern hdl_gpio_pin_t mod_gpi_button;
//extern hdl_timer_t mod_timer0_ms;
//extern hdl_timer_t mod_timer2_ms;
extern hdl_exti_controller_t mod_exti;
extern hdl_nvic_t mod_nvic;
extern hdl_i2c_client_t mod_i2c0_client;


void exti0_1_hundler(uint32_t event_trigger, void *sender, void *context) {
  hdl_gpio_write(&mod_gpo_led, HDL_GPIO_LOW);
}

void main() {
  static uint32_t time_stamp_sys_ms = 0;
  static uint32_t time_stamp_timer0_ms = 0;
  static uint32_t time_stamp_timer2_ms = 0;


  hdl_enable(&mod_timer_ms.module);
  hdl_enable(&mod_gpo_led.module);
  hdl_enable(&mod_gpi_button.module);
  hdl_enable(&mod_exti.module);
  hdl_enable(&mod_i2c0_client.module);
  //hdl_enable(&mod_timer0_ms.module);
  //hdl_enable(&mod_timer2_ms.module);
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  hdl_delegate_t exti_test = {.context =NULL, .handler = &exti0_1_hundler};

  hdl_interrupt_request(&mod_nvic, HDL_NVIC_IRQ5_EXTI0_1, &exti_test);
  
// typedef enum {
//   HDL_I2C_MESSAGE_START          = 0x01, /* Generate start condition */
//   HDL_I2C_MESSAGE_ADDR           = 0x02, /* Send addr */
//   HDL_I2C_MESSAGE_MRSW           = 0x04, /* Receiver mode if option set, else trasmitter mode */
//   HDL_I2C_MESSAGE_ADDR_10        = 0x02, /* Send addr */
//   HDL_I2C_MESSAGE_NACK_LAST      = 0x08, /* Send NACK in the end in receiver mode */
//   HDL_I2C_MESSAGE_STOP           = 0x10, /* Generate stop condition */
// } hdl_i2c_message_options_t;

  /* register address */
  uint8_t i2c_buffer_register_address[2] = {0, 0x80};
  uint8_t i2c_buffer_data[4] = {0, 1, 2, 3};

  hdl_i2c_message_t i2c_msg = {
    .address = 0xA0,
    .buffer = i2c_buffer_register_address,
    .buffer_size = sizeof(i2c_buffer_register_address),
    .options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR,
  };
  hdl_i2c_message_t i2c_msg_data = {
    .address = 0xA0,
    .buffer = i2c_buffer_data,
    .buffer_size = sizeof(i2c_buffer_data),
    .options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR | HDL_I2C_MESSAGE_MRSW | 
                HDL_I2C_MESSAGE_NACK_LAST | HDL_I2C_MESSAGE_STOP,
  };
  _hdl_hal_i2c_transfer_message(&mod_i2c0_client, &i2c_msg);
  _hdl_hal_i2c_transfer_message(&mod_i2c0_client, &i2c_msg_data);
  while (1)
  {
    if (TIME_ELAPSED(time_stamp_sys_ms, 1000, hdl_timer_get(&mod_timer_ms))){
      time_stamp_sys_ms += 1000;
      
      hdl_gpio_toggle(&mod_gpo_led);
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

#ifdef XXX

#define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

extern hdl_nvic_t mod_nvic;
extern hdl_dma_t mod_dma;
extern hdl_core_t mod_sys_core;
extern hdl_adc_t mod_adc;
extern hdl_gpio_pin_t mod_gpio_adc_channel_3v3;
extern hdl_gpio_pin_t mod_gpio_adc_channel_1v5;
extern hdl_timer_t mod_timer_ms;
extern hdl_adc_source_t mod_adc_source_0;
extern hdl_adc_source_t mod_adc_source_1;
extern hdl_clock_prescaler_t mod_clock_apb2;
extern hdl_gpio_pin_t mod_gpo_carrier_pwr_on;

extern  hdl_dma_channel_t mod_m2m_dma_ch;

 void SysTick_Event(uint32_t event, void *sender, void *context){
  switch (event)
  {
  case HDL_BTN_EVENT_RELEASE:
    __NOP();
    break;
  case HDL_BTN_EVENT_PRESS:
    __NOP();
    break;
  case HDL_BTN_EVENT_HOLD:
    __NOP();
    break;
  case HDL_BTN_EVENT_CLICK:
    __NOP();
    break;
  default:
    break;
  }
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

hdl_gpio_mode_t mod_gpio_output_pp = {
  .type = GPIO_MODE_OUTPUT,
  .otype = GPIO_OTYPE_PP,
  .ospeed = GPIO_OSPEED_2MHZ,
  .pull = GPIO_PUPD_NONE,
};

hdl_gpio_mode_t gpio_spi_mode = {
    .type = GPIO_MODE_AF,
    .pull = GPIO_PUPD_NONE,
    .ospeed = GPIO_OSPEED_50MHZ,
    .af = GPIO_AF_0,
    .otype = GPIO_OTYPE_PP,
};




#ifdef SPI_CLIENT
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
    .inactive_default = HDL_GPIO_HIGH,
};


hdl_spi_client_config_t spi_master_config = {
  .endian = HDL_SPI_ENDIAN_MSB,
  .polarity = SPI_CK_PL_LOW_PH_2EDGE,
  .prescale = HDL_SPI_PSC_256,
};
hdl_spi_server_config_t spi_slave_config = {
  .endian = HDL_SPI_ENDIAN_MSB,
  .polarity = SPI_CK_PL_LOW_PH_2EDGE
};


/**************************************************************
 *  DMA
 *************************************************************/
hdl_dma_channel_t mod_dma_ch_spi_rx = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH1,
  .direction = HDL_DMA_DIRECTION_P2M,
  .memory_inc = HDL_DMA_INCREMENT_ON,
  .memory_width = HDL_DMA_SIZE_OF_MEMORY_8_BIT,
  .periph_inc = HDL_DMA_INCREMENT_OFF,
  .periph_width = HDL_DMA_SIZE_OF_MEMORY_8_BIT,
  .mode = HDL_DMA_MODE_SINGLE,
  .priority = 0
};

  hdl_dma_channel_t mod_dma_ch_spi_tx = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH2,
  .direction = HDL_DMA_DIRECTION_M2P,
  .memory_inc = HDL_DMA_INCREMENT_ON,
  .memory_width = HDL_DMA_SIZE_OF_MEMORY_8_BIT,
  .periph_inc = HDL_DMA_INCREMENT_OFF,
  .periph_width = HDL_DMA_SIZE_OF_MEMORY_8_BIT,
  .mode = HDL_DMA_MODE_SINGLE,
  .priority = 0
};

  hdl_dma_channel_t mod_dma_ch_spi_m2m = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH3,
  .direction = HDL_DMA_DIRECTION_M2M,
  .memory_inc = HDL_DMA_INCREMENT_ON,
  .memory_width = HDL_DMA_SIZE_OF_MEMORY_8_BIT,
  .periph_inc = HDL_DMA_INCREMENT_ON,
  .periph_width = HDL_DMA_SIZE_OF_MEMORY_8_BIT,
  .mode = HDL_DMA_MODE_SINGLE,
  .priority = 0
};

/* depends on:
  gpio mosi
  gpio miso  
  gpio sck
  gpio nss
  apb2_bus for SPI 5, 4, 3, 0; apb1_bus for SPI 1, 2
  interrupt controller (nvic)
  hdl_dma_channel rx
  hdl_dma_channel tx
*/
uint8_t spi_dma_rx_0_buf[1024] = {0xaa, 5, 3, 6, 3, 2, 1, 5};
uint8_t spi_dma_rx_1_buf[1024] = {0xaa, 5, 3, 6, 3, 2, 1, 5};
uint8_t spi_dma_tx_0_buf[1024] = {0xaa, 5, 3, 6, 3, 2, 1, 5, 2, 2};
uint8_t spi_dma_tx_1_buf[1024] = {0xaa, 5, 3, 6, 3, 2, 1, 5, 2, 2};

hdl_double_buffer_t spi_rx_buffer = {
  .data[0] = spi_dma_rx_0_buf,
  .data[1] = spi_dma_rx_1_buf,
  .size = sizeof(spi_dma_rx_0_buf),
};

hdl_double_buffer_t spi_tx_buffer = {
  .data[0] = spi_dma_tx_0_buf,
  .data[1] = spi_dma_tx_1_buf,
  .size = sizeof(spi_dma_tx_0_buf),
};

hdl_spi_server_dma_t mod_spi_server_dma = {
  .module.reg = (void *)SPI0,
  .module.dependencies = hdl_module_dependencies(&gpio_pin_spi_mosi.module, &gpio_pin_spi_miso.module, &gpio_pin_spi_sck.module,
                                                  &gpio_pin_spi_cs.module, &mod_clock_apb2.module, &mod_nvic.module, 
                                                  &mod_dma_ch_spi_rx.module, &mod_dma_ch_spi_tx.module),
  .module.init = &hdl_spi_server_dma,
  .config = &spi_slave_config,
  .spi_iterrupt = HDL_NVIC_IRQ25_SPI0,
  .nss_iterrupt = HDL_NVIC_IRQ7_EXTI4_15,
};

hdl_spi_client_t mod_spi_master_0 = {
  .module.init = &hdl_spi_client,
  .module.dependencies = hdl_module_dependencies(&gpio_pin_spi_mosi.module, &gpio_pin_spi_miso.module, &gpio_pin_spi_sck.module,
                                                  &mod_clock_apb2.module, &mod_nvic.module),
  .module.reg = (void *)SPI0,
  .config = &spi_master_config,
  .spi_iterrupt = HDL_NVIC_IRQ25_SPI0,
};

hdl_spi_client_ch_t spi_master_0_ch_0 = {
  .module.init = &hdl_spi_ch,
  .module.dependencies = hdl_module_dependencies(&mod_spi_master_0.module, &gpio_pin_spi_cs.module),
};

#else
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

hdl_spi_server_config_t spi_server_config = {
  .endian = HDL_SPI_ENDIAN_MSB,
  .polarity = SPI_CK_PL_LOW_PH_2EDGE,
  .prescale = HDL_SPI_PSC_2,
};

hdl_spi_server_t mod_spi_slave = {
  .module.dependencies = hdl_module_dependencies(&gpio_pin_spi_mosi.module, &gpio_pin_spi_miso.module, &gpio_pin_spi_sck.module,
                                                  &gpio_pin_spi_cs.module, &mod_clock_apb2.module, &mod_nvic.module),
  .module.reg = (void*)SPI0,
  .module.init = &hdl_spi_server,
  .config = &spi_server_config,
  .spi_iterrupt = HDL_NVIC_IRQ25_SPI0,
  .nss_iterrupt = HDL_NVIC_IRQ7_EXTI4_15,
};
#endif




hdl_gpio_pin_t pin_pa0 = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a1),
    .module.reg = (void *)GPIO_PIN_0,
    .mode = &gpio_input_np,
    .inactive_default = HDL_GPIO_HIGH,
};

hdl_gpio_pin_t pin_pb8 = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b1),
    .module.reg = (void *)GPIO_PIN_8,
    .mode = &gpio_input_np
};


hdl_button_t btn = {
  .module.init = &hdl_button,
  .module.dependencies = hdl_module_dependencies(&pin_pa0.module, &mod_timer_ms.module),\
  .debounce_delay = 100,
  .hold_delay = 3000,
};

hdl_timer_event_t timer_with_event = {
  .module.dependencies = hdl_module_dependencies(&mod_timer_ms.module),
  .module.init = &hdl_timer_event,
  .delay = 3000,
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
  .interrupt = HDL_NVIC_IRQ27_USART0,
  .baudrate = 115200,
  .parity = HDL_UART_PARITY_NONE,
  .stop_bits = HDL_UART_STOP_1BIT,
  .word_len = HDL_UART_WORD_LEN_8BIT,
};



hdl_isr_buffer_t uart_isr_buffer;
uint8_t uart_tx_buff[32];
uint8_t uart_rx_buff[32];




uint32_t arr1[10]={1,2,3,4,5};


uint32_t adc_raw[2];
void test() {
  static uint32_t time_stamp_ms = 0;

  //hdl_enable(&mod_adc.module);
  //hdl_enable(&mod_gpio_adc_channel_3v3.module);
  //hdl_enable(&mod_gpio_adc_channel_1v5.module);
  hdl_enable(&mod_timer_ms.module);
  //hdl_enable(&mod_nvic.module);
  //hdl_enable(&mod_gpo_carrier_pwr_on.module);
  //hdl_enable(&pin_pa0.module);
  //hdl_enable(&pin_pb8.module);
  //hdl_enable(&mod_m2m_dma_ch.module);
  //hdl_enable(&hdl_uart_0.module);
  
  //hdl_enable(&btn.module);
  //hdl_enable(&timer_with_event.module);
  
  hdl_enable(&mod_spi_server_dma.module);
#ifdef SPI_CLIENT
  //hdl_enable(&spi_master_0_ch_0.module);
#else
  hdl_enable(&mod_spi_slave.module);
#endif

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

  uint8_t spi_mem_data[5];
  hdl_basic_buffer_t spi_mem_test = {
    .data = spi_mem_data,
    .size = 5,
  };

  //hdl_uart_set_transceiver(&hdl_uart_0, hdl_get_isr_transceiver_handler(&uart_isr_buffer, &usart_isr_buffer_config));
#ifdef SPI_CLIENT

#else
  hdl_spi_server_set_transceiver(&mod_spi_slave, hdl_get_isr_transceiver_handler(&uart_isr_buffer, &usart_isr_buffer_config));
#endif
  
  static hdl_delegate_t btn_delegate = {
    .handler = &SysTick_Event,
    .context = NULL,
  };
  //hdl_event_subscribe(&timer_with_event.event, &btn_delegate);
  //hdl_timer_event_run_once(&timer_with_event);
  
  static uint8_t tx_spi_buf[2] = {1, 2};
  static uint8_t rx_spi_buf[5] = {0, 0, 0};

  static hdl_spi_message_t spi_msg = {
    .rx_buffer = rx_spi_buf,
    .tx_buffer = tx_spi_buf,
    .tx_len =  sizeof(tx_spi_buf),
    .rx_skip = 0,
    .rx_take = sizeof(rx_spi_buf),
  };
  //hdl_spi_client_xfer(&spi_master_0_ch_0, &spi_msg);
  while (1) {
    cooperative_scheduler(false);
    static uint8_t flag = 0;
    // if(hdl_spi_mem_rx_buffer_take(&mod_spi_server_dma, &spi_mem_test, 5) && !flag)
    // {
    //   flag = 1;
    //   __NOP();
    // }
  
    
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
