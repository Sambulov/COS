#include "hdl.h"
#include "CodeLib.h"

typedef struct {
  simple_circular_buffer_t rx_buf;
  simple_circular_buffer_t tx_buf;
  hdl_transceiver_t transceiver;
} hdl_isr_buffer_private_t;

_Static_assert(sizeof(hdl_isr_buffer_private_t) == sizeof(hdl_isr_buffer_t), "In hdl_isr_buffer.h data structure size of hdl_isr_buffer_private_t doesn't match, check HDL_ISR_BUFFER_PRIVATE_SIZE");

int32_t hdl_isr_buffer_rx_cb_t (void *proto, uint8_t *data, uint16_t count) {
  hdl_isr_buffer_private_t *buf = (hdl_isr_buffer_private_t*)proto;
  if(buf->rx_buf.pucBuffer != NULL) {
    uint16_t i = count;
    while (i--) {
      scb_push(&buf->rx_buf, *data);
      data++;
    }
  }
  return count;
}

int32_t hdl_isr_buffer_tx_cb_t (void *proto, uint8_t *data, uint16_t count) {
  hdl_isr_buffer_private_t *buf = (hdl_isr_buffer_private_t*)proto;
  if(buf->tx_buf.pucBuffer != NULL) {
    uint16_t av = scb_available(&buf->tx_buf);
    count = MIN(count, av);
    uint16_t i = count;
    while (i--) {
      *data = scb_pop(&buf->tx_buf);
      data++;
    }
  }
  else {
    count = 0;
  }
  return count;
}

int32_t hdl_isr_buffer_rx_av(void *proto) {
  hdl_isr_buffer_private_t *buf = (hdl_isr_buffer_private_t*)proto;
  if(buf->rx_buf.pucBuffer != NULL) {
    return scb_available_free(&buf->rx_buf);
  }
  return 0xFFFF;
}

int32_t hdl_isr_buffer_tx_av(void *proto) {
  hdl_isr_buffer_private_t *buf = (hdl_isr_buffer_private_t*)proto;
  if(buf->tx_buf.pucBuffer != NULL) {
    return scb_available(&buf->tx_buf);
  }
  return 0;
}

void hdl_isr_buffer_eot(void *proto) {

}

hdl_transceiver_t *hdl_get_isr_transceiver_handler(hdl_isr_buffer_t *desc, hdl_isr_buffer_congig_t *cnf) {
  hdl_isr_buffer_private_t *buf = (hdl_isr_buffer_private_t*)desc;
  if(buf != NULL) {
    vScbInit(&buf->rx_buf, NULL, 0);
    vScbInit(&buf->tx_buf, NULL, 0);
    if((cnf->rx_buffer != NULL) && (cnf->rx_buffer_size != 0)) {
      vScbInit(&buf->rx_buf, cnf->rx_buffer, cnf->rx_buffer_size);
    }
    if((cnf->tx_buffer != NULL) && (cnf->tx_buffer_size != 0)) {
      vScbInit(&buf->tx_buf, cnf->tx_buffer, cnf->tx_buffer_size);
    }
    buf->transceiver.end_of_transmission = &hdl_isr_buffer_eot;
    buf->transceiver.rx_available = &hdl_isr_buffer_rx_av;
    buf->transceiver.rx_data = &hdl_isr_buffer_rx_cb_t;
    buf->transceiver.tx_available = &hdl_isr_buffer_tx_av;
    buf->transceiver.tx_empty = &hdl_isr_buffer_tx_cb_t;
    buf->transceiver.proto_context = (void*)buf;
    return &buf->transceiver;
  }
  return NULL;
}

uint16_t hdl_isr_buffer_read(hdl_isr_buffer_t *desc, uint8_t *data, uint16_t lenght) {
  hdl_isr_buffer_private_t *buf = (hdl_isr_buffer_private_t*)desc;
  uint16_t available = scb_available(&buf->rx_buf);
  lenght = MIN(lenght, available);
  available = lenght;
  while (available--) {
    *data = scb_pop(&buf->rx_buf);
    data++;
  }
  return lenght;
}

uint16_t hdl_isr_buffer_write(hdl_isr_buffer_t *desc, uint8_t *data, uint16_t lenght) {
  hdl_isr_buffer_private_t *buf = (hdl_isr_buffer_private_t*)desc;
  uint16_t available = scb_available_free(&buf->tx_buf);
  lenght = MIN(lenght, available);
  available = lenght;
  while (available--) {
    scb_push(&buf->tx_buf, *data);
    data++;
  }
  return lenght;
}