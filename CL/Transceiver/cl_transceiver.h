/*
    Common Layer Transceiver
*/
#ifndef CL_TRANSCEIVER_H_
#define CL_TRANSCEIVER_H_

#include "stdint.h"

typedef int32_t(*ProtoXferCallback_t)(void *pxProtoDescriptor, uint8_t *pucData, uint16_t usCount);
typedef int32_t(*ProtoInfoHandler_t)(void *pxProtoDescriptor);

typedef struct {
  ProtoInfoHandler_t pfEndOfTransactionCallback;
  ProtoXferCallback_t pfRxDataCallback;
  ProtoInfoHandler_t pfRxAvailable;
  ProtoXferCallback_t pfTxEmptyCallback;
  ProtoInfoHandler_t pfTxAvailable;
  void *pxProtoDescriptor;
} TransceiverHandler_t;

#endif