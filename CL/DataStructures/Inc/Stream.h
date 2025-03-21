/*!
    Stream.h
  
    Created on: 01.06.2017
  	    Author: Sambulov Dmitry
        e-mail: dmitry.sambulov@gmail.com
  	
 */
#ifndef STREAM_H_INCLUDED
#define STREAM_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "Fifo.h"

#define STREAM_FAIL   ((int32_t)-1)

typedef struct {
	Fifo_t *pxIFifo;
	Fifo_t *pxOFifo;
} Stream_t;

/*!
	@brief Write formated string to stream buffer
	@param[in] pxStream         Pinter to stream descriptor
	@param[in] pcFormat         "{[{char}]{[%[flags][width][.precision][length]specifier]}[{char}]}"
	@param[in] xArgs            Parameters
	@return Streamed bytes count, <0 if error
*/
static inline int32_t lStreamVPrintf(Stream_t *pxStream, const uint8_t* pcFormat, va_list xArgs) {
    if(pxStream == libNULL) return STREAM_FAIL;
	return lFifoVPrintf(pxStream->pxOFifo, pcFormat, xArgs);
}

/*!
	@brief Write formated string to stream buffer
	@param[in] pxStream  Pinter to stream descriptor
	@param[in] pcFormat  "{[{char}]{[%[flags][width][.precision][length]specifier]}[{char}]}"
	\return Streamed bytes count, <0 if error
*/
static inline int32_t lStreamPrintf(Stream_t *pxStream, const uint8_t* pcFormat, ...) {
	va_list args;
	va_start(args, pcFormat);
	int32_t streamed = lStreamVPrintf(pxStream, pcFormat, args);
	va_end(args);
	return streamed;
}

/*!
	@brief Write data to stream buffer
	@param[in] pxStream   Pinter to stream descriptor
	@param[in] pcData     Pointer to user data
	@param[in] lCount     Data count
	\return Streamed bytes count, <0 if error
*/
static inline int32_t lStreamWrite(Stream_t *pxStream, const uint8_t *pcData, uint32_t ulCount) {
    if(pxStream == libNULL) return STREAM_FAIL;
	return lFifoWrite(pxStream->pxOFifo, pcData, ulCount);
}

/*!
	@brief Write data to stream buffer, will write nothing if data not fit in buffer
	@param[in] pxStream   Pinter to stream descriptor
	@param[in] pcData     Pointer to user data
	@param[in] lCount     Data count
	\return Streamed bytes count, <0 if error
*/
static inline int32_t lStreamWriteAll(Stream_t *pxStream, const uint8_t *pcData, uint32_t ulCount) {
    if(pxStream == libNULL) return STREAM_FAIL;
	return lFifoWriteAll(pxStream->pxOFifo, pcData, ulCount);
}

/*!
	@brief Write data till string terminator to stream buffer
	@param[in] pxStream   Pinter to stream descriptor
	@param[in] pcStr      String
	\return Streamed bytes count, <0 if error
*/
static inline int32_t lStreamWriteString(Stream_t *pxStream, const uint8_t *pcStr) {
    if(pxStream == libNULL) return STREAM_FAIL;
	return lFifoWriteString(pxStream->pxOFifo, pcStr);
}

/*!
	@brief Get avaliable free space
	@param[in]pxStream	Stream descriptor
	@return Bytes count available to write, <0 if error
*/
static inline int32_t lStreamWriteAvailable(Stream_t *pxStream) {
    if(pxStream == libNULL) return STREAM_FAIL;
	return lFifoAvailableToWrite(pxStream->pxOFifo);
}

/*!
	@brief Get avaliable data count
	@param[in]pxStream	Stream descriptor
	@return Bytes count available to read, <0 if error
*/
static inline int32_t lStreamReadAvailable(Stream_t *pxStream) {
    if(pxStream == libNULL) return STREAM_FAIL;
	return lFifoAvailableToRead(pxStream->pxIFifo);
}

/*!
	@brief Get data from stream buffer
	@param[in]pxStream	Stream descriptor
	@param[in]pcBuffer	Pointer to user buffer
	@param[in]lCount	Desired data count
	@return readed bytes count, <0 if error
*/
static inline int32_t lStreamRead(Stream_t *pxStream, uint8_t *pcBuffer, uint32_t ulCount) {
    if(pxStream == libNULL) return STREAM_FAIL;
	return lFifoRead(pxStream->pxIFifo, pcBuffer, ulCount);
}

/*!
	@brief Remove data from stream
	@param[in]pxStream	Stream descriptor
	@param[in]lCount	Desired data count to remove
	@return Removed bytes count, <0 if error
*/
static inline int32_t lStreamShift(Stream_t *pxStream, uint32_t ulCount) {
    if(pxStream == libNULL) return STREAM_FAIL;
	return lFifoShift(pxStream->pxIFifo, ulCount);
}

/*!
	@brief Copy data from stream buffer
	@param[in]pxStream	Stream descriptor
	@param[in]pcBuffer	Pointer to user buffer
	@param[in]lCount	Desired data count
	@return Copied bytes count, <0 if error
*/
static inline int32_t lStreamPeek(Stream_t *pxStream, uint8_t *pcBuffer, uint32_t ulCount) {
    if(pxStream == libNULL) return STREAM_FAIL;
	return lFifoPeek(pxStream->pxIFifo, pcBuffer, ulCount);
}

/*!
	@brief Clear input and output fifo buffers
	@param[in] pxStream   Pinter to stream descriptor
*/
static inline void vStreamFlushIO(Stream_t *pxStream) {
	if (pxStream != libNULL){
		vFifoFlush(pxStream->pxIFifo);
		vFifoFlush(pxStream->pxOFifo);
	}
}

/*!
	@brief Clear input fifo buffer
	@param[in] pxStream   Pinter to stream descriptor
*/
static inline void vStreamFlushI(Stream_t *pxStream) {
	if (pxStream != libNULL) vFifoFlush(pxStream->pxIFifo);
}

/*!
	@brief Clear output fifo buffer
	@param[in] pxStream   Pinter to stream descriptor
*/
static inline void vStreamFlushO(Stream_t *pxStream) {
	if (pxStream != libNULL) vFifoFlush(pxStream->pxOFifo);
}

/*!
  Snake notation
*/

typedef Stream_t stream_t;

static inline int32_t stream_vprintf(stream_t *, const uint8_t*, va_list) __attribute__ ((alias ("lStreamVPrintf")));
static inline int32_t stream_printf(stream_t *, const uint8_t*, ...) __attribute__ ((alias ("lStreamPrintf")));
static inline int32_t stream_write(stream_t *, const uint8_t *, uint32_t) __attribute__ ((alias ("lStreamWrite")));
static inline int32_t stream_write_all(stream_t *, const uint8_t *, uint32_t) __attribute__ ((alias ("lStreamWriteAll")));
static inline int32_t stream_write_string(stream_t *, const uint8_t *) __attribute__ ((alias ("lStreamWriteString")));
static inline int32_t stream_write_available(stream_t *) __attribute__ ((alias ("lStreamWriteAvailable")));
static inline int32_t stream_read_available(stream_t *) __attribute__ ((alias ("lStreamReadAvailable")));
static inline int32_t stream_read(stream_t *, uint8_t *, uint32_t) __attribute__ ((alias ("lStreamRead")));
static inline int32_t stream_shift(stream_t *, uint32_t) __attribute__ ((alias ("lStreamShift")));
static inline int32_t stream_peek(stream_t *, uint8_t *, uint32_t) __attribute__ ((alias ("lStreamPeek")));
static inline void stream_flush_io(stream_t *) __attribute__ ((alias ("vStreamFlushIO")));
static inline void stream_flush_i(stream_t *) __attribute__ ((alias ("vStreamFlushI")));
static inline void stream_flush_o(stream_t *) __attribute__ ((alias ("vStreamFlushO")));

#ifdef __cplusplus
}
#endif

#endif /* STREAM_H_INCLUDED */
