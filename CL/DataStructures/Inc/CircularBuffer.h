/*!
    CircularBuffer.h
 
    Created on: 01.06.2017
        Author: Sambulov Dmitry
        e-mail: dmitry.sambulov@gmail.com

*/
#ifndef CIRCULAR_BUFFER_H_INCLUDED
#define CIRCULAR_BUFFER_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define CIRCULAR_BUFFER_DESCRIPTOR_SIZE 12

typedef struct {
	CL_PRIVATE(CIRCULAR_BUFFER_DESCRIPTOR_SIZE);
} CircularBuffer_t;

/*!
	@brief Initialize circular buffer, descriptor will be placed at the beginning of the buffer
	@param[in] pucBuffer            Buffer pointer
	@param[in] uBufferSize          Buffer length
	@return Pointer to circular buffer descriptor
*/
CircularBuffer_t *pxCircularBufferInit(uint8_t *pucBuffer, uint16_t uBufferSize);

/*!
	@brief Get available data count
	@param[in] pxDescriptor			Circle buffer descriptor
	@return Bytes count available to be readed
*/
int32_t lCircularBufferAvailable(CircularBuffer_t *pxDescriptor);

/*!
	@brief Get free space
	@param[in] pxDescriptor		Circle buffer descriptor
	@returns Circular buffer free space length.
*/
int32_t lCircularBufferFree(CircularBuffer_t *pxDescriptor);

/*!
	@brief Moves data to user buffer
	@param[in] pxDescriptor      Circular buffer descriptor
	@param[in] pucDestBuf        Pointer to user buffer
	@param[in] uCount            Desired data bytes count to be readed
	@param[in] uSkip             Desired data bytes count to be skipped before reading
	@param[in] bToPeek           If need not shift data from circular buffer
	@return Copyed data bytes count
*/
int32_t lCircularBufferRead(CircularBuffer_t *pxDescriptor, uint8_t *pucDestBuf, uint16_t uCount, uint16_t uSkip, uint8_t bToPeek);

/*!
	@brief Write data to circular buffer
	@param[in] pxDescriptor      Circular buffer descriptor
	@param[in] pucData           Pointer to user data buffer
	@param[in] uCount            Desired data count to copy
	@param[in] bFill             Will copy first byte from user data buffer uCount times
	@param[in] bPutAll           Will copy all uCount data bytes or nothing
	@param[in] bAsString         Will copy data bytes before uCount reached or string terminator found
	@return Writed data bytes count
*/
int32_t lCircularBufferWrite(CircularBuffer_t *pxDescriptor, const uint8_t *pucData, uint16_t uCount, uint8_t bFill, uint8_t bPutAll, uint8_t bAsString);

/*!
	@brief Backup circular buffer state, followed operations will could be canceled
	@param[in] pxDescriptor			Pointer to circle buffer descriptor
	@return True if ok
*/
uint8_t bCircularBufferBackup(CircularBuffer_t *pxDescriptor);

/*!
	@brief Restore state of circular buffer from backup
	@param[in] pxDescriptor			Pointer to circle buffer descriptor
	@return True if ok
*/
uint8_t bCircularBufferRestore(CircularBuffer_t *pxDescriptor);

/*!
	@brief Commit operations after backup
	@param[in] pxDescriptor			Pointer to circle buffer descriptor
	@return True if ok
*/
uint8_t bCircularBufferCommit(CircularBuffer_t *pxDescriptor);

/*!
	@brief Flush available data
	@param[in] pxDescriptor			Pointer to circle buffer descriptor
*/
void vCircularBufferFlush(CircularBuffer_t *pxDescriptor);

/*!
  Snake notation
*/

typedef CircularBuffer_t circular_buffer_t;

/*!
	@brief Initialize circular buffer, descriptor will be placed at the beginning of the buffer
	@param[in] buffer            Buffer pointer
	@param[in] buffer_size       Buffer length
	@return Pointer to circular buffer descriptor
*/
circular_buffer_t *circular_buffer_init(uint8_t *buffer, uint16_t buffer_size);

/*!
	@brief Get available data count
	@param[in] desc			Circle buffer descriptor
	@return Bytes count available to be readed
*/
int32_t circular_buffer_available(circular_buffer_t *desc);

/*!
	@brief Get free space
	@param[in] desc		Circle buffer descriptor
	@returns Circular buffer free space length.
*/
int32_t circular_buffer_free(circular_buffer_t *desc);

/*!
	@brief Moves data to user buffer
	@param[in] desc      Circular buffer descriptor
	@param[in] dest      Pointer to user buffer
	@param[in] count     Desired data bytes count to be readed
	@param[in] skip      Desired data bytes count to be skipped before reading
	@param[in] to_peek   If need not shift data from circular buffer
	@return Copyed data bytes count
*/
int32_t circular_buffer_read(circular_buffer_t *desc, uint8_t *dest, uint16_t count, uint16_t skip, uint8_t to_peek);

/*!
	@brief Write data to circular buffer
	@param[in] desc      Circular buffer descriptor
	@param[in] data      Pointer to user data buffer
	@param[in] count     Desired amount of data to copy
	@param[in] to_fill   Will copy first byte from user data buffer uCount times
	@param[in] put_all   Will copy all <count> data bytes or nothing
	@param[in] as_string Will copy data bytes before <count> reached or string terminator found
	@return Writed data bytes count
*/
int32_t circular_buffer_write(circular_buffer_t *desc, const uint8_t *data, uint16_t count, uint8_t to_fill, uint8_t put_all, uint8_t as_string);

/*!
	@brief Backup circular buffer state, followed operations will could be canceled
	@param[in] desc			Pointer to circle buffer descriptor
	@return True if ok
*/
uint8_t circular_buffer_backup(circular_buffer_t *desc);

/*!
	@brief Restore state of circular buffer from backup
	@param[in] desc			Pointer to circle buffer descriptor
	@return True if ok
*/
uint8_t circular_buffer_restore(circular_buffer_t *desc);

/*!
	@brief Commit operations after backup
	@param[in] desc			Pointer to circle buffer descriptor
	@return True if ok
*/
uint8_t circular_buffer_commit(circular_buffer_t *desc);

/*!
	@brief Flush available data
	@param[in] desc			Pointer to circle buffer descriptor
*/
void circular_buffer_flush(circular_buffer_t *desc);

#ifdef __cplusplus
}
#endif

#endif /* CIRCULAR_BUFFER_H_INCLUDED */
