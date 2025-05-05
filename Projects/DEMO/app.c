#include "app.h"
#include "CodeLib.h"

#include "SEGGER_RTT.h"

void main() {

  SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);

  SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n\r\n");
  SEGGER_RTT_WriteString(0, "###### Testing SEGGER_printf() ######\r\n");

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
    static uint32_t log_time = 0;
    static uint32_t loops = 0;
    uint32_t now = hdl_time_counter_get(&mod_timer_ms);
    if((now - log_time) >= 100) {
      SEGGER_RTT_printf(0, RTT_CTRL_TEXT_YELLOW"SEGGER RTT Sample. Loops: %u. Uptime: %ums.\n\r", loops, now);
      loops = 0;
      log_time = now;
    }
    cooperative_scheduler(cl_false);
    loops++;
  }
}
