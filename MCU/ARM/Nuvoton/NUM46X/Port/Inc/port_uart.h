#ifndef PORT_UART_H_
#define PORT_UART_H_

/* depends on:
  gpio rx
  gpio tx
  apb2_bus for USART0, apb1_bus for USART1, USART2, USART3, USART4
  interrupt controller (nvic)
  hdl_timer_t
 */

typedef struct {

} hdl_uart_t;

#endif // PORT_UART_H_