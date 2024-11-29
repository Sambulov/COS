#include "app.h"

bldl_uspd_dio_port_t io_port1 = {
  .module.init = &bldl_uspd_dio_port,
  .module.dependencies = hdl_module_dependencies(
    &uspd20k_io1_di.module, 
    &uspd20k_io1_do.module, 
    &uspd20k_io1_di_disable.module
  ),
};
bldl_uspd_dio_port_t io_port2 = {
  .module.init = &bldl_uspd_dio_port,
  .module.dependencies = hdl_module_dependencies(
    &uspd20k_io2_di.module, 
    &uspd20k_io2_do.module, 
    &uspd20k_io2_di_disable.module
  ),
};
bldl_uspd_dio_port_t io_port3 = {
  .module.init = &bldl_uspd_dio_port,
  .module.dependencies = hdl_module_dependencies(
    &uspd20k_io3_di.module, 
    &uspd20k_io3_do.module, 
    &uspd20k_io3_di_disable.module
  ),
};
bldl_uspd_dio_port_t io_port4 = {
  .module.init = &bldl_uspd_dio_port,
  .module.dependencies = hdl_module_dependencies(
    &uspd20k_io4_di.module, 
    &uspd20k_io4_do.module, 
    &uspd20k_io4_di_disable.module
  ),
};
bldl_uspd_dio_port_t io_port5 = {
  .module.init = &bldl_uspd_dio_port,
  .module.dependencies = hdl_module_dependencies(
    &uspd20k_io5_di.module, 
    &uspd20k_io5_do.module, 
    &uspd20k_io5_di_disable.module
  ),
};
bldl_uspd_dio_port_t io_port6 = {
  .module.init = &bldl_uspd_dio_port,
  .module.dependencies = hdl_module_dependencies(
    &uspd20k_io6_di.module, 
    &uspd20k_io6_do.module, 
    &uspd20k_io6_di_disable.module
  ),
};
bldl_uspd_dio_port_t io_port7 = {
  .module.init = &bldl_uspd_dio_port,
  .module.dependencies = hdl_module_dependencies(
    &uspd20k_io7_di.module, 
    &uspd20k_io7_do.module, 
    &uspd20k_io7_di_disable.module
  ),
};
bldl_uspd_dio_port_t io_port8 = {
  .module.init = &bldl_uspd_dio_port,
  .module.dependencies = hdl_module_dependencies(
    &uspd20k_io8_di.module, 
    &uspd20k_io8_do.module, 
    &uspd20k_io8_di_disable.module
  ),
};

hdl_module_t mod_app = {
  .dependencies = hdl_module_dependencies(
    &mod_timer_ms.module, 
    &uspd20k_i2c_som.module, &uspd20k_som_int.module,
    &io_port1.module, &io_port2.module, &io_port3.module, &io_port4.module,
    &io_port5.module, &io_port6.module, &io_port7.module, &io_port8.module
  )
};

app_dio_t app_dio;

const app_mem_map_item_t app_r_mem_map[] = {
  mem_map_item(app_dio, MAP_ADDR_DIO, 0),
};

const app_mem_map_item_t app_w_mem_map[] = {
  mem_map_item(app_dio, MAP_ADDR_DIO, offsetof(app_dio_t, output_state)),
};
