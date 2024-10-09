#ifndef PORT_UART_H_
#define PORT_UART_H_

#define HDL_UART_PRV_SIZE           56

typedef struct {
  rcu_periph_enum rcu;
  hdl_nvic_irq_n_t interrupt;
  uint32_t baudrate;
  uint32_t word_len;          /* USART_WL_8BIT, USART_WL_9BIT */
  uint32_t parity;            /* USART_PM_NONE, USART_PM_ODD, USART_PM_EVEN */
  uint32_t stop_bits;         /* USART_STB_1BIT, USART_STB_0_5BIT, USART_STB_2BIT, USART_STB_1_5BIT */
} hdl_uart_config_t;

#endif // PORT_UART_H_