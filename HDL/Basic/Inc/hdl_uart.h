#ifndef HDL_UART_H_
#define HDL_UART_H_

#include "port_uart.h"
#include "hdl_transceiver.h"

/* Initialization */
hdl_module_state_t hdl_uart(void *desc, uint8_t enable);

void hdl_uart_set_transceiver(hdl_uart_t *uart, hdl_transceiver_t *transceiver);

#endif /* HDL_UART_H_ */
