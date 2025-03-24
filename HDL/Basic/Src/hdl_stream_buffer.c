#include "hdl_iface.h"

static inline int32_t circular_buffer_flush_wrup(void *desc) {
  circular_buffer_flush((circular_buffer_t *)desc);
  return 0;
}

const fifo_iface_t fifo_buffer_iface = {
  .pfBufferAvailable = (buffer_base_t)&circular_buffer_available,
  .pfBufferFlush = (buffer_base_t)&circular_buffer_flush_wrup,
  .pfBufferFree = (buffer_base_t)&circular_buffer_free,
  .pfBufferInit = (buffer_init_t)&circular_buffer_init,
  .pfBufferRead = (buffer_read_t)&circular_buffer_read,
  .pfBufferWrte = (buffer_write_t)&circular_buffer_write,
  .pfIsInIsr = &hdl_core_is_in_isr
};

const fifo_iface_ex_t fifo_buffer_ex_iface = {
  .pfBufferBackup = (buffer_base_bool_t)&circular_buffer_backup,
  .pfBufferCommit = (buffer_base_bool_t)&circular_buffer_commit,
  .pfBufferRestore = (buffer_base_bool_t)&circular_buffer_restore
};

uint8_t hdl_stream_buffer_init(hdl_stream_buffer_t *stream_buffer, uint8_t is_isr_safe) {
  if((stream_buffer == NULL) || 
     (stream_buffer->buffer_in == NULL) || (stream_buffer->buffer_out == NULL) ||
     ((uint32_t)stream_buffer->buffer_in & 3) || ((uint32_t)stream_buffer->buffer_out & 3) || /* unaligned */
     (stream_buffer->size_in < sizeof(fifo_t)) || (stream_buffer->size_out < sizeof(fifo_t)) /* piperiptors will be placed in the buffer */
    ) return HDL_FALSE;
  fifo_t *in = (fifo_t *)stream_buffer->buffer_in;
  fifo_t *out = (fifo_t *)stream_buffer->buffer_out;
  const uint32_t fifo_size = sizeof(fifo_t);
  in->pxIface = out->pxIface = &fifo_buffer_iface;
  in->pxIfaceEx = out->pxIfaceEx = &fifo_buffer_ex_iface;
  if(!fifo_init(in, stream_buffer->buffer_in + fifo_size, stream_buffer->size_in - fifo_size, is_isr_safe) ||
      !fifo_init(out, stream_buffer->buffer_out + fifo_size, stream_buffer->size_out - fifo_size, is_isr_safe)) return HDL_FALSE;
  stream_buffer->stream.pxIFifo = in;
  stream_buffer->stream.pxOFifo = out;
  return HDL_TRUE;
}

uint8_t hdl_transceiver_from_stream(stream_t *stream, hdl_transceiver_t *transceiver, uint8_t reverse) {
  if((stream == NULL) || (transceiver == NULL)) return HDL_FALSE;
  transceiver->receiver_context = stream->pxIFifo;
  transceiver->transmitter_context = stream->pxOFifo;
  if(reverse) {
    transceiver->receiver_context = stream->pxOFifo;
    transceiver->transmitter_context = stream->pxIFifo;
  }
  transceiver->end_of_transmission = NULL;
  transceiver->rx_available = (hdl_transceiver_size_cb_t)&fifo_available_to_write;
  transceiver->tx_available = (hdl_transceiver_size_cb_t)&fifo_available_to_read;
  transceiver->rx_data = (hdl_transceiver_xfer_cb_t)&fifo_write;
  transceiver->tx_empty = (hdl_transceiver_xfer_cb_t)&fifo_read;
  return HDL_TRUE;
}
