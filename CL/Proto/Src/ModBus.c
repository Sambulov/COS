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

typedef struct {
    ModbusIface_t *pxIface;
	ModbusFrame_t xFrame;
	uint16_t usTimer;
	uint8_t ucTransferCounter;
	int8_t ucXferState;
    uint8_t ucState    :7,
            bAsciiMode :1;
	ModbusRequest_t xRequest;
    ModbusEndpoint_t *pxEndpoints;
} _prModbus_t;

//_Static_assert(sizeof(_prModbus_t) == (MODBUS_DESCRIPTOR_SIZE), "In Modbus.h data structure size doesn't match allocation MODBUS_SERVER_DESCRIPTOR_SIZE");

static ModbusResult_t _eModbusDummyHandler(ModbusRequest_t *pxRreq, ModbusFrame_t *pxResp) {
    (void)pxRreq;(void)pxResp;
	return ModbusIllegalFunction;
}

ModbusPacketType_t eModbusFuncToPacketType(uint8_t ucFunc, uint8_t bIsRequest) {
	switch (ucFunc) {
	case 0x01 /* Read coil */:
		return bIsRequest? eModbusPacketBase: eModbusPacketVariableLen;
	case 0x02 /* Read input */:
		return bIsRequest? eModbusPacketBase: eModbusPacketVariableLen;
	case 0x03 /* Read holding registers */:
		return bIsRequest? eModbusPacketBase: eModbusPacketVariableLen;
	case 0x04 /* Read input iegisters */:
		return bIsRequest? eModbusPacketBase: eModbusPacketVariableLen;
	case 0x05 /* Write single coil */:
		return eModbusPacketBase;
	case 0x06 /* Write single register */:
		return eModbusPacketBase;
	//	case 0x07 /* Read Exception Status */:
	//		return ;
	//	case 0x08 /* Diagnostic */:
	//		return ;
	//	case 0x0B /* Get Com Event Counter */:
	//		return ;
	//	case 0x0C /* Get Com Event Log */:
	//		return ;
	case 0x0F /* Write multiple coils */:
		return bIsRequest? eModbusPacketFull: eModbusPacketBase;
	case 0x10 /* Write multiple registers */:
		return bIsRequest? eModbusPacketFull: eModbusPacketBase;
		//	case 0x11 /* Report Slave ID */:
		//		return ;
		//	case 0x14 /* Read File Record */:
		//		return ;
		//	case 0x15 /* Write File Record */:
		//		return ;
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
	if (pxMb->xFrame.ucFunc & MODBUS_RESPONSE_ERROR_FLAG_MASK) {
		pxMb->xRequest.eRxType = eModbusPacketError;
		pxMb->xRequest.eTxType = eModbusPacketNone;
	}
	else {
        ModbusEndpoint_t *currentEP = pxMb->pxEndpoints;
        while (currentEP != libNULL) {
            if ((currentEP->ucAddress == pxMb->xFrame.ucAddr) || 
                ((pxMb->xFrame.ucAddr == 0) && (currentEP->bBroadCast))) break;
            currentEP = currentEP->pxNext;
        }                

		if (currentEP == libNULL) {
			pxMb->xRequest.eTxType = eModbusPacketNone;
			pxMb->xRequest.eRxType = eModbusFuncToPacketType(pxMb->xFrame.ucFunc, 1);
		}
		else {
			ModbusHandler_t *hdesc = libNULL;
			for (int32_t i = 0; i < currentEP->ucHandlersCount; i++) {
				if (currentEP->paxHandlers[i]->ucFunctionNo == pxMb->xFrame.ucFunc) {
					hdesc = currentEP->paxHandlers[i];
					break;
				}
			}
			if (hdesc == libNULL) {
				pxMb->xRequest.eTxType = eModbusPacketError;
				pxMb->xRequest.eRxType = eModbusFuncToPacketType(pxMb->xFrame.ucFunc, 1);
			}
			else {
				pxMb->xRequest.pfOnComplete = hdesc->pfOnReceive;
				pxMb->xRequest.eRxType = hdesc->eRxType;
				pxMb->xRequest.eTxType = hdesc->eTxType;
			}
            if (pxMb->xFrame.ucAddr == 0) pxMb->xRequest.eTxType = eModbusPacketNone;
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
    //asscii_encoded = swap_bytes(asscii_encoded); /* TODO: check it */
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
    //asscii_encoded = swap_bytes(asscii_encoded); /* TODO: check it */
    int32_t res = pxMb->pxIface->lWrite(pxMb->pxIface->pxTxPhy, (uint8_t *)&asscii_encoded, 2);
    if (res <= 0) return false;
  }
  return true;
}

static int8_t _TxFrame(_prModbus_t *pxMb, uint8_t bRequest) {
	if (pxMb->ucXferState < 0) pxMb->ucXferState = 0;
	if (pxMb->xRequest.eTxType == eModbusPacketNone) return -1;
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
        /* fall through */
    case 2 /* Tx addr */:
        if(lDataWrite(pxMb, &frame->ucAddr, 1) <= 0) return 2;
        frame->_usCrc = usCrcCalc(&frame->ucAddr, 1, libNULL);
        /* fall through */
    case 3 /* Tx func */: 
        if (lDataWrite(pxMb, &frame->ucFunc, 1) <=0 ) return 3;
        frame->_usCrc = usCrcCalc(&frame->ucFunc, 1, &frame->_usCrc);
        /* fall through */
    case 4 /* Tx Reg addr */:
        if (!(frame->ucFunc & MODBUS_RESPONSE_ERROR_FLAG_MASK) && (pxMb->xRequest.eTxType & eModbusPacketBase)) {
            uint16_t regAddr = swap_bytes(frame->usRegAddr);
            if (!bBlockWrite(pxMb, (uint8_t *)&regAddr, 2)) return 4;
        frame->_usCrc = usCrcCalc((uint8_t *)&regAddr, 2, &frame->_usCrc);
        } /* fall through */
    case 5 /* Tx Reg count */:
        if (!(frame->ucFunc & MODBUS_RESPONSE_ERROR_FLAG_MASK) && (pxMb->xRequest.eTxType & eModbusPacketBase)) {
            uint16_t regValCount = swap_bytes(frame->usRegValueCount);
            if (!bBlockWrite(pxMb, (uint8_t *)&regValCount, 2)) return 5;
            frame->_usCrc = usCrcCalc((uint8_t *)&regValCount, 2, &frame->_usCrc);
        } /* fall through */
    case 6 /* Tx length */:
        if (pxMb->xRequest.eTxType & (eModbusPacketVariableLen | eModbusPacketError)) {
            if (lDataWrite(pxMb, &frame->ucLengthError, 1) <= 0) return 6;
            frame->_usCrc = usCrcCalc(&frame->ucLengthError, 1, &frame->_usCrc);
        } /* fall through */
    case 7 /* Tx data */:
        if (pxMb->xRequest.eTxType & eModbusPacketVariableLen) {
        uint8_t *data = frame->pucData + pxMb->ucTransferCounter;
        uint32_t left = frame->ucDataLength - pxMb->ucTransferCounter;
        int32_t count = lDataWrite(pxMb, data, left);
        if(count <= 0) return 7;
        pxMb->ucTransferCounter += count;
            if (pxMb->ucTransferCounter < frame->ucLengthError) return 7;
            frame->_usCrc = usCrcCalc(frame->pucData, frame->ucLengthError, &frame->_usCrc);
        } /* fall through */
    case 8 /* Tx crc */: 
        if (pxMb->bAsciiMode) {
            uint8_t crc = frame->_usCrc;
            if (!lDataWrite(pxMb, &crc, 1)) return 8;
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
    //asscii = swap_bytes(asscii); /* TODO: check it */
    if((pxMb->pxIface->lRead(pxMb->pxIface->pxRxPhy, (uint8_t *)&asscii, 2) != 2) ||
       !bConvertAsciiHexToByte(asscii, pucData++)) return -1;
    ar -= 2;
    count++;
  }
  count >>= 2;
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
    //asscii = swap_bytes(asscii); /* TODO: check it */
    if((pxMb->pxIface->lRead(pxMb->pxIface->pxRxPhy, (uint8_t *)&asscii, 2) != 2) ||
       !convert_ascii_hex_to_byte(asscii, pucData++)) return false;
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
            /* fall through */
        case 2 /* Rx addr & func */: {
            uint8_t buf[2];
            int32_t res = lDataRead(pxMb, buf, 2);
            if (res <= 0) return ((res < 0)? -1: 2);
                frame->ucAddr = buf[0];
                frame->ucFunc = buf[1];
                frame->_usCrc = usCrcCalc(buf, 2, libNULL);
                return MODBUS_RX_STATE_DEFINE_PACKET_TYPE;
            } /* fall through */
        case MODBUS_RX_STATE_DEFINE_PACKET_TYPE:
        case 4 /* Rx Reg addr & count */:
            if (pxMb->xRequest.eRxType & eModbusPacketBase) {
                uint8_t buf[4];
                if (bBlockRead(pxMb, buf, 4)) return 4;
                frame->usRegAddr = (((uint16_t)buf[0]) << 8) | buf[1];
                frame->usRegValueCount = (((uint16_t)buf[2]) << 8) | buf[3];
                frame->_usCrc = usCrcCalc(buf, 4, &frame->_usCrc);
            } /* fall through */
        case 5 /* Rx length | err */:
            if ((pxMb->xRequest.eRxType & eModbusPacketVariableLen) || (pxMb->xRequest.eRxType == eModbusPacketError)) {
                int32_t res = lDataRead(pxMb, &frame->ucLengthError, 1);
                if (res <= 0) return ((res < 0)? -1: 5);
                pxMb->ucTransferCounter = 0;
                frame->_usCrc = usCrcCalc(&frame->ucLengthError, 1, &frame->_usCrc);
            } /* fall through */
        case 6:
            if ((pxMb->xRequest.eRxType & eModbusPacketVariableLen) && (frame->ucLengthError > frame->ucDataLength)) {
                frame->ucLengthError = 0;
                return ((pxMb->bAsciiMode)? -1: MODBUS_RX_STATE_SKIP_PACKET);
            } /* fall through */
        case 7 /* Rx data */:
            if (pxMb->xRequest.eRxType & eModbusPacketVariableLen) {
                int32_t res = lDataRead(pxMb, frame->pucData + pxMb->ucTransferCounter, frame->ucLengthError - pxMb->ucTransferCounter);
                if (res <= 0) return ((res < 0)? -1: 7);
                pxMb->ucTransferCounter += res;
                frame->_usCrc = usCrcCalc(frame->pucData, res, &frame->_usCrc);			
                if (frame->ucLengthError > pxMb->ucTransferCounter) return 7;
            } /* fall through */
        case 8 /* Rx unknown */:
            if (pxMb->xRequest.eRxType == eModbusPacketNone) {
                if (pxMb->bAsciiMode) return -1;
                else {
                frame->ucLengthError = 0;
                return MODBUS_RX_STATE_SKIP_PACKET;
                }
            } /* fall through */
        case 9 /* Rx crc */: {
            uint8_t buf[2];
            uint8_t count = ((pxMb->bAsciiMode)? 1: 2);
            if (bBlockRead(pxMb, buf, count) <= 0) return 9;
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
                if (frame->ucLengthError > 0) {
                    uint16_t crc = (((uint16_t)frame->pucData[0]) << 8) | (frame->pucData[1]);
                    if ((crc == frame->_usCrc) || (frame->ucLengthError >= 251)) return -1;
                    frame->_usCrc = usCrc16ModbusRtu(frame->pucData, 1, &frame->_usCrc);
                }
                frame->ucLengthError++;
                frame->pucData[0] = frame->pucData[1];
            }
        break;
    }
    return MODBUS_RX_STATE_SKIP_PACKET;
}

static void _vModbusServerWork(_prModbus_t *pxMb) {
    if (pxMb->ucState & MODBUS_STATE_RECEIVING) {
        uint16_t now = pxMb->pxIface->ulTimerMs(pxMb->pxIface->pxTimerPhy);
        if(pxMb->ucXferState <= 1) pxMb->usTimer = now;
        if ((now - pxMb->usTimer) >= MODBUS_SERVER_RX_TIMEOUT) pxMb->ucXferState = 0;
        _RxFrame(pxMb, true);
        if (pxMb->ucXferState == MODBUS_RX_STATE_DEFINE_PACKET_TYPE) _vModbusServerSetHandler(pxMb);
        if (pxMb->ucXferState == 0) pxMb->ucState = MODBUS_STATE_RECEIVED;
    }
    if (pxMb->ucState & MODBUS_STATE_RECEIVED) {
        ModbusResult_t result = pxMb->xRequest.pfOnComplete(&pxMb->xRequest, &pxMb->xFrame);
        if (pxMb->xRequest.eTxType != eModbusPacketNone) {
            if (result != ModbusOk) { /* tx error */
                pxMb->xFrame.ucFunc |= MODBUS_RESPONSE_ERROR_FLAG_MASK;
                pxMb->xFrame.ucLengthError = result;
                pxMb->xRequest.eTxType = eModbusPacketError;
            }
            pxMb->ucState = MODBUS_STATE_TRANSMITTING;
        }
        else pxMb->ucState = MODBUS_STATE_RECEIVING;
    }
    if (pxMb->ucState & MODBUS_STATE_TRANSMITTING) {
        _TxFrame(pxMb, false);
        if (pxMb->ucXferState <= 0) pxMb->ucState = MODBUS_STATE_RECEIVING;
        //TODO: check tx timeout
    }
}

static void _vModbusClientWork(_prModbus_t *pxMb) {
	if (pxMb->ucState == MODBUS_STATE_IDLE) return;
    if (pxMb->ucState == MODBUS_STATE_TRANSMITTING) {
        uint16_t now = pxMb->pxIface->ulTimerMs(pxMb->pxIface->pxTimerPhy);
        if(pxMb->ucXferState <= 0) pxMb->usTimer = now;
        _TxFrame(pxMb, true);
        if(pxMb->ucXferState == 0) {
            pxMb->ucState = MODBUS_STATE_RECEIVING;
            pxMb->usTimer = now;
        }
        if(pxMb->ucXferState < 0) pxMb->ucState = MODBUS_STATE_TX_ERR;
        if((now - pxMb->usTimer) >= MODBUS_CLIENT_TX_TIMEOUT) pxMb->ucState = MODBUS_STATE_TX_TIMEOUT;
    }
    if (pxMb->ucState == MODBUS_STATE_RECEIVING) {
        if(pxMb->xRequest.eRxType != eModbusPacketNone) {
            uint16_t now = pxMb->pxIface->ulTimerMs(pxMb->pxIface->pxTimerPhy);
            _RxFrame(pxMb, false);
            if (pxMb->ucXferState == MODBUS_RX_STATE_DEFINE_PACKET_TYPE) {
                if (pxMb->xFrame.ucFunc & MODBUS_RESPONSE_ERROR_FLAG_MASK) pxMb->xRequest.eRxType = eModbusPacketError;
                else if((pxMb->xFrame.ucAddr != pxMb->xRequest.xFrame.ucAddr) ||
                        (pxMb->xFrame.ucFunc != pxMb->xRequest.xFrame.ucFunc))
                    pxMb->xRequest.eRxType = eModbusFuncToPacketType(pxMb->xFrame.ucFunc, 0);
            }
            else if (pxMb->ucXferState <= 0) {
                pxMb->ucState = MODBUS_STATE_RX_ERR;
                if (pxMb->ucXferState == 0) {
                    if((pxMb->xFrame.ucAddr == pxMb->xRequest.xFrame.ucAddr) &&
                        ((pxMb->xFrame.ucFunc & ~MODBUS_RESPONSE_ERROR_FLAG_MASK) == pxMb->xRequest.xFrame.ucFunc)) {
                        pxMb->ucState = MODBUS_STATE_COMPLETE;
                    }
                    else {
                        pxMb->ucState = MODBUS_STATE_RECEIVING;
                        pxMb->usTimer = now;
                    }
                }
            }
            else if ((now - pxMb->usTimer) >= MODBUS_CLIENT_RX_TIMEOUT) pxMb->ucState = MODBUS_STATE_RX_TIMEOUT;
        }
        else pxMb->ucState = MODBUS_STATE_COMPLETE_NO_RESPONSE;
    }
    if(pxMb->ucState & MODBUS_STATE_COMPLETE) {
        if (pxMb->xRequest.pfOnComplete != libNULL) {
            uint8_t *buf = pxMb->xFrame.pucData;
            uint8_t bufSz = pxMb->xFrame.ucDataLength;
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
                pxMb->xFrame.ucLengthError = err;
            }
            pxMb->xRequest.pfOnComplete(&pxMb->xRequest, &pxMb->xFrame);
            pxMb->xFrame.pucData = buf;
            pxMb->xFrame.ucDataLength = bufSz;
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

static uint8_t _bModbusServerLinkEndpoints(_prModbus_t *pxMb, ModbusEndpoint_t *pxMbEp) {
    _prModbus_t *mb = (_prModbus_t*)pxMb;
    uint8_t check = (pxMbEp->ucAddress != 0) && (pxMbEp->ucHandlersCount > 0);
    ModbusEndpoint_t *current = pxMbEp;
    while (current !=libNULL) {
        for (uint8_t i = 0; i < current->ucHandlersCount; i++) {
            check = check && 
                (pxMbEp->paxHandlers[i]->eRxType < eModbusPacketTypesAmount) &&
                (pxMbEp->paxHandlers[i]->eRxType != eModbusPacketNone) &&
                (pxMbEp->paxHandlers[i]->eTxType < eModbusPacketTypesAmount) &&
                (pxMbEp->paxHandlers[i]->pfOnReceive != libNULL);
        }
        current = current->pxNext;
    }
    if (check) {
        mb->pxEndpoints = pxMbEp;
        mb->ucState = MODBUS_STATE_RECEIVING;
    }
    return check;
}

uint8_t bModbusInit(Modbus_t *pxMb, ModbusConfig_t *pxConfig) {
	if ((pxMb == libNULL) || 
        (pxConfig == libNULL) || 
        (pxConfig->pucPayLoadBuffer == libNULL) || 
        (pxConfig->ucPayLoadBufferSize == 0) ||
        (pxConfig->bIsServer && (pxConfig->pxMbEp == libNULL))) return false;
    _prModbus_t *mb = (_prModbus_t*)pxMb;
    mb->pxIface = pxConfig->pxIface;
    if(pxConfig->bIsServer) {
        if(!_bModbusServerLinkEndpoints(mb, pxConfig->pxMbEp)) return false;
        mb->ucState = MODBUS_STATE_RECEIVING;
    }
    else {
        mb->pxEndpoints = libNULL;
        mb->ucState = MODBUS_STATE_IDLE;
    }
    mb->xFrame.pucData = pxConfig->pucPayLoadBuffer;
    mb->xFrame.ucDataLength = pxConfig->ucPayLoadBufferSize;
    mb->ucXferState = 0;
    mb->bAsciiMode = pxConfig->bAsciiMode;
    return true;
}

uint8_t bModbusClientRequest(Modbus_t *pxMb, ModbusRequest_t *pxRequest) {
    _prModbus_t *mb = (_prModbus_t *)pxMb;
	if ((pxMb == libNULL) || 
        (mb->pxEndpoints != libNULL) || /* server mode */
        (mb->ucState != MODBUS_STATE_IDLE) ||
        (pxRequest == libNULL) ||
        (pxRequest->eRxType >= eModbusPacketTypesAmount) ||
        (pxRequest->eTxType == eModbusPacketNone) ||
        ((pxRequest->eTxType & eModbusPacketVariableLen) && ((pxRequest->xFrame.ucDataLength == 0) || (pxRequest->xFrame.pucData == libNULL))))
        return false;
    mb->xRequest = *pxRequest;
    mb->pxIface->vFifoFlush(mb->pxIface->pxRxPhy);
    mb->ucState = MODBUS_STATE_TRANSMITTING;
    return true;
}
