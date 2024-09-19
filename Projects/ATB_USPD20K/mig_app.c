#include "app.h"

const hdl_uspd_ain_port_config_t hdl_uspd_ain_port_cnf_default = {
  .circuit_config = USPD20K_CIRCUIT_CONFIG_4K3_PD
};

bldl_uspd_ain_port_t hdl_uspd_ain_port1 = {
  .module.init = &bldl_uspd_ain_port,
  .module.dependencies = hdl_module_dependencies(
    &uspd20k_ai1_ntc_pu.module, &uspd20k_ai1_cur_scr_high.module, &uspd20k_ai1_cur_scr_low.module, 
    &uspd20k_ai1_4K3_pd.module, &uspd20k_ai1_150r_pd.module, &uspd20k_ai1_1k_pd.module),
  .config = &hdl_uspd_ain_port_cnf_default
};

bldl_uspd_ain_port_t hdl_uspd_ain_port2 = {
  .module.init = &bldl_uspd_ain_port,
  .module.dependencies = hdl_module_dependencies(
    &uspd20k_ai2_ntc_pu.module, &uspd20k_ai2_cur_scr_high.module, &uspd20k_ai2_cur_scr_low.module, 
    &uspd20k_ai2_4K3_pd.module, &uspd20k_ai2_150r_pd.module, &uspd20k_ai2_1k_pd.module),
  .config = &hdl_uspd_ain_port_cnf_default
};

bldl_uspd_ain_port_t hdl_uspd_ain_port3 = {
  .module.init = &bldl_uspd_ain_port,
  .module.dependencies = hdl_module_dependencies(
    &uspd20k_ai3_ntc_pu.module, &uspd20k_ai3_cur_scr_high.module, &uspd20k_ai3_cur_scr_low.module, 
    &uspd20k_ai3_4K3_pd.module, &uspd20k_ai3_150r_pd.module, &uspd20k_ai3_1k_pd.module),
  .config = &hdl_uspd_ain_port_cnf_default
};

bldl_uspd_ain_port_t hdl_uspd_ain_port4 = {
  .module.init = &bldl_uspd_ain_port,
  .module.dependencies = hdl_module_dependencies(    
    &uspd20k_ai4_ntc_pu.module, &uspd20k_ai4_cur_scr_high.module, &uspd20k_ai4_cur_scr_low.module, 
    &uspd20k_ai4_4K3_pd.module, &uspd20k_ai4_150r_pd.module, &uspd20k_ai4_1k_pd.module),
  .config = &hdl_uspd_ain_port_cnf_default
};

app_circuit_config_t ai_circuit_config = {
  .active.ai1 = USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_150R_PD,
  .active.ai2 = USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_1K_PD,
  .active.ai3 = USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_150R_PD | USPD20K_CIRCUIT_CONFIG_1K_PD,
  .active.ai4 = USPD20K_CIRCUIT_CONFIG_150R_PD | USPD20K_CIRCUIT_CONFIG_1K_PD,
  .user.ai1 = USPD20K_CIRCUIT_CONFIG_4K3_PD,
  .user.ai2 = USPD20K_CIRCUIT_CONFIG_4K3_PD,
  .user.ai3 = USPD20K_CIRCUIT_CONFIG_4K3_PD,
  .user.ai4 = USPD20K_CIRCUIT_CONFIG_4K3_PD,
  .sync_key = CIRCUIT_CONFIG_SYNC_KEY,
};

app_adc_config_t adc_config = {
  .src_user = {{
    .config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                  !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                  !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF | MS5194T_CONFIG_REG_CH_SEL(0),
    .options = HDL_ADC_MS5194T_CHANNEL_ENABLE | HDL_ADC_MS5194T_CHANNEL_ZERO_SCALE_CALIBRATE | HDL_ADC_MS5194T_CHANNEL_FULL_SCALE_CALIBRATE
  },
  {
    .config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                  !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                  !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF | MS5194T_CONFIG_REG_CH_SEL(1),
    .options = HDL_ADC_MS5194T_CHANNEL_ENABLE | HDL_ADC_MS5194T_CHANNEL_ZERO_SCALE_CALIBRATE | HDL_ADC_MS5194T_CHANNEL_FULL_SCALE_CALIBRATE
  },
  {
    .config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                  !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                  !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF | MS5194T_CONFIG_REG_CH_SEL(2),
    .options = HDL_ADC_MS5194T_CHANNEL_ENABLE | HDL_ADC_MS5194T_CHANNEL_ZERO_SCALE_CALIBRATE | HDL_ADC_MS5194T_CHANNEL_FULL_SCALE_CALIBRATE    
  },
  {
    .config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                  !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                  !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF | MS5194T_CONFIG_REG_CH_SEL(3),
    .options = HDL_ADC_MS5194T_CHANNEL_ENABLE | HDL_ADC_MS5194T_CHANNEL_ZERO_SCALE_CALIBRATE | HDL_ADC_MS5194T_CHANNEL_FULL_SCALE_CALIBRATE    
  },
  {
    .config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
              !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
              !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF | MS5194T_CONFIG_REG_CH_SEL(4),
    .options = HDL_ADC_MS5194T_CHANNEL_ENABLE | HDL_ADC_MS5194T_CHANNEL_ZERO_SCALE_CALIBRATE | HDL_ADC_MS5194T_CHANNEL_FULL_SCALE_CALIBRATE
  },
  {
    .config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                  !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                  !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF | MS5194T_CONFIG_REG_CH_SEL(5),
    .options = HDL_ADC_MS5194T_CHANNEL_ENABLE | HDL_ADC_MS5194T_CHANNEL_ZERO_SCALE_CALIBRATE | HDL_ADC_MS5194T_CHANNEL_FULL_SCALE_CALIBRATE
  }},
  .adc_io_user = MS5194T_IO_REG_DEFAULT,
  .adc_mode_user = MS5194T_MODE_REG_MS_PWR_DWN | !MS5194T_MODE_REG_PSW | !MS5194T_MODE_REG_AMP_CM | 
                       MS5194T_MODE_REG_CLK_INT64K | !MS5194T_MODE_REG_CHOP_DIS | MS5194T_MODE_REG_FILTER_RATE(5)
};

hdl_adc_ms5194t_config_t mod_adc_cnf = {
  .io_reg = MS5194T_IO_REG_DEFAULT,
  .mode_reg = MS5194T_MODE_REG_MS_PWR_DWN | !MS5194T_MODE_REG_PSW | !MS5194T_MODE_REG_AMP_CM | 
              MS5194T_MODE_REG_CLK_INT64K | !MS5194T_MODE_REG_CHOP_DIS | MS5194T_MODE_REG_FILTER_RATE(5),
  .sources = hdl_adc_ms5194t_sources(&adc_config.src_active[0], &adc_config.src_active[1], &adc_config.src_active[2],
                                     &adc_config.src_active[3], &adc_config.src_active[4], &adc_config.src_active[5])
};

hdl_adc_ms5194t_t mod_adc = {
  .module.init = &hdl_adc_ms5194t,
  .module.dependencies = hdl_module_dependencies(&uspd20k_adc_spi.module, &uspd20k_adc_rdy.module, &mod_timer_ms.module),
  .config = &mod_adc_cnf
};

hdl_module_t mod_app = {
  .dependencies = hdl_module_dependencies(
    &mod_timer_ms.module, 
    
    &uspd20k_i2c_som.module, &uspd20k_som_int.module,

    &uspd20k_i2c_eeprom.module,
    
    &uspd20k_adc_spi.module,
    
    &hdl_uspd_ain_port1.module, &hdl_uspd_ain_port2.module, &hdl_uspd_ain_port3.module, &hdl_uspd_ain_port4.module)
};

app_measure_log_t adc_log[APP_ADC_LOG_SIZE] = { 0 };
