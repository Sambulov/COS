#ifndef HDL_CAN_H_
#define HDL_CAN_H_

typedef enum {
  HDL_CAN_MESSAGE_STATUS_INITIAL           = 0x00,
  HDL_CAN_MESSAGE_STATUS_XFER              = 0x01,
  HDL_CAN_MESSAGE_FAULT_NACK_ERROR         = 0x10,
  HDL_CAN_MESSAGE_FAULT_BUS_ERROR          = 0x20,
  HDL_CAN_MESSAGE_STATUS_COMPLETE          = 0x80
} hdl_can_message_status_t;

typedef enum {
 HDL_CAN_MESSAGE_IDE            = 0x01, /* Extended ID 29-bits */
 HDL_CAN_MESSAGE_RTR            = 0x02, /* Remote Transmission Request */
} hdl_can_message_options_t;

typedef struct {
  uint8_t payload[8];
  uint8_t dlc;         /* Data Length Code 0..8 */
  hdl_can_message_status_t status;
  hdl_can_message_options_t options;
} hdl_can_message_t;

typedef uint8_t (* hdl_can_transfer_message_t)(const void *desc, hdl_can_message_t *message);

typedef struct {
  hdl_module_initializer_t init;
  hdl_can_transfer_message_t transfer;
} hdl_can_iface_t;

hdl_module_new_t(hdl_can_t, 0, void, hdl_can_iface_t);

__STATIC_INLINE uint8_t hdl_can_transfer(const void *desc, hdl_can_message_t *message) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_can_t *)desc)->iface->transfer(desc, message);
}

#endif /* HDL_CAN_H_ */