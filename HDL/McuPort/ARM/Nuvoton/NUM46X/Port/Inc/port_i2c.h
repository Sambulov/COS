#ifndef PORT_I2C_H_
#define PORT_I2C_H_

#define HDL_I2C_PRV_SIZE                    80

typedef struct {
  const uint32_t speed;        /* Max 1000000 */
  const uint8_t general_call_enable : 1,
                addr_10_bits        : 1,
                stretch_enable      : 1;
  const uint16_t addr0;
  const uint16_t addr1;
  const uint16_t addr2;
  const uint16_t addr3;
  const hdl_interrupt_t *interrupt;
  const uint32_t rcu;
} hdl_i2c_config_t;


#endif // PORT_I2C_H_
