#include "hdl_iface.h"

typedef struct {
  hdl_delegate_t uart_isr;
  const hdl_transceiver_t *transceiver;
  coroutine_t worker;
} hdl_uart_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_uart_var_t, *((hdl_uart_mcu_t *)0)->obj_var, HDL_UART_VAR_SIZE, port_uart.h);

__STATIC_INLINE uint8_t _hdl_uart_word_len(hdl_uart_mcu_t *uart) {
  return ((uart->config->word_len == USART_WL_9BIT) && (uart->config->parity==USART_PM_NONE))? 2: 1;
}

static uint8_t _uart_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_uart_mcu_t *uart = (hdl_uart_mcu_t *) arg;
  hdl_uart_var_t *uart_var = (hdl_uart_var_t *)uart->obj_var;
  if((uart_var->transceiver != NULL) && !(USART_CTL0((uint32_t)uart->config->phy) & USART_CTL0_TBEIE)) {
    if((uart_var->transceiver->tx_available != NULL) && (uart_var->transceiver->tx_empty != NULL) && 
       (uart_var->transceiver->tx_available(uart_var->transceiver->transmitter_context) >= _hdl_uart_word_len(uart))) {
      usart_interrupt_enable((uint32_t)uart->config->phy, USART_INT_TBE);
    }
  }
  return cancel;
}

static void _rst_uart_status(hdl_uart_mcu_t *uart) {
  __IO uint32_t tmpreg;
  tmpreg = USART_STAT(uart->config->phy);
  tmpreg = USART_DATA(uart->config->phy);
  (void)tmpreg;
}

static void event_uart_isr(uint32_t event, void *sender, void *context) {
  (void)event; (void)sender;
  hdl_uart_mcu_t *uart = (hdl_uart_mcu_t *)context;
  hdl_uart_var_t *uart_var = (hdl_uart_var_t *)uart->obj_var;
  uint32_t periph = (uint32_t)uart->config->phy;
	if (usart_interrupt_flag_get(periph, USART_INT_FLAG_IDLE)) {
    usart_interrupt_flag_clear(periph, USART_INT_FLAG_IDLE);
    if((uart_var->transceiver != NULL) && (uart_var->transceiver->end_of_transmission != NULL))
      uart_var->transceiver->end_of_transmission(uart_var->transceiver->receiver_context);
	}
	if (usart_interrupt_flag_get(periph, USART_INT_FLAG_ERR_NERR) ||
      usart_interrupt_flag_get(periph, USART_INT_FLAG_ERR_FERR) ||
      usart_interrupt_flag_get(periph, USART_INT_FLAG_ERR_ORERR)) {
      _rst_uart_status(uart);
      return;
  }
  /* UART in mode Receiver ---------------------------------------------------*/
  if (usart_interrupt_flag_get(periph, USART_INT_FLAG_RBNE)) {
    uint16_t data = usart_data_receive(periph);
    usart_interrupt_enable(periph, USART_INT_IDLE);
    uint8_t wl = _hdl_uart_word_len(uart);
    if((uart_var->transceiver != NULL) && (uart_var->transceiver->rx_available != NULL) && (uart_var->transceiver->rx_data != NULL)) {
      if(uart_var->transceiver->rx_available(uart_var->transceiver->receiver_context) >= wl) {          
        uart_var->transceiver->rx_data(uart_var->transceiver->receiver_context, (uint8_t *)&data, wl);
      }
    }
  }
  ///* UART in mode Transmitter ------------------------------------------------*/
  if (usart_interrupt_flag_get(periph, USART_INT_FLAG_TBE)) {
    usart_interrupt_disable(periph, USART_INT_TBE);
    uint8_t wl = _hdl_uart_word_len(uart);
    if((uart_var->transceiver != NULL) && (uart_var->transceiver->tx_available != NULL) && (uart_var->transceiver->tx_empty != NULL)) {
      if(uart_var->transceiver->tx_available(uart_var->transceiver->transmitter_context) >= wl) {
        uint16_t data = 0;
        uart_var->transceiver->tx_empty(uart_var->transceiver->transmitter_context, (uint8_t *)&data, wl);
        usart_data_transmit(periph, data);
        USART_CTL0(periph) |= USART_CTL0_TBEIE;
      }
    }
  }
}

static hdl_module_state_t _hdl_uart(const void *desc, uint8_t enable) {
  hdl_uart_mcu_t *uart = (hdl_uart_mcu_t*)desc;
  hdl_uart_var_t *uart_var = (hdl_uart_var_t *)uart->obj_var;
  uint32_t periph = (uint32_t)uart->config->phy;
  usart_deinit(periph);
  if(enable) {
    rcu_periph_clock_enable(uart->config->rcu);
    usart_baudrate_set(periph, uart->config->baudrate);
    usart_word_length_set(periph, uart->config->word_len);
    usart_parity_config(periph, uart->config->parity);
    usart_stop_bit_set(periph, uart->config->stop_bits);
    usart_transmit_config(periph, USART_TRANSMIT_ENABLE);
    usart_receive_config(periph, USART_RECEIVE_ENABLE);
    coroutine_add(&uart_var->worker, &_uart_worker, uart);
    uart_var->transceiver = NULL;
    hdl_interrupt_controller_t *ic = (hdl_interrupt_controller_t *)uart->dependencies[3];
    uart_var->uart_isr.context = uart;
    uart_var->uart_isr.handler = &event_uart_isr;
    hdl_event_subscribe(&uart->config->interrupt->event, &uart_var->uart_isr);
    hdl_interrupt_request(ic, uart->config->interrupt);
    usart_interrupt_enable(periph, USART_INT_PERR);
    usart_interrupt_enable(periph, USART_INT_RBNE);
    usart_interrupt_enable(periph, USART_INT_ERR);
    // USART_CTL0(periph) |= USART_CTL0_RBNEIE | USART_CTL0_PERRIE;
    // USART_CTL2(periph) |= USART_CTL2_ERRIE;
    usart_enable(periph);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&uart_var->worker);
  rcu_periph_clock_disable(uart->config->rcu);
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
