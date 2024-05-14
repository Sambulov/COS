#include "cl_transceiver_reg_protocol.h"

cl_reg_protocol_command_t *_reg_protocol_find_command(cl_reg_protocol_transceiver_h *h, uint8_t command) {
    cl_reg_protocol_command_t **command_array = h->cmd_array;
    cl_reg_protocol_command_t *rez = NULL;
    /* Find command number */
    while (*command_array != NULL) {
        if (command == (*command_array)->unique_user_cmd) {
            rez = (*command_array);
            break;
        }
    }
    return rez;
}

void cl_protocol_transceiver_worker(cl_reg_protocol_transceiver_h *h) {
    cl_reg_protocol_command_t *command = NULL;
    uint8_t new_value_from_interface;
    uint8_t end_of_transaction = 0;
    /* If we have`t goten new data, return */
    if(h->transceiver_handler->pfEndOfTransactionCallback(&h->transceiver_handler->pxProtoDescriptor) == 0)
        end_of_transaction = 1;
    else if (h->transceiver_handler->pfRxDataCallback(&h->transceiver_handler->pxProtoDescriptor, &new_value_from_interface, 1) == 0)
        return;
        
    if(h->state_machine == CL_REG_PROTOCOL_SM_ERROR)
        h->state_machine = CL_REG_PROTOCOL_SM_WAITING_CMD;
    switch (h->state_machine)
    {
        case CL_REG_PROTOCOL_SM_WAITING_CMD: {
            command = _reg_protocol_find_command(h, new_value_from_interface);
            if(command != NULL) {
                h->state_machine = CL_REG_PROTOCOL_SM_COMMAND_PROC;
                h->current_cmd = new_value_from_interface;
            }
        }
            break;
        case CL_REG_PROTOCOL_SM_COMMAND_PROC: {
            command = _reg_protocol_find_command(h, h->current_cmd);
            if(end_of_transaction) {
                /* Call user function */
                command->end_of_transaction_callback(&command->context);
                h->current_cmd = 0;
                h->state_machine = CL_REG_PROTOCOL_SM_WAITING_CMD;
                break;
            }         
            /* Check current state machine in the context */
            if(command->context.state == CL_REG_PROTOCOL_CONTEXT_SM_WAITING_ADDRESS ||
                command->context.state == CL_REG_PROTOCOL_CONTEXT_SM_PROC) 
            {
                /* Call user function trnasfer */
                command->rx_data_callback(&command->context, &new_value_from_interface, 1, h->current_cmd);
                /* Switch state machine, if transfer complete */
                if(command->context.state == CL_REG_PROTOCOL_CONTEXT_SM_COMPLETED || 
                        command->context.state == CL_REG_PROTOCOL_CONTEXT_SM_ERROR)
                {
                    h->current_cmd = 0;
                    h->state_machine = CL_REG_PROTOCOL_SM_WAITING_CMD;
                }
            }
            else {
                h->current_cmd = 0;
                h->state_machine = CL_REG_PROTOCOL_SM_WAITING_CMD;
            }
        }
            break;
    default:
        break;
    }
    return;
}