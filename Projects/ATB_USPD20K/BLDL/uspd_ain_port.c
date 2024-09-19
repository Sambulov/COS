#include "app.h"

typedef struct {
  hdl_module_t module;
  const hdl_uspd_ain_port_config_t *config;
  hdl_uspd20k_circuit_cofig_t current_config;
} hdl_uspd_ain_port_private_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_uspd_ain_port_private_t, bldl_uspd_ain_port_t, HDL_USPD_AIN_PORT_PRV_SIZE, uspd_ain_port.h);

static void _uspd_ain_port_set_circuit_pin(hdl_gpio_pin_t *pin, uint8_t en) {
  if((pin != NULL) && !HDL_IS_NULL_MODULE(pin)) {
    if(en) hdl_gpio_set_active(pin);
    else hdl_gpio_set_inactive(pin);
  }
}

static void _bldl_uspd_ain_port_set_circuit(hdl_uspd_ain_port_private_t *port, hdl_uspd20k_circuit_cofig_t cnf) {
  //TODO: check if config valid
  _uspd_ain_port_set_circuit_pin((hdl_gpio_pin_t *)port->module.dependencies[0], (cnf & USPD20K_CIRCUIT_CONFIG_NTC_PU) != 0);
  _uspd_ain_port_set_circuit_pin((hdl_gpio_pin_t *)port->module.dependencies[1], (cnf & USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH) != 0);
  _uspd_ain_port_set_circuit_pin((hdl_gpio_pin_t *)port->module.dependencies[2], (cnf & USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW) != 0);
  _uspd_ain_port_set_circuit_pin((hdl_gpio_pin_t *)port->module.dependencies[3], (cnf & USPD20K_CIRCUIT_CONFIG_4K3_PD) != 0);
  _uspd_ain_port_set_circuit_pin((hdl_gpio_pin_t *)port->module.dependencies[4], (cnf & USPD20K_CIRCUIT_CONFIG_150R_PD) != 0);
  _uspd_ain_port_set_circuit_pin((hdl_gpio_pin_t *)port->module.dependencies[5], (cnf & USPD20K_CIRCUIT_CONFIG_1K_PD) != 0);
}

void bldl_uspd_ain_port_set_circuit(bldl_uspd_ain_port_t *desc, hdl_uspd20k_circuit_cofig_t cnf) {
  if((desc != NULL) && (hdl_state(&desc->module) == HDL_MODULE_ACTIVE)) {
    _bldl_uspd_ain_port_set_circuit((hdl_uspd_ain_port_private_t *)desc, cnf);
  }
}

hdl_module_state_t bldl_uspd_ain_port(void *desc, uint8_t enable) {
  hdl_uspd_ain_port_private_t *port = (hdl_uspd_ain_port_private_t *)desc;
  if(enable) {
    port->current_config = port->config->circuit_config;
    _bldl_uspd_ain_port_set_circuit(port, port->current_config);
    return HDL_MODULE_ACTIVE;
  }
  _bldl_uspd_ain_port_set_circuit(port, port->config->circuit_config);
  return HDL_MODULE_ACTIVE;
}
