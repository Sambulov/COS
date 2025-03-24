#include "CodeLib.h"

#define FIFO_VALIDATION_MARKER    ((uint16_t)(0xCBAF))

typedef struct FIFO_Internal_T {
	const FifoIface_t *const pxIface;
	const FifoIfaceEx_t *const pxIfaceEx;
	uint16_t validation;
	uint8_t rdBufIndex      : 1,
	        isDouble        : 1,
	        isInTransaction : 1;
	uint8_t reserved;
	void *buffer[2];
} _Fifo_t;

LIB_ASSERRT_STRUCTURE_CAST(_Fifo_t, Fifo_t, FIFIO_DESCRIPTOR_SIZE, "Fifo.h");

static uint8_t _bSwitchBuffer(_Fifo_t *pxDesc) {
	if (pxDesc->isDouble && !pxDesc->isInTransaction && !pxDesc->pxIface->pfIsInIsr()) {
		if (pxDesc->pxIface->pfBufferAvailable(pxDesc->buffer[pxDesc->rdBufIndex]) == 0) {
			pxDesc->rdBufIndex = !pxDesc->rdBufIndex;
			return true;
		}
	}
	return false;
}

uint8_t bFifoIsValid(Fifo_t *pxFifo) {
	return pxFifo != libNULL && ((_Fifo_t *)pxFifo)->validation == FIFO_VALIDATION_MARKER;
}

static int32_t _lFifoAvailableTo(_Fifo_t *pxDesc, uint8_t bReadData) {
	int32_t av = -1;
	if (bFifoIsValid((Fifo_t *)pxDesc)) {
		if (bReadData) {
			_bSwitchBuffer(pxDesc);
			av = pxDesc->pxIface->pfBufferAvailable(pxDesc->buffer[pxDesc->rdBufIndex]);
			if ((pxDesc->isDouble) && (!pxDesc->pxIface->pfIsInIsr()))
				av += pxDesc->pxIface->pfBufferAvailable(pxDesc->buffer[!pxDesc->rdBufIndex]);
		}
		else {
			if (pxDesc->isDouble) av = pxDesc->pxIface->pfBufferFree(pxDesc->buffer[!pxDesc->rdBufIndex]);
			else av = pxDesc->pxIface->pfBufferFree(pxDesc->buffer[0]);
		}
	}
	return av;
}

uint8_t bFifoInit(Fifo_t *pxFifo, uint8_t *pucBuffer, uint16_t uSize, uint8_t isDoubleBufferization) {
	if ((pxFifo == libNULL) || (pucBuffer == libNULL) || (pxFifo->pxIface == libNULL)) 
	  return false;
	_Fifo_t *pxDescriptor = (_Fifo_t *)pxFifo;
	pxDescriptor->validation = FIFO_VALIDATION_MARKER;
	pxDescriptor->isDouble = 0;
	pxDescriptor->isInTransaction = 0;
	pxDescriptor->isDouble = (isDoubleBufferization != 0);
	pxDescriptor->rdBufIndex = 0;
	if (isDoubleBufferization) {
		uSize >>= 1;
		pxDescriptor->buffer[1] = pxDescriptor->pxIface->pfBufferInit(pucBuffer + uSize, uSize);
		if (pxDescriptor->buffer[1] == libNULL) {
			return false;
		}
	}
	pxDescriptor->buffer[0] = pxDescriptor->pxIface->pfBufferInit(pucBuffer, uSize);
	if (pxDescriptor->buffer[0] == libNULL) {
		return false;
	}
	return true;
}

void vFifoFlush(Fifo_t *pxDescriptor) {
	_Fifo_t *desc = (_Fifo_t *)pxDescriptor;
	if (bFifoIsValid(pxDescriptor)) {
		pxDescriptor->pxIface->pfBufferFlush(desc->buffer[desc->rdBufIndex]);
		if (desc->isDouble) {
			pxDescriptor->pxIface->pfBufferFlush(desc->buffer[!desc->rdBufIndex]);
		}
	}
}

int32_t lFifoShift(Fifo_t *pxDescriptor, uint16_t uCount) {
	_Fifo_t *desc = (_Fifo_t *)pxDescriptor;
	if (bFifoIsValid(pxDescriptor)) {
		uint8_t dummyBuf;
		int32_t removed = pxDescriptor->pxIface->pfBufferRead(desc->buffer[desc->rdBufIndex], &dummyBuf, 0, uCount, 0);
		if ((removed > 0) && _bSwitchBuffer(desc)) {
			if (removed < uCount) {
				uint32_t res = pxDescriptor->pxIface->pfBufferRead(desc->buffer[desc->rdBufIndex], &dummyBuf, 0, uCount - removed, 0);
				if (res > 0) {
					removed += res;
				}
			}
		}
		return removed;
	}
	return -1;
}

int32_t lFifoAvailableToRead(Fifo_t *pxDescriptor) {
	return _lFifoAvailableTo((_Fifo_t *)pxDescriptor, 1);
}

int32_t lFifoPeek(Fifo_t *pxDescriptor, uint8_t *pucOutBuf, uint16_t uCount) {
	_Fifo_t *desc = (_Fifo_t *)pxDescriptor;
	if (bFifoIsValid(pxDescriptor)) {
		int32_t readed = pxDescriptor->pxIface->pfBufferRead(desc->buffer[desc->rdBufIndex], pucOutBuf, uCount, 0, 1);
		if (desc->isDouble && (readed >= 0) && !desc->pxIface->pfIsInIsr() && (readed < uCount)) {
			int32_t res = pxDescriptor->pxIface->pfBufferRead(desc->buffer[!desc->rdBufIndex], pucOutBuf + readed, uCount - readed, 0, 1);
			if (res > 0) {
				readed += res;
			}
		}
		return readed;
	}
	return -1;
}

int32_t lFifoRead(Fifo_t *pxDescriptor, uint8_t *pucOutBuf, uint16_t uCount) {
	_Fifo_t *desc = (_Fifo_t *)pxDescriptor;
	if (bFifoIsValid(pxDescriptor)) {
		int32_t readed = pxDescriptor->pxIface->pfBufferRead(desc->buffer[desc->rdBufIndex], pucOutBuf, uCount, 0, 0);
		if ((readed >= 0) && (readed < uCount) && _bSwitchBuffer(desc)) {
			int32_t res = pxDescriptor->pxIface->pfBufferRead(desc->buffer[desc->rdBufIndex], pucOutBuf + readed, uCount - readed, 0, 0);
			if (res > 0) {
				readed += res;
			}
		}
		return readed;
	}
	return -1;
}

uint8_t bFifoReadByte(Fifo_t *pxDescriptor, uint8_t* pucOutData) {
	return lFifoRead(pxDescriptor, pucOutData, 1) > 0;
}

int32_t lFifoAvailableToWrite(Fifo_t *pxDescriptor) {
	return _lFifoAvailableTo((_Fifo_t *)pxDescriptor, 0);
}

static inline int32_t _lFifoWrite(Fifo_t *pxDescriptor, const uint8_t *pucData, uint16_t uCount, uint8_t bRepeatMode, uint8_t bAllOrNothing, uint8_t bAsString) {
	_Fifo_t *desc = (_Fifo_t *)pxDescriptor;
	if (bFifoIsValid(pxDescriptor)) {
		int wrBufIndex = 0;
		if (desc->isDouble) {
			wrBufIndex = !(desc->rdBufIndex);
		}
		int32_t writed = pxDescriptor->pxIface->pfBufferWrte(desc->buffer[wrBufIndex], pucData, uCount, bRepeatMode, bAllOrNothing, bAsString);
		if (writed >= 0 && _bSwitchBuffer(desc)) {
			if ((writed == 0) || (((!bAsString)&&(writed < uCount)) || ((bAsString)&&(pucData[writed - 1] != '\0')))) {
				int res = pxDescriptor->pxIface->pfBufferWrte(desc->buffer[!desc->rdBufIndex], pucData + writed, uCount - writed, bRepeatMode, bAllOrNothing, bAsString);
				if (res > 0) {
					writed += res;
				}
			}
		}
		return writed;
	}
	return -1;
}

int32_t lFifoWrite(Fifo_t *pxDescriptor, const uint8_t *pucData, uint16_t uCount) {
	return _lFifoWrite(pxDescriptor, pucData, uCount, 0, 0, 0);
}

int32_t lFifoWriteAll(Fifo_t *pxDescriptor, const uint8_t *pucData, uint16_t uCount) {
	return _lFifoWrite(pxDescriptor, pucData, uCount, 0, 1, 0);
}

uint8_t bFifoWriteByte(Fifo_t *pxDescriptor, uint8_t ucData) {
	return _lFifoWrite(pxDescriptor, &ucData, 1, 0, 0, 0) > 0;
}

int32_t lFifoFill(Fifo_t *pxDescriptor, uint8_t cFiller, uint16_t uCount) {
	return _lFifoWrite(pxDescriptor, &cFiller, uCount, 1, 0, 0);
}

int32_t lFifoFillAll(Fifo_t *pxDescriptor, uint8_t cFiller, uint16_t uCount) {
	return _lFifoWrite(pxDescriptor, &cFiller, uCount, 1, 1, 0);
}

int32_t lFifoWriteString(Fifo_t *pxDescriptor, const uint8_t *pcString) {
	return _lFifoWrite(pxDescriptor, (uint8_t *)pcString, -1, 0, 0, 1);
}

int32_t lFifoWriteStringAll(Fifo_t *pxDescriptor, uint8_t *pcString) {
	return _lFifoWrite(pxDescriptor, (uint8_t *)pcString, -1, 0, 1, 1);
}

uint8_t bFifoTransactionBegin(Fifo_t *pxDescriptor) {
	_Fifo_t *desc = (_Fifo_t *)pxDescriptor;
	if (bFifoIsValid(pxDescriptor) && (desc->pxIfaceEx != libNULL)) {
		if (!desc->isInTransaction) {
			uint8_t wrBufInd = 0;
			if (desc->isDouble) {
				wrBufInd = !desc->rdBufIndex;
			}
			desc->isInTransaction = desc->pxIfaceEx->pfBufferBackup(desc->buffer[wrBufInd]);
		}
		return desc->isInTransaction;
	}
	return false;
}

uint8_t bFifoTransactionCommit(Fifo_t *pxDescriptor) {
	_Fifo_t *desc = (_Fifo_t *)pxDescriptor;
	if (bFifoIsValid(pxDescriptor) && (desc->pxIfaceEx != libNULL)) {
		if (desc->isInTransaction) {
			uint8_t wrBufInd = 0;
			if (desc->isDouble) {
				wrBufInd = !desc->rdBufIndex;
			}
			desc->isInTransaction = !desc->pxIfaceEx->pfBufferCommit(desc->buffer[wrBufInd]);
			if (!desc->isInTransaction) {
				_bSwitchBuffer(desc);
			}
		}
		return !desc->isInTransaction;
	}
	return false;
}

uint8_t bFifoTransactionRollback(Fifo_t *pxDescriptor) {
	_Fifo_t *desc = (_Fifo_t *)pxDescriptor;
	if (bFifoIsValid(pxDescriptor) && (desc->pxIfaceEx != libNULL)) {
		if (desc->isInTransaction) {
			uint8_t wrBufInd = 0;
			if (desc->isDouble) {
				wrBufInd = !desc->rdBufIndex;
			}
			desc->isInTransaction = !desc->pxIfaceEx->pfBufferRestore(desc->buffer[wrBufInd]);
		}
		return !desc->isInTransaction;
	}
	return false;
}

/*!
  Snake notation
*/

uint8_t fifo_init(Fifo_t *, uint8_t *, uint16_t , uint8_t) __attribute__ ((alias ("bFifoInit")));
uint8_t fifo_is_valid(fifo_t *) __attribute__ ((alias ("bFifoIsValid")));
int32_t fifo_available_to_read(fifo_t *) __attribute__ ((alias ("lFifoAvailableToRead")));
int32_t fifo_available_to_write(fifo_t *) __attribute__ ((alias ("lFifoAvailableToWrite")));
int32_t fifo_write(fifo_t *, const uint8_t *, uint16_t) __attribute__ ((alias ("lFifoWrite")));
int32_t fifo_write_all(fifo_t *, const uint8_t *, uint16_t) __attribute__ ((alias ("lFifoWriteAll")));
int32_t fifo_fill(fifo_t *, uint8_t, uint16_t) __attribute__ ((alias ("lFifoFill")));
int32_t fifo_fill_all(fifo_t *, uint8_t, uint16_t) __attribute__ ((alias ("lFifoFillAll")));
int32_t fifo_write_string(fifo_t *, const uint8_t *) __attribute__ ((alias ("lFifoWriteString")));
int32_t fifo_write_string_all(fifo_t *, uint8_t *) __attribute__ ((alias ("lFifoWriteStringAll")));
uint8_t fifo_write_byte(fifo_t *, uint8_t) __attribute__ ((alias ("bFifoWriteByte")));
int32_t fifo_read(fifo_t *, uint8_t *, uint16_t) __attribute__ ((alias ("lFifoRead")));
int32_t fifo_peek(fifo_t *, uint8_t *, uint16_t) __attribute__ ((alias ("lFifoPeek")));
int32_t fifo_shift(fifo_t *, uint16_t) __attribute__ ((alias ("lFifoShift")));
void fifo_flush(fifo_t *) __attribute__ ((alias ("vFifoFlush")));
uint8_t fifo_read_byte(fifo_t *, uint8_t*) __attribute__ ((alias ("bFifoReadByte")));

uint8_t fifo_transaction_begin(fifo_t *) __attribute__ ((alias ("bFifoTransactionBegin")));
uint8_t fifo_transaction_commit(fifo_t *) __attribute__ ((alias ("bFifoTransactionCommit")));
uint8_t fifo_transaction_rollback(fifo_t *) __attribute__ ((alias ("bFifoTransactionRollback")));
