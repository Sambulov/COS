/*!
    LinkedList.h
  
    Created on: 21.05.2024
        Author: Sambulov Dmitry
        e-mail: dmitry.sambulov@gmail.com
 */
#ifndef SMALL_CIRCULAR_BUFFER_H_INCLUDED
#define SMALL_CIRCULAR_BUFFER_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint8_t *pucBuffer;
  uint16_t usTail;
  uint16_t usHead;
  uint16_t usSize;
} SimpleCircularBuffer_t;


/*!
  Note: buffer size must be power of 2
*/
static inline void vScbInit(SimpleCircularBuffer_t *pxBuf, uint8_t *pucBuffer, uint16_t usSize) {
  pxBuf->usHead = pxBuf->usTail = 0;
  pxBuf->usSize = usSize;
  pxBuf->pucBuffer = pucBuffer;
}

static inline uint16_t usScbAvailable(SimpleCircularBuffer_t *pxBuf) {
  return (pxBuf->usHead - pxBuf->usTail) & (pxBuf->usSize - 1);
}

static inline uint16_t usScbNonSegmentedAvailable(SimpleCircularBuffer_t *pxBuf) {
  return ((pxBuf->usHead > pxBuf->usTail)? (pxBuf->usHead - pxBuf->usTail): (pxBuf->usSize - pxBuf->usTail));
}

static inline uint16_t usScbAvailableFree(SimpleCircularBuffer_t *pxBuf) {
  return (pxBuf->usSize - 1 - usScbAvailable(pxBuf));
}

static inline uint16_t usScbNonSegmentedAvailableFree(SimpleCircularBuffer_t *pxBuf) {
  return ((pxBuf->usHead < pxBuf->usTail)? (pxBuf->usTail - pxBuf->usHead - 1): (pxBuf->usSize - pxBuf->usHead - ((pxBuf->usTail == 0)? 1: 0)));
}

static inline void vScbMoveTail(SimpleCircularBuffer_t *pxBuf, uint16_t usCount) {
  uint16_t cnt = MIN(usScbAvailable(pxBuf), usCount);
  pxBuf->usTail = (pxBuf->usTail + cnt) & (pxBuf->usSize - 1);
}

static inline void vScbMoveHead(SimpleCircularBuffer_t *pxBuf, uint16_t usCount) {
  uint16_t cnt = MIN(usScbAvailableFree(pxBuf), usCount);
  pxBuf->usHead = (pxBuf->usHead + cnt) & (pxBuf->usSize - 1);
}

static inline uint8_t *pucScbData(SimpleCircularBuffer_t *pxBuf) {
  return pxBuf->pucBuffer + pxBuf->usTail;
}

static inline uint8_t *pucScbBuffer(SimpleCircularBuffer_t *pxBuf) {
  return pxBuf->pucBuffer + pxBuf->usHead;
}

static inline void vScbPush(SimpleCircularBuffer_t *pxBuf, uint8_t ucData) {
  pxBuf->pucBuffer[pxBuf->usHead] = ucData;
  vScbMoveHead(pxBuf, 1);
}

static inline uint8_t ucScbPop(SimpleCircularBuffer_t *pxBuf) {
  uint8_t data = pxBuf->pucBuffer[pxBuf->usTail];
  vScbMoveTail(pxBuf, 1);
  return data;
}

/*!
  Snake notation
*/

typedef SimpleCircularBuffer_t simple_circular_buffer_t;

static inline void scb_init(simple_circular_buffer_t *, uint8_t *, uint16_t)\
                                                      __attribute__ ((alias ("vScbInit")));

static inline uint16_t scb_available(simple_circular_buffer_t *)\
                                                      __attribute__ ((alias ("usScbAvailable")));

static inline uint16_t scb_non_segmented_available(simple_circular_buffer_t *)\
                                                      __attribute__ ((alias ("usScbNonSegmentedAvailable")));

static inline uint16_t scb_available_free(simple_circular_buffer_t *)\
                                                      __attribute__ ((alias ("usScbAvailableFree")));

static inline uint16_t scb_non_segmented_available_free(simple_circular_buffer_t *)\
                                                      __attribute__ ((alias ("usScbNonSegmentedAvailableFree")));

static inline void scb_move_tail(simple_circular_buffer_t *, uint16_t)\
                                                      __attribute__ ((alias ("vScbMoveTail")));

static inline void scb_move_head(simple_circular_buffer_t *, uint16_t)\
                                                      __attribute__ ((alias ("vScbMoveHead")));

static inline uint8_t *scb_data(simple_circular_buffer_t *)\
                                                      __attribute__ ((alias ("pucScbData")));

static inline uint8_t *scb_buffer(simple_circular_buffer_t *)\
                                                      __attribute__ ((alias ("pucScbBuffer")));

static inline void scb_push(simple_circular_buffer_t *, uint8_t)\
                                                      __attribute__ ((alias ("vScbPush")));

static inline uint8_t scb_pop(simple_circular_buffer_t *)\
                                                      __attribute__ ((alias ("ucScbPop")));
#ifdef __cplusplus
}
#endif

#endif /* SMALL_CIRCULAR_BUFFER_H_INCLUDED */
