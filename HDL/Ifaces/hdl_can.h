#ifndef HDL_CAN_H_
#define HDL_CAN_H_

#define HDL_CAN_ID_MASK                  0x07FFU
#define HDL_CAN_IDE_MASK                 0x1FFFFFFFUL

typedef enum {
  HDL_CAN_MESSAGE_STATUS_INITIAL         = 0x00,
  HDL_CAN_MESSAGE_STATUS_PENDING         = 0x01,
  HDL_CAN_MESSAGE_STATUS_NACK            = 0x02,
  HDL_CAN_MESSAGE_FAULT_ABORT            = 0x10,
  HDL_CAN_MESSAGE_FAULT_ARBITRATION_LOST = 0x20,
  HDL_CAN_MESSAGE_FAULT_XFER_ERROR       = 0x70,
  HDL_CAN_MESSAGE_STATUS_COMPLETE        = 0x80
} hdl_can_message_status_t;

typedef enum {
  HDL_CAN_MESSAGE_IDE               = 0x01, /* Extended ID 29-bits */
  HDL_CAN_MESSAGE_RTR               = 0x02, /* Remote Transmission Request */
  /* TODO: CAN FD OPTIONS */
} hdl_can_message_options_t;

typedef struct {
  uint8_t dlc;         /* Data Length Code 0..8 */
  hdl_can_message_status_t status;
  hdl_can_message_options_t options;
  uint32_t id;
  uint8_t payload[8];
} hdl_can_message_t;

typedef struct {
  hdl_can_message_t base;
  uint8_t payload[56];
} hdl_can_fd_message_t;

typedef struct {
  uint32_t id;
  uint32_t id_mask;
  uint8_t masked   : 1,
          id_ex    : 1,
          rtr      : 1,
          rtr_mask : 1;
} hdl_can_filter_t;

typedef uint8_t (*hdl_can_set_t)(const void *desc, uint32_t qt, uint8_t prop, uint8_t phase1, uint8_t phase2);
typedef uint8_t (*hdl_can_transfer_t)(const void *desc, hdl_can_message_t *message);
typedef void (*hdl_can_subscribe_t)(const void *desc, hdl_delegate_t *delegate);
typedef uint8_t (*hdl_can_set_filter_t)(const void *desc, const hdl_can_filter_t * const *filters);

typedef struct {
  hdl_module_initializer_t init;
  hdl_can_set_t set;
  hdl_can_set_filter_t set_filter;
  hdl_can_subscribe_t subscribe;
  hdl_can_transfer_t transmit;
  hdl_can_transfer_t cancel;
} hdl_can_iface_t;

hdl_module_new_t(hdl_can_t, 0, void*, hdl_can_iface_t);

__STATIC_INLINE uint8_t hdl_can_transmit(const void *desc, hdl_can_message_t *message) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_can_t *)desc)->iface->transmit(desc, message);
}

__STATIC_INLINE uint8_t hdl_can_cancel(const void *desc, hdl_can_message_t *message) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_can_t *)desc)->iface->cancel(desc, message);
}

__STATIC_INLINE void hdl_can_subscribe(const void *desc, hdl_delegate_t *delegate) {
  MODULE_ASSERT(desc, );
  ((hdl_can_t *)desc)->iface->subscribe(desc, delegate);
}

/* filters: null terminated array of hdl_can_filter_t pointers */
__STATIC_INLINE uint8_t hdl_can_filter(const void *desc, const hdl_can_filter_t * const *filters) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_can_t *)desc)->iface->set_filter(desc, filters);
}

/* Bit time
  |Sync|Prop|Phase1|Phase2|
  |<-->|           ^
    1QT       Semple point
  Sample point must be within 50 ~ 90% of the bit time. 87.5% is the preffered by CANOpen & DeviceNet, 75% by ARINC 825
  ((Sync + Prop + Phase1) : Phase2 ~ 87.5 : 12.5)
  qt     : ticks per second
  prop   : Prop stage ticks
  phase1 : Phase1 stage ticks
  phase2 : Phase2 stage ticks
*/
__STATIC_INLINE uint8_t hdl_can_set(const void *desc, uint32_t qt, uint8_t prop, uint8_t phase1, uint8_t phase2) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_can_t *)desc)->iface->set(desc, qt, prop, phase1, phase2);
}

#endif /* HDL_CAN_H_ */
