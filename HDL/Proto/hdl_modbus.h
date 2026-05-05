#ifndef HDL_MODBUS_H_
#define HDL_MODBUS_H_

#include "hdl_modbus_types.h"

#define HDL_MODBUS_VAR_SIZE 28

typedef enum {
	/*! The function code received in the query is not an allowable action for the slave.
	  This may be because the function code is only applicable to newer devices, and was
	  not implemented in the unit selected.  It could also indicate that the slave is in
	  the wrong state to process a request of this type, for example because it is
	  unconfigured and is being asked to return register values. If a Poll Program
	  Complete command was issued, this code indicates that no program function preceded
	  it. */
  HDL_MODBUS_ILLEGAL_FUNCTION = 0x01,

	/*! The data address received in the query is not an allowable address for the slave.
	  More specifically, the combination of reference number and transfer length is
	  invalid. For a controller with 100 registers, a request with offset 96 and length 4
	  would succeed, a request with offset 96 and length 5 will generate exception 02. */
  HDL_MODBUS_ILLEGAL_DATA_ADDRESS = 0x02,

	/*! A value contained in the query data field is not an allowable value for the slave.
	  This indicates a fault in the structure of remainder of a complex request, such as
	  that the implied length is incorrect. It specifically does NOT mean that a data item
	  submitted for storage in a register has a value outside the expectation of the
	  application program, since the MODBUS protocol is unaware of the significance of any
	  particular value of any particular register. */
  HDL_MODBUS_ILLEGAL_DATA_VALUE = 0x03,

	/* An unrecoverable error occurred while the slave was attempting to perform the
	  requested action. */
  HDL_MODBUS_SLAVE_DEVICE_FAILURE = 0x04,

	/* Specialized use in conjunction with programming commands.
	  The slave has accepted the request and is processing it, but a long duration of time
	  will be required to do so.  This response is returned to prevent a timeout error
	  from occurring in the master. The master can next issue a Poll Program Complete
	  message to determine if processing is completed. */
  HDL_MODBUS_ACKNOWLEDGE = 0x05,

	/* Specialized use in conjunction with programming commands.
	  The slave is engaged in processing a long-duration program command.  The master
	  should retransmit the message later when the slave is free.. */
  HDL_MODBUS_SLAVE_DEVICE_BUSY = 0x06,

	/*! The slave cannot perform the program function received in the query. This code is
	  returned for an unsuccessful programming request using function code 13 or 14
	  decimal. The master should request diagnostic or error information from the slave. */
  HDL_MODBUS_NEGATIVE_ACKNOWLEDGE = 0x07,

	/*! Specialized use in conjunction with function codes 20 and 21 and reference type 6,
	  to indicate that the extended file area failed to pass a consistency check.
      The slave attempted to read extended memory or record file, but detected a parity
      error in memory. The master can retry the request, but service may be required on
      the slave device. */
  HDL_MODBUS_MEMORY_PARITY_ERROR = 0x08,

	/*! Specialized use in conjunction with gateways, indicates that the gateway was
	  unable to allocate an internal communication path from the input port to the output
	  port for processing the request. Usually means the gateway is misconfigured or
	  overloaded. */
  HDL_MODBUS_GATEWAY_PATH_UNAVAILABLE = 0x0A,

	/*! Specialized use in conjunction with gateways, indicates that no response was
	  obtained from the target device. Usually means that the device is not present on the
	  network. */
  HDL_MODBUS_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND = 0x0B,
} hdl_modbus_exception_code_t;

typedef enum {
  HDL_MODBUS_ROLE_SERVER,
  HDL_MODBUS_ROLE_CLIENT
} hdl_modbus_role_t;

typedef enum {
  HDL_MODBUS_RTU,
  HDL_MODBUS_ASCII,
  HDL_MODBUS_TCP,
  HDL_MODBUS_UDP,
  HDL_MODBUS_RAW_PDU
} hdl_modbus_adu_type_t;

typedef union {
  struct {
    uint8_t func_code;
    uint16_t payload_len;
    uint8_t *payload;
  } pdu;
  struct {
    uint8_t func_code;
    hdl_modbus_exception_code_t exception_code;
  } exception;
} hdl_modbus_pdu_t;

typedef uint8_t (*hdl_modbus_request_t)(const void *desc, hdl_modbus_pdu_t *pdu, hdl_delegate_t *cb_delegate, uint32_t timeout);
typedef uint8_t (*hdl_modbus_transfer_t)(const void *desc, hdl_modbus_pdu_t *pdu);

typedef hdl_modbus_pdu_t hdl_modbus_event_trigger_t;

typedef struct {
  hdl_modbus_role_t role;
  hdl_modbus_adu_type_t type;
  uint32_t request_timeout;
} hdl_modbus_cnf_t;

typedef struct {
  hdl_module_initializer_t init;
  hdl_modbus_request_t request;
  hdl_modbus_transfer_t response;
  hdl_modbus_transfer_t abort;
  hdl_event_subscribtion_t on_receive;
} hdl_modbus_iface_t;

/* depends on
  hdl_transport_t
  hdl_timecounter_t
 */
hdl_module_new_t(hdl_modbus_t, HDL_MODBUS_VAR_SIZE, hdl_modbus_cnf_t *, hdl_modbus_iface_t);

__STATIC_INLINE uint8_t hdl_modbus_request(const void *desc, hdl_modbus_pdu_t *pdu, hdl_delegate_t *cb_delegate, uint32_t timeout) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_modbus_iface_t *)((hdl_module_base_t *)desc)->iface)->request(desc, pdu, cb_delegate, timeout);
}

__STATIC_INLINE uint8_t hdl_modbus_response(const void *desc, hdl_modbus_pdu_t *pdu) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_modbus_iface_t *)((hdl_module_base_t *)desc)->iface)->response(desc, pdu);
}

__STATIC_INLINE uint8_t hdl_modbus_abort(const void *desc, hdl_modbus_pdu_t *pdu) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_modbus_iface_t *)((hdl_module_base_t *)desc)->iface)->abort(desc, pdu);
}

__STATIC_INLINE void hdl_modbus_on_receive(const void *desc, hdl_delegate_t *delegate) {
  MODULE_ASSERT(desc, );
  return ((hdl_modbus_iface_t *)((hdl_module_base_t *)desc)->iface)->on_receive(desc, delegate);
}

#endif /* HDL_MODBUS_H_ */
