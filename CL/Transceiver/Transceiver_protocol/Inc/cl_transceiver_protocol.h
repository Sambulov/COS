/*
    Common Layer Transceiver Protocol
*/
#ifndef CL_TRANSCEIVER_PROTOCOL_H_
#define CL_TRANSCEIVER_PROTOCOL_H_

#include "cl_transceiver.h"



typedef struct {
    uint8_t cmd; /* cmd value */
    void *context;  /* see BLDL protocol */
    ProtoXferCallback_t command_transfer;
} cl_protocol_command_t;

typedef struct {
    TransceiverHandler_t *transceiver_handler; /* transceiver interface */
    cl_protocol_command_t **cmd_array;
    uint8_t state_machine; /* initial, command, error */
    uint8_t cmd;           /* current command number */
} cl_protocol_transceiver_h;

#define cl_protocol_add_command(...) ((cl_protocol_command_t *[]){__VA_ARGS__, NULL})

#endif