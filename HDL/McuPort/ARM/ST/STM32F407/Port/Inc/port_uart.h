#ifndef PORT_UART_H_
#define PORT_UART_H_

#include "hdl_uart.h"

#define HDL_UART_VAR_SIZE     56

#define HDL_UART_STOP_BITS1       (0)
#define HDL_UART_STOP_BITS0_5     (USART_CR2_STOP_0)
#define HDL_UART_STOP_BITS2       (USART_CR2_STOP_1)
#define HDL_UART_STOP_BITS1_5     (USART_CR2_STOP_0 | USART_CR2_STOP_1)

#define HDL_UART_PARITY_NONE      (0)
#define HDL_UART_PARITY_ODD       (USART_CR1_PCE | USART_CR1_PS)
#define HDL_UART_PARITY_EVEN      (USART_CR1_PCE)

#define HDL_UART_WORD_8BIT        (0)
#define HDL_UART_WORD_9BIT        (USART_CR1_M)

typedef struct {
  uint32_t phy;               /* 	USART1, USART2, USART3, UART4, UART5, USART6 */
  uint32_t rcu;               /* RCC_APB1ENR_USART2EN .. RCC_APB2ENR_USART1EN */
  hdl_interrupt_t *interrupt;
  uint32_t baudrate;
  uint32_t word_len;          /* HDL_UART_WORD_8BIT, HDL_UART_WORD_9BIT */
  uint32_t parity;            /* HDL_UART_PARITY_NONE, HDL_UART_PARITY_ODD, HDL_UART_PARITY_EVEN */
  uint32_t stop_bits;         /* HDL_UART_STOP_BITS1, HDL_UART_STOP_BITS0_5, HDL_UART_STOP_BITS2, HDL_UART_STOP_BITS1_5 */
} hdl_uart_config_t;

/* depends on:
  gpio rx
  gpio tx
  apb1_bus for USART2/3/4/5, apb2_bus for USART1/6
  interrupt controller (nvic)
  hdl_timer_t
  gpio rts
 */
hdl_module_new_t(hdl_uart_mcu_t, HDL_UART_VAR_SIZE, hdl_uart_config_t, hdl_uart_iface_t);

extern const hdl_uart_iface_t hdl_uart_iface;

#endif // PORT_UART_H_