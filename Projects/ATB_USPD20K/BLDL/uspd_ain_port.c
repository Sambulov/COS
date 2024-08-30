#include "app.h"

typedef struct {
  hdl_module_t module;
  const hdl_uspd_ain_port_config_t *config;
  hdl_uspd20k_circuit_cofig_t current_config;
} hdl_uspd_ain_port_private_t;

_Static_assert(sizeof(hdl_uspd_ain_port_private_t) == sizeof(hdl_uspd_ain_port_t), "In uspd_ain_port.h data structure size of hdl_uspd_ain_port_t doesn't match, check HDL_USPD_AIN_PORT_PRV_SIZE");


static void _hdl_uspd_ain_port_set_circuit(hdl_uspd_ain_port_private_t *port, hdl_uspd20k_circuit_cofig_t cnf) {
  //TODO: check if config valid
  hdl_gpio_pin_t *ctrl_pin_ntc_pu = (hdl_gpio_pin_t *)port->module.dependencies[0];
  hdl_gpio_pin_t *ctrl_pin_cur_scr_high = (hdl_gpio_pin_t *)port->module.dependencies[1];
  hdl_gpio_pin_t *ctrl_pin_cur_scr_low = (hdl_gpio_pin_t *)port->module.dependencies[2];
  hdl_gpio_pin_t *ctrl_pin_4K3_pd = (hdl_gpio_pin_t *)port->module.dependencies[3];
  hdl_gpio_pin_t *ctrl_pin_150r_pd = (hdl_gpio_pin_t *)port->module.dependencies[4];
  hdl_gpio_pin_t *ctrl_pin_1k_pd = (hdl_gpio_pin_t *)port->module.dependencies[5];
  if((ctrl_pin_ntc_pu != NULL) && HDL_IS_NULL_MODULE(ctrl_pin_ntc_pu)) {
    if(cnf & USPD20K_CIRCUIT_CONFIG_NTC_PU) 
      hdl_gpio_set_active(ctrl_pin_ntc_pu);
    else 
      hdl_gpio_set_inactive(ctrl_pin_ntc_pu);
  }
  if((ctrl_pin_cur_scr_high != NULL) && HDL_IS_NULL_MODULE(ctrl_pin_cur_scr_high)) {
    if(cnf & USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH) 
      hdl_gpio_set_active(ctrl_pin_cur_scr_high);
    else 
      hdl_gpio_set_inactive(ctrl_pin_cur_scr_high);
  }
  if((ctrl_pin_cur_scr_low != NULL) && HDL_IS_NULL_MODULE(ctrl_pin_cur_scr_low)) {
    if(cnf & USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW) 
      hdl_gpio_set_active(ctrl_pin_cur_scr_low);
    else 
      hdl_gpio_set_inactive(ctrl_pin_cur_scr_low);
  }
  if((ctrl_pin_4K3_pd != NULL) && HDL_IS_NULL_MODULE(ctrl_pin_4K3_pd)) {
    if(cnf & USPD20K_CIRCUIT_CONFIG_4K3_PD) 
      hdl_gpio_set_active(ctrl_pin_4K3_pd);
    else 
      hdl_gpio_set_inactive(ctrl_pin_4K3_pd);
  }
  if((ctrl_pin_150r_pd != NULL) && HDL_IS_NULL_MODULE(ctrl_pin_150r_pd)) {
    if(cnf & USPD20K_CIRCUIT_CONFIG_150R_PD) 
      hdl_gpio_set_active(ctrl_pin_150r_pd);
    else 
      hdl_gpio_set_inactive(ctrl_pin_150r_pd);
  }
  if((ctrl_pin_1k_pd != NULL) && HDL_IS_NULL_MODULE(ctrl_pin_1k_pd)) {
    if(cnf & USPD20K_CIRCUIT_CONFIG_1K_PD) 
      hdl_gpio_set_active(ctrl_pin_1k_pd);
    else 
      hdl_gpio_set_inactive(ctrl_pin_1k_pd);
  }
}

void hdl_uspd_ain_port_set_circuit(hdl_uspd_ain_port_t *desc, hdl_uspd20k_circuit_cofig_t cnf) {
  if((desc != NULL) && (hdl_state(&desc->module) == HDL_MODULE_ACTIVE)) {
    _hdl_uspd_ain_port_set_circuit((hdl_uspd_ain_port_private_t *)desc, cnf);
  }
}

hdl_module_state_t hdl_uspd_ain_port(void *desc, uint8_t enable) {
  hdl_uspd_ain_port_private_t *port = (hdl_uspd_ain_port_private_t *)desc;
  if(enable) {
    port->current_config = port->config->circuit_config;
    _hdl_uspd_ain_port_set_circuit(port, port->current_config);
    return HDL_MODULE_ACTIVE;
  }
  _hdl_uspd_ain_port_set_circuit(port, port->config->circuit_config);
  return HDL_MODULE_ACTIVE;
}
