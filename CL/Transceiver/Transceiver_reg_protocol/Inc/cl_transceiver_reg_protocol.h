/*
    Common Layer
    Register Protocol
*/
#ifndef CL_TRANSCEIVER_REG_PROTOCOL_H_
#define CL_TRANSCEIVER_REG_PROTOCOL_H_

typedef enum {
    CL_REG_PROTOCOL_SM_WAITING_CMD,
    CL_REG_PROTOCOL_SM_COMMAND_PROC,
    CL_REG_PROTOCOL_SM_ERROR,
} cl_reg_protocol_state_machine_e;

typedef enum {
    CL_REG_CONTEXT_SM_PROC,
    CL_REG_CONTEXT_SM_COMPLETED,
} cl_reg_protocol_context_state_machine_e;

typedef struct {
    void *user_context;
    /* Private */
    cl_reg_protocol_context_state_machine_e state; /* Notify protocol about end transaction */
} cl_reg_protocol_context_base_t;

/* This function returns data from interface */
typedef int32_t(*cl_reg_protocol_rx_data_callback)(void *context_base, uint8_t *data, uint16_t len);
/* End of transaction */
typedef int32_t(*cl_reg_protocol_end_of_transaction_callback)(void *context_base);
/* Request for writing data */
typedef int32_t(*cl_reg_protocol_tx_data_callback)(void *context_base, uint8_t *data, uint16_t len);
/* User defined */
typedef struct {
    uint8_t unique_user_cmd;
    cl_reg_protocol_context_base_t context_base;  
    cl_reg_protocol_rx_data_callback rx_data_callback;  /* New data from interface */
    cl_reg_protocol_end_of_transaction_callback end_of_transaction_callback; /* End of transaction */
    cl_reg_protocol_tx_data_callback tx_data_callback;  /* Request for writing data */
} cl_reg_protocol_command_t;

typedef struct {
    TransceiverHandler_t *transceiver_handler; /* transceiver interface */
    cl_reg_protocol_command_t **cmd_array;
    /* Private */
    cl_reg_protocol_state_machine_e state_machine;
    uint8_t current_cmd;           /* Current handling command  */
} cl_reg_protocol_transceiver_h;

#define cl_reg_protocol_add_command(...) ((cl_reg_protocol_command_t *[]){__VA_ARGS__, NULL})

int32_t cl_protocol_reg_tx_data_callback(void *context_base, uint8_t *data, uint16_t len);
int32_t cl_protocol_reg_rx_data_callback(void *context_base, uint8_t *data, uint16_t len);
void cl_protocol_reg_end_of_transaction_callback(void *context_base);
void cl_protocol_transceiver_worker(cl_reg_protocol_transceiver_h *h);

#endif