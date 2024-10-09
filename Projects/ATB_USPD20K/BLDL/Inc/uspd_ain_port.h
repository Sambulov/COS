#ifndef USPD_AIN_PORT_H_
#define USPD_AIN_PORT_H_

#define HDL_USPD_AIN_PORT_PRV_SIZE       4

typedef enum {
  USPD20K_CIRCUIT_CONFIG_FLOATING        =((uint8_t)(0 << 0)),
  USPD20K_CIRCUIT_CONFIG_NTC_PU          =((uint8_t)(1 << 0)),
  USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH    =((uint8_t)(1 << 1)),
  USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW     =((uint8_t)(1 << 2)),
  USPD20K_CIRCUIT_CONFIG_4K3_PD          =((uint8_t)(1 << 3)),
  USPD20K_CIRCUIT_CONFIG_150R_PD         =((uint8_t)(1 << 4)),
  USPD20K_CIRCUIT_CONFIG_1K_PD           =((uint8_t)(1 << 5))
} hdl_uspd20k_circuit_cofig_t;

typedef struct {
  hdl_uspd20k_circuit_cofig_t circuit_config;
} hdl_uspd_ain_port_config_t;

/*
    depends on 
    gpio ntc_pu
    gpio cur_src_high
    gpio cur_src_low
    gpio 4k3_pd
    gpio 150r_pd
    gpio 1k_pd
 */
typedef struct {
  hdl_module_t module;
  const hdl_uspd_ain_port_config_t *config;
  PRIVATE(hw, HDL_USPD_AIN_PORT_PRV_SIZE);
} bldl_uspd_ain_port_t;

hdl_module_state_t bldl_uspd_ain_port(void *desc, uint8_t enable);
void bldl_uspd_ain_port_set_circuit(bldl_uspd_ain_port_t *desc, hdl_uspd20k_circuit_cofig_t cnf);

#endif /* USPD_AIN_PORT_H_ */
