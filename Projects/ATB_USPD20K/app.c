#include "app.h"
#include "CodeLib.h"

//extern hdl_core_t mod_sys_core;

typedef struct {

} hdl_ad7794_adc_source_t;

/* depends on 
  spi

 */
typedef struct {
  hdl_module_t module;

} hdl_ad7794_adc_t;

hdl_ad7794_adc_source_t mod_adc_source_0 = {

};

hdl_ad7794_adc_source_t mod_adc_source_1 = {

};

hdl_ad7794_adc_source_t mod_adc_source_2 = {

};

hdl_ad7794_adc_source_t mod_adc_source_3 = {

};

void main() {
  hdl_enable(&mod_app);
  //hdl_enable(&mod_sys_core.module);
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  uint32_t timer;

  uint8_t data[4] = {1, 2, 3, 4};

  hdl_spi_message_t reset_spi_adc_msg = {
    .options = HDL_SPI_MESSAGE_CH_SELECT | HDL_SPI_MESSAGE_CH_RELEASE,
    .tx_buffer = (uint8_t []){0xFF, 0xFF, 0xFF, 0xFF},
    .tx_len = 4,
    .rx_skip = 0,
    .rx_take = 0,
    .rx_buffer = NULL
  };

  hdl_spi_message_t cnf_w_adc_msg = {
    .options = HDL_SPI_MESSAGE_CH_SELECT | HDL_SPI_MESSAGE_CH_RELEASE,
    .tx_buffer = (uint8_t []){0x10, 0x00, 0x10},
    .tx_len = 3,
    .rx_skip = 0,
    .rx_take = 0,
    .rx_buffer = NULL
  };

  hdl_spi_message_t select_read_cnf_msg = {
    .options = HDL_SPI_MESSAGE_CH_SELECT,
    .tx_buffer = (uint8_t []){0x50},
    .tx_len = 1,
    .rx_skip = 0,
    .rx_take = 0,
    .rx_buffer = NULL
  };

  hdl_spi_message_t read_reg_msg = {
    .options = HDL_SPI_MESSAGE_CH_RELEASE,
    .tx_buffer = NULL,
    .tx_len = 0,
    .rx_skip = 0,
    .rx_take = 3,
    .rx_buffer = data
  };

  hdl_spi_message_t singl_conv_msg = {
    .options = HDL_SPI_MESSAGE_CH_SELECT,
    .tx_buffer = (uint8_t []){0x08, 0x20, 0x0A},
    .tx_len = 3,
    .rx_take = 0,
    .rx_buffer = NULL
  };

  hdl_spi_message_t chip_select_msg = {
    .options = HDL_SPI_MESSAGE_CH_SELECT,
    .tx_buffer = NULL,
    .tx_len = 0,
    .rx_take = 0,
    .rx_buffer = NULL
  };
  
  hdl_spi_message_t get_data_msg = {
    .options = HDL_SPI_MESSAGE_CH_RELEASE,
    .tx_buffer = (uint8_t []){0x58, 0x00},
    .tx_len = 2,
    .rx_skip = 1,
    .rx_take = 3,
    .rx_buffer = data
  };  

  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_NTC_PU);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_4K3_PD);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_150R_PD);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_1K_PD);

  hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_150R_PD | USPD20K_CIRCUIT_CONFIG_1K_PD);
  // hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_FLOATING);
  // hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port2, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_150R_PD | USPD20K_CIRCUIT_CONFIG_1K_PD);
  // hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port2, USPD20K_CIRCUIT_CONFIG_FLOATING);
  // hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port3, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_150R_PD | USPD20K_CIRCUIT_CONFIG_1K_PD);
  // hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port3, USPD20K_CIRCUIT_CONFIG_FLOATING);
  // hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port4, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_150R_PD | USPD20K_CIRCUIT_CONFIG_1K_PD);
  // hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port4, USPD20K_CIRCUIT_CONFIG_FLOATING);

  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_FLOATING);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port2, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port2, USPD20K_CIRCUIT_CONFIG_FLOATING);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port3, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port3, USPD20K_CIRCUIT_CONFIG_FLOATING);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port4, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port4, USPD20K_CIRCUIT_CONFIG_FLOATING);

  uint8_t state = 0;

  while (1) {
    switch (state) {
    case 0:
      if(hdl_spi_transfer_message(&uspd20k_adc_spi, &reset_spi_adc_msg))
        state++;
      break;
    case 1:
      if(reset_spi_adc_msg.state & HDL_SPI_MESSAGE_STATUS_COMPLETE) state += 3;
      break;
    // case 2:
    //   if(hdl_spi_transfer_message(&uspd20k_adc_spi, &cnf_w_adc_msg))
    //     state++;
    //   break;
    // case 3:
    //   if(cnf_w_adc_msg.state & HDL_SPI_MESSAGE_STATUS_COMPLETE) state++;
    //   break;
    case 4:
      if(hdl_spi_transfer_message(&uspd20k_adc_spi, &select_read_cnf_msg))
        state++;
      break;
    case 5:
      //if(hdl_gpio_read(&uspd20k_adc_rdy) == HDL_GPIO_LOW) 
      state++;
      break;
    case 6:
      if(hdl_spi_transfer_message(&uspd20k_adc_spi, &read_reg_msg))
        state++;
      break;
    case 7:
      if(read_reg_msg.state & HDL_SPI_MESSAGE_STATUS_COMPLETE) 
        state++;
      break;




    case 8:
      //hdl_spi_transfer_message(&uspd20k_adc_spi, &singl_conv_msg);
      hdl_spi_transfer_message(&uspd20k_adc_spi, &chip_select_msg);
      state++;
      break;
    case 9:
      if((chip_select_msg.state & HDL_SPI_MESSAGE_STATUS_COMPLETE) 
         //&& (hdl_gpio_read(&uspd20k_adc_rdy) == HDL_GPIO_HIGH)
         ) state++;
      break;
    case 10:
      if (hdl_gpio_read(&uspd20k_adc_rdy) == HDL_GPIO_LOW) state++;
      break;
    case 11:
      hdl_spi_transfer_message(&uspd20k_adc_spi, &get_data_msg);
      state++;
      break;
    case 12:
      if(get_data_msg.state & HDL_SPI_MESSAGE_STATUS_COMPLETE) state++;
      timer = hdl_time_counter_get(&mod_timer_ms);
      break;
    case 13: {
      uint32_t now = hdl_time_counter_get(&mod_timer_ms);
      if(TIME_ELAPSED(timer, 100, now)) state++;
      break;
    }
    case 14:
    default:
      state = 4;
      break;
    }


    // 
    cooperative_scheduler(false);
  }
}
