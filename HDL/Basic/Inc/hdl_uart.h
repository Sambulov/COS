#ifndef HDL_UART_H_
#define HDL_UART_H_

typedef struct {
  hdl_module_initializer_t init;
  hdl_set_transceiver_t transceiver_set;
} hdl_uart_iface_t;

hdl_module_new_t(hdl_uart_t, 0, void, hdl_uart_iface_t);

__STATIC_INLINE uint8_t hdl_uart_set_transceiver(const void *desc, const hdl_transceiver_t *transceiver, uint32_t channel_id) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_uart_t *)desc)->iface->transceiver_set(desc, transceiver, channel_id);
}

#endif /* HDL_UART_H_ */
