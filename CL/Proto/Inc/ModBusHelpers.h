/*!
   Modbus.h

    Created on: 25.04.2023
        Author: Sambulov Dmitry
        e-mail: dmitry.sambulov@gmail.com
 */

#ifndef MODBUS_HELPERS_H_INCLUDED
#define MODBUS_HELPERS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "ModBus.h"

/*!
	@brief Init request structure for read discrete outputs, modbus coils, function 0x01
	@param[out] pxOutReq			Request buffer
	@param[in] pxContext            Callback context
	@param[in] pfCb	                Callback function
	@param[in] ucDevAddr            Modbus device address
	@param[in] usRegAddr            Modbus register address
	@param[in] usAmount             Amount to read
*/
static inline void vModbusInitRequestReadOutputs(ModbusRequest_t *pxOutReq, void* pxContext, ModbusCb_t pfCb, 
	                                             uint8_t ucDevAddr, uint16_t usRegAddr, uint16_t usAmount) {
	if((pxOutReq == libNULL) || (usAmount == 0)) return;
	pxOutReq->pxContext = pxContext;
	pxOutReq->pfOnComplete = pfCb;
	pxOutReq->xFrame.ucAddr = ucDevAddr;
	pxOutReq->xFrame.ucFunc = MB_FUNC_READ_OUTPUTS;
	pxOutReq->xFrame.usRegAddr = usRegAddr;
	pxOutReq->xFrame.usRegValueCount = usAmount;
}

/*!
	@brief Init request structure for read discrete inputs, function 0x02
	@param[out] pxOutReq			Request buffer
	@param[in] pxContext            Callback context
	@param[in] pfCb	                Callback function
	@param[in] ucDevAddr            Modbus device address
	@param[in] usRegAddr            Modbus register address
	@param[in] usAmount             Amount to read
*/
static inline void vModbusInitRequestReadInputs(ModbusRequest_t *pxOutReq, void* pxContext, ModbusCb_t pfCb, 
	                                            uint8_t ucDevAddr, uint16_t usRegAddr, uint16_t usAmount) {
	if((pxOutReq == libNULL) || (usAmount == 0)) return;
	vModbusInitRequestReadOutputs(pxOutReq, pxContext, pfCb, ucDevAddr, usRegAddr, usAmount);
	pxOutReq->xFrame.ucFunc = MB_FUNC_READ_INPUTS;
}

/*!
	@brief Init request structure for read analog outputs, modbus holding registers, function 0x03
	@param[out] pxOutReq			Request buffer
	@param[in] pxContext            Callback context
	@param[in] pfCb	                Callback function
	@param[in] ucDevAddr            Modbus device address
	@param[in] usRegAddr            Modbus register address
	@param[in] usAmount             Amount to read
*/
static inline void vModbusInitRequestReadHoldings(ModbusRequest_t *pxOutReq, void* pxContext, ModbusCb_t pfCb, 
	                                              uint8_t ucDevAddr, uint16_t usRegAddr, uint16_t usAmount) {
	if((pxOutReq == libNULL) || (usAmount == 0)) return;
	vModbusInitRequestReadOutputs(pxOutReq, pxContext, pfCb, ucDevAddr, usRegAddr, usAmount);
	pxOutReq->xFrame.ucFunc = MB_FUNC_READ_HOLDINGS;
}

/*!
	@brief Init request structure for read analog inputs, modbus input registers, function 0x04
	@param[out] pxOutReq			Request buffer
	@param[in] pxContext            Callback context
	@param[in] pfCb	                Callback function
	@param[in] ucDevAddr            Modbus device address
	@param[in] usRegAddr            Modbus register address
	@param[in] usAmount             Amount to read
*/
static inline void vModbusInitRequestReadInputRegisters(ModbusRequest_t *pxOutReq, void* pxContext, ModbusCb_t pfCb, 
	                                                    uint8_t ucDevAddr, uint16_t usRegAddr, uint16_t usAmount) {
	if((pxOutReq == libNULL) || (usAmount == 0)) return;
	vModbusInitRequestReadOutputs(pxOutReq, pxContext, pfCb, ucDevAddr, usRegAddr, usAmount);
	pxOutReq->xFrame.ucFunc = MB_FUNC_READ_INPUT_REGISTERS;
}

/*!
	@brief Init request structure for write descrete output, modbus coil, function 0x05
	@param[out] pxOutReq			Request buffer
	@param[in] pxContext            Callback context
	@param[in] pfCb	                Callback function
	@param[in] ucDevAddr            Modbus device address
	@param[in] usRegAddr            Modbus register address
	@param[in] bState               Value: 0-reset, !0- set
*/
static inline void vModbusInitRequestWriteOutput(ModbusRequest_t *pxOutReq, void* pxContext, ModbusCb_t pfCb, 
	                                             uint8_t ucDevAddr, uint16_t usRegAddr, uint8_t bState) {
	if(pxOutReq == libNULL) return;
	vModbusInitRequestReadOutputs(pxOutReq, pxContext, pfCb, ucDevAddr, usRegAddr, bState? 0xFF00 :0x0000);
	pxOutReq->xFrame.ucFunc = MB_FUNC_WRITE_OUTPUT;
}

/*!
	@brief Init request structure for write analog output, modbus holding register, function 0x06
	@param[out] pxOutReq			Request buffer
	@param[in] pxContext            Callback context
	@param[in] pfCb	                Callback function
	@param[in] ucDevAddr            Modbus device address
	@param[in] usRegAddr            Modbus register address
	@param[in] usValue              Register value
*/
static inline void vModbusInitRequestWriteHolding(ModbusRequest_t *pxOutReq, void* pxContext, ModbusCb_t pfCb, 
	                                              uint8_t ucDevAddr, uint16_t usRegAddr, uint16_t usValue) {
	if(pxOutReq == libNULL) return;
	vModbusInitRequestReadOutputs(pxOutReq, pxContext, pfCb, ucDevAddr, usRegAddr, usValue);
	pxOutReq->xFrame.ucFunc = MB_FUNC_WRITE_OUTPUT;
}

/*!
	@brief Init request structure for write multiple descrete outputs, modbus coils, function 0x0F
	@param[out] pxOutReq			Request buffer
	@param[in] pxContext            Callback context
	@param[in] pfCb	                Callback function
	@param[in] ucDevAddr            Modbus device address
	@param[in] usRegAddr            Modbus register address
	@param[in] usAmount             Amount to write
	@param[in] pucValues            Coils states
*/
static inline void vModbusInitRequestWriteCoils(ModbusRequest_t *pxOutReq, void* pxContext, ModbusCb_t pfCb, 
	                                            uint8_t ucDevAddr, uint16_t usRegAddr, uint16_t usAmount, uint8_t *pucValues) {
	if((pxOutReq == libNULL) || (usAmount == 0) || (usAmount > 2040)) return;
	vModbusInitRequestReadOutputs(pxOutReq, pxContext, pfCb, ucDevAddr, usRegAddr, usAmount);
	pxOutReq->xFrame.ucFunc = MB_FUNC_WRITE_COILS;
	pxOutReq->xFrame.pucData = pucValues;
	pxOutReq->xFrame.ucLengthError = (usAmount >> 3);
}

/*!
	@brief Init request structure for write multiple analog outputs, modbus holding registres, function 0x10
	@param[out] pxOutReq			Request buffer
	@param[in] pxContext            Callback context
	@param[in] pfCb	                Callback function
	@param[in] ucDevAddr            Modbus device address
	@param[in] usRegAddr            Modbus register address
	@param[in] usAmount             Amount to write
	@param[in] pusValues            Registers values
*/
static inline void vModbusInitRequestWriteHoldings(ModbusRequest_t *pxOutReq, void* pxContext, ModbusCb_t pfCb, 
	                                            uint8_t ucDevAddr, uint16_t usRegAddr, uint16_t usAmount, uint16_t *pusValues) {
	if((pxOutReq == libNULL) || (usAmount == 0) || (usAmount > 127)) return;
	vModbusInitRequestReadOutputs(pxOutReq, pxContext, pfCb, ucDevAddr, usRegAddr, usAmount);
	pxOutReq->xFrame.ucFunc = MB_FUNC_WRITE_HOLDINGS;
	pxOutReq->xFrame.pucData = (uint8_t *)pusValues;
	pxOutReq->xFrame.ucLengthError = (usAmount * 2);
}



#ifdef __cplusplus
}
#endif

#endif /* MODBUS_HELPERS_H_INCLUDED */
