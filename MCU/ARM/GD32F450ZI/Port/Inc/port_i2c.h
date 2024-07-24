#ifndef PORT_I2C_H_
#define PORT_I2C_H_

#define HDL_I2C_PRV_SIZE                    76

typedef struct {
  const uint32_t dtcy;         /* I2C_DTCY_2 or I2C_DTCY_16_9 */
  const uint32_t speed;        /* Max 1000000 */
  const uint8_t general_call_enable : 1,
                addr_10_bits        : 1,
                dual_address        : 1,
                stretch_enable      : 1;
  const uint16_t addr0;
  const uint16_t addr1;
  const hdl_nvic_irq_n_t err_interrupt;
  const hdl_nvic_irq_n_t ev_interrupt;
} hdl_i2c_config_t;




// typedef struct {
//   const uint32_t dtcy;         /* I2C_DTCY_2 or I2C_DTCY_16_9 */
//   const uint32_t speed;        /* Max 1000000 */
//   const uint32_t mode;         /* 0 - disable, 1 - client, 2 - server, 3 - both */
//   const uint32_t stretching;   /* SCL stretching when data is not ready: 0 - disable, 1 - enable */
//   const uint32_t general_call; /* responce on general call: 0 - disable, 1 - enable */
//   const uint32_t addr_size;    /* I2C_ADDFORMAT_7BITS or I2C_ADDFORMAT_10BITS */
//   const uint32_t addr;         /* address in server mode */
//   const hdl_nvic_irq_n_t err_interrupt;
//   const hdl_nvic_irq_n_t ev_interrupt;
// } hdl_i2c_server_hw_t;


#endif // PORT_I2C_H_
