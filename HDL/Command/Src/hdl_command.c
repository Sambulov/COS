#include "hdl.h"

hdl_user_command_t *_hdl_command_find_command(hdl_command_t *h, uint8_t command) {
    hdl_user_command_t **command_array = h->cmd_array;
    hdl_user_command_t *rez = NULL;
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

void hdl_command_end_of_transaction_callback(void *context_base) {
    hdl_command_t *h = (hdl_command_t *)context_base;
    switch (h->state_machine) {
        case HDL_COMMAND_SM_COMMAND_PROC: {
            /* Call user function */
            h->current_cmd->command_transceiver.end_of_transmission(h->current_cmd->command_transceiver.proto_context);
        }
            break;
        case HDL_COMMAND_SM_ERROR: {
            hdl_command_end_of_execution(h);
        } 
            break;
        default:
            break;
    }
}

uint32_t hdl_command_tx_data_callback(void *context_base, uint8_t *data, uint32_t count) {
    hdl_command_t *h = (hdl_command_t *)context_base;
    if (h->state_machine == HDL_COMMAND_SM_COMMAND_PROC) {
        /* Call user function trnasfer */
        return h->current_cmd->command_transceiver.tx_empty(h->current_cmd->command_transceiver.proto_context, data, count);
    }
    return 0;
}

uint32_t hdl_command_tx_available_callback(void *context_base) {
    hdl_command_t *h = (hdl_command_t *)context_base;
    if (h->state_machine == HDL_COMMAND_SM_COMMAND_PROC) {
        /* Call user function trnasfer */
        return h->current_cmd->command_transceiver.tx_available(h->current_cmd->command_transceiver.proto_context);
    }
    return 0;
}

uint32_t hdl_command_rx_available_callback(void *context_base) {
    hdl_command_t *h = (hdl_command_t *)context_base;
    if (h->state_machine == HDL_COMMAND_SM_COMMAND_PROC) {
        /* Call user function trnasfer */
        return h->current_cmd->command_transceiver.rx_available(h->current_cmd->command_transceiver.proto_context);
    }
    return (h->state_machine == HDL_COMMAND_SM_WAITING_CMD) ? 1 : 0;
}

uint32_t hdl_command_rx_data_callback(void *context_base, uint8_t *data, uint32_t count) {
    hdl_command_t *h = (hdl_command_t *)context_base;
    int32_t command_byte = 0;
    switch (h->state_machine) {
        case HDL_COMMAND_SM_WAITING_CMD: {
            /* Find new command */
            h->current_cmd = NULL;
            h->current_cmd = _hdl_command_find_command(h, *data);
            if(h->current_cmd != NULL) {
                h->state_machine = HDL_COMMAND_SM_COMMAND_PROC;
                data++;
                count--;
                command_byte = 1;
            }
            else {
                h->state_machine = HDL_COMMAND_SM_ERROR;
                break;
            }          
        }
        case HDL_COMMAND_SM_COMMAND_PROC: {
            return command_byte + h->current_cmd->command_transceiver.rx_data(h->current_cmd->command_transceiver.proto_context, data, count) ;
        }
        default:
            break;
    }
    return count;
}

void hdl_command_end_of_execution(hdl_command_t *h) {
    h->current_cmd = NULL;
    h->state_machine = HDL_COMMAND_SM_WAITING_CMD;
}

hdl_transceiver_t *hdl_command_get_transiver(hdl_command_t *protocol) {
    if(protocol != NULL) {
        protocol->transceiver_handler.rx_data = &hdl_command_rx_data_callback;
        protocol->transceiver_handler.tx_empty = &hdl_command_tx_data_callback;
        protocol->transceiver_handler.end_of_transmission = &hdl_command_end_of_transaction_callback;
        protocol->transceiver_handler.tx_available = &hdl_command_tx_available_callback;
        protocol->transceiver_handler.rx_available = &hdl_command_rx_available_callback;
        protocol->transceiver_handler.proto_context = (void *)protocol;
        return &protocol->transceiver_handler;
    }
    return NULL;
}
