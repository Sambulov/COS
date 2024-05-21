#ifndef PORT_UART_H_
#define PORT_UART_H_


#define UART_PRIVATE_SIZE           20

typedef enum{
  HDL_UART_WORD_LEN_8BIT = USART_WL_8BIT,
  HDL_UART_WORD_LEN_9BIT = USART_WL_9BIT
} hdl_uart_word_len_t;

typedef enum{
  HDL_UART_PARITY_NONE = USART_PM_NONE,
  HDL_UART_PARITY_ODD = USART_PM_ODD,
  HDL_UART_PARITY_EVEN = USART_PM_EVEN,
} hdl_uart_parity_t;

typedef enum{
  HDL_UART_STOP_1BIT = USART_STB_1BIT,
  HDL_UART_STOP_0_5BIT = USART_STB_0_5BIT, /* bit, not available for UARTx(x=3,4) */
  HDL_UART_STOP_2BITS = USART_STB_2BIT,
  HDL_UART_STOP_1_5BITS = USART_STB_1_5BIT, /* bits, not available for UARTx(x=3,4) */
} hdl_uart_stop_bits_t;

/* depends on:
  gpio rx
  gpio tx
  apb2_bus for USART0, apb1_bus for USART1, USART2, USART3, USART4
  interrupt controller (nvic)
  hdl_timer_t
 */

typedef struct {
  hdl_module_t module;
  hdl_nvic_irq_n_t iterrupt;
  uint32_t baudrate;
  hdl_uart_word_len_t word_len;
  hdl_uart_parity_t parity;
  hdl_uart_stop_bits_t stop_bits;
  uint8_t __private[UART_PRIVATE_SIZE];
} hdl_uart_t;

#endif // PORT_UART_H_