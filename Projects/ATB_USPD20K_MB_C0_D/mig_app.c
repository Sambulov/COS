#include "app.h"

hdl_module_t mod_app = {
  .dependencies = hdl_module_dependencies(
    &mod_timer_ms.module, 
    &uspd20k_i2c_som.module, &uspd20k_som_int.module)
};

const app_mem_map_item_t app_r_mem_map[] = {
  mem_map_item(mod_app, MAP_ADDR_CIRCUIT_CONFIG, 0),

};

const app_mem_map_item_t app_w_mem_map[] = {
  mem_map_item(mod_app, MAP_ADDR_CIRCUIT_CONFIG, 0),//offsetof(app_circuit_config_t, user)),
};
