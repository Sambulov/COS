#include "hdl_portable.h"
#include "CodeLib.h"

typedef enum {
	NoError      = 0,
	ParityError  = USART_STAT_PERR,
	FrameError   = USART_STAT_FERR,
	NoiseError   = USART_STAT_NERR,
	OverRunError = USART_STAT_ORERR,
	StateMask    = ParityError | FrameError | NoiseError | OverRunError
} UartError_t;

typedef struct {
  hdl_module_t module;
  hdl_nvic_irq_n_t iterrupt;
  uint32_t baudrate;
  hdl_uart_word_len_t word_len;
  hdl_uart_parity_t parity;
  hdl_uart_stop_bits_t stop_bits;
  /* private */
  hdl_delegate_t uart_isr;
  hdl_transceiver_t *transceiver;
  __linked_list_object__;
} hdl_uart_private_t;

_Static_assert(sizeof(hdl_uart_private_t) == sizeof(hdl_uart_t), "In port_uart.h data structure size of hdl_uart_t doesn't match, check UART_PRIVATE_SIZE");

static void _uart_handler(linked_list_item_t *uart_item, void *arg) {
  hdl_uart_private_t *uart = linked_list_get_object(hdl_uart_private_t, uart_item);
  if((uart->transceiver != NULL) && !(USART_CTL0((uint32_t)uart->module.reg) & USART_CTL0_TBEIE)) {
    uint8_t wl = (USART_CTL0((uint32_t)uart->module.reg) & USART_CTL0_WL)? 2: 1;
    if((uart->transceiver->tx_available != NULL) && (uart->transceiver->tx_empty != NULL) && 
       (uart->transceiver->tx_available(uart->transceiver->proto_context) >= wl)) {
      USART_CTL0((uint32_t)uart->module.reg) |= USART_CTL0_TBEIE;
    }
  }
}

static uint8_t _uart_worker(coroutine_desc_t this, uint8_t cancel, void *arg) {
  linked_list_t uarts = (linked_list_t)arg;
  linked_list_do_foreach(uarts, &_uart_handler, NULL);
  return cancel;
}

static void _rst_uart_status(hdl_uart_private_t *uart) {
  USART_INTC((uint32_t)uart->module.reg) |= USART_INTC_PEC | USART_INTC_FEC | USART_INTC_NEC | USART_INTC_OREC;
	__IO uint32_t tmpreg;
	tmpreg = USART_RDATA((uint32_t)uart->module.reg);
	(void)tmpreg;
}

static void event_uart_isr(uint32_t event, void *sender, void *context) {
  hdl_uart_private_t *uart = (hdl_uart_private_t *)context;
  uint32_t state = USART_STAT((uint32_t)uart->module.reg);
  uint32_t cr0 = USART_CTL0((uint32_t)uart->module.reg);
	if (state & USART_STAT_IDLEF) {
		USART_INTC((uint32_t)uart->module.reg) |= USART_INTC_IDLEC;
    if((uart->transceiver != NULL) && (uart->transceiver->end_of_transmission != NULL))
      uart->transceiver->end_of_transmission(uart->transceiver->proto_context);
	}
	if ((state & StateMask) == NoError) {
		/* UART in mode Receiver ---------------------------------------------------*/
		if (state & USART_STAT_RBNE) {
			uint16_t data = USART_RDATA((uint32_t)uart->module.reg);
      USART_CTL0((uint32_t)uart->module.reg) |= USART_CTL0_IDLEIE;
      uint8_t wl = (USART_CTL0((uint32_t)uart->module.reg) & USART_CTL0_WL)? 2: 1;
      if((uart->transceiver != NULL) && (uart->transceiver->rx_available != NULL) && (uart->transceiver->rx_data != NULL)) {
        if(uart->transceiver->rx_available(uart->transceiver->proto_context) >= wl) {          
          uart->transceiver->rx_data(uart->transceiver->proto_context, (uint8_t *)&data, wl);
        }
      }
		}
		///* UART in mode Transmitter ------------------------------------------------*/
		if ((state & USART_STAT_TBE) && (cr0 & USART_CTL0_TBEIE)) {
      USART_CTL0((uint32_t)uart->module.reg) &= ~USART_CTL0_TBEIE;
      uint8_t wl = (USART_CTL0((uint32_t)uart->module.reg) & USART_CTL0_WL)? 2: 1;
      if((uart->transceiver != NULL) && (uart->transceiver->tx_available != NULL) && (uart->transceiver->tx_empty != NULL)) {
        if(uart->transceiver->tx_available(uart->transceiver->proto_context) >= wl) {
          uint16_t data;
          uart->transceiver->tx_empty(uart->transceiver->proto_context, (uint8_t *)&data, wl);
          USART_TDATA((uint32_t)uart->module.reg) = data;
          USART_CTL0((uint32_t)uart->module.reg) |= USART_CTL0_TBEIE;
        }
      }
		}
	}
	else {
		_rst_uart_status(uart);
	}
}

static void _usart_baudrate_set(hdl_uart_private_t *uart) {
  hdl_clock_t *clock = (hdl_clock_t *)uart->module.dependencies[2];
  uint32_t uclk = 0U, intdiv = 0U, fradiv = 0U, udiv = 0U;
  uint32_t baudval = uart->baudrate;
  uint32_t usart_periph = (uint32_t)uart->module.reg;
  if(USART_CTL0(usart_periph) & USART_CTL0_OVSMOD) {
      /* oversampling by 8, configure the value of USART_BAUD */
      udiv = ((2U * clock->freq / clock->div) + baudval / 2U) / baudval;
      intdiv = udiv & 0x0000fff0U;
      fradiv = (udiv>>1U) & 0x00000007U;
      USART_BAUD(usart_periph) = ((USART_BAUD_FRADIV | USART_BAUD_INTDIV) & (intdiv | fradiv));
  }
  else {
      /* oversampling by 16, configure the value of USART_BAUD */
      udiv = (clock->freq / clock->div + baudval / 2U) / baudval;
      intdiv = udiv & 0x0000fff0U;
      fradiv = udiv & 0x0000000fU;
      USART_BAUD(usart_periph) = ((USART_BAUD_FRADIV | USART_BAUD_INTDIV) & (intdiv | fradiv));
  }
}

hdl_module_state_t hdl_uart(void *desc, uint8_t enable) {
  static coroutine_desc_static_t uart_task_buf;
  static linked_list_t uarts = NULL;
  hdl_uart_private_t *uart = (hdl_uart_private_t*)desc;
  rcu_periph_enum rcu;
  switch ((uint32_t)uart->module.reg) {
    case USART0:
    rcu = RCU_USART0;
    break;
    case USART1:
    rcu = RCU_USART1;
    break;
    default:
    return HDL_MODULE_INIT_FAILED;
  }
  usart_deinit((uint32_t)uart->module.reg);

  if(enable) {
    rcu_periph_clock_enable(rcu);
    _usart_baudrate_set(uart);
    usart_word_length_set((uint32_t)uart->module.reg, uart->word_len);
    usart_parity_config((uint32_t)uart->module.reg, uart->parity);
    usart_stop_bit_set((uint32_t)uart->module.reg, uart->stop_bits);
    usart_transmit_config((uint32_t)uart->module.reg, USART_TRANSMIT_ENABLE);
    usart_receive_config((uint32_t)uart->module.reg, USART_RECEIVE_ENABLE);
    USART_CTL0((uint32_t)uart->module.reg) |= USART_CTL0_RBNEIE | USART_CTL0_PERRIE;
    USART_CTL2((uint32_t)uart->module.reg) |= USART_CTL2_ERRIE;
    usart_enable((uint32_t)uart->module.reg);
    linked_list_insert_last(&uarts, linked_list_item(uart));
    coroutine_add_static(&uart_task_buf, &_uart_worker, (void *)uarts);
    uart->transceiver = NULL;
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)uart->module.dependencies[3];
    uart->uart_isr.context = desc;
    uart->uart_isr.handler = &event_uart_isr;
    
    if(hdl_interrupt_request(ic, uart->iterrupt, &uart->uart_isr))
      return HDL_MODULE_INIT_OK;
  }
  linked_list_unlink(linked_list_item(uart));
  rcu_periph_clock_disable(rcu);
  return HDL_MODULE_DEINIT_OK;
}

void hdl_uart_set_transceiver(hdl_uart_t *uart, hdl_transceiver_t *transceiver) {
  if(uart != NULL) {
    ((hdl_uart_private_t*)uart)->transceiver = transceiver;
  }
}
