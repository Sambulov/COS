#ifndef USPD_DIO_PORT_H_
#define USPD_DIO_PORT_H_

#define HDL_USPD_DIO_PORT_PRV_SIZE       4

typedef struct {
} hdl_uspd_dio_port_config_t;

/*
    depends on 
    gpio di
    gpio do
    gpio di_disable
*/
typedef struct {
  hdl_module_t module;
  const hdl_uspd_dio_port_config_t *config;
  PRIVATE(hw, HDL_USPD_DIO_PORT_PRV_SIZE);
} bldl_uspd_dio_port_t;

hdl_module_state_t bldl_uspd_dio_port(void *desc, uint8_t enable);

hdl_gpio_state bldl_uspd_dio_read(const bldl_uspd_dio_port_t *dio);
hdl_gpio_state bldl_uspd_dio_read_output(const bldl_uspd_dio_port_t *dio);
void bldl_uspd_dio_write(const bldl_uspd_dio_port_t *dio, const hdl_gpio_state state);

void bldl_uspd_dio_input_hold(const bldl_uspd_dio_port_t *dio, const hdl_gpio_state state);
void bldl_uspd_dio_input_enable(const bldl_uspd_dio_port_t *dio);


#endif /* USPD_AIN_PORT_H_ */
