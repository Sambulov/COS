#include "app.h"
#include "CodeLib.h"

//extern hdl_core_t mod_sys_core;



void main() {
  hdl_enable(&mod_timer_ms.module); 
  //hdl_enable(&mod_sys_core.module);
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  // hdl_spi_message_t sing_conv_msg = {
  //   .options = HDL_SPI_MESSAGE_BUS_KEEP_HOLD,
  //   .tx_buffer = (uint8_t []){0x08, 0x20, 0x0A},
  //   .tx_len = 3,
  //   .rx_take = 0,
  //   .rx_buffer = NULL
  // };
  // hdl_spi_transfer_message(&uspd20k_adc_spi, &sing_conv_msg);

  // while (!(sing_conv_msg.state & HDL_SPI_MESSAGE_STATUS_COMPLETE)) {
  //   cooperative_scheduler(false);
  // }
  // uint8_t data[16];
  // hdl_spi_message_t get_data_msg = {
  //   .options = 0,
  //   .tx_buffer = (uint8_t []){0x58},
  //   .tx_len = 1,
  //   .rx_skip = 1,
  //   .rx_take = 4,
  //   .rx_buffer = data
  // };
  // hdl_spi_transfer_message(&uspd20k_adc_spi, &get_data_msg);

  // while (!(get_data_msg.state & HDL_SPI_MESSAGE_STATUS_COMPLETE)) {
  //   cooperative_scheduler(false);
  // }
  

  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_NTC_PU);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_4K3_PD);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_150R_PD);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_1K_PD);

  // hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_150R_PD | USPD20K_CIRCUIT_CONFIG_1K_PD);
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
  while (1) {
    cooperative_scheduler(false);
  }
}
