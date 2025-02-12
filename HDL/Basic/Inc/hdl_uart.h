#ifndef HDL_UART_H_
#define HDL_UART_H_

#include "port_uart.h"

typedef struct {
  hdl_module_initializer_t init;
  hdl_set_transceiver_t transceiver_set;
} hdl_uart_iface_t;

hdl_module_new_t(hdl_uart_t, HDL_UART_VAR_SIZE, hdl_uart_config_t, hdl_uart_iface_t);

extern const hdl_uart_iface_t hdl_uart_iface;

#endif /* HDL_UART_H_ */
