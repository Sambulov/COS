/*!
   Fifo.h

   Created on: 01.06.2017
       Author: Sambulov Dmitry
        e-mail: dmitry.sambulov@gmail.com

*/
#ifndef FIFO_H_INCLUDED
#define FIFO_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define FIFIO_DESCRIPTOR_SIZE    12

typedef enum {
	UNSIGNED_INT  = 0,
	SIGNED_INT    = 1,
	BIN_INT       = 2,
	OCT_INT       = 3,
	HEX_INT       = 4,
	INT_FLAG_MASK = 7,
	NO_PREFIX     = 8
} FifoPrintIntegerFlags_t;

/*!
	@brief Initialize buffer, storing self descriptor with in provaded users buffer is allowed
	@param[in] pucBuffer            User buffer pointer
	@param[in] uBufferSize          Buffer length
	@return Pointer to buffer descriptor
*/
typedef void *(*BufferInit_t)(uint8_t *pucBuffer, uint16_t uBufferSize);

/*!
	@brief Used for basic buffer operations
	@param[in] pxDescriptor			Buffer descriptor
	@return Depends on function
*/
typedef int32_t (*BufferBase_t)(void *pxDescriptor);

/*!
	@brief Used for basic buffer operations
	@param[in] pxDescriptor			Buffer descriptor
	@return Depends on function
*/
typedef uint8_t (*BufferBaseBool_t)(void *pxDescriptor);

/*!
	@brief Moves data to user buffer
	@param[in] pxDescriptor      Buffer descriptor
	@param[in] pucDestBuf        Pointer to user buffer
	@param[in] uCount            Desired data bytes count to be readed
	@param[in] uSkip             Desired data bytes count to be skipped before reading
	@param[in] bToPeek           Lave data in buffer
	@return Copyed data bytes count
*/
typedef int32_t (*BufferRead_t)(void *pxDescriptor, uint8_t *pucDest, uint16_t uTake, uint16_t uSkip, uint8_t bToPeek);

/*!
	@brief Write data to fifo buffer
	@param[in] pxDescriptor      buffer descriptor
	@param[in] pucData           Pointer to user data buffer
	@param[in] uCount            Desired data count to copy
	@param[in] bFill             Write first byte from user data buffer uCount times
	@param[in] bPutAll           Write all uCount data bytes or nothing
	@param[in] bAsString         Write data bytes before uCount reached or string terminator met
	@return Writed data bytes count
*/
typedef int32_t (*BufferWrite_t)(void *pxDescriptor, const uint8_t *pucData, uint16_t uCount, uint8_t bFill, uint8_t bPutAll, uint8_t bAsString);

typedef struct {
	BufferInit_t pfBufferInit;
	BufferBase_t pfBufferAvailable;
	BufferBase_t pfBufferFree;
	BufferBase_t pfBufferFlush;
	BufferRead_t pfBufferRead;
	BufferWrite_t pfBufferWrte;
	uint8_t (*pfIsInIsr)(void); /* check if in interrupt routine; return !0 if call from ISR */
} FifoIface_t;

typedef struct {
	BufferBaseBool_t pfBufferBackup; /* backup buffer state; return: !0 if ok*/
	BufferBaseBool_t pfBufferCommit; /* apply all changes after backup; return: !0 if ok */
	BufferBaseBool_t pfBufferRestore; /* cancel all changes after backup; return: !0 if ok */
} FifoIfaceEx_t;

typedef struct {
	const FifoIface_t *const pxIface; /* provide buffer methods */
	const FifoIfaceEx_t *const pxIfaceEx; /* optionally provide extended buffer methods */
	uint8_t dummy[FIFIO_DESCRIPTOR_SIZE];
} Fifo_t;

/*!
	@brief Initialize fifo buffer, descriptor will be placed at the beginning of the buffer
	@param[in] xpFifo			    FIFO descriptor buffer
	@param[in] pucBuffer            FIFO buffer, note that buffer descriptors allso will be placed in it
	@param[in] uSize	            buffer size
	@param[in] bDoubleBufferization Use double bufferization for usage fifo with in interrupts
	@return !NULL if init ok
*/
uint8_t bFifoInit(Fifo_t *pxFifo, uint8_t *pucBuffer, uint16_t uSize, uint8_t bDoubleBufferization);

/*!
	@brief Validate fifo descriptor
	@param[in] xpFifo			FIFO descriptor
	@return !0 if Valid
*/
uint8_t bFifoIsValid(Fifo_t *pxFifo);
	
/*!
	@brief Get available data count
	@param[in] xpFifo			FIFO descriptor
	@return Bytes count available to be readed
*/
int32_t lFifoAvailableToRead(Fifo_t *xpFifo);

/*!
	@brief Get free space
	@param[in] xpFifo			FIFO descriptor
	@returns Bytes count available to be writed
*/
int32_t lFifoAvailableToWrite(Fifo_t *xpFifo);


/*!
	@brief Write data to fifo buffer
	@param[in] xpFifo			FIFO descriptor
	@param[in] pucData			Pointer to user data
	@param[in] uCount			User data length
	@return Writed bytes count
*/
int32_t lFifoWrite(Fifo_t *xpFifo, const uint8_t *pucData, uint16_t uCount);

/*!
	@brief Write data to fifo buffer
	@param[in] xpFifo			FIFO descriptor
	@param[in] pucData			Pointer to user data
	@param[in] uCount			User data length
	@return Writed bytes count or -1 if data doesn't fits in stream buffer
*/
int32_t lFifoWriteAll(Fifo_t *xpFifo, const uint8_t *pucData, uint16_t uCount);

/*!
	@brief Fill fifo buffer with specified data
	@param[in]xpFifo			FIFO descriptor
	@param[in]cFiller			Data byte with to fill the buffer
	@param[in]uCount			Repeat times
	@return Writed bytes count
*/
int32_t lFifoFill(Fifo_t *xpFifo, uint8_t cFiller, uint16_t uCount);

/*!
	@brief Fill fifo buffer with specified data
	@param[in]xpFifo			FIFO descriptor
	@param[in]cFiller			Data byte with to fill the buffer
	@param[in]ulCount			Repeat times
	@return Writed bytes count or -1 if data doesn't fits in stream buffer
*/
int32_t lFifoFillAll(Fifo_t *xpFifo, uint8_t cFiller, uint16_t uCount);

/*!
	@brief Write data to fifo buffer till string terminator. Terminator included.
	@param[in] xpFifo			FIFO descriptor
	@param[in] pucData			Pointer to string
	@param[in] uCount			User data length
	@return Writed bytes count
*/
int32_t lFifoWriteString(Fifo_t *xpFifo, const uint8_t *pcString);

/*!
	@brief Write data to fifo buffer till string terminator. Terminator included.
	@param[in] xpFifo			FIFO descriptor
	@param[in] pucData			Pointer to string
	@param[in] uCount			User data length
	@return Writed bytes count or -1 if data doesn't fits in stream buffer
*/
int32_t lFifoWriteStringAll(Fifo_t *xpFifo, uint8_t *pcString);

/*!
	@brief Write byte to fifo buffer
	@param[in]xpFifo			FIFO descriptor
	@param[in]cFiller			Data byte
	@return True if writed
*/
uint8_t bFifoWriteByte(Fifo_t *xpFifo, uint8_t ucData);

/*!
	@brief Read data from fifo buffer
	@param[in] xpFifo			FIFO descriptor
	@param[in] pucData			Pointer to user buffer
	@param[in] uCount			Desired data count to be readed
	@return Readed bytes count
*/
int32_t lFifoRead(Fifo_t *xpFifo, uint8_t *pucOutBuf, uint16_t uCount);

/*!
	@brief Read data from fifo buffer without shift
	@param[in] xpFifo			FIFO descriptor
	@param[in] pucData			Pointer to user buffer
	@param[in] uCount			Desired data count to be readed
	@return Copyed bytes count
*/
int32_t lFifoPeek(Fifo_t *xpFifo, uint8_t *pucOutBuf, uint16_t uCount);

/*!
	@brief Remove available data from fifo buffer
	@param[in] xpFifo			FIFO descriptor
	@param[in] uCount			Data count to be removed
	@return Removed bytes count
*/
int32_t lFifoShift(Fifo_t *xpFifo, uint16_t uCount);

/*!
	@brief Write string representation of a float value
	@param[in] pxFifo         FIFO descriptor
	@param[in] fpValue        Float value
	@return writed bytes count, <0 if error
*/
int32_t lFifoPrintFloat(Fifo_t *pxFifo, float fpValue);

/*!
	@brief Write string representation of an integer value
	@param[in] pxFifo           FIFO descriptor
	@param[in] ullValue         Integer value
	@param[in] eFlags           Converter options
	@return Streamed bytes count, <0 if error
*/
int32_t lFifoPrintInteger(Fifo_t *pxFifo, uint64_t ullValue, FifoPrintIntegerFlags_t eFlags);

/*!
	@brief Write formated string
	@param[in] pxFifo           FIFO descriptor
	@param[in] pcFormat         "{[{char}]{[%[flags][width][.precision][length]specifier]}[{char}]}"
	@param[in] xArgs            Parameters
	@return Writed bytes count, <0 if error
*/
int32_t lFifoVPrintf(Fifo_t *xFifo, const uint8_t* pcFormat, va_list xArgs);

/*!
	@brief Write formated string to stream buffer
	@param[in] pxFifo    FIFO descriptor
	@param[in] pcFormat  "{[{char}]{[%[flags][width][.precision][length]specifier]}[{char}]}"
	\return Writed bytes count, <0 if error
*/
static inline int32_t lFifoPrintf(Fifo_t *pxFifo, const uint8_t* pcFormat, ...) {
	va_list args;
	va_start(args, pcFormat);
	int32_t streamed = lFifoVPrintf(pxFifo, pcFormat, args);
	va_end(args);
	return streamed;
}

/*!
	@brief Clear fifo buffer
	@param[in] xpFifo			FIFO descriptor
*/
void vFifoFlush(Fifo_t *xpFifo);

/*!
	@brief Read byte from fifo buffer
	@param[in] xpFifo			FIFO descriptor
	@param[out] pucOutData		Pointer to user byte buffer
	@return True if ok
*/
uint8_t bFifoReadByte(Fifo_t *xpFifo, uint8_t* pucOutData);

/*!
	@brief Begin transaction, followed operations will could be canceled
	@param[in] xpFifo			FIFO descriptor
	@return True if ok
*/
uint8_t bFifoTransactionBegin(Fifo_t *xpFifo);

/*!
	@brief Apply all changes, end transaction
	@param[in] xpFifo			FIFO descriptor
	@return True if ok
*/
uint8_t bFifoTransactionCommit(Fifo_t *xpFifo);

/*!
	@brief Reject all changes, end transaction
	@param[in] xpFifo			FIFO descriptor
	@return True if ok
*/
uint8_t bFifoTransactionRollback(Fifo_t *xpFifo);

/*!
  Snake notation
*/

typedef BufferInit_t buffer_init_t;
typedef BufferBase_t buffer_base_t;
typedef BufferBaseBool_t buffer_base_bool_t;
typedef BufferRead_t buffer_read_t;
typedef BufferWrite_t buffer_write_t;

typedef FifoPrintIntegerFlags_t fifo_print_integer_flags_t;
typedef FifoIface_t fifo_iface_t;
typedef FifoIfaceEx_t fifo_iface_ex_t;
typedef Fifo_t fifo_t;

/*!
	@brief Initialize fifo buffer, descriptor will be placed at the beginning of the buffer
	@param[in] fifo		    	    FIFO descriptor buffer
	@param[in] buffer               FIFO buffer, note that buffer descriptors allso will be placed in it
	@param[in] size	                buffer size
	@param[in] double_bufferization Use double bufferization for usage fifo with in interrupts
	@return !0 if init ok
*/
uint8_t fifo_init(fifo_t *fifo, uint8_t *buffer, uint16_t size, uint8_t double_bufferization);

/*!
	@brief Validate fifo descriptor
	@param[in] fifo			FIFO descriptor
	@return !0 if Valid
*/
uint8_t fifo_is_valid(fifo_t *fifo);
	
/*!
	@brief Get available data count
	@param[in] fifo			FIFO descriptor
	@return Bytes count available to be readed
*/
int32_t fifo_available_to_read(fifo_t *fifo);

/*!
	@brief Get free space
	@param[in] fifo			FIFO descriptor
	@returns Bytes count available to be writed
*/
int32_t fifo_available_to_write(fifo_t *fifo);

/*!
	@brief Write data to fifo buffer
	@param[in] fifo			FIFO descriptor
	@param[in] data			Pointer to user data
	@param[in] count		User data length
	@return Writed bytes count
*/
int32_t fifo_write(fifo_t *fifo, const uint8_t *data, uint16_t count);

/*!
	@brief Write data to fifo buffer
	@param[in] fifo			FIFO descriptor
	@param[in] data			Pointer to user data
	@param[in] count		User data length
	@return Writed bytes count or -1 if data doesn't fits in stream buffer
*/
int32_t fifo_write_all(fifo_t *fifo, const uint8_t *data, uint16_t count);

/*!
	@brief Fill fifo buffer with specified data
	@param[in] fifo			FIFO descriptor
	@param[in] filler		Data byte with to fill the buffer
	@param[in] count		Repeat times
	@return Writed bytes count
*/
int32_t fifo_fill(fifo_t *fifo, uint8_t filler, uint16_t count);

/*!
	@brief Fill fifo buffer with specified data
	@param[in] fifo			FIFO descriptor
	@param[in] filler		Data byte with to fill the buffer
	@param[in] count		Repeat times
	@return Writed bytes count or -1 if data doesn't fits in stream buffer
*/
int32_t fifo_fill_all(fifo_t *fifo, uint8_t filler, uint16_t count);

/*!
	@brief Write data to fifo buffer till string terminator. Terminator included.
	@param[in] fifo  FIFO descriptor
	@param[in] data	 Pointer to string
	@param[in] count User data length
	@return Writed bytes count
*/
int32_t fifo_write_string(fifo_t *fifo, const uint8_t *string);

/*!
	@brief Write data to fifo buffer till string terminator. Terminator included.
	@param[in] fifo	 FIFO descriptor
	@param[in] data  Pointer to string
	@param[in] count User data length
	@return Writed bytes count or -1 if data doesn't fits in stream buffer
*/
int32_t fifo_write_string_all(fifo_t *fifo, uint8_t *string);

/*!
	@brief Write byte to fifo buffer
	@param[in] fifo   FIFO descriptor
	@param[in] filler Data byte
	@return True if writed
*/
uint8_t fifo_write_byte(fifo_t *fifo, uint8_t data);

/*!
	@brief Read data from fifo buffer
	@param[in] fifo  FIFO descriptor
	@param[in] data  Pointer to user buffer
	@param[in] count Desired data count to be readed
	@return Readed bytes count
*/
int32_t fifo_read(fifo_t *fifo, uint8_t *out_buf, uint16_t count);

/*!
	@brief Write string representation of a float value
	@param[in] fifo         FIFO descriptor
	@param[in] value        Float value
	@return writed bytes count, <0 if error
*/
int32_t fifo_print_float(Fifo_t *fifo, float value);

/*!
	@brief Write string representation of an integer value
	@param[in] fifo          FIFO descriptor
	@param[in] value         Integer value
	@param[in] flags         Converter options
	@return Writed bytes count, <0 if error
*/
int32_t fifo_print_integer(fifo_t *fifo, uint64_t value, fifo_print_integer_flags_t flags);

/*!
	@brief Write formated string
	@param[in] fifo           FIFO descriptor
	@param[in] format         "{[{char}]{[%[flags][width][.precision][length]specifier]}[{char}]}"
	@param[in] args           Parameters
	@return Writed bytes count, <0 if error
*/
int32_t fifo_vprintf(fifo_t *fifo, const uint8_t* format, va_list args);

/*!
	@brief Write formated string to stream buffer
	@param[in] fifo    FIFO descriptor
	@param[in] format  "{[{char}]{[%[flags][width][.precision][length]specifier]}[{char}]}"
	\return Writed bytes count, <0 if error
*/
static inline int32_t fifo_printf(fifo_t *fifo, const uint8_t* format, ...)  __attribute__ ((alias ("lFifoPrintf")));

/*!
	@brief Read data from fifo buffer without shift
	@param[in] fifo			FIFO descriptor
	@param[in] data			Pointer to user buffer
	@param[in] count		Desired data count to be readed
	@return Copyed bytes count
*/
int32_t fifo_peek(fifo_t *fifo, uint8_t *out_buf, uint16_t count);

/*!
	@brief Remove available data from fifo buffer
	@param[in] fifo			FIFO descriptor
	@param[in] count			Data count to be removed
	@return Removed bytes count
*/
int32_t fifo_shift(fifo_t *fifo, uint16_t count);

/*!
	@brief Clear fifo buffer
	@param[in] fifo			FIFO descriptor
*/
void fifo_flush(fifo_t *fifo);

/*!
	@brief Read byte from fifo buffer
	@param[in] fifo			FIFO descriptor
	@param[out] out_data	Pointer to user byte buffer
	@return True if ok
*/
uint8_t fifo_read_byte(fifo_t *fifo, uint8_t* out_data);

/*!
	@brief Begin transaction, followed operations will could be canceled
	@param[in] fifo			FIFO descriptor
	@return True if ok
*/
uint8_t fifo_transaction_begin(fifo_t *fifo);

/*!
	@brief Apply all changes, end transaction
	@param[in] fifo			FIFO descriptor
	@return True if ok
*/
uint8_t fifo_transaction_commit(fifo_t *fifo);

/*!
	@brief Reject all changes, end transaction
	@param[in] fifo			FIFO descriptor
	@return True if ok
*/
uint8_t fifo_transaction_rollback(fifo_t *fifo);

#ifdef __cplusplus
}
#endif

#endif /* FIFO_H_INCLUDED */
