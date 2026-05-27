#ifndef TRANSPORT_MODBUS_H_
#define TRANSPORT_MODBUS_H_

#define HDL_TRANSPORT_MODBUS_VAR_SIZE      112

typedef struct {
  uint32_t tx_timeout;
  uint32_t rx_timeout;
  void *mb_buffer;
  uint16_t mb_buffer_size;
  modbus_mode_t mode;
  uint8_t server_mode :1;
} hdl_transport_modbus_cnf_t;

#define TRANSPORT_MODBUS_DEPENDENCY_ID_TIMER       0
#define TRANSPORT_MODBUS_DEPENDENCY_ID_TRANSPORT   1

/* depends on
  hdl_timecounter_t
  hdl_transport_t
 */
hdl_module_new_t(hdl_transport_modbus_t, HDL_TRANSPORT_MODBUS_VAR_SIZE, hdl_transport_modbus_cnf_t *, hdl_modbus_iface_t);

extern const hdl_modbus_iface_t hdl_transport_modbus_iface;

#endif /* TRANSPORT_MODBUS_H_ */
