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
    .options = !HDL_ADC_MS5194T_CHANNEL_ENABLE    
  },
  {
    .config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
              !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
              !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF | MS5194T_CONFIG_REG_CH_SEL(4),
    .options = !HDL_ADC_MS5194T_CHANNEL_ENABLE
  },
  {
    .config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                  !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                  !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF | MS5194T_CONFIG_REG_CH_SEL(5),
    .options = HDL_ADC_MS5194T_CHANNEL_ENABLE | HDL_ADC_MS5194T_CHANNEL_ZERO_SCALE_CALIBRATE | HDL_ADC_MS5194T_CHANNEL_FULL_SCALE_CALIBRATE
  }},
  .adc_io_user = MS5194T_IO_REG_DEFAULT,
  .adc_mode_user = MS5194T_MODE_REG_MS_PWR_DWN | !MS5194T_MODE_REG_PSW | !MS5194T_MODE_REG_AMP_CM | 
                       MS5194T_MODE_REG_CLK_INT64K | !MS5194T_MODE_REG_CHOP_DIS | MS5194T_MODE_REG_FILTER_RATE(3)
};

hdl_adc_ms5194t_config_t mod_adc_cnf = {
  // NOTE: dynamic config takes from adc_config
  .sources = hdl_adc_ms5194t_sources(&adc_config.src_active[0], &adc_config.src_active[1], &adc_config.src_active[2],
                                     &adc_config.src_active[3], &adc_config.src_active[4], &adc_config.src_active[5])
};

hdl_adc_ms5194t_t mod_adc = {
  .module.init = &hdl_adc_ms5194t,
  .module.dependencies = hdl_module_dependencies(&uspd20k_adc_spi.module, &uspd20k_adc_rdy.module, &mod_timer_ms.module),
  .config = &mod_adc_cnf
};

const hdl_eeprom_i2c_config_t mod_eeprom_cnf = {
  .block_size = 128,
  .chip_address = 0x50,
  .size = 65536,
  .write_time = 5
};

hdl_eeprom_i2c_t mod_eeprom = {
  .module.init = hdl_eeprom_i2c,
  .module.dependencies = hdl_module_dependencies(&uspd20k_i2c_eeprom.module, &mod_timer_ms.module),
  .config = &mod_eeprom_cnf
};

hdl_module_t mod_app = {
  .dependencies = hdl_module_dependencies(
    &mod_timer_ms.module, 
    
    &uspd20k_i2c_som.module, &uspd20k_som_int.module,

    &mod_eeprom.module,
    
    &uspd20k_adc_spi.module,
    
    &hdl_uspd_ain_port1.module, &hdl_uspd_ain_port2.module, &hdl_uspd_ain_port3.module, &hdl_uspd_ain_port4.module)
};

app_adc_measure_t adc_log[APP_ADC_LOG_SIZE] = { 0 };

const app_adc_config_preset_t app_adc_off_preset = {
  .ch_config.config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                          !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                          !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF, // | MS5194T_CONFIG_REG_CH_SEL(),
  .ch_config.options = !HDL_ADC_MS5194T_CHANNEL_ENABLE,
  .circuit_config = USPD20K_CIRCUIT_CONFIG_4K3_PD
};

const app_adc_config_preset_t app_adc_rtd_preset = {
  .ch_config.config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                          !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                          !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF, // | MS5194T_CONFIG_REG_CH_SEL(),
  .ch_config.options = HDL_ADC_MS5194T_CHANNEL_ENABLE | HDL_ADC_MS5194T_CHANNEL_ZERO_SCALE_CALIBRATE | HDL_ADC_MS5194T_CHANNEL_FULL_SCALE_CALIBRATE,
  .circuit_config = USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW
};

const app_adc_config_preset_t app_adc_cur_preset = {
  .ch_config.config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                          !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                          !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF, // | MS5194T_CONFIG_REG_CH_SEL(),
  .ch_config.options = HDL_ADC_MS5194T_CHANNEL_ENABLE | HDL_ADC_MS5194T_CHANNEL_ZERO_SCALE_CALIBRATE | HDL_ADC_MS5194T_CHANNEL_FULL_SCALE_CALIBRATE,
  .circuit_config = USPD20K_CIRCUIT_CONFIG_150R_PD
};

const app_adc_config_preset_t app_adc_vlt_preset = {
  .ch_config.config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                          !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                          !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF, // | MS5194T_CONFIG_REG_CH_SEL(),
  .ch_config.options = HDL_ADC_MS5194T_CHANNEL_ENABLE | HDL_ADC_MS5194T_CHANNEL_ZERO_SCALE_CALIBRATE | HDL_ADC_MS5194T_CHANNEL_FULL_SCALE_CALIBRATE,
  .circuit_config = USPD20K_CIRCUIT_CONFIG_4K3_PD
};

const app_adc_config_preset_t app_adc_ntc_preset = {
  .ch_config.config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                          !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                          !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF, // | MS5194T_CONFIG_REG_CH_SEL(),
  .ch_config.options = HDL_ADC_MS5194T_CHANNEL_ENABLE | HDL_ADC_MS5194T_CHANNEL_ZERO_SCALE_CALIBRATE | HDL_ADC_MS5194T_CHANNEL_FULL_SCALE_CALIBRATE,
  .circuit_config = USPD20K_CIRCUIT_CONFIG_NTC_PU
};

const app_adc_config_preset_t app_adc_floating_preset = {
  .ch_config.config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                          !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                          !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF, // | MS5194T_CONFIG_REG_CH_SEL(),
  .ch_config.options = HDL_ADC_MS5194T_CHANNEL_ENABLE | HDL_ADC_MS5194T_CHANNEL_ZERO_SCALE_CALIBRATE | HDL_ADC_MS5194T_CHANNEL_FULL_SCALE_CALIBRATE,
  .circuit_config = USPD20K_CIRCUIT_CONFIG_FLOATING
};

const app_adc_config_preset_t app_adc_ain_calib_preset = {
  .ch_config.config_reg = MS5194T_CONFIG_REG_VBIAS_DISBL | !MS5194T_CONFIG_REG_BO | MS5194T_CONFIG_REG_UB | 
                          !MS5194T_CONFIG_REG_BOOST | MS5194T_CONFIG_REG_IN_AMP(0) | MS5194T_CONFIG_REG_REFSEL_EXTIN1 |
                          !MS5194T_CONFIG_REG_REF_DET | MS5194T_CONFIG_REG_BUF, // | MS5194T_CONFIG_REG_CH_SEL(),
  .ch_config.options = HDL_ADC_MS5194T_CHANNEL_ENABLE | HDL_ADC_MS5194T_CHANNEL_ZERO_SCALE_CALIBRATE | HDL_ADC_MS5194T_CHANNEL_FULL_SCALE_CALIBRATE,
  .circuit_config = USPD20K_CIRCUIT_CONFIG_1K_PD | USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW
};

const app_adc_config_preset_t *app_adc_presets[] = {&app_adc_off_preset, &app_adc_rtd_preset, &app_adc_cur_preset, &app_adc_vlt_preset, &app_adc_ntc_preset, &app_adc_floating_preset, &app_adc_ain_calib_preset};

app_adc_preset_config_t app_adc_preset_config = {0};

// -50 ~ 50 C
const uint32_t pt1000_arr[] = {
/* -50 */
   8030628,  8070334,  8110026,  8149704,  8189368,  8229018,  8268654,  8308277,  8347887,  8387483, 
   8427065,  8466634,  8506190,  8545733,  8585262,  8624779,  8664282,  8703772,  8743249,  8743249,
   8822166,  8861605,  8901031,  8940444,  8979845,  9019234,  9058610,  9097973,  9137325,  9176663,
   9215990,  9255304,  9294606,  9333896,  9373174,  9412439,  9451693,  9490935,  9530164,  9569382,
   9608588,  9647782,  9686964,  9726134,  9765293,  9804440,  9843575,  9882699,  9921811,  9960911,
/* 0 */
  10000000, 10039077, 10078143, 10117197, 10156240, 10195271, 10234290, 10273298, 10312294, 10351279,
  10390252, 10429214, 10468164, 10507103, 10546030, 10584946, 10623850, 10662742, 10701623, 10740492,
  10779350, 10818196, 10857031, 10895854, 10934666, 10973466, 11012254, 11012254, 11089796, 11089796, 
  11167292, 11206023, 11244742, 11283450, 11322146, 11360831, 11399504, 11438165, 11476815, 11515453,
  11554080, 11592695, 11631299, 11669891, 11708472, 11747041, 11785598, 11824144, 11862678, 11901201,
/* 50 */
  11939713, 11978212, 12016700, 12055177, 12093642, 12132096, 12170538, 12208968, 12247387, 12285794

};
