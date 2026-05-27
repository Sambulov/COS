#ifndef SERIAL_MODBUS_H_
#define SERIAL_MODBUS_H_

#define HDL_SERIAL_MODBUS_VAR_SIZE      136

typedef struct {
  uint32_t tx_timeout;
  uint32_t rx_timeout;
  void *mb_buffer;
  uint16_t mb_buffer_size;
  modbus_mode_t mode;
  uint8_t server_mode :1;
} hdl_serial_modbus_cnf_t;

#define SERIAL_MODBUS_DEPENDENCY_ID_TIMER       0
#define SERIAL_MODBUS_DEPENDENCY_ID_UART        1

/* depends on
  hdl_timecounter_t
  hdl_uart_t
 */
hdl_module_new_t(hdl_serial_modbus_t, HDL_SERIAL_MODBUS_VAR_SIZE, hdl_serial_modbus_cnf_t *, hdl_modbus_iface_t);

extern const hdl_modbus_iface_t hdl_serial_modbus_iface;

#endif /* SERIAL_MODBUS_H_ */
