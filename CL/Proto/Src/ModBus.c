#include "CodeLib.h"

#define MODBUS_RX_STATE_DEFINE_PACKET_TYPE     3
#define MODBUS_RX_STATE_SKIP_PACKET            20

#define MODBUS_STATE_RECEIVING                  0x08
#define MODBUS_STATE_RECEIVED                   0x10
#define MODBUS_STATE_TRANSMITTING               0x20
#define MODBUS_STATE_TRANSMITED                 0x40
#define MODBUS_STATE_IDLE                       0x00

#define MODBUS_STATE_TX_ERR                     0x01
#define MODBUS_STATE_RX_ERR                     0x02
#define MODBUS_STATE_TX_TIMEOUT                 0x03
#define MODBUS_STATE_RX_TIMEOUT                 0x04
#define MODBUS_STATE_COMPLETE_NO_RESPONSE       0x06
#define MODBUS_STATE_COMPLETE                   0x07

/* eRxPacketType/eTxPacketType definitions. */
typedef enum {
    eModbusPacketNone        = 0,    /* Undefined */
    eModbusPacketBase        = 1,    /* [ADDR][FUNC][MSB_REG_ADDR][LSB_REG_ADDR][MSB_COUNT][LSB_COUNT][MSB_CRC][LSB_CRC] */
    eModbusPacketVariableLen = 2,    /* [ADDR][FUNC][LENGTH][DATA...][MSB_CRC][LSB_CRC] */
    eModbusPacketFull        = 3,    /* [ADDR][FUNC][MSB_REG_ADDR][LSB_REG_ADDR][MSB_COUNT][LSB_COUNT][LENGTH][DATA...][MSB_CRC][LSB_CRC] */
    eModbusPacketCode        = 4,    /* [ADDR][FUNC][CODE][MSB_CRC][LSB_CRC] */
    eModbusPacketCall        = 8,    /* [ADDR][FUNC][MSB_CRC][LSB_CRC] */
    eModbusPacketTypesAmount
} ModbusPacketType_t;

typedef struct {
    const ModbusIface_t *pxIface;
	ModbusFrame_t xFrame;
	uint16_t usTimer;
    uint16_t rx_timeout;
	uint16_t tx_timeout;
    ModbusPacketType_t eTxType;
    ModbusPacketType_t eRxType;
	uint8_t ucTransferCounter;
	int8_t ucXferState;
    uint8_t ucState    :7,
            bAsciiMode :1;
	uint8_t ucPayLoadBufferSize;
	uint8_t *pucPayLoadBuffer;
	ModbusRequest_t xRequest;
    const ModbusEndpoint_t *pxEndpoints;
} _prModbus_t;

LIB_ASSERRT_STRUCTURE_CAST(_prModbus_t, Modbus_t, MODBUS_DESCRIPTOR_SIZE, "ModBus.h");

static ModbusResult_t _eModbusDummyHandler(ModbusRequest_t *pxRreq, ModbusFrame_t *pxResp) {
    (void)pxRreq;(void)pxResp;
	return ModbusIllegalFunction;
}

static ModbusPacketType_t _eModbusFuncToPacketType(uint8_t ucFunc, uint8_t bIsRequest) {
    if(ucFunc & MODBUS_ERROR_FLAG) return eModbusPacketCode;
	switch (ucFunc) {
	case 0x01 /* Read coil */:
		return bIsRequest? eModbusPacketBase: eModbusPacketVariableLen;
	case 0x02 /* Read input */:
		return bIsRequest? eModbusPacketBase: eModbusPacketVariableLen;
	case 0x03 /* Read holding registers */:
		return bIsRequest? eModbusPacketBase: eModbusPacketVariableLen;
	case 0x04 /* Read input registers */:
		return bIsRequest? eModbusPacketBase: eModbusPacketVariableLen;
	case 0x05 /* Write single coil */:
		return eModbusPacketBase;
	case 0x06 /* Write single register */:
		return eModbusPacketBase;
	case 0x07 /* Read Exception Status */:
		return bIsRequest? eModbusPacketCall: eModbusPacketCode;
	//	case 0x08 /* Diagnostic */:
	//		return ;
	case 0x0B /* Get Com Event Counter */:
        return bIsRequest? eModbusPacketCall: eModbusPacketBase;
	case 0x0C /* Get Com Event Log */:
        return bIsRequest? eModbusPacketCall: eModbusPacketVariableLen;
	case 0x0F /* Write multiple coils */:
		return bIsRequest? eModbusPacketFull: eModbusPacketBase;
	case 0x10 /* Write multiple registers */:
		return bIsRequest? eModbusPacketFull: eModbusPacketBase;
    case 0x11 /* Report Slave ID */:
        return bIsRequest? eModbusPacketCall: eModbusPacketVariableLen;
    case 0x14 /* Read File Record */:
        return bIsRequest? eModbusPacketVariableLen: eModbusPacketVariableLen;
    case 0x15 /* Write File Record */:
        return bIsRequest? eModbusPacketVariableLen: eModbusPacketVariableLen;
    //	case 0x16 /* Mask Write Register */:
    //		return ;
    //	case 0x18 /* Read FIFO Queue */:
    //		return ;
    //	case 0x2B /* Encapsulated Interface Transport */:
    //		return ;
	default:
		return eModbusPacketNone;
	}
}

static void _vModbusServerSetHandler(_prModbus_t *pxMb) {
	pxMb->xRequest.pfOnComplete = &_eModbusDummyHandler;
    pxMb->eRxType = _eModbusFuncToPacketType(pxMb->xRequest.xFrame.ucFunc, 1);
    pxMb->eTxType = eModbusPacketNone;
	if (!(pxMb->xRequest.xFrame.ucFunc & MODBUS_ERROR_FLAG)) {
        const ModbusEndpoint_t *currentEP = pxMb->pxEndpoints;
        while ((currentEP != libNULL) && (currentEP->ucAddress != pxMb->xRequest.xFrame.ucAddr))
            currentEP = currentEP->pxNext;
		if (currentEP != libNULL) {
			const ModbusHandler_t *hdesc = libNULL;
			for (int32_t i = 0; currentEP->paxHandlers[i] != libNULL; i++) {
				if (currentEP->paxHandlers[i]->ucFunctionNo == pxMb->xRequest.xFrame.ucFunc) {
					hdesc = currentEP->paxHandlers[i];
					break;
				}
			}
			if (hdesc == libNULL) pxMb->eTxType = eModbusPacketCode;
			else {
				pxMb->xRequest.pfOnComplete = hdesc->pfOnRequest;
				pxMb->eTxType = _eModbusFuncToPacketType(pxMb->xRequest.xFrame.ucFunc, 0);
			}
            if (pxMb->xRequest.xFrame.ucAddr == 0) pxMb->eTxType = eModbusPacketNone;
		}
	}	
}

static uint16_t _usCrc16ModbusAscii(const uint8_t *pucData, int32_t ulLen, const uint16_t *pusCrc) {
    uint16_t val = (pusCrc != libNULL)? *pusCrc: 0;
	while (ulLen--) val -= *pucData++;
	return val;
}

static inline int32_t _lWrite(_prModbus_t *pxMb, uint8_t *pucData, int32_t lSize) {
    return pxMb->pxIface->lWrite(pxMb->pxIface->pxTxPhy, pucData, lSize);
}

static int32_t _lWriteAscii(_prModbus_t *pxMb, uint8_t *pucData, int32_t lSize) {
  int32_t aw = pxMb->pxIface->lAvailableToWrite(pxMb->pxIface->pxTxPhy) >> 1;
  int32_t count = 0;
  while (lSize-- && aw--) {
    uint16_t asscii_encoded = usConvertByteToAsciiHex(*pucData++, true);
    asscii_encoded = swap_bytes(asscii_encoded);
    if(pxMb->pxIface->lWrite(pxMb->pxIface->pxTxPhy, (uint8_t *)&asscii_encoded, 2) != 2) break;
    count++;
  }
  return count;
}

static uint8_t _bWriteBlock(_prModbus_t *pxMb, uint8_t *pucData, int32_t lSize) {
  int32_t wa = pxMb->pxIface->lAvailableToWrite(pxMb->pxIface->pxTxPhy);
  if(wa < lSize) return false;
  return (lSize >= 0) && (pxMb->pxIface->lWrite(pxMb->pxIface->pxTxPhy, pucData, lSize) == lSize);
}

static uint8_t _bWriteBlockAscii(_prModbus_t *pxMb, uint8_t *pucData, int32_t lSize) {
  if(pxMb->pxIface->lAvailableToWrite(pxMb->pxIface->pxTxPhy) < (lSize * 2)) return false;
  while (lSize--) {
    uint16_t asscii_encoded = convert_byte_to_ascii_hex(*pucData++, true);
    asscii_encoded = swap_bytes(asscii_encoded);
    int32_t res = pxMb->pxIface->lWrite(pxMb->pxIface->pxTxPhy, (uint8_t *)&asscii_encoded, 2);
    if (res <= 0) return false;
  }
  return true;
}

static int8_t _TxFrame(_prModbus_t *pxMb, uint8_t bRequest) {
	if (pxMb->ucXferState < 0) pxMb->ucXferState = 0;
	if (pxMb->eTxType == eModbusPacketNone) return -1;
    int32_t (*lDataWrite)(_prModbus_t *, uint8_t *, int32_t) = &_lWrite;
    if(pxMb->bAsciiMode) lDataWrite = &_lWriteAscii;
    uint8_t (*bBlockWrite)(_prModbus_t *, uint8_t *, int32_t) = &_bWriteBlock;
    if(pxMb->bAsciiMode) bBlockWrite = &_bWriteBlockAscii;
    uint16_t (*usCrcCalc)(const uint8_t *, int32_t, const uint16_t *) = &usCrc16ModbusRtu;
    if (pxMb->bAsciiMode) usCrcCalc = &_usCrc16ModbusAscii;
    ModbusFrame_t *frame = &pxMb->xFrame;
    if(bRequest) frame = &pxMb->xRequest.xFrame;
    switch (pxMb->ucXferState) {
    case 0:
    case 1 /* Tx frame start */:
        if ((pxMb->bAsciiMode) && (_lWrite(pxMb, (uint8_t *)":", 1) <= 0)) return 1;
        pxMb->ucTransferCounter = 0;
        /* fall through */
    case 2 /* Tx addr */:
        if(lDataWrite(pxMb, &frame->ucAddr, 1) <= 0) return 2;
        frame->_usCrc = usCrcCalc(&frame->ucAddr, 1, libNULL);
        /* fall through */
    case 3 /* Tx func */: 
        if (lDataWrite(pxMb, &frame->ucFunc, 1) <= 0 ) return 3;
        frame->_usCrc = usCrcCalc(&frame->ucFunc, 1, &frame->_usCrc);
        /* fall through */
    case 4 /* Tx Reg addr */:
        if (!(frame->ucFunc & MODBUS_ERROR_FLAG) && (pxMb->eTxType & eModbusPacketBase)) {
            uint16_t regAddr = swap_bytes(frame->usRegAddr);
            if (!bBlockWrite(pxMb, (uint8_t *)&regAddr, 2)) return 4;
        frame->_usCrc = usCrcCalc((uint8_t *)&regAddr, 2, &frame->_usCrc);
        } /* fall through */
    case 5 /* Tx Reg count */:
        if (!(frame->ucFunc & MODBUS_ERROR_FLAG) && (pxMb->eTxType & eModbusPacketBase)) {
            uint16_t regValCount = swap_bytes(frame->usRegValueCount);
            if (!bBlockWrite(pxMb, (uint8_t *)&regValCount, 2)) return 5;
            frame->_usCrc = usCrcCalc((uint8_t *)&regValCount, 2, &frame->_usCrc);
        } /* fall through */
    case 6 /* Tx length */:
        if (pxMb->eTxType & (eModbusPacketVariableLen | eModbusPacketCode)) {
            if (lDataWrite(pxMb, &frame->ucLengthCode, 1) <= 0) return 6;
            frame->_usCrc = usCrcCalc(&frame->ucLengthCode, 1, &frame->_usCrc);
        } /* fall through */
    case 7 /* Tx data */:
        if (pxMb->eTxType & eModbusPacketVariableLen) {
        int32_t res = lDataWrite(pxMb, frame->pucData + pxMb->ucTransferCounter, frame->ucLengthCode - pxMb->ucTransferCounter);
        if(res <= 0) return 7;
            frame->_usCrc = usCrcCalc(frame->pucData + pxMb->ucTransferCounter, res, &frame->_usCrc);			
            pxMb->ucTransferCounter += res;
            if (frame->ucLengthCode > pxMb->ucTransferCounter) return 7;
        } /* fall through */
    case 8 /* Tx crc */: 
        if (pxMb->bAsciiMode) {
            uint8_t crc = frame->_usCrc;
            if (lDataWrite(pxMb, &crc, 1) <= 0) return 8;
        }
        else if(!bBlockWrite(pxMb, (uint8_t *)&frame->_usCrc, 2)) return 8;
        /* fall through */
    case 9: /* Tx frame terminator */
        if (pxMb->bAsciiMode) {
            uint32_t aw = pxMb->pxIface->lAvailableToWrite(pxMb->pxIface->pxTxPhy);
            if((aw < 2) || (_lWrite(pxMb, (uint8_t *)"\r\n", 2) <= 0)) return 9;
        }
    default:
        break;
    }
    return 0;
}

static inline int32_t _lRead(_prModbus_t *pxMb, uint8_t *pucData, int32_t lSize) {
    return pxMb->pxIface->lRead(pxMb->pxIface->pxRxPhy, pucData, lSize);
}

static int32_t _lReadAscii(_prModbus_t *pxMb, uint8_t *pucData, int32_t lSize) {
  int32_t count = 0;
  uint16_t asscii;
  uint32_t ar = pxMb->pxIface->lAvailableToRead(pxMb->pxIface->pxRxPhy);
  while (lSize-- && ar >= 2) {
    if((pxMb->pxIface->lRead(pxMb->pxIface->pxRxPhy, (uint8_t *)&asscii, 2) != 2) ||
       !bConvertAsciiHexToByte(swap_bytes(asscii), pucData++)) return -1;
    ar -= 2;
    count++;
  }
  return count;
}

static uint8_t _bReadBlock(_prModbus_t *pxMb, uint8_t *pucData, int32_t lSize) {
  if (pxMb->pxIface->lAvailableToRead(pxMb->pxIface->pxRxPhy) < lSize) return false;
  return pxMb->pxIface->lRead(pxMb->pxIface->pxRxPhy, pucData, lSize) == lSize;
}

static uint8_t _ReadBlockAscii(_prModbus_t *pxMb, uint8_t *pucData, int32_t lSize) {
  if (pxMb->pxIface->lAvailableToRead(pxMb->pxIface->pxRxPhy) < (lSize * 2)) return false;
  uint16_t asscii;
  while (lSize--) {
    if((pxMb->pxIface->lRead(pxMb->pxIface->pxRxPhy, (uint8_t *)&asscii, 2) != 2) ||
       !convert_ascii_hex_to_byte(swap_bytes(asscii), pucData++)) return false;
  }
  return true;
}

static int8_t _RxFrame(_prModbus_t *pxMb, uint8_t bRequest) {
    int32_t (*lDataRead)(_prModbus_t *, uint8_t *, int32_t) = &_lRead;
    if(pxMb->bAsciiMode) lDataRead = &_lReadAscii;
    uint8_t (*bBlockRead)(_prModbus_t *, uint8_t *, int32_t) = &_bReadBlock;
    if(pxMb->bAsciiMode) bBlockRead = &_ReadBlockAscii;
    uint16_t (*usCrcCalc)(const uint8_t *, int32_t, const uint16_t *) = &usCrc16ModbusRtu;
    if (pxMb->bAsciiMode) usCrcCalc = &_usCrc16ModbusAscii;
    if (pxMb->ucXferState <= 0) pxMb->ucXferState = 1;
    ModbusFrame_t *frame = &pxMb->xFrame;
    if(bRequest) frame = &pxMb->xRequest.xFrame;

    switch (pxMb->ucXferState) {
        case 1 /* Rx frame start */:
            if (pxMb->bAsciiMode) {
                if ((_lRead(pxMb, &frame->ucAddr, 1) <= 0) || (frame->ucAddr != ':')) return 1;
            }
            else if (pxMb->pxIface->lAvailableToRead(pxMb->pxIface->pxRxPhy) <= 0) return 1;
            pxMb->ucTransferCounter = 0;
            /* fall through */
        case 2 /* Rx addr & func */: {
            uint8_t buf[2];
            if (!bBlockRead(pxMb, buf, 2)) return 2;
            frame->ucAddr = buf[0];
            frame->ucFunc = buf[1];
            frame->_usCrc = usCrcCalc(buf, 2, libNULL);
            return MODBUS_RX_STATE_DEFINE_PACKET_TYPE;
        } /* fall through */
        case MODBUS_RX_STATE_DEFINE_PACKET_TYPE:
        case 4 /* Rx Reg addr & count */:
            if (pxMb->eRxType & eModbusPacketBase) {
                uint8_t buf[4];
                if (!bBlockRead(pxMb, buf, 4)) return 4;
                frame->usRegAddr = (((uint16_t)buf[0]) << 8) | buf[1];
                frame->usRegValueCount = (((uint16_t)buf[2]) << 8) | buf[3];
                frame->_usCrc = usCrcCalc(buf, 4, &frame->_usCrc);
            } /* fall through */
        case 5 /* Rx length | err */:
            if ((pxMb->eRxType & eModbusPacketVariableLen) || (pxMb->eRxType == eModbusPacketCode)) {
                if (lDataRead(pxMb, &frame->ucLengthCode, 1) <= 0) return 5;
                frame->_usCrc = usCrcCalc(&frame->ucLengthCode, 1, &frame->_usCrc);
            } /* fall through */
        case 6:
            if ((pxMb->eRxType & eModbusPacketVariableLen) && (frame->ucLengthCode > pxMb->ucPayLoadBufferSize)) {
                frame->ucLengthCode = 0;
                return ((pxMb->bAsciiMode)? -1: MODBUS_RX_STATE_SKIP_PACKET);
            } /* fall through */
        case 7 /* Rx data */:
            if (pxMb->eRxType & eModbusPacketVariableLen) {
                int32_t res = lDataRead(pxMb, frame->pucData + pxMb->ucTransferCounter, frame->ucLengthCode - pxMb->ucTransferCounter);
                if (res <= 0) return 7;
                frame->_usCrc = usCrcCalc(frame->pucData + pxMb->ucTransferCounter, res, &frame->_usCrc);			
                pxMb->ucTransferCounter += res;
                if (frame->ucLengthCode > pxMb->ucTransferCounter) return 7;
            } /* fall through */
        case 8 /* Rx unknown */:
            if (pxMb->eRxType == eModbusPacketNone) {
                if (pxMb->bAsciiMode) return -1;
                else {
                    frame->ucLengthCode = 0;
                    return MODBUS_RX_STATE_SKIP_PACKET;
                }
            } /* fall through */
        case 9 /* Rx crc */: {
            uint8_t buf[2];
            uint8_t count = ((pxMb->bAsciiMode)? 1: 2);
            if (!bBlockRead(pxMb, buf, count)) return 9;
            if (pxMb->bAsciiMode) {
                if (((uint8_t)frame->_usCrc) != buf[0]) return -1;
            }
            else return (frame->_usCrc == ((((uint16_t)buf[1]) << 8) | buf[0]))? 0: -1;
        } /* fall through */
        case 10:
            if (pxMb->bAsciiMode) {
                if (pxMb->pxIface->lAvailableToRead(pxMb->pxIface->pxRxPhy) < 2) return 10;
                uint8_t buf[2];
                _lRead(pxMb, buf, 2);
                return (((buf[0] == '\r') && (buf[1] == '\n'))? 0: -1);
            } /* fall through */
        case MODBUS_RX_STATE_SKIP_PACKET:
        default: /* Will try find end of frame. Break if crc match or too many data received, or timeout occurred */
            while (_lRead(pxMb, &frame->pucData[1], 1) > 0) {
                if (frame->ucLengthCode > 0) {
                    uint16_t crc = (((uint16_t)frame->pucData[0]) << 8) | (frame->pucData[1]);
                    if ((crc == frame->_usCrc) || (frame->ucLengthCode >= 251)) return -1;
                    frame->_usCrc = usCrc16ModbusRtu(frame->pucData, 1, &frame->_usCrc);
                }
                frame->ucLengthCode++;
                frame->pucData[0] = frame->pucData[1];
            }
        break;
    }
    return MODBUS_RX_STATE_SKIP_PACKET;
}

static void _vModbusServerWork(_prModbus_t *pxMb) {
    if (pxMb->ucState & MODBUS_STATE_RECEIVING) {
        uint16_t now = pxMb->pxIface->ulTimerMs(pxMb->pxIface->pxTimerPhy);
        if(pxMb->ucXferState <= 1) {
            pxMb->usTimer = now;
            pxMb->xRequest.xFrame.pucData = pxMb->pucPayLoadBuffer;
            pxMb->xRequest.xFrame.ucLengthCode = pxMb->ucPayLoadBufferSize;
        }
        if ((uint16_t)(now - pxMb->usTimer) >= pxMb->rx_timeout) pxMb->ucXferState = 0;
        pxMb->ucXferState = _RxFrame(pxMb, true);
        if (pxMb->ucXferState == MODBUS_RX_STATE_DEFINE_PACKET_TYPE) _vModbusServerSetHandler(pxMb);
        if (pxMb->ucXferState == 0) pxMb->ucState = MODBUS_STATE_RECEIVED;
    }
    if (pxMb->ucState & MODBUS_STATE_RECEIVED) {
        pxMb->xFrame = pxMb->xRequest.xFrame;
        pxMb->xFrame.pucData = pxMb->pucPayLoadBuffer;
        pxMb->xFrame.ucLengthCode = pxMb->ucPayLoadBufferSize;
        ModbusResult_t result = pxMb->xRequest.pfOnComplete(&pxMb->xRequest, &pxMb->xFrame);
        if (pxMb->eTxType != eModbusPacketNone) {
            if (result != ModbusOk) { /* tx error */
                pxMb->xFrame.ucFunc |= MODBUS_ERROR_FLAG;
                pxMb->xFrame.ucLengthCode = result;
                pxMb->eTxType = eModbusPacketCode;
            }
            pxMb->usTimer = pxMb->pxIface->ulTimerMs(pxMb->pxIface->pxTimerPhy);
            pxMb->ucState = MODBUS_STATE_TRANSMITTING;
        }
        else pxMb->ucState = MODBUS_STATE_RECEIVING;
    }
    if (pxMb->ucState & MODBUS_STATE_TRANSMITTING) {
        uint16_t now = pxMb->pxIface->ulTimerMs(pxMb->pxIface->pxTimerPhy);
        pxMb->ucXferState = _TxFrame(pxMb, false);
        if ((pxMb->ucXferState <= 0) || ((uint16_t)(now - pxMb->usTimer) >= pxMb->tx_timeout)) {
            pxMb->ucXferState = 0;
            pxMb->ucState = MODBUS_STATE_RECEIVING;
            //TODO: cb tx timeout???
        }
    }
}

static void _vModbusClientWork(_prModbus_t *pxMb) {
	if (pxMb->ucState == MODBUS_STATE_IDLE) return;
    if (pxMb->ucState == MODBUS_STATE_TRANSMITTING) {
        uint16_t now = pxMb->pxIface->ulTimerMs(pxMb->pxIface->pxTimerPhy);
        if(pxMb->ucXferState <= 0) pxMb->usTimer = now;
        pxMb->ucXferState = _TxFrame(pxMb, true);
        if(pxMb->ucXferState == 0) {
            pxMb->ucState = MODBUS_STATE_RECEIVING;
            pxMb->xFrame.pucData = pxMb->pucPayLoadBuffer;
            pxMb->usTimer = now;
        }
        if(pxMb->ucXferState < 0) pxMb->ucState = MODBUS_STATE_TX_ERR;
        if((uint16_t)(now - pxMb->usTimer) >= pxMb->tx_timeout) pxMb->ucState = MODBUS_STATE_TX_TIMEOUT;
    }
    if (pxMb->ucState == MODBUS_STATE_RECEIVING) {
        if(pxMb->eRxType != eModbusPacketNone) {
            uint16_t now = pxMb->pxIface->ulTimerMs(pxMb->pxIface->pxTimerPhy);
            pxMb->ucXferState = _RxFrame(pxMb, false);
            if (pxMb->ucXferState == MODBUS_RX_STATE_DEFINE_PACKET_TYPE) {
                if (pxMb->xFrame.ucFunc & MODBUS_ERROR_FLAG) pxMb->eRxType = eModbusPacketCode;
                else if((pxMb->xFrame.ucAddr != pxMb->xRequest.xFrame.ucAddr) ||
                        (pxMb->xFrame.ucFunc != pxMb->xRequest.xFrame.ucFunc))
                    pxMb->eRxType = _eModbusFuncToPacketType(pxMb->xFrame.ucFunc, 0);
            }
            else if (pxMb->ucXferState <= 0) {
                pxMb->ucState = MODBUS_STATE_RX_ERR;
                if (pxMb->ucXferState == 0) {
                    if((pxMb->xFrame.ucAddr == pxMb->xRequest.xFrame.ucAddr) &&
                        ((pxMb->xFrame.ucFunc & ~MODBUS_ERROR_FLAG) == pxMb->xRequest.xFrame.ucFunc)) {
                        pxMb->ucState = MODBUS_STATE_COMPLETE;
                    }
                    else {
                        pxMb->ucState = MODBUS_STATE_RECEIVING;
                        pxMb->usTimer = now;
                    }
                }
            }
            else if ((uint16_t)(now - pxMb->usTimer) >= pxMb->rx_timeout) pxMb->ucState = MODBUS_STATE_RX_TIMEOUT;
        }
        else pxMb->ucState = MODBUS_STATE_COMPLETE_NO_RESPONSE;
    }
    if(pxMb->ucState & MODBUS_STATE_COMPLETE) {
        if (pxMb->xRequest.pfOnComplete != libNULL) {
            ModbusResult_t err = 0;
            if((pxMb->ucState == MODBUS_STATE_COMPLETE_NO_RESPONSE) ||
                (pxMb->ucState == MODBUS_STATE_TX_TIMEOUT) ||
                (pxMb->ucState == MODBUS_STATE_RX_TIMEOUT))
                err = ModbusGatewayTargetDeviceFailedtoRespond;
            if((pxMb->ucState == MODBUS_STATE_TX_ERR) || (pxMb->ucState == MODBUS_STATE_RX_ERR))
                err = ModbusGatewayPathUnavailable;
            if(err) {
                pxMb->xFrame = pxMb->xRequest.xFrame;
                pxMb->xFrame.ucFunc = (pxMb->xRequest.xFrame.ucFunc | 0x80);
                pxMb->xFrame.ucLengthCode = err;
            }
            pxMb->xRequest.pfOnComplete(&pxMb->xRequest, &pxMb->xFrame);
            pxMb->ucXferState = -1;
        }
        pxMb->ucState = MODBUS_STATE_IDLE;
    }
}

void vModbusWork(Modbus_t *pxMb) {
    _prModbus_t *mb = (_prModbus_t *)pxMb;
    if(pxMb == libNULL) return;
    if(mb->pxEndpoints != libNULL) _vModbusServerWork(mb);
    else _vModbusClientWork(mb);
}

uint8_t bModbusServerLinkEndpoints(Modbus_t *pxMb, const ModbusEndpoint_t *pxMbEp) {
    _prModbus_t *mb = (_prModbus_t*)pxMb;
    uint8_t check = 1;
    const ModbusEndpoint_t *current = pxMbEp;
    while (current != libNULL) {
        for (uint8_t i = 0; pxMbEp->paxHandlers[i] != libNULL; i++)
            check = check && (pxMbEp->paxHandlers[i]->pfOnRequest != libNULL);
        current = current->pxNext;
    }
    if (check) {
        mb->pxEndpoints = pxMbEp;
        if(pxMbEp != libNULL) mb->ucState = MODBUS_STATE_RECEIVING;
        else mb->ucState = MODBUS_STATE_IDLE;
    }
    return check;
}

uint8_t bModbusInit(Modbus_t *pxMb, const ModbusConfig_t *pxConfig) {
	if ((pxMb == libNULL) || 
        (pxConfig == libNULL) || 
        (pxConfig->pucPayLoadBuffer == libNULL) || 
        (pxConfig->ucPayLoadBufferSize == 0)) return false;
    _prModbus_t *mb = (_prModbus_t*)pxMb;
    mb->pxIface = pxConfig->pxIface;
    mb->ucState = MODBUS_STATE_IDLE;
    mb->ucPayLoadBufferSize = pxConfig->ucPayLoadBufferSize;
    mb->pucPayLoadBuffer = pxConfig->pucPayLoadBuffer;
    mb->ucXferState = 0;
    mb->rx_timeout = pxConfig->rx_timeout;
    mb->tx_timeout = pxConfig->tx_timeout;
    mb->bAsciiMode = pxConfig->bAsciiMode;
    return true;
}

uint8_t bModbusRequest(Modbus_t *pxMb, ModbusRequest_t *pxRequest) {
    _prModbus_t *mb = (_prModbus_t *)pxMb;
	if ((pxMb == libNULL) || 
        (mb->pxEndpoints != libNULL) || /* server mode */
        (mb->ucState != MODBUS_STATE_IDLE) ||
        (pxRequest == libNULL))
        return false;
    mb->eTxType = _eModbusFuncToPacketType(pxRequest->xFrame.ucFunc, 1);
    mb->eRxType = _eModbusFuncToPacketType(pxRequest->xFrame.ucFunc, 0);
    if((mb->eTxType == eModbusPacketNone) ||
       ((mb->eTxType & eModbusPacketVariableLen) && 
         ((pxRequest->xFrame.ucLengthCode == 0) || (pxRequest->xFrame.pucData == libNULL)))) 
        return false;
    mb->xRequest = *pxRequest;
    mb->pxIface->vFlush(mb->pxIface->pxRxPhy);
    mb->ucState = MODBUS_STATE_TRANSMITTING;
    return true;
}
