/*!
   Modbus.h

    Created on: 25.04.2023
        Author: Sambulov Dmitry
        e-mail: dmitry.sambulov@gmail.com
 */

#ifndef MODBUS_H_INCLUDED
#define MODBUS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define MODBUS_SERVER_RX_TIMEOUT            1000
#define MODBUS_CLIENT_RX_TIMEOUT            200
#define MODBUS_CLIENT_TX_TIMEOUT            200

#define MODBUS_DESCRIPTOR_SIZE              52

#define MODBUS_RESPONSE_ERROR_FLAG_MASK     0x80

typedef uint8_t (*ReadDiscreteHandler_t)(uint16_t usAddr);
typedef uint16_t (*ReadRegisterHandler_t)(uint16_t usAddr);
typedef void (*WriteRegisterHandler_t)(uint16_t usAddr, uint16_t usValue);
typedef uint8_t (*AssertHandler_t)(uint16_t usAddr, uint16_t pusLengthValue);

typedef enum {
	ModbusOk = 0x00,
	/*! The function code received in the query is not an allowable action for the slave.
	  This may be because the function code is only applicable to newer devices, and was
	  not implemented in the unit selected.  It could also indicate that the slave is in
	  the wrong state to process a request of this type, for example because it is
	  unconfigured and is being asked to return register values. If a Poll Program
	  Complete command was issued, this code indicates that no program function preceded
	  it. */
	ModbusIllegalFunction = 0x01,

	/*! The data address received in the query is not an allowable address for the slave.
	  More specifically, the combination of reference number and transfer length is
	  invalid. For a controller with 100 registers, a request with offset 96 and length 4
	  would succeed, a request with offset 96 and length 5 will generate exception 02. */
	ModbusIllegalDataAddress = 0x02,

	/*! A value contained in the query data field is not an allowable value for the slave.
	  This indicates a fault in the structure of remainder of a complex request, such as
	  that the implied length is incorrect. It specifically does NOT mean that a data item
	  submitted for storage in a register has a value outside the expectation of the
	  application program, since the MODBUS protocol is unaware of the significance of any
	  particular value of any particular register. */
	ModbusIllegalDataValue = 0x03,

	/* An unrecoverable error occurred while the slave was attempting to perform the
	  requested action. */
	ModbusSlaveDeviceFailure = 0x04,

	/* Specialized use in conjunction with programming commands.
	  The slave has accepted the request and is processing it, but a long duration of time
	  will be required to do so.  This response is returned to prevent a timeout error
	  from occurring in the master. The master can next issue a Poll Program Complete
	  message to determine if processing is completed. */
	ModbusAcknowledge = 0x05,

	/* Specialized use in conjunction with programming commands.
	  The slave is engaged in processing a long-duration program command.  The master
	  should retransmit the message later when the slave is free.. */
	ModbusSlaveDeviceBusy = 0x06,

	/*! The slave cannot perform the program function received in the query. This code is
	  returned for an unsuccessful programming request using function code 13 or 14
	  decimal. The master should request diagnostic or error information from the slave. */
	ModbusNegativeAcknowledge = 0x07,

	/*! Specialized use in conjunction with function codes 20 and 21 and reference type 6,
	  to indicate that the extended file area failed to pass a consistency check.
      The slave attempted to read extended memory or record file, but detected a parity
      error in memory. The master can retry the request, but service may be required on
      the slave device. */
	ModbusMemoryParityError = 0x08,

	/*! Specialized use in conjunction with gateways, indicates that the gateway was
	  unable to allocate an internal communication path from the input port to the output
	  port for processing the request. Usually means the gateway is misconfigured or
	  overloaded. */
	ModbusGatewayPathUnavailable = 0x0A,

	/*! Specialized use in conjunction with gateways, indicates that no response was
	  obtained from the target device. Usually means that the device is not present on the
	  network. */
	ModbusGatewayTargetDeviceFailedtoRespond = 0x0B,
} ModbusResult_t;

typedef struct {
	uint8_t ucAddr;
	uint8_t ucFunc;
	uint16_t usRegAddr;
	uint16_t usRegValueCount;
	uint8_t ucLengthError;
	uint8_t ucDataLength;
	uint8_t *pucData;
	uint16_t _usCrc;
} ModbusFrame_t;

/* eRxPacketType/eTxPacketType definitions. */
typedef enum {
    eModbusPacketNone        = 0,    /* Undefined */
    eModbusPacketBase        = 1,    /* [ADDR][FUNC][MSB_REG_ADDR][LSB_REG_ADDR][MSB_COUNT][LSB_COUNT][MSB_CRC][LSB_CRC] */
    eModbusPacketVariableLen = 2,    /* [ADDR][FUNC][LENGTH][DATA...][MSB_CRC][LSB_CRC] */
    eModbusPacketFull        = 3,    /* [ADDR][FUNC][MSB_REG_ADDR][LSB_REG_ADDR][MSB_COUNT][LSB_COUNT][LENGTH][DATA...][MSB_CRC][LSB_CRC] */
    eModbusPacketError       = 4,    /* [ADDR|0x80][FUNC][ERR][MSB_CRC][LSB_CRC] */
    eModbusPacketTypesAmount
} ModbusPacketType_t;

typedef struct MBRH_t ModbusRequest_t;

typedef ModbusResult_t (*ModbusCb_t)(ModbusRequest_t *pxRreq, ModbusFrame_t *pxResp);

struct MBRH_t {
	ModbusPacketType_t eRxType;
	ModbusPacketType_t eTxType;
	void *pxContext;
	ModbusCb_t pfOnComplete;
	ModbusFrame_t xFrame;
};

typedef struct {
	ModbusPacketType_t eRxType;
	ModbusPacketType_t eTxType;
	void *pxContext;
    ModbusCb_t pfOnReceive;
	uint8_t ucFunctionNo;
} ModbusHandler_t;

typedef struct MBSEP_t ModbusEndpoint_t;

struct MBSEP_t {
	ModbusHandler_t **paxHandlers;
	uint8_t ucHandlersCount;
	uint8_t ucAddress;
	uint8_t bBroadCast :1;
	ModbusEndpoint_t *pxNext;
};

typedef struct {
	uint8_t dummy[MODBUS_DESCRIPTOR_SIZE];
} Modbus_t;

typedef struct {
	int32_t (*lAvailableToWrite)(void *pxPhy);
	int32_t (*lAvailableToRead)(void *pxPhy);
	int32_t (*lRead)(void *pxPhy, uint8_t *pucBuf, uint32_t ulSize);
	int32_t (*lWrite)(void *pxPhy, uint8_t *pucBuf, uint32_t ulSize);
	void (*vFifoFlush)(void *pxPhy);
	uint32_t (*ulTimerMs)(void *pxTimerPhy);
	void *pxTxPhy;
	void *pxRxPhy;
	void *pxTimerPhy;
} ModbusIface_t;

typedef struct {
	ModbusIface_t *pxIface;
	ModbusEndpoint_t *pxMbEp;
	uint8_t *pucPayLoadBuffer;
	uint8_t ucPayLoadBufferSize;
	uint8_t bAsciiMode :1,
	        bIsServer  :1;
} ModbusConfig_t;

uint8_t bModbusInit(Modbus_t *pxMb, ModbusConfig_t *pxConfig);
void vModbusWork(Modbus_t *pxMb);

uint8_t bModbusRequest(Modbus_t *pxMb, ModbusRequest_t *pxRequest);

static inline uint8_t bModbusErrorFrame(ModbusFrame_t *pxFrame) {
    return ((pxFrame == libNULL) || ((pxFrame->ucFunc & MODBUS_RESPONSE_ERROR_FLAG_MASK) != 0));
}
ModbusPacketType_t eModbusFuncToPacketType(uint8_t ucFunc, uint8_t bIsRequest);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_H_INCLUDED */
