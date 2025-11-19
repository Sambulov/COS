#include "hdl_iface.h"

static inline int32_t circular_buffer_flush_wrup(void *desc) {
  circular_buffer_flush((circular_buffer_t *)desc);
  return 0;
}

const fifo_iface_t fifo_circular_buffer_iface = {
  .pfBufferAvailable = (buffer_base_t)&circular_buffer_available,
  .pfBufferFlush = (buffer_base_t)&circular_buffer_flush_wrup,
  .pfBufferFree = (buffer_base_t)&circular_buffer_free,
  .pfBufferInit = (buffer_init_t)&circular_buffer_init,
  .pfBufferRead = (buffer_read_t)&circular_buffer_read,
  .pfBufferWrite = (buffer_write_t)&circular_buffer_write,
  .pfIsInIsr = &hdl_core_is_in_isr
};

const fifo_iface_ex_t fifo_circular_buffer_ex_iface = {
  .pfBufferBackup = (buffer_base_bool_t)&circular_buffer_backup,
  .pfBufferCommit = (buffer_base_bool_t)&circular_buffer_commit,
  .pfBufferRestore = (buffer_base_bool_t)&circular_buffer_restore
};

uint8_t hdl_transceiver_from_stream(stream_t *stream, uint8_t reverse, hdl_transceiver_t *transceiver) {
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

uint8_t hdl_transceiver_from_fifos(fifo_t *i_fifo, fifo_t *o_fifo, hdl_transceiver_t *transceiver) {
  stream_t stream;
  stream.pxIFifo = i_fifo;
  stream.pxOFifo = o_fifo;
  return hdl_transceiver_from_stream(&stream, HDL_FALSE, transceiver);
}
