#include "app.h"
#include "CodeLib.h"

hdl_adc_ms5194t_source_t mod_adc_src_1 = {
  .config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF | MS5194T_CONFIG_REG_CH_SEL(1)
};

hdl_adc_ms5194t_source_t mod_adc_src_2 = {
  .config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF | MS5194T_CONFIG_REG_CH_SEL(2)
};

hdl_adc_ms5194t_source_t mod_adc_src_3 = {
  .config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF | MS5194T_CONFIG_REG_CH_SEL(3)
};

hdl_adc_ms5194t_source_t mod_adc_src_4 = {
  .config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF | MS5194T_CONFIG_REG_CH_SEL(4)
};

hdl_adc_ms5194t_source_t mod_adc_src_5 = {
  .config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF | MS5194T_CONFIG_REG_CH_SEL(5)
};

hdl_adc_ms5194t_source_t mod_adc_src_6 = {
  .config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF | MS5194T_CONFIG_REG_CH_SEL(6)
};

const hdl_adc_ms5194t_config_t mod_adc_cnf = {
  .io_reg = MS5194T_IO_REG_DEFAULT,
  .mode_reg = MS5194T_MODE_REG_MS_PWR_DWN | !MS5194T_MODE_REG_PSW | !MS5194T_MODE_REG_AMP_CM | MS5194T_MODE_REG_CLK_INT64K |
              !MS5194T_MODE_REG_CHOP_DIS | MS5194T_MODE_REG_FILTER_RATE(5),
  .sources = hdl_adc_ms5194t_sources(&mod_adc_src_1, &mod_adc_src_2, &mod_adc_src_3, &mod_adc_src_4, &mod_adc_src_5, &mod_adc_src_6)
};

hdl_adc_ms5194t_t mod_adc = {
  .module.init = &hdl_adc_ms5194t,
  .module.dependencies = hdl_module_dependencies(&uspd20k_adc_spi.module, &mod_timer_ms.module),
  .config = &mod_adc_cnf
};

extern hdl_gpio_pin_t mod_spi_0_cs;

void main() {
  //hdl_enable(&mod_app);
  hdl_enable(&hdl_uspd_ain_port1.module);
  hdl_enable(&hdl_uspd_ain_port2.module);
  hdl_enable(&hdl_uspd_ain_port3.module);
  hdl_enable(&hdl_uspd_ain_port4.module);
  hdl_enable(&mod_adc.module);
  //hdl_enable(&mod_sys_core.module);
  //hdl_enable(&mod_spi_0_cs.module);
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  hdl_gpio_set_active(&mod_spi_0_cs);

  uint32_t timer;

  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_NTC_PU);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_4K3_PD);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_150R_PD);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW);
  //hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_1K_PD);

  hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_150R_PD);
  hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port2, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_1K_PD);
  hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port3, USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_150R_PD | USPD20K_CIRCUIT_CONFIG_1K_PD);
  hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port4, USPD20K_CIRCUIT_CONFIG_150R_PD | USPD20K_CIRCUIT_CONFIG_1K_PD);
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

  uint32_t a1[100];
  uint32_t a2[100];
  uint32_t a3[100];
  uint32_t a4[100];
  uint32_t a5[100];
  uint32_t a6[100];

  uint32_t adc_age = 0, valind = 0;
  
  while (1) {
    cooperative_scheduler(false);
    uint32_t age = hdl_adc_ms5194t_age(&mod_adc);
    if(adc_age != age) {
      a1[valind] = hdl_adc_ms5194t_get(&mod_adc, 0);
      a2[valind] = hdl_adc_ms5194t_get(&mod_adc, 1);
      a3[valind] = hdl_adc_ms5194t_get(&mod_adc, 2);
      a4[valind] = hdl_adc_ms5194t_get(&mod_adc, 3);
      a5[valind] = hdl_adc_ms5194t_get(&mod_adc, 4);
      a6[valind] = hdl_adc_ms5194t_get(&mod_adc, 5);
      adc_age = age;
      valind++;
      if(valind>=100) {
        valind = 0;
      }
    }

  }
}
