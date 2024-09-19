#ifndef APP_H_
#define APP_H_

#include "hdl.h"

#include "ms5194t.h"
#include "uspd_ain_port.h"
#include "som_link.h"

#include "mig.h"

typedef struct {
  hdl_adc_ms5194t_source_t src_active[6];
  uint8_t adc_io_active;
  uint16_t adc_mode_active;
  hdl_adc_ms5194t_source_t src_user[6];
  uint8_t adc_io_user;
  uint16_t adc_mode_user;
  uint32_t sync_key;
} app_adc_config_t;

#define ADC_CONFIG_SYNC_KEY       0xABCDEFAB

typedef struct {
  uint32_t ain1;
  uint32_t ain2;
  uint32_t ain3;
  uint32_t ain4;
  uint32_t ain5;
  uint32_t ain6;
  uint32_t timestamp;
  uint32_t valid;
} app_measure_log_t;

#define MEASURE_VALID   0xCAFEFEED

typedef struct {
  uint8_t ai1;
  uint8_t ai2;
  uint8_t ai3;
  uint8_t ai4;
} circuit_config_ports_t;

typedef struct {
  circuit_config_ports_t active;
  circuit_config_ports_t user;
  uint32_t sync_key;
} app_circuit_config_t;

#define CIRCUIT_CONFIG_SYNC_KEY       0x12345678

typedef struct {
  uint16_t current_address;
  uint8_t state;
} som_proto_context_t;


#define MAP_ADDR_ADC_LOG          0x0000
#define MAP_ADDR_CIRCUIT_CONFIG   0x1000
#define MAP_ADDR_ADC_CONFIG       0x2000


#define APP_ADC_LOG_SIZE    64

extern bldl_uspd_ain_port_t hdl_uspd_ain_port1;
extern bldl_uspd_ain_port_t hdl_uspd_ain_port2;
extern bldl_uspd_ain_port_t hdl_uspd_ain_port3;
extern bldl_uspd_ain_port_t hdl_uspd_ain_port4;

extern hdl_module_t mod_app;

extern app_adc_config_t adc_config;
extern app_circuit_config_t ai_circuit_config;
extern app_measure_log_t adc_log[APP_ADC_LOG_SIZE];

void main();

#endif /* APP_H_ */
