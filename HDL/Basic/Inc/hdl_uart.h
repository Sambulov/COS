#ifndef HDL_UART_H_
#define HDL_UART_H_

typedef enum {
  HDL_UART_WORD_7BIT = 0,
  HDL_UART_WORD_8BIT = 1,
  HDL_UART_WORD_9BIT = 2
} hdl_uart_word_t;

typedef enum {
  HDL_UART_PARITY_NONE = 0,
  HDL_UART_PARITY_ODD  = 1,
  HDL_UART_PARITY_EVEN = 2
} hdl_uart_parity_t;

typedef enum {
  HDL_UART_STOP_BITS1      = 0,
  HDL_UART_STOP_BITS2      = 1,
  HDL_UART_STOP_BITS0_5    = 2,
  HDL_UART_STOP_BITS1_5    = 3
} hdl_uart_stop_bits_t;

typedef uint8_t (*hdl_uart_set_t)(const void *, hdl_uart_word_t, uint32_t, hdl_uart_parity_t, hdl_uart_stop_bits_t);

typedef struct {
  hdl_module_initializer_t init;
  hdl_set_transceiver_t transceiver_set;
  hdl_uart_set_t set;
} hdl_uart_iface_t;

hdl_module_new_t(hdl_uart_t, 0, void, hdl_uart_iface_t);

__STATIC_INLINE uint8_t hdl_uart_set_transceiver(const void *desc, const hdl_transceiver_t *transceiver, uint32_t channel_id) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_uart_t *)desc)->iface->transceiver_set(desc, transceiver, channel_id);
}

__STATIC_INLINE uint8_t hdl_uart_set(const void *desc, hdl_uart_word_t bits, uint32_t boud, hdl_uart_parity_t parity, hdl_uart_stop_bits_t stop) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_uart_t *)desc)->iface->set(desc, bits, boud, parity, stop);
}

#endif /* HDL_UART_H_ */
