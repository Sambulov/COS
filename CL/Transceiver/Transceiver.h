/*
    Common Layer Transceiver
*/
#ifndef CL_TRANSCEIVER_H_
#define CL_TRANSCEIVER_H_

typedef int32_t(*ProtoXferCallback_t)(void *pxProtoDescriptor, uint8_t *pucData, uint16_t usCount);
typedef int32_t(*ProtoInfoHandler_t)(void *pxProtoDescriptor);
typedef void(*ProtoEotHandler_t)(void *pxProtoDescriptor);

typedef struct {
  ProtoEotHandler_t pfEndOfTransactionCallback;
  ProtoXferCallback_t pfRxDataCallback;
  ProtoInfoHandler_t pfRxAvailable;
  ProtoXferCallback_t pfTxEmptyCallback;
  ProtoInfoHandler_t pfTxAvailable;
  void *pxProtoDescriptor;
} TransceiverHandler_t;

/* snake notation */

typedef ProtoXferCallback_t hdl_transceiver_xfer_cb_t;
typedef ProtoInfoHandler_t hdl_transceiver_get_size_cb_t;
typedef ProtoEotHandler_t hdl_transceiver_eot_cb_t;


typedef struct {
  hdl_transceiver_eot_cb_t end_of_transmission;
  hdl_transceiver_xfer_cb_t rx_data;
  hdl_transceiver_get_size_cb_t rx_available;
  hdl_transceiver_xfer_cb_t tx_empty;
  hdl_transceiver_get_size_cb_t tx_available;
  void *proto_context;
} hdl_transceiver_t;

#endif