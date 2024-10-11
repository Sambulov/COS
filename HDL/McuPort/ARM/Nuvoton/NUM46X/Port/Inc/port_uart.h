#ifndef PORT_UART_H_
#define PORT_UART_H_

#define HDL_UART_PRV_SIZE        0

/* depends on:
  gpio rx
  gpio tx
  apb2_bus for USART0, apb1_bus for USART1, USART2, USART3, USART4
  interrupt controller (nvic)
  hdl_timer_t
 */

typedef struct {

} hdl_uart_config_t;


#endif // PORT_UART_H_