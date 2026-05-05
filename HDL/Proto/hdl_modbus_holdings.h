#ifndef HDL_MODBUS_HOLDING_REGS_H_
#define HDL_MODBUS_HOLDING_REGS_H_

#include "hdl_modbus_types.h"

#define HDL_MODBUS_VAR_SIZE 28

typedef struct {
  uint16_t regs_addr;
  uint16_t regs_amount;
} hdl_modbus_holding_reg_request_t;

typedef struct {
  uint8_t data_length;
  uint16_t regs[];
} hdl_modbus_holding_reg_responce_t;

// typedef struct {
//   hdl_module_initializer_t init;
//   hdl_modbus_request_t request;
//   hdl_event_subscribtion_t on_receive;
// } hdl_modbus_iface_t;

// /* depends on
//   hdl_transport_t
//   hdl_timecounter_t
//  */
// hdl_module_new_t(hdl_modbus_t, HDL_MODBUS_VAR_SIZE, hdl_modbus_cnf_t *, hdl_modbus_iface_t);

// __STATIC_INLINE uint8_t hdl_modbus_request(const void *desc, hdl_modbus_pdu_t *pdu, hdl_delegate_t *cb_delegate, uint32_t timeout) {
//   MODULE_ASSERT(desc, HDL_FALSE);
//   return ((hdl_modbus_iface_t *)((hdl_module_base_t *)desc)->iface)->request(desc, pdu, cb_delegate, timeout);
// }

// __STATIC_INLINE uint8_t hdl_modbus_response(const void *desc, hdl_modbus_pdu_t *pdu) {
//   MODULE_ASSERT(desc, HDL_FALSE);
//   return ((hdl_modbus_iface_t *)((hdl_module_base_t *)desc)->iface)->response(desc, pdu);
// }

// __STATIC_INLINE uint8_t hdl_modbus_abort(const void *desc, hdl_modbus_pdu_t *pdu) {
//   MODULE_ASSERT(desc, HDL_FALSE);
//   return ((hdl_modbus_iface_t *)((hdl_module_base_t *)desc)->iface)->abort(desc, pdu);
// }

// __STATIC_INLINE void hdl_modbus_on_receive(const void *desc, hdl_delegate_t *delegate) {
//   MODULE_ASSERT(desc, );
//   return ((hdl_modbus_iface_t *)((hdl_module_base_t *)desc)->iface)->on_receive(desc, delegate);
// }

#endif /* HDL_MODBUS_HOLDING_REGS_H_ */
