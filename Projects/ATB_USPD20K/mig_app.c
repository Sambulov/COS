#include "app.h"

const hdl_uspd_ain_port_config_t hdl_uspd_ain_port_cnf_default = {
  .circuit_config = USPD20K_CIRCUIT_CONFIG_FLOATING
};

hdl_uspd_ain_port_t hdl_uspd_ain_port1 = {
  .module.init = &hdl_uspd_ain_port,
  .module.dependencies = hdl_module_dependencies(
    &uspd20k_ai1_ntc_pu.module, &uspd20k_ai1_cur_scr_high.module, &uspd20k_ai1_cur_scr_low.module, 
    &uspd20k_ai1_4K3_pd.module, &uspd20k_ai1_150r_pd.module, &uspd20k_ai1_1k_pd.module),
  .config = &hdl_uspd_ain_port_cnf_default
};

hdl_uspd_ain_port_t hdl_uspd_ain_port2 = {
  .module.init = &hdl_uspd_ain_port,
  .module.dependencies = hdl_module_dependencies(
    &uspd20k_ai2_ntc_pu.module, &uspd20k_ai2_cur_scr_high.module, &uspd20k_ai2_cur_scr_low.module, 
    &uspd20k_ai2_4K3_pd.module, &uspd20k_ai2_150r_pd.module, &uspd20k_ai2_1k_pd.module),
  .config = &hdl_uspd_ain_port_cnf_default
};

hdl_uspd_ain_port_t hdl_uspd_ain_port3 = {
  .module.init = &hdl_uspd_ain_port,
  .module.dependencies = hdl_module_dependencies(
    &uspd20k_ai3_ntc_pu.module, &uspd20k_ai3_cur_scr_high.module, &uspd20k_ai3_cur_scr_low.module, 
    &uspd20k_ai3_4K3_pd.module, &uspd20k_ai3_150r_pd.module, &uspd20k_ai3_1k_pd.module),
  .config = &hdl_uspd_ain_port_cnf_default
};

hdl_uspd_ain_port_t hdl_uspd_ain_port4 = {
  .module.init = &hdl_uspd_ain_port,
  .module.dependencies = hdl_module_dependencies(    
    &uspd20k_ai4_ntc_pu.module, &uspd20k_ai4_cur_scr_high.module, &uspd20k_ai4_cur_scr_low.module, 
    &uspd20k_ai4_4K3_pd.module, &uspd20k_ai4_150r_pd.module, &uspd20k_ai4_1k_pd.module),
  .config = &hdl_uspd_ain_port_cnf_default
};

hdl_module_t mod_app = {
  .dependencies = hdl_module_dependencies(
    &mod_timer_ms.module, 
    
    &uspd20k_i2c_som.module, &uspd20k_som_int.module,

    &uspd20k_adc_spi.module, &uspd20k_adc_rdy.module,

    &uspd20k_i2c_eeprom.module,

    &hdl_uspd_ain_port1.module, &hdl_uspd_ain_port2.module, &hdl_uspd_ain_port3.module, &hdl_uspd_ain_port4.module)
};
