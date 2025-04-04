#include "hdl_iface.h"

typedef struct {
  hdl_delegate_t uart_isr;
  const hdl_transceiver_t *transceiver;
  coroutine_t worker;
  uint8_t tx_data[2];
  uint8_t rx_byte  : 1;
  uint8_t tx_byte  : 1;
} hdl_uart_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_uart_var_t, *((hdl_uart_mcu_t *)0)->obj_var, HDL_UART_VAR_SIZE, port_uart.h);

static inline uint8_t _uart_word_len(USART_TypeDef *periph) {
  return (periph->CR1 & (USART_CR1_M | USART_CR1_PCE)) == (USART_CR1_M)? 2: 1;
}

static uint8_t _uart_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_uart_mcu_t *uart = (hdl_uart_mcu_t *) arg;
  hdl_uart_var_t *uart_var = (hdl_uart_var_t *)uart->obj_var;
  hdl_gpio_pin_t *rts = (hdl_gpio_pin_t *)uart->dependencies[5];
  USART_TypeDef *periph = (USART_TypeDef *)uart->config->phy;
  if((uart_var->transceiver != NULL) && !(periph->CR1 & USART_CR1_TXEIE) &&
     (uart_var->transceiver->tx_available != NULL) && (uart_var->transceiver->tx_empty != NULL) &&
     (uart_var->transceiver->tx_available(uart_var->transceiver->transmitter_context) > 0)) {
    hdl_gpio_set_active(rts);
    periph->CR1 |= USART_CR1_TXEIE;
  }
  return cancel;
}

static void _rst_uart_status(USART_TypeDef *uart) {
	__IO uint32_t tmpreg;
	tmpreg = uart->SR;
	tmpreg = uart->DR;
	(void)tmpreg;
}

static void event_uart_isr(uint32_t event, void *sender, void *context) {
  (void)event; (void)sender;
  hdl_uart_mcu_t *uart = (hdl_uart_mcu_t *)context;
  hdl_uart_var_t *uart_var = (hdl_uart_var_t *)uart->obj_var;
  USART_TypeDef *periph = (USART_TypeDef *)uart->config->phy;
  uint8_t wl = _uart_word_len(periph);
  
	if (periph->SR & USART_SR_IDLE) {
    //(void)periph->DR; /* dummy read */
    _rst_uart_status(periph);
    if((uart_var->transceiver != NULL) && (uart_var->transceiver->end_of_transmission != NULL))
        uart_var->transceiver->end_of_transmission(uart_var->transceiver->receiver_context);
	}
	if (periph->SR & (USART_SR_NE | USART_SR_FE | USART_SR_ORE | USART_SR_PE)) {
    _rst_uart_status(periph);
    return;
  }
  /* UART in mode Receiver ---------------------------------------------------*/
  if (periph->SR & USART_SR_RXNE) {
    uint16_t data = periph->DR;
    if((uart_var->transceiver != NULL) && (uart_var->transceiver->rx_data != NULL))
      uart_var->transceiver->rx_data(uart_var->transceiver->receiver_context, (uint8_t *)&data, wl);
  }
  ///* UART in mode Transmitter ------------------------------------------------*/
  if ((periph->CR1 & USART_CR1_TXEIE) && (periph->SR & USART_SR_TXE)) {
    periph->CR1 &= ~USART_CR1_TXEIE;
    int32_t len = uart_var->transceiver->tx_empty(uart_var->transceiver->transmitter_context, 
      &uart_var->tx_data[uart_var->tx_byte], wl - uart_var->tx_byte);
    if(len > 0) {
      if((len + uart_var->tx_byte) == wl) {
        //if(wl == 1) uart_var->tx_data[1] = 0;
        periph->DR = *((uint16_t *)uart_var->tx_data);
        uart_var->tx_byte = 0;
        periph->CR1 |= USART_CR1_TXEIE;
      }
      else uart_var->tx_byte = len;
    }
    else periph->CR1 |= USART_CR1_TCIE;
  }
  if((periph->CR1 & USART_CR1_TCIE) && (periph->SR & USART_SR_TC)) {
    CL_REG_CLEAR(periph->CR1, USART_CR1_TCIE);
    CL_REG_CLEAR(periph->SR, USART_SR_TC);
    hdl_gpio_set_inactive(uart->dependencies[5]);
  }

}

static hdl_module_state_t _hdl_uart(const void *desc, uint8_t enable) {
  hdl_uart_mcu_t *uart = (hdl_uart_mcu_t*)desc;
  hdl_uart_var_t *uart_var = (hdl_uart_var_t *)uart->obj_var;
  //hdl_time_counter_t *timer = (hdl_time_counter_t *)uart->dependencies[4];

  USART_TypeDef *periph = (USART_TypeDef *)uart->config->phy;
  volatile uint32_t *rcc_en = &RCC->APB1ENR;
  volatile uint32_t *rcc_rst = &RCC->APB1RSTR;
  
  switch (uart->config->phy) {
    case (uint32_t)USART1:
    case (uint32_t)USART6:
      rcc_en = &RCC->APB2ENR;
      rcc_rst = &RCC->APB2RSTR;
      break;
    case (uint32_t)USART2:
    case (uint32_t)USART3:
    case (uint32_t)UART4:
    case (uint32_t)UART5:
      break;
    default:
      return HDL_MODULE_FAULT;
  }
  CL_REG_SET(*rcc_rst, uart->config->rcu);
  CL_REG_CLEAR(*rcc_rst, uart->config->rcu);
  if(enable) {
    CL_REG_SET(*rcc_en, uart->config->rcu);
    hdl_clock_t *clk = (hdl_clock_t *)uart->dependencies[2];
    hdl_clock_freq_t freq;
    hdl_clock_get(clk, &freq);
    periph->CR1 = uart->config->parity | uart->config->word_len | USART_CR1_TE | USART_CR1_RE;
    periph->CR1 |= USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_IDLEIE;
    periph->CR2 = uart->config->stop_bits;
    periph->CR3 = USART_CR3_EIE;
    periph->BRR = freq.num / (freq.denom * uart->config->baudrate);
    coroutine_add(&uart_var->worker, &_uart_worker, uart);
    uart_var->transceiver = NULL;
    uart_var->tx_byte = 0;
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)uart->dependencies[3];
    uart_var->uart_isr.context = uart;
    uart_var->uart_isr.handler = &event_uart_isr;
    hdl_event_subscribe(&uart->config->interrupt->event, &uart_var->uart_isr);
    hdl_interrupt_request(ic, uart->config->interrupt);
    periph->CR1 |= USART_CR1_UE;
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&uart_var->worker);
  CL_REG_CLEAR(*rcc_en, uart->config->rcu);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_uart_set_transceiver(const void *desc, const hdl_transceiver_t *transceiver, uint32_t channel_id) {
  (void)channel_id;
  if(desc != NULL) {
    hdl_uart_mcu_t *uart = (hdl_uart_mcu_t *) desc;
    hdl_uart_var_t *uart_var = (hdl_uart_var_t *)uart->obj_var;  
    uart_var->transceiver = transceiver;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

const hdl_uart_iface_t hdl_uart_iface = {
  .init = &_hdl_uart,
  .transceiver_set = &_hdl_uart_set_transceiver
};
