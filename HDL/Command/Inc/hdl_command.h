/*
    Common Layer
    Register Protocol
*/
#ifndef HDL_COMMAND_H_
#define HDL_COMMAND_H_

typedef enum {
    HDL_COMMAND_SM_WAITING_CMD,
    HDL_COMMAND_SM_COMMAND_PROC,
    HDL_COMMAND_SM_ERROR,
} hdl_command_state_machine_e;

/* User defined */
typedef struct {
    uint8_t unique_user_cmd;
    hdl_transceiver_t command_transceiver;
} hdl_user_command_t;

typedef struct {
    hdl_user_command_t **cmd_array;
    /* Private */
    hdl_command_state_machine_e state_machine;
    hdl_user_command_t *current_cmd;           /* Current handling command  */
    hdl_transceiver_t transceiver_handler; /* transceiver interface */
} hdl_command_t;


#define hdl_commands(...) ((hdl_user_command_t *[]){__VA_ARGS__, NULL})

void hdl_command_end_of_execution(hdl_command_t *protocol);

hdl_transceiver_t *hdl_command_get_transiver(hdl_command_t *protocol);


#endif