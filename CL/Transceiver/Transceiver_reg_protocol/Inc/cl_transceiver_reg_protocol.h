/*
    Common Layer
    Register Protocol
*/
#ifndef CL_TRANSCEIVER_REG_PROTOCOL_H_
#define CL_TRANSCEIVER_REG_PROTOCOL_H_

#include "cl_transceiver.h"

typedef enum {
    CL_REG_PROTOCOL_SM_WAITING_CMD,
    CL_REG_PROTOCOL_SM_COMMAND_PROC,
    CL_REG_PROTOCOL_SM_ERROR,
} cl_reg_protocol_state_machine_e;

typedef enum {
    CL_REG_PROTOCOL_CONTEXT_SM_WAITING_ADDRESS,
    CL_REG_PROTOCOL_CONTEXT_SM_PROC,
    CL_REG_PROTOCOL_CONTEXT_SM_COMPLETED,
    CL_REG_PROTOCOL_CONTEXT_SM_ERROR,
} cl_reg_protocol_context_state_machine_e;

typedef struct {
    cl_reg_protocol_context_state_machine_e state;
    uint8_t current_cmd;
    void *user_context;
} cl_reg_protocol_context_base_t;

/* This function have to return data from interface */
typedef int32_t(*cl_reg_protocol_rx_data_callback)(void *context_base, uint8_t *data, uint16_t len, uint8_t unique_user_cmd);
/* End of transaction */
typedef int32_t(*cl_reg_protocol_end_of_transaction_callback)(void *context_base);

/* User defined */
typedef struct {
    uint8_t unique_user_cmd;
    cl_reg_protocol_context_base_t context;  
    cl_reg_protocol_rx_data_callback rx_data_callback;  /* New data from interface */
    cl_reg_protocol_end_of_transaction_callback end_of_transaction_callback; /* End of transaction */
} cl_reg_protocol_command_t;

typedef struct {
    TransceiverHandler_t *transceiver_handler; /* transceiver interface */
    cl_reg_protocol_command_t **cmd_array;
    cl_reg_protocol_state_machine_e state_machine;
    uint8_t current_cmd;           /* Current handling command  */
} cl_reg_protocol_transceiver_h;

#define cl_reg_protocol_add_command(...) ((cl_reg_protocol_command_t *[]){__VA_ARGS__, NULL})

void cl_protocol_transceiver_worker(cl_reg_protocol_transceiver_h *h);

#endif