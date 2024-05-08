#ifndef BLDL_atb3500_IO_H_
#define BLDL_atb3500_IO_H_

/* atb3500 digital input:
- gpio pin (direct);
*/
typedef hdl_plc_port_t bldl_atb3500_di_port_t;
/* atb3500 digital otput:
- gpio pin (direct);
*/
typedef hdl_plc_port_t bldl_atb3500_do_port_t;
/* atb3500 analog input port depends on:
- adc;
set module.reg to adc source
*/
typedef hdl_plc_port_t bldl_atb3500_ain_port_t;
/* atb3500 led port depends on: 
- discrete output pin(active high)
*/
typedef hdl_plc_port_t bldl_atb3500_led_port_t;
/* atb3500 port expander depends on all plc ports
*/
typedef hdl_module_t bldl_atb3500_port_expander_t;

hdl_module_state_t bldl_atb3500_led_port(void *desc, uint8_t enable);
hdl_module_state_t bldl_atb3500_di_port(void *desc, uint8_t enable);
hdl_module_state_t bldl_atb3500_do_port(void *desc, uint8_t enable);
hdl_module_state_t bldl_atb3500_ain_port(void *desc, uint8_t enable);
hdl_module_state_t bldl_atb3500_port_expander(void *desc, uint8_t enable);

#endif // BLDL_atb3500_IO_H_
