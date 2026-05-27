#ifndef HDL_MODBUS_H_
#define HDL_MODBUS_H_

#include "CodeLib.h"

typedef modbus_frame_t hdl_modbus_frame_t;
typedef modbus_state_t hdl_modbus_state_t;

typedef uint8_t (*hdl_modbus_request_t)(const void *desc, hdl_modbus_frame_t *frame, uint32_t timeout);
typedef uint8_t (*hdl_modbus_transfer_t)(const void *desc, hdl_modbus_frame_t *pdu);
typedef uint8_t (*hdl_modbus_reset_t)(const void *desc);
typedef hdl_modbus_state_t (*hdl_modbus_get_state_t)(const void *desc);

typedef struct {
  hdl_module_initializer_t init;
  hdl_modbus_request_t request;
  hdl_modbus_transfer_t response;
  hdl_modbus_reset_t reset;
  hdl_modbus_get_state_t state;
  hdl_event_subscribtion_t on_receive;
} hdl_modbus_iface_t;

hdl_module_new_t(hdl_modbus_t, 0, void *, hdl_modbus_iface_t);

__STATIC_INLINE uint8_t hdl_modbus_request(const void *desc, hdl_modbus_frame_t *frame, uint32_t timeout) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_modbus_iface_t *)((hdl_module_base_t *)desc)->iface)->request(desc, frame, timeout);
}

__STATIC_INLINE uint8_t hdl_modbus_response(const void *desc, hdl_modbus_frame_t *frame) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_modbus_iface_t *)((hdl_module_base_t *)desc)->iface)->response(desc, frame);
}

__STATIC_INLINE uint8_t hdl_modbus_reset(const void *desc) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_modbus_iface_t *)((hdl_module_base_t *)desc)->iface)->reset(desc);
}

__STATIC_INLINE hdl_modbus_state_t hdl_modbus_get_state(const void *desc) {
  MODULE_ASSERT(desc, MB_STATE_UNDEF);
  return ((hdl_modbus_iface_t *)((hdl_module_base_t *)desc)->iface)->state(desc);
}

__STATIC_INLINE void hdl_modbus_on_receive(const void *desc, hdl_delegate_t *delegate) {
  MODULE_ASSERT(desc, );
  return ((hdl_modbus_iface_t *)((hdl_module_base_t *)desc)->iface)->on_receive(desc, delegate);
}

#endif /* HDL_MODBUS_H_ */
