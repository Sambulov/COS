#include "atb3500_reg_protocol.h"

#define REG_ADDRESS_1        (uint8_t)1


/* 0 - read, 1 - write*/
void read_write(uint32_t *data, uint8_t address, uint8_t direction) {
    // todo direction
    switch (address)
    {
    case REG_ADDRESS_1:

    *data = 10;
    break;

    default:
        *data = -1;
    }
}

// void write_reg(uint32_t data, uint8_t address) {
//     read_write(&data, address, 1);
// }

// uint32_t read_reg(uint8_t address) {
//     uint32_t data;
//     read_write(&data, address, 0);
//     return data;
// }

reg_protocol_atb3500_read_write_reg_contex_t reg_protocol_atb3500_read_write_reg_context = {
    .write_read_reg = &read_write,
};

static void _reset_field_read_write_reg_context(reg_protocol_atb3500_read_write_reg_contex_t *user_context) {
    if (user_context != NULL) {
        user_context->cnt = 0;
        user_context->new_value = 0;
        user_context->reg_address = 0;
    }
}
int32_t reg_protocol_atb3500_cmd_read_write_rx_data_callback(void* context_base, uint8_t *data, uint16_t len, uint8_t unique_user_cmd) {
    // TODO: if len > 1 ????
    if(context_base != NULL) {
        cl_reg_protocol_context_base_t *context = (cl_reg_protocol_context_base_t *)context_base;
        reg_protocol_atb3500_read_write_reg_contex_t *user_context = (reg_protocol_atb3500_read_write_reg_contex_t *)context->user_context;
        switch (context->state)
        {
        case CL_REG_PROTOCOL_CONTEXT_SM_WAITING_ADDRESS:
        case CL_REG_PROTOCOL_CONTEXT_SM_COMPLETED:
        case CL_REG_PROTOCOL_CONTEXT_SM_ERROR:
        {
            user_context->reg_address = *data;
            context->current_cmd = unique_user_cmd;
            context->state = CL_REG_PROTOCOL_SM_COMMAND_PROC;
        }
            break;
        case CL_REG_PROTOCOL_CONTEXT_SM_PROC:
        {
            /* 4 byte register */
            if(user_context->cnt <= 3){
                uint8_t *p_tmp = (uint8_t *)&user_context->new_value;
                p_tmp[user_context->cnt] = *data;
                user_context->cnt++;
            }
            if(user_context->cnt == 4){
                reg_protocol_atb3500_read_write_reg_e read_write_option = REG_PROTOCOL_ATB3500_READ_REG;
                if(context->current_cmd == REG_PROTOCOL_ATB3500_CMD_WRITE_4_BYTE)
                    read_write_option = REG_PROTOCOL_ATB3500_WRITE_REG;
                else if (context->current_cmd == REG_PROTOCOL_ATB3500_CMD_READ_4_BYTE)
                    read_write_option = REG_PROTOCOL_ATB3500_READ_REG;
                else {
                    context->state = CL_REG_PROTOCOL_SM_ERROR;
                    _reset_field_read_write_reg_context(user_context);
                    break;
                }
                /* Call function with register mapping*/
                user_context->write_read_reg(&user_context->new_value, user_context->reg_address, read_write_option);
                context->state = CL_REG_PROTOCOL_CONTEXT_SM_COMPLETED;
                _reset_field_read_write_reg_context(user_context);
            }
        }
            break;

        default:
            break;
        }
    }
    return 0;
}

int32_t reg_protocol_atb3500_cmd_read_write_end_of_transaction_callback(void *context_base) {
    if (context_base != NULL) {
        cl_reg_protocol_context_base_t *context = (cl_reg_protocol_context_base_t *)context_base;
        reg_protocol_atb3500_read_write_reg_contex_t *user_context = (reg_protocol_atb3500_read_write_reg_contex_t *)context->user_context;
        _reset_field_read_write_reg_context(user_context);
        context->state = CL_REG_PROTOCOL_CONTEXT_SM_COMPLETED;
    }
}

cl_reg_protocol_command_t reg_protocol_atb3500_cmd_read_4_byte_reg = {
    .context.user_context = &reg_protocol_atb3500_read_write_reg_context,
    .unique_user_cmd = REG_PROTOCOL_ATB3500_CMD_READ_4_BYTE,
    .rx_data_callback = &reg_protocol_atb3500_cmd_read_write_rx_data_callback,
    .end_of_transaction_callback = &reg_protocol_atb3500_cmd_read_write_end_of_transaction_callback,
};
cl_reg_protocol_command_t reg_protocol_atb3500_cmd_write_4_byte_reg = {
    .context.user_context = &reg_protocol_atb3500_read_write_reg_context,
    .unique_user_cmd = REG_PROTOCOL_ATB3500_CMD_WRITE_4_BYTE,
    .rx_data_callback = &reg_protocol_atb3500_cmd_read_write_rx_data_callback,
    .end_of_transaction_callback = &reg_protocol_atb3500_cmd_read_write_end_of_transaction_callback,
};

cl_reg_protocol_transceiver_h spi_slave = {
    .cmd_array = cl_reg_protocol_add_command(&reg_protocol_atb3500_cmd_read_4_byte_reg, &reg_protocol_atb3500_cmd_write_4_byte_reg),
};


