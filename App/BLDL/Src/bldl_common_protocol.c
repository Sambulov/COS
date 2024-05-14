#include "cl_transceiver_protocol.h"
#include "bldl.h"

void write_reg(uint32_t data, uint8_t address) {
    return;
}

uint32_t read_reg(uint8_t address) {
    return 1;
}

bldl_context_write_register_t register_write_context_led0 = {
   .current_reg_adr = 0x10,
   .write_reg = &write_reg,
};

bldl_context_read_register_t register_read_context_led0 = {
   .current_reg_adr = 0x11,
   .read_reg = &read_reg,
};

cl_protocol_command_t register_write_command_led0 = {
    .context = &register_write_context_led0,
    .cmd = BLDL_COMMON_PROTOCOL_CMD_WRITE,
};
cl_protocol_command_t register_read_command_led0 = {
    .context = &register_read_context_led0,
    .cmd = BLDL_COMMON_PROTOCOL_CMD_READ,
};

cl_protocol_transceiver_h spi_slave = {
    .cmd_array = cl_protocol_add_command(&register_read_command_led0, &register_write_command_led0),
};

cl_protocol_command_t *_common_protocol_find_command(cl_protocol_transceiver_h *h, uint8_t command) {
    cl_protocol_command_t **command_array = h->cmd_array;
    cl_protocol_command_t *rez = NULL;
    /* Find command number */
    while (*command_array != NULL) {
        if (command == (*command_array)->cmd) {
            rez = (*command_array);
            break;
        }
    }
    return rez;
}

void common_protocol_worker(cl_protocol_transceiver_h *h)
{
    bldl_common_protocol_state_machine_e *sm = &h->state_machine;
    cl_protocol_command_t *command = NULL;
    uint8_t new_value_from_interface = 1;
    h->state_machine = BLDL_COMMON_PROTOCOL_SM_WAITING_CMD;
    // if (sm == BLDL_COMMON_PROTOCOL_SM_INITAL)
    //     return;
    // if (h->transceiver_handler == NULL)
    //     return;
    // /* If we have`t goten new data, return */
    // if (h->transceiver_handler->pfRxDataCallback(&h->transceiver_handler->pxProtoDescriptor, &new_value_from_interface, 1) == 0)
    //     return;
    switch (h->state_machine)
    {
        case BLDL_COMMON_PROTOCOL_SM_WAITING_CMD: {
            command = _common_protocol_find_command(h, new_value_from_interface);
            if(command != NULL) {
                h->state_machine = BLDL_COMMON_PROTOCOL_SM_GETTING_ADDRESS;
                h->cmd = new_value_from_interface;
            }
        }
            break;
        case BLDL_COMMON_PROTOCOL_SM_GETTING_ADDRESS: {

        }
            break;

    default:
        break;
    }

    return;
}
