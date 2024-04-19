/**************************************************************************//**
 * @file     main.c
 * @version  
 * @brief    
 *
 ******************************************************************************/

#include "app.h"
#include "CodeLib.h"

extern hdl_nvic_t mod_nvic;
extern hdl_nvic_interrupt_t mod_exti_0_1_irq;
extern hdl_nvic_interrupt_t mod_exti_2_3_irq;
extern hdl_nvic_interrupt_t mod_unexisting_8_irq;
extern hdl_dma_t mod_dma;
extern hdl_adc_t mod_adc;

static void power_btn_handler(uint32_t event, void *sender, void *context) {
  // hdl_button_t *btn = (hdl_button_t *)sender;
  // bldl_som_power_state_t *som_state = (bldl_som_power_state_t *)context;
  // switch ((hdl_btn_event_t)event)
  // {
  //   case HDL_BTN_EVENT_PRESS:
  //   case HDL_BTN_EVENT_RELEASE:
  //     hdl_gpio_write(&mod_gpo_pmic_power_on, ((hdl_btn_event_t)event == HDL_BTN_EVENT_PRESS)? HDL_GPIO_HIGH: HDL_GPIO_LOW);
  //     break;
  //   case HDL_BTN_EVENT_CLICK:
  //     if(som_state->current_state == BLDL_SOM_STATE_OFF)
  //       //bldl_som_exec_state(som_state, BLDL_SOM_STATE_ON);
  //     break;
  //   case HDL_BTN_EVENT_HOLD:
  //     if(som_state->current_state == BLDL_SOM_STATE_ON)
  //       //bldl_som_exec_state(som_state, BLDL_SOM_STATE_OFF);
  //   default:
  //     break;
  // }
}

static void reset_btn_handler(uint32_t event, void *sender, void *context) {
  // hdl_button_t *btn = (hdl_button_t *)sender;
  // bldl_som_power_state_t *som_state = (bldl_som_power_state_t *)context;
  // switch ((hdl_btn_event_t)event) {
  //   case HDL_BTN_EVENT_PRESS:
  //     if(som_state->current_state == BLDL_SOM_STATE_ON)
  //       bldl_som_exec_state(som_state, BLDL_SOM_STATE_RESET);
  //     break;
  //   case HDL_BTN_EVENT_RELEASE:
  //     if(som_state->current_state == BLDL_SOM_STATE_RESET)
  //       bldl_som_exec_state(som_state, BLDL_SOM_STATE_ON);
  //   default:
  //     break;
  // }  
}

// void i2c_handler(uint32_t event, void *sender, void *context) {

// }

// void test_read_pmic_regs() {
//   hdl_callback_t i2c_cb = {
//     .context = NULL,
//     .handler = &i2c_handler,
//   };

//   hdl_i2c_t i2c = {
//     .hw_conf = &mod_i2c_0,
//     .server_event_cb = &i2c_cb
//   };

//   #define U16_MSB(val)   ((uint8_t)(val >> 8))
//   #define U16_LSB(val)   ((uint8_t)(val))

//   uint16_t reg_arr[] = {
//     RK809_REG_PMIC_CHIP_NAME,
//     RK809_REG_PMIC_CHIP_VER,
//     RK809_REG_PMIC_SYS_CFG3,
//   };
//   uint8_t reg_addr[2];
//   uint8_t reg_data[1];

//   hdl_i2c_transaction_t transaction;
//   hdl_i2c_client_transaction_init_reset(&transaction);

//   hdl_i2c_message_t start_mess = {
//     .address = RK809_PMIC_I2C_ADDR,
//     .buffer = reg_addr,
//     .buffer_size = sizeof(reg_addr),
//     .flags = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR
//   };

//   hdl_i2c_message_t data_mess = {
//     .address = RK809_PMIC_I2C_ADDR,
//     .buffer = reg_data,
//     .buffer_size = sizeof(reg_data),
//     .flags = HDL_I2C_MESSAGE_START |
//              HDL_I2C_MESSAGE_ADDR |
//              HDL_I2C_MESSAGE_MRSW |
//              HDL_I2C_MESSAGE_NACK_LAST |
//              HDL_I2C_MESSAGE_STOP
//   };

//   for(int32_t i =0; i<(sizeof(reg_arr)/sizeof(uint16_t)); i++) {
//     reg_addr[0] = U16_MSB(reg_arr[i]);
//     reg_addr[1] = U16_LSB(reg_arr[i]);
//     hdl_i2c_client_transaction_add_message(&transaction, &start_mess);
//     hdl_i2c_client_transaction_add_message(&transaction, &data_mess);
//     hdl_i2c_client_enqueue_transaction(&i2c, &transaction);
//     while(hdl_i2c_client_transaction_state(&transaction) < HDL_I2C_TRANSACTION_EXECUTED) {
//       hdl_i2c_work(&i2c);
//     }
//     hdl_i2c_client_transaction_init_reset(&transaction);
//   }

// }


void event_exti_2_3_isr(uint32_t event, void *sender, void *context) {
  __NOP();
  NVIC_SetPendingIRQ(EXTI0_1_IRQn);
  while (1)
  {
    __NOP();
  }  
}

void event_exti_0_1_isr(uint32_t event, void *sender, void *context) {
  __NOP();
}

void event_unexisting_8_isr(uint32_t event, void *sender, void *context) {
  __NOP();
  NVIC_SetPendingIRQ(EXTI0_1_IRQn);
  while (1)
  {
    __NOP();
  }
}


uint16_t adc_value[4];

void main() {

  hdl_enable(&mod_nvic.module);

  

  // static bldl_som_power_state_t som_state = {
  //    .module = &mod_som_state_ctrl
  // };

  // // hdl_system_init(&mod_sys_cnf);

  // hdl_callback_t power_btn_cb = {
  //   .context = &som_state,
  //   .handler = &power_btn_handler
  // };

  // hdl_callback_t reset_btn_cb = {
  //   .context = &som_state,
  //   .handler = &reset_btn_handler
  // };

  // hdl_button_t power_button = {
  //   .module = &mod_btn_power,
  //   .button_event_cb = &power_btn_cb,
  //   .event_mask = HDL_BTN_EVENT_RELEASE |
  //                 HDL_BTN_EVENT_PRESS |
  //                 HDL_BTN_EVENT_HOLD |
  //                 HDL_BTN_EVENT_CLICK
  // };

  // hdl_button_t reset_button = {
  //   .module = &mod_btn_reset,
  //   .button_event_cb = &reset_btn_cb,
  //   .event_mask = HDL_BTN_EVENT_RELEASE |
  //                 HDL_BTN_EVENT_PRESS
  // };

  // bldl_som_exec_state(&som_state, BLDL_SOM_STATE_OFF);
  // //bldl_som_exec_state(&som_state, BLDL_SOM_STATE_ON);
  // test_read_pmic_regs();
  //hdl_enable(&mod_gpi_carrier_power_btn.module, HDL_TRUE);
  //hdl_enable(&mod_gpo_emmc_lock.module);
  hdl_enable(&mod_gpo_carrier_pwr_on.module);
  hdl_enable(&mod_sys_timer_ms.module);
  //hdl_enable(&mod_gpi_carrier_boot_sel2.module);
  //hdl_kill(&mod_gpo_emmc_lock.module);

  //mod_sys_timer_ms.val
  hdl_enable(&mod_sys_timer_ms.module);
  hdl_enable(&mod_timer0_counter.module);
  hdl_enable(&mod_dma.module);
  hdl_enable(&mod_adc.module);

  //syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN0);
  //exti_init(EXTI_0, EXTI_INTERRUPT, EXTI_TRIG_RISING);
  // uint32_t a = 0xff;
  // uint32_t b = 0x0;
  // hdl_dma_config_t config;
  // config.memory_addr = (uint32_t)&b;
  // config.memory_width = HDL_DMA_SIZE_OF_MEMORY_32_BIT;
  // config.memory_inc = HDL_DMA_INCREMENT_OFF;
  // config.periph_addr = (uint32_t)&a;
  // config.periph_inc = HDL_DMA_INCREMENT_OFF;
  // config.periph_width = HDL_DMA_SIZE_OF_MEMORY_32_BIT;
  // config.amount = 1;
  // config.direction = HDL_DMA_DIRECTION_M2M;
  // config.dma_mode = HDL_DMA_MODE_SINGLE_CONVERSION;
  // config.priority = 0;

  while (hdl_state(&mod_nvic.module) != HDL_MODULE_INIT_OK) {
      cooperative_scheduler(false);
  }

  hdl_interrupt_request(&mod_nvic, &mod_exti_2_3_irq, &event_exti_2_3_isr, NULL);
  hdl_interrupt_request(&mod_nvic, &mod_unexisting_8_irq, &event_unexisting_8_isr, NULL);
  hdl_interrupt_request(&mod_nvic, &mod_exti_0_1_irq, &event_exti_0_1_isr, NULL);
  hdl_exti_request(&mod_nvic, EXTI_0);
  //NVIC_SetPendingIRQ(8);

  while(1) {
    static uint8_t flag = 0;
    static uint32_t time_stamp_ms = 0;
    uint32_t cnt = TIMER_CNT(TIMER0);
    cooperative_scheduler(false);

    // if(hdl_state(&mod_adc.module) == HDL_MODULE_INIT_OK){
    //   hdl_adc_start(&mod_adc, adc_value);
    // }

    // /* DMA test */
    // if(hdl_state(&mod_dma.module) == HDL_MODULE_INIT_OK){
    //   if(!flag)
    //   {
    //     flag = 1;
    //     hdl_dma_config(NULL, &config, 0);
    //     hdl_dma_sw_triger(NULL, 0);
    //   }
    // }

    if(hdl_state(&mod_gpo_carrier_pwr_on.module) == HDL_MODULE_INIT_OK)
    {
      if(hdl_state(&mod_sys_timer_ms.module) == HDL_MODULE_INIT_OK)
      {
        if(TIME_ELAPSED(time_stamp_ms, 1000, millis(&mod_sys_timer_ms)))
        {
          time_stamp_ms = millis(&mod_sys_timer_ms);
          hdl_gpio_toggle(&mod_gpo_carrier_pwr_on);
        }
          
      }
    }
    // hdl_btn_work(&power_button);
    // hdl_btn_work(&reset_button);
  }
}
