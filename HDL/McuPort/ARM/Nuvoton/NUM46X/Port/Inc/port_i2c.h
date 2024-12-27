#ifndef PORT_I2C_H_
#define PORT_I2C_H_

#define HDL_I2C_PRV_SIZE                    72

/* module depends on 
  hdl_clock_t HDL_CLOCK_TYPE_APB0 for I2C0, I2C2, I2C4 or HDL_CLOCK_TYPE_APB0 for I2C1, I2C3
  hdl_gpio_pin_t scl pin, 
  hdl_gpio_pin_t sda pin,
  hdl_interrupt_controller_t
  hdl_time_counter_t
*/ 

typedef struct {
  const uint32_t speed;        /* Max 1000000 */
  const uint8_t general_call_enable : 1,
                swap_scl_sda        : 1,
                addr_10_bits        : 1;
  const uint16_t addr0;
  const uint16_t addr1;
  const uint16_t addr2;
  const uint16_t addr3;
  hdl_interrupt_t *interrupt;
} hdl_i2c_config_t;

#endif // PORT_I2C_H_
