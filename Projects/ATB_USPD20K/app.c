#include "app.h"
#include "CodeLib.h"

hdl_module_t mod_app = {
  .dependencies = hdl_module_dependencies(
    &mod_timer_ms.module, 
    
    &uspd20k_i2c_som.module, &uspd20k_som_int.module,

    &uspd20k_adc_spi.module,

    &uspd20k_i2c_eeprom.module,

    &uspd20k_ai1_ntc_pu.module, &uspd20k_ai1_1k_pd.module, &uspd20k_ai1_cur_scr_low.module, 
    &uspd20k_ai1_cur_scr_high.module, &uspd20k_ai1_150r_pd.module, &uspd20k_ai1_4K3_pd.module,

    &uspd20k_ai2_ntc_pu.module, &uspd20k_ai2_1k_pd.module, &uspd20k_ai2_cur_scr_low.module, 
    &uspd20k_ai2_cur_scr_high.module, &uspd20k_ai2_150r_pd.module, &uspd20k_ai2_4K3_pd.module,

    &uspd20k_ai3_ntc_pu.module, &uspd20k_ai3_1k_pd.module, &uspd20k_ai3_cur_scr_low.module, 
    &uspd20k_ai3_cur_scr_high.module, &uspd20k_ai3_150r_pd.module, &uspd20k_ai3_4K3_pd.module,

    &uspd20k_ai4_ntc_pu.module, &uspd20k_ai4_1k_pd.module, &uspd20k_ai4_cur_scr_low.module, 
    &uspd20k_ai4_cur_scr_high.module, &uspd20k_ai4_150r_pd.module, &uspd20k_ai4_4K3_pd.module),
};

extern hdl_core_t mod_sys_core;

void main() {
  //hdl_enable(&mod_app); 
  hdl_enable(&mod_sys_core.module);
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  while (1) {
    cooperative_scheduler(false);
  }
}
