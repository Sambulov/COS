#ifndef HDL_UART_H_
#define HDL_UART_H_

#include "port_uart.h"

typedef struct {
  hdl_module_t module;
  hdl_uart_config_t *config;
  PRIVATE(hw, HDL_UART_PRV_SIZE);
} hdl_uart_t;

/* Initialization */
hdl_module_state_t hdl_uart(void *desc, uint8_t enable);

void hdl_uart_set_transceiver(hdl_uart_t *uart, hdl_transceiver_t *transceiver);

#endif /* HDL_UART_H_ */
