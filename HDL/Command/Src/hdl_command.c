#include "hdl.h"

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

void cl_protocol_reg_end_of_transaction_callback(void *context_base) {
    cl_reg_protocol_transceiver_h *h = (cl_reg_protocol_transceiver_h *)context_base;
    switch (h->state_machine) {
        case CL_REG_PROTOCOL_SM_COMMAND_PROC: {
            h->current_cmd->context_base.state = CL_REG_CONTEXT_SM_COMPLETED;
            /* Call user function */
            h->current_cmd->end_of_transaction_callback(&h->current_cmd->context_base);
        }
            break;
        default:
            break;
    }
    h->current_cmd = NULL;
    h->state_machine = CL_REG_PROTOCOL_SM_WAITING_CMD;
    return;
}

int32_t cl_protocol_reg_tx_data_callback(void *context_base, uint8_t *data, uint16_t count) {
    cl_reg_protocol_transceiver_h *h = (cl_reg_protocol_transceiver_h *)context_base;
    switch (h->state_machine) {
        case CL_REG_PROTOCOL_SM_COMMAND_PROC: {
            /* Check current state machine in the context */
            if(h->current_cmd->context_base.state == CL_REG_CONTEXT_SM_PROC) {
                /* Call user function trnasfer */
                return h->current_cmd->tx_data_callback(&h->current_cmd->context_base, data, count);
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
    int32_t number_of_obtained_bytes = 0;
    while (count != 0) {
        switch (h->state_machine) {
            case CL_REG_PROTOCOL_SM_WAITING_CMD: {
                /* Find new command */
                h->current_cmd = NULL;
                h->current_cmd = _reg_protocol_find_command(h, *data);
                count--;
                number_of_obtained_bytes++;
                if(h->current_cmd != NULL) {
                    h->state_machine = CL_REG_PROTOCOL_SM_COMMAND_PROC;
                    /* Switch context state */
                     h->current_cmd->context_base.state = CL_REG_CONTEXT_SM_PROC;
                }
                else
                    h->state_machine = CL_REG_PROTOCOL_SM_ERROR;
            }
                break;
            case CL_REG_PROTOCOL_SM_COMMAND_PROC: {
                int32_t obtained_byte_by_command = h->current_cmd->rx_data_callback(&h->current_cmd->context_base, &data[number_of_obtained_bytes], count);
                number_of_obtained_bytes += obtained_byte_by_command;
                return number_of_obtained_bytes;
            }
                break;
            default:
                return number_of_obtained_bytes + count;
                break;
        }
    }
    return count;
}
