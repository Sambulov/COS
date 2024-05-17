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
                    if(user_context->cnt_addres < REG_PROTOCOL_AMOUNT_ADDRESS_BYTE) {
                        p_tmp = (uint8_t *)&user_context->reg_address;
                        p_tmp[user_context->cnt_addres] = data[amount_of_processed_bytes];
                        user_context->cnt_addres++;
                        amount_of_processed_bytes++;
                        count--;
                    }
                    if(user_context->cnt_addres == REG_PROTOCOL_AMOUNT_ADDRESS_BYTE)
                        user_context->state_machine = REG_PROTOCOL_SM_WRITE_REQUEST;
                    else if(user_context->cnt_addres > REG_PROTOCOL_AMOUNT_ADDRESS_BYTE)
                        user_context->state_machine = REG_PROTOCOL_SM_ERROR;
                }
                    break;
                case REG_PROTOCOL_SM_WRITE_REQUEST: {
                    /* Skip this data */
                    return amount_of_processed_bytes + count;
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
    cl_reg_protocol_context_base_t *context = (cl_reg_protocol_context_base_t *)context_base;
    reg_protocol_atb3500_read_reg_contex_t *user_context = (reg_protocol_atb3500_read_reg_contex_t *)context->user_context;
    int16_t available_bytes = (int16_t)max_available_bytes;
    int32_t index = 0;
    uint8_t *p_tmp = NULL;
    if(context_base != NULL) {
        switch (user_context->state_machine) {
            case REG_PROTOCOL_SM_WRITE_REQUEST: {
                while (available_bytes >= REG_PROTOCOL_AMOUNT_REGISTER_BYTE) {
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
        user_context->cnt_addres = 0;
        user_context->cnt_reg_value = 0;
        user_context->reg_value = 0;
        user_context->reg_address = 0;
    }
}

int32_t reg_protocol_atb3500_cmd_write_tx_data_callback(void *context_base, uint8_t *data, uint16_t count) {
    return 0;
}

int32_t reg_protocol_atb3500_cmd_write_rx_data_callback(void* context_base, uint8_t *data, uint16_t count) {
    cl_reg_protocol_context_base_t *context = (cl_reg_protocol_context_base_t *)context_base;
    reg_protocol_atb3500_write_reg_contex_t *user_context = (reg_protocol_atb3500_write_reg_contex_t *)context->user_context;       
    int32_t amount_of_processed_bytes = 0;
    uint8_t *p_tmp = NULL;
    if(context_base != NULL || count == 0) {  
        while (count != 0) {
            switch (user_context->state_machine) {
                case REG_PROTOCOL_SM_COMPLETED:
                case REG_PROTOCOL_SM_INITIAL: {
                    _reset_field_write_reg_context(user_context);
                    user_context->state_machine = REG_PROTOCOL_SM_WAITING_ADDRESS;
                }
                    break;
                case REG_PROTOCOL_SM_WAITING_ADDRESS: { 
                    /* Write data into register_address */
                    if(user_context->cnt_addres < REG_PROTOCOL_AMOUNT_ADDRESS_BYTE) {
                        p_tmp = (uint8_t *)&user_context->reg_address;
                        p_tmp[user_context->cnt_addres] = data[amount_of_processed_bytes];
                        user_context->cnt_addres++;
                        amount_of_processed_bytes++;
                        count--;
                    }

                    if(user_context->cnt_addres == REG_PROTOCOL_AMOUNT_ADDRESS_BYTE)
                        user_context->state_machine = REG_PROTOCOL_SM_WAITING_REGISTER;
                    else if(user_context->cnt_addres > REG_PROTOCOL_AMOUNT_ADDRESS_BYTE)
                        user_context->state_machine = REG_PROTOCOL_SM_ERROR;
                }
                    break;
                case REG_PROTOCOL_SM_WAITING_REGISTER: {
                    /* Write data into register value */
                    if(user_context->cnt_reg_value < REG_PROTOCOL_AMOUNT_REGISTER_BYTE) {
                        p_tmp = (uint8_t *)&user_context->reg_value;
                        p_tmp[user_context->cnt_reg_value] = data[amount_of_processed_bytes];
                        user_context->cnt_reg_value++;
                        amount_of_processed_bytes++;
                        count--;
                    }
                    if(user_context->cnt_reg_value == REG_PROTOCOL_AMOUNT_REGISTER_BYTE) {
                        user_context->cnt_reg_value = 0;
                        if(user_context->write_reg(&user_context->reg_value, user_context->reg_address, REG_PROTOCOL_ATB3500_WRITE_REG)) {
                            user_context->state_machine = REG_PROTOCOL_SM_ERROR;
                            break;
                        }
                        user_context->reg_address++;
                    }
                    else if(user_context->cnt_reg_value > REG_PROTOCOL_AMOUNT_REGISTER_BYTE)
                        user_context->state_machine = REG_PROTOCOL_SM_ERROR;
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

extern cl_reg_protocol_transceiver_h spi_slave;

hdl_transceiver_t spi_slave_trasceiver = {
    .rx_data = &cl_protocol_reg_rx_data_callback,
    .tx_empty = &cl_protocol_reg_tx_data_callback,
    .end_of_transmission = &cl_protocol_reg_end_of_transaction_callback,
    .proto_context = &spi_slave,
};

cl_reg_protocol_transceiver_h spi_slave = {
    .transceiver_handler = &spi_slave_trasceiver,
    .cmd_array = cl_reg_protocol_add_command(&reg_protocol_atb3500_cmd_read_4_byte_reg, &reg_protocol_atb3500_cmd_write_4_byte_reg),
    .state_machine = 0,
};


