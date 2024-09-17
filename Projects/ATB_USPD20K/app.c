#include "app.h"
#include "CodeLib.h"

measure_t measures[MEASURES_LOG_SIZE] = { 0 };

circuit_config_t ai_circuit_config = {
  .active.ai1 = USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_150R_PD,
  .active.ai2 = USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_1K_PD,
  .active.ai3 = USPD20K_CIRCUIT_CONFIG_CUR_SRC_HIGH | USPD20K_CIRCUIT_CONFIG_CUR_SRC_LOW | USPD20K_CIRCUIT_CONFIG_150R_PD | USPD20K_CIRCUIT_CONFIG_1K_PD,
  .active.ai4 = USPD20K_CIRCUIT_CONFIG_150R_PD | USPD20K_CIRCUIT_CONFIG_1K_PD,
  .user.ai1 = USPD20K_CIRCUIT_CONFIG_4K3_PD,
  .user.ai2 = USPD20K_CIRCUIT_CONFIG_4K3_PD,
  .user.ai3 = USPD20K_CIRCUIT_CONFIG_4K3_PD,
  .user.ai4 = USPD20K_CIRCUIT_CONFIG_4K3_PD,
  .sync_key = 0//CIRCUIT_CONFIG_SYNC_KEY,
};

void reset_measures() {
  for(uint8_t i = 0; i < MEASURES_LOG_SIZE; i++) {
    measures[i].valid = 0;
  }
}

void set_circuit_config() {
  hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, ai_circuit_config.user.ai1);
  ai_circuit_config.active.ai1 = ai_circuit_config.user.ai1;
  hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port2, ai_circuit_config.user.ai2);
  ai_circuit_config.active.ai2 = ai_circuit_config.user.ai2;
  hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port3, ai_circuit_config.user.ai3);
  ai_circuit_config.active.ai3 = ai_circuit_config.user.ai3;
  hdl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port4, ai_circuit_config.user.ai4);
  ai_circuit_config.active.ai4 = ai_circuit_config.user.ai4;
  ai_circuit_config.sync_key = 0;
  reset_measures();
}


void log_measures() {
  static uint32_t adc_age = 0;
  static uint32_t ind = 0;
  uint32_t age = hdl_adc_ms5194t_age(&mod_adc);
  if(adc_age != age) {
    measures[ind].valid = 0;
    measures[ind].ain1 = hdl_adc_ms5194t_get(&mod_adc, 0);
    measures[ind].ain2 = hdl_adc_ms5194t_get(&mod_adc, 1);
    measures[ind].ain3 = hdl_adc_ms5194t_get(&mod_adc, 2);
    measures[ind].ain4 = hdl_adc_ms5194t_get(&mod_adc, 3);
    measures[ind].ain5 = hdl_adc_ms5194t_get(&mod_adc, 4);
    measures[ind].ain6 = hdl_adc_ms5194t_get(&mod_adc, 5);
    measures[ind].timestamp = age;
    measures[ind].valid = MEASURE_VALID;
    ind++;
    if(ind >= MEASURES_LOG_SIZE) {
      ind = 0;
    }

    adc_age = age;      
  }
}

uint8_t bldl_som_link_read_byte_cb(uint16_t addr) {
  uint16_t mem_end = MAP_ADDR_MEASURES + sizeof(measures);
  if((addr >= MAP_ADDR_MEASURES) && (addr < mem_end)) {
    return ((uint8_t*)&measures)[addr - MAP_ADDR_MEASURES];
  }
  else {
    mem_end = MAP_ADDR_CIRCUIT_CONFIG + sizeof(ai_circuit_config);
    if((addr >= MAP_ADDR_CIRCUIT_CONFIG) && (addr < mem_end)) {
      return ((uint8_t*)&ai_circuit_config)[addr - MAP_ADDR_CIRCUIT_CONFIG];
    }
    else {
      mem_end = MAP_ADDR_ADC_CONFIG + sizeof(adc_config);
      if((addr >= MAP_ADDR_ADC_CONFIG) && (addr < mem_end)) {
        return ((uint8_t*)&adc_config)[addr - MAP_ADDR_ADC_CONFIG];
      }
    }
  }
  return 0;
}

void bldl_som_link_write_byte_cb(uint16_t addr, uint8_t data) {
  uint16_t mem_end = MAP_ADDR_CIRCUIT_CONFIG + sizeof(ai_circuit_config);
  if((addr >= (MAP_ADDR_CIRCUIT_CONFIG + offsetof(circuit_config_t, user))) && (addr < mem_end)) {
    ((uint8_t*)&ai_circuit_config)[addr - MAP_ADDR_CIRCUIT_CONFIG] = data;
  }
  else {
  uint16_t mem_end = MAP_ADDR_CIRCUIT_CONFIG + sizeof(ai_circuit_config);
    if((addr >= (MAP_ADDR_CIRCUIT_CONFIG + offsetof(circuit_config_t, user))) && (addr < mem_end)) {
      ((uint8_t*)&ai_circuit_config)[addr - MAP_ADDR_CIRCUIT_CONFIG] = data;
    }
  }
}

void set_adc_config() { 
  adc_config.src_active[0] = adc_config.src_user[0];
  adc_config.src_active[1] = adc_config.src_user[1];
  adc_config.src_active[2] = adc_config.src_user[2];
  adc_config.src_active[3] = adc_config.src_user[3];
  adc_config.src_active[4] = adc_config.src_user[4];
  adc_config.src_active[5] = adc_config.src_user[5];

  adc_config.adc_io_active = adc_config.adc_io_user;
  adc_config.adc_mode_active = adc_config.adc_mode_active;
  adc_config.sync_key = 0;
}

#define APP_STATE_ENABLE_ADC  0
#define APP_STATE_AWAIT_ADC   1
#define APP_STATE_WORK_ADC    2
#define APP_STATE_RESET       3

void main() {

  uint8_t state = APP_STATE_ENABLE_ADC;

  hdl_enable(&mod_app);
  //hdl_enable(&mod_adc.module);
  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }
  bldl_som_link_init();
  while (1) {
    cooperative_scheduler(false);
    switch (state) {
      case APP_STATE_ENABLE_ADC:
        set_adc_config();
        hdl_enable(&mod_adc.module);
        state = APP_STATE_AWAIT_ADC;
        break;
      case APP_STATE_AWAIT_ADC:
        if(hdl_state(&mod_adc.module) == HDL_MODULE_ACTIVE)
          state = APP_STATE_WORK_ADC;
        break;
      case APP_STATE_WORK_ADC:
        log_measures();
        if(adc_config.sync_key == ADC_CONFIG_SYNC_KEY) {
          hdl_kill(&mod_adc.module);
          state = APP_STATE_RESET;
        }
        break;
      case APP_STATE_RESET:
        if(hdl_state(&mod_adc.module) == HDL_MODULE_UNLOADED) {
          state = APP_STATE_ENABLE_ADC;
        }
      default:
        break;
    }

    if(ai_circuit_config.sync_key == CIRCUIT_CONFIG_SYNC_KEY) {
      set_circuit_config();
    }
  }
}
