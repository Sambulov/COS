#include "atb3500_reg_protocol.h"

#define REG_ADDRESS_1        (uint8_t)1

/* 0 - read, 1 - write*/
int32_t write_read(uint32_t *data, uint8_t address, reg_protocol_atb3500_read_write_reg_e option) {
    static uint32_t test_value = 10;
    if(option == REG_PROTOCOL_ATB3500_READ_REG) {
        switch (address)
        {
        case REG_ADDRESS_1:
            *data = 0xAABBCCDD;
            break;
        
        default:
            return 0;
            break;
        }
        return 1;
    }
    else if(option == REG_PROTOCOL_ATB3500_WRITE_REG) {
        switch (address)
        {
        case REG_ADDRESS_1:
            test_value = *data;
            break;
        
        default:
            return 0;
            break;
        }
        return 1;
    }
    else
        return 0;
}

/*********************************************************************************************
 * Description command: REG_PROTOCOL_ATB3500_READ_REG
**********************************************************************************************/
static void _reset_field_read_reg_context(reg_protocol_atb3500_read_reg_contex_t *user_context) {
    if (user_context != NULL) {
        user_context->cnt_addres = 0;
        user_context->reg_value = 0;
        user_context->reg_address = 0;
    }
}

int32_t reg_protocol_atb3500_cmd_read_rx_data_callback(void* context_base, uint8_t *data, uint16_t count) {
    cl_reg_protocol_context_base_t *context = (cl_reg_protocol_context_base_t *)context_base;
    reg_protocol_atb3500_read_reg_contex_t *user_context = (reg_protocol_atb3500_read_reg_contex_t *)context->user_context;       
    int32_t amount_of_processed_bytes = 0;
    uint8_t *p_tmp = NULL;
    if(context_base != NULL || count == 0) {  
        while (count != 0) {
            switch (user_context->state_machine) {
                case REG_PROTOCOL_SM_COMPLETED:
                case REG_PROTOCOL_SM_INITIAL: {
                    _reset_field_read_reg_context(user_context);
                    user_context->state_machine = REG_PROTOCOL_SM_WAITING_ADDRESS;
                }
                    break;
                case REG_PROTOCOL_SM_WAITING_ADDRESS: { 
                    /* Write data into register_address */
                    if(user_context->cnt_addres <= REG_PROTOCOL_AMOUNT_ADDRESS_BYTE) {
                        p_tmp = (uint8_t *)&user_context->reg_address;
                        p_tmp[user_context->cnt_addres] = data[user_context->cnt_addres];
                        user_context->cnt_addres++;
                        amount_of_processed_bytes++;
                        count--;
                    }
                    if(user_context->cnt_addres == REG_PROTOCOL_AMOUNT_ADDRESS_BYTE)
                        user_context->state_machine = REG_PROTOCOL_SM_WRITE_REQUEST;
                }
                    break;
                case REG_PROTOCOL_SM_WRITE_REQUEST: {
                    /* Skip this data */
                    amount_of_processed_bytes += count;
                }
                    break;
                default:
                    break;
            }
        }
    }
    return amount_of_processed_bytes;
}

int32_t reg_protocol_atb3500_cmd_read_tx_data_callback(void *context_base, uint8_t *data, uint16_t max_available_bytes) {
    cl_reg_protocol_context_base_t *context = (cl_reg_protocol_context_base_t *)context_base;
    reg_protocol_atb3500_read_reg_contex_t *user_context = (reg_protocol_atb3500_read_reg_contex_t *)context->user_context;
    int16_t available_bytes = (int16_t)max_available_bytes;
    int32_t index = 0;
    uint8_t *p_tmp = NULL;
    if(context_base != NULL) {
        switch (user_context->state_machine) {
            case REG_PROTOCOL_SM_WRITE_REQUEST: {
                while (available_bytes > REG_PROTOCOL_AMOUNT_REGISTER_BYTE) {
                    /* Mapping register address with register value */
                    if(user_context->read_reg(&user_context->reg_value, user_context->reg_address, REG_PROTOCOL_ATB3500_READ_REG)){
                        user_context->state_machine = REG_PROTOCOL_SM_ERROR;
                        break;
                    }
                    /* Update tx buffer */
                    p_tmp = (uint8_t *)&user_context->reg_value;
                    for(int32_t i = 0; i < REG_PROTOCOL_AMOUNT_REGISTER_BYTE; i++) {
                        data[index] = p_tmp[i];
                        index++;
                        available_bytes--;
                    }
                    user_context->reg_address++;
                }
            }
                break;
            default:
                break;
        }
    }
    return index;
}

int32_t reg_protocol_atb3500_cmd_read_end_of_transaction_callback(void *context_base) {
    if (context_base != NULL) {
        cl_reg_protocol_context_base_t *context = (cl_reg_protocol_context_base_t *)context_base;
        reg_protocol_atb3500_read_reg_contex_t *user_context = (reg_protocol_atb3500_read_reg_contex_t *)context->user_context;
        _reset_field_read_reg_context(user_context);
        user_context->state_machine = REG_PROTOCOL_SM_COMPLETED;
        context->state = CL_REG_CONTEXT_SM_COMPLETED;
    }
}

/*********************************************************************************************
 * Description command: REG_PROTOCOL_ATB3500_WRITE_REG
**********************************************************************************************/
static void _reset_field_write_reg_context(reg_protocol_atb3500_write_reg_contex_t *user_context) {
    if (user_context != NULL) {
        user_context->cnt = 0;
        user_context->reg_value = 0;
        user_context->reg_address = 0;
    }
}

int32_t reg_protocol_atb3500_cmd_write_tx_data_callback(void *context_base, uint8_t *data, uint16_t count) {
    return 0;
}

int32_t reg_protocol_atb3500_cmd_write_rx_data_callback(void* context_base, uint8_t *data, uint16_t count) {
    cl_reg_protocol_context_base_t *context = (cl_reg_protocol_context_base_t *)context_base;
    reg_protocol_atb3500_read_reg_contex_t *user_context = (reg_protocol_atb3500_read_reg_contex_t *)context->user_context;       
    int32_t amount_of_processed_bytes = 0;
    uint8_t *p_tmp = NULL;
    if(context_base != NULL || count == 0) {  
        while (count != 0) {
            switch (user_context->state_machine) {
                case REG_PROTOCOL_SM_COMPLETED:
                case REG_PROTOCOL_SM_INITIAL: {
                    _reset_field_read_reg_context(user_context);
                    user_context->state_machine = REG_PROTOCOL_SM_WAITING_ADDRESS;
                }
                    break;
                case REG_PROTOCOL_SM_WAITING_ADDRESS: { 
                    /* Write data into register_address */
                    if(user_context->cnt_addres <= REG_PROTOCOL_AMOUNT_ADDRESS_BYTE) {
                        p_tmp = (uint8_t *)&user_context->reg_address;
                        p_tmp[user_context->cnt_addres] = data[amount_of_processed_bytes];
                        user_context->cnt_addres++;
                        amount_of_processed_bytes++;
                        count--;
                    }
                    if(user_context->cnt_addres == REG_PROTOCOL_AMOUNT_ADDRESS_BYTE)
                        user_context->state_machine = REG_PROTOCOL_SM_WAITING_REGISTER;
                }
                    break;
                default:
                    /* Skip this data */
                    amount_of_processed_bytes += count;
                    break;
            }
        }
    }
    return amount_of_processed_bytes;
}

int32_t reg_protocol_atb3500_cmd_write_end_of_transaction_callback(void *context_base) {
    if (context_base != NULL) {
        cl_reg_protocol_context_base_t *context = (cl_reg_protocol_context_base_t *)context_base;
        reg_protocol_atb3500_write_reg_contex_t *user_context = (reg_protocol_atb3500_write_reg_contex_t *)context->user_context;
        _reset_field_write_reg_context(user_context);
        user_context->state_machine = REG_PROTOCOL_SM_COMPLETED;
        context->state = CL_REG_CONTEXT_SM_COMPLETED;
    }
}
/*********************************************************************************************
 * Binding
**********************************************************************************************/

reg_protocol_atb3500_read_reg_contex_t reg_protocol_atb3500_read_reg_context = {
    .read_reg = &write_read,
};
reg_protocol_atb3500_write_reg_contex_t reg_protocol_atb3500_write_reg_context = {
    .write_reg = &write_read,
};

cl_reg_protocol_command_t reg_protocol_atb3500_cmd_read_4_byte_reg = {
    .context_base.user_context = &reg_protocol_atb3500_read_reg_context,
    .unique_user_cmd = REG_PROTOCOL_ATB3500_CMD_READ_4_BYTE,
    .rx_data_callback = &reg_protocol_atb3500_cmd_read_rx_data_callback,
    .tx_data_callback = &reg_protocol_atb3500_cmd_read_tx_data_callback,
    .end_of_transaction_callback = &reg_protocol_atb3500_cmd_read_end_of_transaction_callback,
};
cl_reg_protocol_command_t reg_protocol_atb3500_cmd_write_4_byte_reg = {
    .context_base.user_context = &reg_protocol_atb3500_write_reg_context,
    .unique_user_cmd = REG_PROTOCOL_ATB3500_CMD_WRITE_4_BYTE,
    .rx_data_callback = &reg_protocol_atb3500_cmd_write_rx_data_callback,
    .tx_data_callback = &reg_protocol_atb3500_cmd_write_tx_data_callback,
    .end_of_transaction_callback = &reg_protocol_atb3500_cmd_write_end_of_transaction_callback,
};

TransceiverHandler_t spi_slave_trasceiver = {
    .pfRxDataCallback = &cl_protocol_reg_rx_data_callback,
    .pfTxEmptyCallback = &cl_protocol_reg_tx_data_callback,
};

cl_reg_protocol_transceiver_h spi_slave = {
    .transceiver_handler = &spi_slave_trasceiver,
    .cmd_array = cl_reg_protocol_add_command(&reg_protocol_atb3500_cmd_read_4_byte_reg, &reg_protocol_atb3500_cmd_write_4_byte_reg),
};


