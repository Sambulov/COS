#include "app.h"
#include "CodeLib.h"

hdl_adc_ms5194t_source_t mod_adc_src_0 = {
  .config_reg = 0
};

const hdl_adc_ms5194t_config_t mod_adc_cnf = {
  .io_reg = 0,
  .mode_reg = 0,
  .sources = hdl_adc_ms5194t_sources(&mod_adc_src_0)
};

hdl_adc_ms5194t_t mod_adc = {
  .module.init = &hdl_adc_ms5194t,
  .module.dependencies = hdl_module_dependencies(&uspd20k_adc_spi.module, &mod_timer_ms.module),
  .config = &mod_adc_cnf
};

void main() {
  hdl_enable(&mod_app);
  hdl_enable(&mod_adc.module);
  //hdl_enable(&mod_sys_core.module);
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  uint32_t timer;

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

  while (1) {
    cooperative_scheduler(false);
  }
}
