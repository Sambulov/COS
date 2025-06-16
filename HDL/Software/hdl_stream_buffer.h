#ifndef STREAM_BUFFER_H_
#define STREAM_BUFFER_H_

typedef struct {
  uint32_t size_in;
  uint32_t size_out;
  uint8_t *buffer_in;
  uint8_t *buffer_out;
  stream_t stream;
} hdl_stream_buffer_t;

uint8_t hdl_stream_buffer_init(hdl_stream_buffer_t *pipe, uint8_t is_isr_safe);

uint8_t hdl_transceiver_from_stream(stream_t *stream, hdl_transceiver_t *out_transceiver, uint8_t reverse);

extern const fifo_iface_t fifo_buffer_iface;
extern const fifo_iface_ex_t fifo_buffer_ex_iface;

#endif /* STREAM_BUFFER_H_ */
