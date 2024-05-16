#include "cl_transceiver_reg_protocol.h"

/* Write register 1 */
#define TEST_1 
/* Unsuppoted cmd */ 
//#define TEST_2 
/* Unsuppoted reg address */ 
//#define TEST_3
/* Read register 1 */
//#define TEST_4 


cl_reg_protocol_command_t *_reg_protocol_find_command(cl_reg_protocol_transceiver_h *h, uint8_t command) {
    cl_reg_protocol_command_t **command_array = h->cmd_array;
    cl_reg_protocol_command_t *rez = NULL;
    /* Find command number */
    while (*command_array != NULL) {
        if (command == (*command_array)->unique_user_cmd) {
            rez = (*command_array);
            break;
        }
        command_array++;
    }
    return rez;
}

int32_t cl_protocol_reg_end_of_transaction_callback(void *context_base) {
    cl_reg_protocol_transceiver_h *h = (cl_reg_protocol_transceiver_h *)context_base;
    cl_reg_protocol_command_t *command = NULL;
    switch (h->state_machine) {
        case CL_REG_PROTOCOL_SM_COMMAND_PROC: {
            command = _reg_protocol_find_command(h, h->current_cmd);
            command->context_base.state = CL_REG_CONTEXT_SM_COMPLETED;
            /* Call user function */
            command->end_of_transaction_callback(&command->context_base);
        }
            break;
        default:
            break;
    }
    h->current_cmd = 0;
    h->state_machine = CL_REG_PROTOCOL_SM_WAITING_CMD;
    return 0;
}

int32_t cl_protocol_reg_tx_data_callback(void *context_base, uint8_t *data, uint16_t count) {
    cl_reg_protocol_transceiver_h *h = (cl_reg_protocol_transceiver_h *)context_base;
    cl_reg_protocol_command_t *command = NULL;
    //data = NULL;
    switch (h->state_machine) {
        case CL_REG_PROTOCOL_SM_COMMAND_PROC: {
            command = _reg_protocol_find_command(h, h->current_cmd);
            /* Check current state machine in the context */
            if(command->context_base.state == CL_REG_CONTEXT_SM_PROC) {
                /* Call user function trnasfer */
                 return command->tx_data_callback(&command->context_base, data, count);
            }
        }
            break;
        default:
            break;
    }
    return 0;
}

int32_t cl_protocol_reg_rx_data_callback(void *context_base, uint8_t *data, uint16_t count) {
    cl_reg_protocol_transceiver_h *h = (cl_reg_protocol_transceiver_h *)context_base;
    cl_reg_protocol_command_t *command = NULL;
    int32_t number_of_obtained_bytes = 0;
    while (count != 0) {
        switch (h->state_machine)
        {
            case CL_REG_PROTOCOL_SM_WAITING_CMD:
            case CL_REG_PROTOCOL_SM_ERROR: {
                /* Find new command */
                command = _reg_protocol_find_command(h, *data);
                if(command != NULL) {
                    h->state_machine = CL_REG_PROTOCOL_SM_COMMAND_PROC;
                    h->current_cmd = *data;
                    /* Switch context state */
                    command->context_base.state = CL_REG_CONTEXT_SM_PROC;
                    count--;
                    number_of_obtained_bytes++;
                }
                else
                    return 1;   /* First byte it is`t supported command */
            }
                break;
            case CL_REG_PROTOCOL_SM_COMMAND_PROC: {
                command = _reg_protocol_find_command(h, h->current_cmd);
                if(command != NULL) {
                    int32_t obtained_byte_by_command = command->rx_data_callback(&command->context_base, data, count);
                    number_of_obtained_bytes += obtained_byte_by_command;
                    return number_of_obtained_bytes;
                }
                else {
                    h->state_machine = CL_REG_PROTOCOL_SM_ERROR;
                    return number_of_obtained_bytes;
                }
            }
                break;
        default:
            break;
        }
    }
    return number_of_obtained_bytes;
}

void cl_protocol_transceiver_worker(cl_reg_protocol_transceiver_h *h) {
#ifdef TEST_1
    uint8_t input_data[] = {2, 1, 0xaa, 0, 0, 0};
    static uint8_t cnt = 0;
    uint8_t size = (sizeof(input_data)/sizeof(input_data[0])); 

    if(cnt >= size)
        cnt = 0;

    h->transceiver_handler->pfRxDataCallback(h, &input_data[cnt], 1);
    cnt++;
#endif

#ifdef TEST_2
    uint8_t input_data[] = {3, 1, 0, 0, 0};
    static uint8_t cnt = 0;

    if(cnt > (sizeof(input_data)/sizeof(input_data[0])))
        cnt = 0;

    h->transceiver_handler->pfRxDataCallback(h, &input_data[cnt], 1);
    cnt++;
#endif

#ifdef TEST_3
    uint8_t input_data[] = {2, 0, 0, 0, 0};
    static uint8_t cnt = 0;

    if(cnt > (sizeof(input_data)/sizeof(input_data[0])))
        cnt = 0;

    h->transceiver_handler->pfRxDataCallback(h, &input_data[cnt], 1);
    cnt++;
#endif

#ifdef TEST_4
    uint8_t input_data[] = {1, 0, 0, 0, 1};
    static uint8_t cnt = 0;

    if(cnt > (sizeof(input_data)/sizeof(input_data[0])))
        cnt = 0;
    
    if(cnt < 1)
        h->transceiver_handler->pfRxDataCallback(h, &input_data[cnt], 1);
    else
        h->transceiver_handler->pfTxEmptyCallback(h, &input_data[cnt], 1);
    cnt++;
#endif
    
}