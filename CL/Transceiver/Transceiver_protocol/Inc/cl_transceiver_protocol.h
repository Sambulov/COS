/*
    Common Layer Transceiver Protocol
*/
#ifndef CL_TRANSCEIVER_PROTOCOL_H_
#define CL_TRANSCEIVER_PROTOCOL_H_

#include "Transceiver.h"


typedef struct {
    void *context;  /* see BLDL protocol */
    uint8_t cmd; /* cmd value */
    ProtoXferCallback_t command_transfer;
} cl_protocol_command_t;

typedef struct {
    TransceiverHandler_t *transceiver_handler; /* transceiver interface */
    cl_protocol_command_t **cmd_array;
    uint8_t state_machine; /* initial, command, error */
    uint8_t cmd;           /* current command number */
} cl_protocol_transceiver_h;

#endif