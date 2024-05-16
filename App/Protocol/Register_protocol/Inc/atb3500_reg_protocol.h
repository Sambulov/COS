#ifndef ATB3500_REG_PROTOCOL_H_
#define ATB3500_REG_PROTOCOL_H_

/* Protocol */
/*
* Byte amount [ 1 ] [   1   ] [  1  ] [   4    ]
* Read 4 byte [CMD] [ADDRESS] [DUMMY] [REGISTER] [STOP CONDITIONAL]
*/

#define REG_PROTOCOL_AMOUNT_ADDRESS_BYTE            ((int16_t) 1)
#define REG_PROTOCOL_AMOUNT_REGISTER_BYTE           ((int16_t) 4)



/* Register mood */
typedef enum {
    REG_PROTOCOL_ATB3500_CMD_READ_4_BYTE = (uint8_t)1,
    REG_PROTOCOL_ATB3500_CMD_WRITE_4_BYTE = (uint8_t)2,
} reg_protocol_atb3500_cmd_e;

typedef enum {
    REG_PROTOCOL_ATB3500_READ_REG,
    REG_PROTOCOL_ATB3500_WRITE_REG,
} reg_protocol_atb3500_read_write_reg_e;

typedef enum {
    REG_PROTOCOL_SM_INITIAL,
    REG_PROTOCOL_SM_WAITING_ADDRESS,
    REG_PROTOCOL_SM_WAITING_REGISTER,
    REG_PROTOCOL_SM_WRITE_REQUEST,
    REG_PROTOCOL_SM_COMPLETED,
    REG_PROTOCOL_SM_ERROR,
} reg_protocol_state_machine_e;

typedef struct {
    reg_protocol_state_machine_e state_machine;
    uint32_t reg_address; /* buffer for saving register adrress */
    int16_t cnt_addres;     /* buffer for saving rx bytes counter */
    uint32_t reg_value;  /* buffer for saving new reg value */
    int32_t (*read_reg)(uint32_t *data, uint8_t address, reg_protocol_atb3500_read_write_reg_e option);
} reg_protocol_atb3500_read_reg_contex_t;

typedef struct {
    reg_protocol_state_machine_e state_machine;
    uint8_t reg_address; /* buffer for saving register adrress */
    uint8_t cnt; /* buffer for saving byte transfer */
    uint32_t reg_value;  /* buffer for saving new reg value */
    int32_t (*write_reg)(uint32_t *data, uint8_t address, reg_protocol_atb3500_read_write_reg_e option);
} reg_protocol_atb3500_write_reg_contex_t;


#endif