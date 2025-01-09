#ifndef PORT_I2C_H_
#define PORT_I2C_H_

#define HDL_I2C_PRV_SIZE                    104

typedef struct {
  uint16_t address;
  hdl_transceiver_t *transceiver;
} hdl_i2c_channel_t;

typedef struct {
  const uint32_t dtcy;         /* I2C_DTCY_2 or I2C_DTCY_16_9 */
  const uint32_t speed;        /* Max 1000000 */
  const uint8_t general_call_enable : 1,
                addr_10_bits        : 1,
                dual_address        : 1,
                stretch_enable      : 1;
  hdl_i2c_channel_t **channels;
  hdl_interrupt_t *err_interrupt;
  hdl_interrupt_t *ev_interrupt;
  const rcu_periph_enum rcu;
} hdl_i2c_config_t;

#define hdl_i2c_channels(...) ((hdl_i2c_channel_t *[]){__VA_ARGS__, NULL})

#endif // PORT_I2C_H_
