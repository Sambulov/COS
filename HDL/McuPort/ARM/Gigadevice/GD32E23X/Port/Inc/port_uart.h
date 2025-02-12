#ifndef PORT_UART_H_
#define PORT_UART_H_

#include "hdl_uart.h"

#define HDL_UART_VAR_SIZE           52

/* depends on:
  gpio rx
  gpio tx
  apb2_bus for USART0, apb1_bus for USART1, USART2, USART3, USART4
  interrupt controller (nvic)
  hdl_timer_t
 */
typedef struct {
  uint32_t phy;
  rcu_periph_enum rcu;
  hdl_interrupt_t *interrupt;
  uint32_t baudrate;
  uint32_t word_len;          /* USART_WL_8BIT, USART_WL_9BIT */
  uint32_t parity;            /* USART_PM_NONE, USART_PM_ODD, USART_PM_EVEN */
  uint32_t stop_bits;         /* USART_STB_1BIT, USART_STB_0_5BIT, USART_STB_2BIT, USART_STB_1_5BIT */
} hdl_uart_config_t;

hdl_module_new_t(hdl_uart_t, HDL_UART_VAR_SIZE, hdl_uart_config_t, hdl_uart_iface_t);

extern const hdl_uart_iface_t hdl_uart_iface;

#endif // PORT_UART_H_