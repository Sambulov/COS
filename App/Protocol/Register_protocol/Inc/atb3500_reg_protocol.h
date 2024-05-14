#ifndef ATB3500_REG_PROTOCOL_H_
#define ATB3500_REG_PROTOCOL_H_

#include "cl_transceiver_reg_protocol.h"

typedef enum {
    BLDL_COMMON_PROTOCOL_SM_INITAL,
    BLDL_COMMON_PROTOCOL_SM_WAITING_CMD,
    BLDL_COMMON_PROTOCOL_SM_GETTING_ADDRESS,
    BLDL_COMMON_PROTOCOL_SM_GETTING_DATA_TRANSFER,
    BLDL_COMMON_PROTOCOL_SM_GETTING_ERROR,
} bldl_common_protocol_state_machine_e;
/* Value of command can be specific for other protocol */
typedef enum {
    BLDL_COMMON_PROTOCOL_CMD_READ = (uint8_t)1,
    BLDL_COMMON_PROTOCOL_CMD_WRITE = (uint8_t)2,
} bldl_common_protocol_cmd_e;

/* Register mood */
typedef enum {
    BLDL_COMMON_PROTOCOL_MODE_READ_ONLY = (uint8_t)1,
    BLDL_COMMON_PROTOCOL_WRITE_ONLY = (uint8_t)2,
    BLDL_COMMON_PROTOCOL_MODE_READ_AND_WRITE = (uint8_t)3,
} bldl_common_protocol_mode_e;

typedef struct BLDL_REG_CONTEXT_BASE{
    
} bldl_reg_context_t;

typedef struct {
    uint8_t current_reg_adr; /* register address */
    uint8_t cnt;             /* for example: if cnt == 4 call func write_reg */
    uint32_t new_value;      /* value will be write into address */
    void (*write_reg)(uint32_t data, uint8_t address);
} bldl_context_write_register_t;

typedef struct
{
    uint8_t current_reg_adr; /* register address */
    uint8_t cnt;             /* if cnt == 4 call write_reg */
    uint32_t reg_value;
    uint32_t (*read_reg)(uint8_t address);
} bldl_context_read_register_t;

typedef struct
{
    uint8_t cmd[3];
    uint8_t current_reg_adr; /* register address */
    uint8_t cnt;             /* if cnt == 4 call write_reg */
    uint32_t reg_value;
    uint32_t (*read_reg)(uint8_t address);
} bldl_context_4byte_cmd_register_t;

void common_protocol_worker(cl_reg_protocol_transceiver_h *h);

#endif