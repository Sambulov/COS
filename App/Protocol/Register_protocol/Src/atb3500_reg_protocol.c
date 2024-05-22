#include "CodeLib.h"
#include "app.h"

#define REG_ADDRESS_1        (uint32_t)1
#define REG_ADDRESS_2        (uint32_t)2
#define REG_ADDRESS_3        (uint32_t)3
#define REG_ADDRESS_4        (uint32_t)4
#define REG_ADDRESS_5        (uint32_t)5
#define REG_ADDRESS_6        (uint32_t)6
#define REG_ADDRESS_7        (uint32_t)7
#define REG_ADDRESS_8        (uint32_t)8
#define REG_ADDRESS_9        (uint32_t)9
#define REG_ADDRESS_10        (uint32_t)10

/*!
    \brief          Mapping register address with value
    \param[in]      reg_value
    \param[in]      address
    \param[in]      option
      \arg            REG_PROTOCOL_ATB3500_READ_REG
      \arg            REG_PROTOCOL_ATB3500_WRITE_REG
    \return         
      \retval         0 - OK
      \retval         1 - ERROR
 */
int32_t write_read(uint32_t *reg_value, uint32_t address, reg_protocol_atb3500_read_write_reg_e option) {
    static uint32_t test_value = 10;
    if(option == REG_PROTOCOL_ATB3500_READ_REG) {
        switch (address)
        {
        case REG_ADDRESS_1:
            *reg_value = 0x1ABBCCDD;
            break;
        case REG_ADDRESS_2:
            *reg_value = 0x2ABBCCDD;
            break;
        case REG_ADDRESS_3:
            *reg_value = 0x3ABBCCDD;
            break;
        case REG_ADDRESS_4:
            *reg_value = 0x4ABBCCDD;
            break;
        case REG_ADDRESS_5:
            *reg_value = 0x5ABBCCDD;
            break;
        case REG_ADDRESS_6:
            *reg_value = 0x6ABBCCDD;
            break;
        case REG_ADDRESS_7:
            *reg_value = 0x7ABBCCDD;
            break;
        case REG_ADDRESS_8:
            *reg_value = 0x8ABBCCDD;
            break;
        case REG_ADDRESS_9:
            *reg_value = 0x9ABBCCDD;
            break;
        case REG_ADDRESS_10:
            *reg_value = 0xAABBCCDD;
            break;        
        default:
            return 1;
            break;
        }
        return 0;
    }
    else if(option == REG_PROTOCOL_ATB3500_WRITE_REG) {
        switch (address)
        {
        case REG_ADDRESS_1:
            test_value = *reg_value;
            break;
        
        default:
            return 1;
            break;
        }
        return 0;
    }
    else
        return 1;
}

/*********************************************************************************************
 * Description command: REG_PROTOCOL_ATB3500_READ_REG
**********************************************************************************************/
static void _reset_field_read_reg_context(reg_protocol_atb3500_read_reg_contex_t *context) {
    if (context != NULL) {
        context->cnt_addres = 0;
        context->cnt_reg_value = 0;
        context->reg_value = 0;
        context->reg_address = 0;
        context->request_len = 0;
    }
}

int32_t reg_protocol_atb3500_cmd_read_rx_data_callback(void* context_base, uint8_t *data, uint16_t count) {
    reg_protocol_atb3500_read_reg_contex_t *context = (reg_protocol_atb3500_read_reg_contex_t *)context_base;  
    int32_t amount_of_processed_bytes = 0;
    uint8_t *p_tmp = NULL;
    if(context_base != NULL || count == 0) {  
        while (count != 0) {
            switch (context->state_machine) {
                case REG_PROTOCOL_SM_COMPLETED:
                case REG_PROTOCOL_SM_INITIAL: {
                    _reset_field_read_reg_context(context);
                    context->state_machine = REG_PROTOCOL_SM_WAITING_ADDRESS;
                }
                    break;
                case REG_PROTOCOL_SM_WAITING_ADDRESS: { 
                    /* Write data into register_address */
                    if(context->cnt_addres < REG_PROTOCOL_ADDRESS_SIZE) {
                        p_tmp = (uint8_t *)&context->reg_address;
                        p_tmp[context->cnt_addres] = data[amount_of_processed_bytes];
                        context->cnt_addres++;
                        amount_of_processed_bytes++;
                        count--;
                    }
                    if(context->cnt_addres == REG_PROTOCOL_ADDRESS_SIZE)
                        context->state_machine = REG_PROTOCOL_SM_WAITING_LEN;
                    else if(context->cnt_addres > REG_PROTOCOL_ADDRESS_SIZE)
                        context->state_machine = REG_PROTOCOL_SM_ERROR;
                }
                    break;
                case REG_PROTOCOL_SM_WAITING_LEN : {
                    if (context->cnt_request_len < REG_PROTOCOL_LEN_SIZE) {
                        p_tmp = (uint8_t *)&context->request_len;
                        p_tmp[context->cnt_request_len] = data[amount_of_processed_bytes];
                        context->cnt_request_len++;
                        amount_of_processed_bytes++;
                        count--;
                    }
                    if(context->cnt_request_len == REG_PROTOCOL_LEN_SIZE) {
                        context->state_machine = REG_PROTOCOL_SM_WRITE_REQUEST;
                    }
                    else if(context->cnt_request_len > REG_PROTOCOL_LEN_SIZE)
                        context->state_machine = REG_PROTOCOL_SM_ERROR;
                }
                    break;
                case REG_PROTOCOL_SM_WRITE_REQUEST: {
                    /* Skip this data */
                    context->state_machine = REG_PROTOCOL_SM_INITIAL;
                    break;
                }
                    break;
                default:
                    return amount_of_processed_bytes + count;
                    break;
            }
        }
    }
    return amount_of_processed_bytes;
}

int32_t reg_protocol_atb3500_cmd_read_tx_data_callback(void *context_base, uint8_t *data, uint16_t max_available_bytes) {
    reg_protocol_atb3500_read_reg_contex_t *context = (reg_protocol_atb3500_read_reg_contex_t *)context_base;
    int16_t available_bytes = (int16_t)max_available_bytes;
    int32_t amount = 0;
    uint8_t *p_tmp = NULL;
    if(context_base != NULL || max_available_bytes == 0) {
        switch (context->state_machine) {
            case REG_PROTOCOL_SM_WRITE_REQUEST: {
                while (available_bytes--) {
                    if(context->cnt_reg_value == 0) {
                        /* Mapping register address with register value */
                        if(context->read_reg(&context->reg_value, context->reg_address, REG_PROTOCOL_ATB3500_READ_REG)){
                            context->state_machine = REG_PROTOCOL_SM_ERROR;
                        }
                        else {
                            context->reg_address++;
                            context->cnt_reg_value = REG_PROTOCOL_REGISTER_SIZE;
                        }
                    }
                    if(context->cnt_reg_value != 0) {
                        *data = context->reg_value;
                        data++;
                        amount++;
                        context->cnt_reg_value--;
                        context->reg_value >>= 8;
                    }
                    else
                        break;
                }
            }
                break;
            default:
                break;
        }
    }
    return amount;
}

void reg_protocol_atb3500_cmd_read_end_of_transaction_callback(void *context_base) {
    if (context_base != NULL) {
        reg_protocol_atb3500_read_reg_contex_t *context = (reg_protocol_atb3500_read_reg_contex_t *)context_base;
        _reset_field_read_reg_context(context);
        context->state_machine = REG_PROTOCOL_SM_COMPLETED;
    }
}
int32_t reg_protocol_atb3500_cmd_read_rx_available_callback(void *context_base) {
    return 0xffff;
}

int32_t reg_protocol_atb3500_cmd_read_tx_available_callback(void *context_base) {
    reg_protocol_atb3500_read_reg_contex_t *context = (reg_protocol_atb3500_read_reg_contex_t *)context_base;
    uint8_t *p_tmp = NULL;
    int32_t amount = 0;
    uint32_t tmp_reg;
    if (context_base != NULL) {
        if (context->state_machine == REG_PROTOCOL_SM_WRITE_REQUEST) {
            amount = context->cnt_reg_value;
            if (context->cnt_reg_value == 0) {
                /* Mapping register address with register value */
                if (!context->read_reg(&tmp_reg, context->reg_address, REG_PROTOCOL_ATB3500_READ_REG))
                    amount = REG_PROTOCOL_REGISTER_SIZE;
            }
        }
    }
    return amount;
}

/*********************************************************************************************
 * Description command: REG_PROTOCOL_ATB3500_WRITE_REG
**********************************************************************************************/
static void _reset_field_write_reg_context(reg_protocol_atb3500_write_reg_contex_t *context) {
    if (context != NULL) {
        context->cnt_addres = 0;
        context->cnt_reg_value = 0;
        context->reg_value = 0;
        context->reg_address = 0;
    }
}

int32_t reg_protocol_atb3500_cmd_write_tx_data_callback(void *context_base, uint8_t *data, uint16_t count) {
    return 0;
}

int32_t reg_protocol_atb3500_cmd_write_rx_data_callback(void* context_base, uint8_t *data, uint16_t count) {
    reg_protocol_atb3500_write_reg_contex_t *context = (reg_protocol_atb3500_write_reg_contex_t *)context_base;
    int32_t amount_of_processed_bytes = 0;
    uint8_t *p_tmp = NULL;
    if(context != NULL || count == 0) {  
        while (count != 0) {
            switch (context->state_machine) {
                case REG_PROTOCOL_SM_COMPLETED:
                case REG_PROTOCOL_SM_INITIAL: {
                    _reset_field_write_reg_context(context);
                    context->state_machine = REG_PROTOCOL_SM_WAITING_ADDRESS;
                }
                    break;
                case REG_PROTOCOL_SM_WAITING_ADDRESS: { 
                    /* Write data into register_address */
                    if(context->cnt_addres < REG_PROTOCOL_ADDRESS_SIZE) {
                        p_tmp = (uint8_t *)&context->reg_address;
                        p_tmp[context->cnt_addres] = data[amount_of_processed_bytes];
                        context->cnt_addres++;
                        amount_of_processed_bytes++;
                        count--;
                    }

                    if(context->cnt_addres == REG_PROTOCOL_ADDRESS_SIZE)
                        context->state_machine = REG_PROTOCOL_SM_WAITING_REGISTER;
                    else if(context->cnt_addres > REG_PROTOCOL_ADDRESS_SIZE)
                        context->state_machine = REG_PROTOCOL_SM_ERROR;
                }
                    break;
                case REG_PROTOCOL_SM_WAITING_REGISTER: {
                    /* Write data into register value */
                    if(context->cnt_reg_value < REG_PROTOCOL_REGISTER_SIZE) {
                        p_tmp = (uint8_t *)&context->reg_value;
                        p_tmp[context->cnt_reg_value] = data[amount_of_processed_bytes];
                        context->cnt_reg_value++;
                        amount_of_processed_bytes++;
                        count--;
                    }
                    if(context->cnt_reg_value == REG_PROTOCOL_REGISTER_SIZE) {
                        context->cnt_reg_value = 0;
                        if(context->write_reg(&context->reg_value, context->reg_address, REG_PROTOCOL_ATB3500_WRITE_REG)) {
                            context->state_machine = REG_PROTOCOL_SM_ERROR;
                            break;
                        }
                        context->reg_address++;
                    }
                    else if(context->cnt_reg_value > REG_PROTOCOL_REGISTER_SIZE)
                        context->state_machine = REG_PROTOCOL_SM_ERROR;
                }
                    break;
                default:
                    /* Skip this data */
                    return amount_of_processed_bytes + count;
                    break;
            }
        }
    }
    return amount_of_processed_bytes;
}

void reg_protocol_atb3500_cmd_write_end_of_transaction_callback(void *context_base) {
    if (context_base != NULL) {
        reg_protocol_atb3500_write_reg_contex_t *context = (reg_protocol_atb3500_write_reg_contex_t *)context_base;
        _reset_field_write_reg_context(context);
        context->state_machine = REG_PROTOCOL_SM_COMPLETED;
    }
}

int32_t reg_protocol_atb3500_cmd_write_rx_available_callback(void *context_base) {
    return 0xffff;
}

int32_t reg_protocol_atb3500_cmd_write_tx_available_callback(void *context_base) {
    return 0;
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

hdl_user_command_t reg_protocol_atb3500_cmd_read_4_byte_reg = {
    .unique_user_cmd = REG_PROTOCOL_ATB3500_CMD_READ_4_BYTE,
    .command_transceiver.proto_context = &reg_protocol_atb3500_read_reg_context,
    .command_transceiver.end_of_transmission = &reg_protocol_atb3500_cmd_read_end_of_transaction_callback,
    .command_transceiver.rx_available = &reg_protocol_atb3500_cmd_read_rx_available_callback,
    .command_transceiver.rx_data = &reg_protocol_atb3500_cmd_read_rx_data_callback,
    .command_transceiver.tx_available = &reg_protocol_atb3500_cmd_read_tx_available_callback,
    .command_transceiver.tx_empty = &reg_protocol_atb3500_cmd_read_tx_data_callback,
};
hdl_user_command_t reg_protocol_atb3500_cmd_write_4_byte_reg = {
    .unique_user_cmd = REG_PROTOCOL_ATB3500_CMD_WRITE_4_BYTE,
    .command_transceiver.proto_context =&reg_protocol_atb3500_write_reg_context,
    .command_transceiver.end_of_transmission = &reg_protocol_atb3500_cmd_write_end_of_transaction_callback,
    .command_transceiver.rx_available = &reg_protocol_atb3500_cmd_write_rx_available_callback,
    .command_transceiver.rx_data = &reg_protocol_atb3500_cmd_write_rx_data_callback,
    .command_transceiver.tx_available = &reg_protocol_atb3500_cmd_write_tx_available_callback,
    .command_transceiver.tx_empty = &reg_protocol_atb3500_cmd_write_tx_data_callback,
};


hdl_command_t spi_slave = {
    .cmd_array = hdl_commands(&reg_protocol_atb3500_cmd_read_4_byte_reg, &reg_protocol_atb3500_cmd_write_4_byte_reg),
};


