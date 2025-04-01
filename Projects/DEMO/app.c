#include "app.h"
#include "CodeLib.h"

void main() {
  hdl_enable(&mod_app);

  while (!hdl_init_complete()) {
    cooperative_scheduler(cl_false);
  }
  gpio_test();
  spi_client_test();

  //stream_test(uart_stream_init());
  modbus_client_test(app_modbus_init(uart_stream_init()));
  //modbus_server_test(app_modbus_init(uart_stream_init()));

  while (1) {
    cooperative_scheduler(cl_false);
  }
}
