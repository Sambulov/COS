#ifndef TRANSCEIVER_H_
#define TRANSCEIVER_H_

typedef int32_t(*hdl_transceiver_xfer_cb_t)(void *pxProtoDescriptor, uint8_t *pucData, uint16_t usCount);
typedef int32_t(*hdl_transceiver_get_size_cb_t)(void *pxProtoDescriptor);
typedef void(*hdl_transceiver_eot_cb_t)(void *pxProtoDescriptor);

typedef struct {
  hdl_transceiver_eot_cb_t end_of_transmission;
  hdl_transceiver_xfer_cb_t rx_data;
  hdl_transceiver_get_size_cb_t rx_available;
  hdl_transceiver_xfer_cb_t tx_empty;
  hdl_transceiver_get_size_cb_t tx_available;
  void *proto_context;
} hdl_transceiver_t;

#endif