#ifndef ATB3500_REG_PROTOCOL_H_
#define ATB3500_REG_PROTOCOL_H_

#include "cl_transceiver_reg_protocol.h"

/* Register mood */
typedef enum {
    REG_PROTOCOL_ATB3500_CMD_READ_4_BYTE = (uint8_t)1,
    REG_PROTOCOL_ATB3500_CMD_WRITE_4_BYTE = (uint8_t)2,
} reg_protocol_atb3500_cmd_e;

typedef enum {
    REG_PROTOCOL_ATB3500_READ_REG,
    REG_PROTOCOL_ATB3500_WRITE_REG,
} reg_protocol_atb3500_read_write_reg_e;

typedef struct {
    uint8_t reg_address; /* buffer for saving register adrress */
    uint8_t cnt;         /* buffer for saving byte transfer */
    uint32_t new_value;  /* buffer for saving new reg value */
    void (*write_read_reg)(uint32_t *data, uint8_t address, reg_protocol_atb3500_read_write_reg_e option);
} reg_protocol_atb3500_read_write_reg_contex_t;



#endif