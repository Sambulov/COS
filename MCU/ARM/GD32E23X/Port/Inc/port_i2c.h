#ifndef PORT_I2C_H_
#define PORT_I2C_H_

#define HDL_I2C_HAL_PRV_SIZE                16
#define HDL_I2C_HW_MESSAGE_PRV_SIZE         8

typedef struct {
const uint32_t reg;       /* I2C0, I2C1 */
const uint32_t dtcy;         /* I2C_DTCY_2 or I2C_DTCY_16_9 */
const uint32_t speed;        /* Max 1000000 */
const uint32_t mode;         /* 0 - disable, 1 - client, 2 - server, 3 - both */
const uint32_t stretching;   /* SCL stretching when data is not ready: 0 - disable, 1 - enable */
const uint32_t general_call; /* responce on general call: 0 - disable, 1 - enable */
const uint32_t addr_size;    /* I2C_ADDFORMAT_7BITS or I2C_ADDFORMAT_10BITS */
const uint32_t addr;         /* address in server mode */
} hdl_i2c_hw_t;


#endif // PORT_I2C_H_