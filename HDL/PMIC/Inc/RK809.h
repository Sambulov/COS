#ifndef RK809_H_
#define RK809_H_

#define RK809_PMIC_I2C_ADDR       0x20
#define RK809_REG_OPERATIONAL_BASE  0x0000  //????

#define RK809_REG_PMIC_CHIP_NAME  ((uint16_t)(RK809_REG_OPERATIONAL_BASE + 0x00ed))
#define RK809_REG_PMIC_CHIP_VER   ((uint16_t)(RK809_REG_OPERATIONAL_BASE + 0x00ee))
#define RK809_REG_PMIC_SYS_CFG3   ((uint16_t)(RK809_REG_OPERATIONAL_BASE + 0x00f4))

typedef struct {
  hdl_i2c_client_t i2c;

} hdl_pmic_rk809_t;


void hdl_pmic_rk809_read_register(hdl_pmic_rk809_t desc, uint16_t reg);

void hdl_pmic_rk809_write_register();

#endif // RK809_H_
