#include "app.h"

const hdl_module_base_t mod_app = {
  .dependencies = hdl_module_dependencies(
    &mod_ic,
    &mod_tick_counter,
    &mod_timer_ms,
    &mod_output1_pin,
    &mod_output2_pin,
    &mod_led1_pin,
    &mod_led2_pin,
    &mod_led3_pin,
    &mod_led4_pin,
    &mod_uart,
    &mod_rs485,
    &mod_spi_client
  ),
  .mod_var = static_malloc(HDL_MODULE_VAR_SIZE)
};
