#ifndef HDL_UART_H_
#define HDL_UART_H_

#include "port_uart.h"
#include "hdl_transceiver.h"

/* depends on:
  gpio rx
  gpio tx
  apb2_bus for USART0, apb1_bus for USART1, USART2, USART3, USART4
  interrupt controller (nvic)
  hdl_timer_t
 */
typedef struct {
  hdl_module_t module;
  hdl_uart_config_t *config;
  PRIVATE(hw, HDL_UART_PRV_SIZE);
} hdl_uart_t;

/* Initialization */
hdl_module_state_t hdl_uart(void *desc, uint8_t enable);

void hdl_uart_set_transceiver(hdl_uart_t *uart, hdl_transceiver_t *transceiver);

#endif /* HDL_UART_H_ */
