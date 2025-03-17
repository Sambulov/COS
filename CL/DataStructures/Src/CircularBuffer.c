#include "CodeLib.h"

#define CB_VALIDATION_NUMBER 0xCCB5

/*
  Warning! Don't change fields order.
  See dependent functions:
  - vCircularBufferFlush
  - bCircularBufferBackupState
  - bCircularBufferRestoreState
  - bCircularBufferCommitState
 */
typedef struct {
	uint16_t tail;
	uint16_t head;
	uint16_t tailBackup;
	uint16_t headBackup;
	uint16_t size;
	uint16_t validation;
	uint8_t buffer[4];
}_CircularBuffer_t;

//LIB_ASSERRT_STRUCTURE_CAST(_CircularBuffer_t, CircularBuffer_t, CIRCULAR_BUFFER_DESCRIPTOR_SIZE, "CircularBuffer.h");
_Static_assert(sizeof(_CircularBuffer_t) == (sizeof(CircularBuffer_t) + 4), "In ""\"CircularBuffer.h\""" data structure size of ""CircularBuffer_t"" doesn't match, check ""CIRCULAR_BUFFER_DESCRIPTOR_SIZE");

static _CircularBuffer_t *_pxCircularBufferCastDescriptor(CircularBuffer_t *pxDescriptor) {
	_CircularBuffer_t *desc = (_CircularBuffer_t*)pxDescriptor;
	return ((desc == libNULL) || (desc->validation != CB_VALIDATION_NUMBER))? libNULL: desc;
}

static void _vCircularBufferGetWrPtrs(_CircularBuffer_t *pxDescriptor, uint16_t *uOutTail, uint16_t *uOutHead) {
	*uOutTail = pxDescriptor->tail;
	*uOutHead = pxDescriptor->head;
	if (pxDescriptor->tailBackup != 0xFFFF) {
		if (pxDescriptor->headBackup == 0xFFFF) {
			*uOutHead = pxDescriptor->headBackup;
			return;
		}
		if (*uOutHead == *uOutTail) {
			*uOutHead = pxDescriptor->headBackup;
		}
		*uOutTail = pxDescriptor->tailBackup;
	}
}

CircularBuffer_t *pxCircularBufferInit(uint8_t *pucBuffer, uint16_t uBufferSize) {
	if ((uBufferSize > (sizeof(CircularBuffer_t))) && (pucBuffer != libNULL)) {
		_CircularBuffer_t *desc = (_CircularBuffer_t *)pucBuffer;
		desc->head = desc->tail = 0;
		desc->headBackup = desc->tailBackup = 0xFFFF;
		desc->size = uBufferSize - sizeof(CircularBuffer_t);
		desc->validation = CB_VALIDATION_NUMBER;
		return (CircularBuffer_t *)desc;
	}
	return libNULL;
}

int32_t lCircularBufferAvailable(CircularBuffer_t *pxDescriptor) {
	_CircularBuffer_t *desc = _pxCircularBufferCastDescriptor(pxDescriptor);
	uint16_t av = -1;
	if (desc != libNULL) {
		if (desc->head == 0xFFFF) av = desc->size;
		else av = ((desc->head >= desc->tail)? (desc->head - desc->tail): ((desc->size - desc->tail) + desc->head));
	}
	return av;
}

int32_t lCircularBufferFree(CircularBuffer_t *pxDescriptor) {
	_CircularBuffer_t *desc = _pxCircularBufferCastDescriptor(pxDescriptor);
	uint16_t av = -1;
	if (desc != libNULL) {
		uint16_t tail, head;
		_vCircularBufferGetWrPtrs(desc, &tail, &head);
		if (head == 0xFFFF) av = 0;
		else av = (head < tail)? (tail - head) : ((desc->size - head) + tail);
	}
	return av;
}

int32_t lCircularBufferRead(CircularBuffer_t *pxDescriptor, uint8_t *pucOutBuf, uint16_t uCount, uint16_t uSkip, uint8_t bPeek) {
	_CircularBuffer_t *desc = _pxCircularBufferCastDescriptor(pxDescriptor);
	if (desc == libNULL || pucOutBuf == libNULL) {
		return -1;
	}
	int32_t res = 0;
	uint16_t dataCount = lCircularBufferAvailable(pxDescriptor);
	if ((uSkip || uCount) && dataCount) {
		uint16_t tail = desc->tail;
		uint16_t head = desc->head;
		if (head == 0xFFFF) {
			head = tail;
		}
		if (uSkip) {
			if (dataCount < uSkip) {
				uSkip = dataCount;
			}
			tail += uSkip;
			if (tail >= desc->size) {
				tail -= desc->size;
			}
			dataCount -= uSkip;
		}
		if (uCount && dataCount) {
			do {
				*pucOutBuf++ = desc->buffer[tail++];
				if (tail >= desc->size) {
					tail = 0;
				}
				res++;
			} while (res != uCount && tail != head);
		}
		res += uSkip;
		if (!bPeek) {
			desc->tail = tail;
			desc->head = head;
		}
	}
	return res;
}

int32_t lCircularBufferWrite(CircularBuffer_t *pxDescriptor, const uint8_t *pucData, uint16_t uCount, uint8_t bFillMode, uint8_t bAllOrNothing, uint8_t bStringMode) {
	_CircularBuffer_t *desc = _pxCircularBufferCastDescriptor(pxDescriptor);
	if (desc == libNULL || pucData == libNULL) {
		return -1;
	}
	int32_t res = 0;
	if (uCount != 0) {
		uint16_t tail, head;
		_vCircularBufferGetWrPtrs(desc, &tail, &head);
		while ((res != uCount) && (head != 0xFFFF)) {
			if ((bStringMode) && (*pucData == '\0')) {
				uCount = res;
				break;
			}
			desc->buffer[head++] = *pucData;
			res++;
			if (head >= desc->size) {
				head = 0;
			}
			if (head == tail) {
				head = 0xFFFF;
			}
			if (!bFillMode) {
				pucData++;
			}
		}
		if (!bAllOrNothing || uCount == res) {
			desc->head = head;
		}
		else {
			res = -1;
		}
	}
	return res;
}

void vCircularBufferFlush(CircularBuffer_t *pxDescriptor) {
	_CircularBuffer_t *desc = _pxCircularBufferCastDescriptor(pxDescriptor);
	if (desc != libNULL) {
		if (desc->tailBackup == 0xFFFF) {
			*(((uint32_t *)pxDescriptor)) = 0;
		}
		else {
			if (desc->headBackup != 0xFFFF) {
				uint32_t val = (((uint32_t)desc->headBackup) << 16) | desc->headBackup;
				*((uint32_t *)pxDescriptor) = val;
			}
			else {
				desc->tail = desc->head;
			}
		}
	}
}

uint8_t bCircularBufferBackup(CircularBuffer_t *pxDescriptor) {
	_CircularBuffer_t *desc = _pxCircularBufferCastDescriptor(pxDescriptor);
	if (desc != libNULL) {
		if (desc->tailBackup == 0xFFFF) {
			*(((uint32_t *)pxDescriptor) + 1) = *((uint32_t *)pxDescriptor);
			return 1;
		}
	}
	return 0;
}

uint8_t bCircularBufferRestore(CircularBuffer_t *pxDescriptor) {
	_CircularBuffer_t *desc = _pxCircularBufferCastDescriptor(pxDescriptor);
	if (desc != libNULL) {
		if (desc->tailBackup != 0xFFFF) {
			*((uint32_t *)pxDescriptor) = *(((uint32_t *)pxDescriptor) + 1);
			*(((uint32_t *)pxDescriptor) + 1) = 0xFFFFFFFF;
			return 1;
		}
	}
	return 0;
}

uint8_t bCircularBufferCommit(CircularBuffer_t *pxDescriptor) {
	_CircularBuffer_t *desc = _pxCircularBufferCastDescriptor(pxDescriptor);
	if (desc != libNULL) {
		if (desc->tailBackup != 0xFFFF) {
			*(((uint32_t *)pxDescriptor) + 1) = 0xFFFFFFFF;
			return 1;
		}
	}
	return 0;
}


circular_buffer_t *circular_buffer_init(uint8_t *buffer, uint16_t buffer_size) __attribute__ ((alias ("pxCircularBufferInit")));
int32_t circular_buffer_available(circular_buffer_t *desc) __attribute__ ((alias ("lCircularBufferAvailable")));
int32_t circular_buffer_free(circular_buffer_t *desc) __attribute__ ((alias ("lCircularBufferFree")));
int32_t circular_buffer_read(circular_buffer_t *desc, uint8_t *dest, uint16_t count, uint16_t skip, uint8_t to_peek) __attribute__ ((alias ("lCircularBufferRead")));
int32_t circular_buffer_write(circular_buffer_t *desc, const uint8_t *data, uint16_t count, uint8_t to_fill, uint8_t put_all, uint8_t as_string) __attribute__ ((alias ("lCircularBufferWrite")));
uint8_t circular_buffer_backup(circular_buffer_t *desc) __attribute__ ((alias ("bCircularBufferBackup")));
uint8_t circular_buffer_restore(circular_buffer_t *desc) __attribute__ ((alias ("bCircularBufferRestore")));
uint8_t circular_buffer_commit(circular_buffer_t *desc) __attribute__ ((alias ("bCircularBufferCommit")));
void circular_buffer_flush(circular_buffer_t *desc) __attribute__ ((alias ("vCircularBufferFlush")));
