#ifndef BLDL_COMMON_PROTOCOL
#define BLDL_COMMON_PROTOCOL

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

#endif