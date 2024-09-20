#include "app.h"
#include "CodeLib.h"

void reset_measures() {
  for(uint8_t i = 0; i < APP_ADC_LOG_SIZE; i++) {
    adc_log[i].valid = 0;
  }
}

void set_circuit_config() {
  bldl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, ai_circuit_config.user.ai1);
  ai_circuit_config.active.ai1 = ai_circuit_config.user.ai1;
  bldl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port2, ai_circuit_config.user.ai2);
  ai_circuit_config.active.ai2 = ai_circuit_config.user.ai2;
  bldl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port3, ai_circuit_config.user.ai3);
  ai_circuit_config.active.ai3 = ai_circuit_config.user.ai3;
  bldl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port4, ai_circuit_config.user.ai4);
  ai_circuit_config.active.ai4 = ai_circuit_config.user.ai4;
  reset_measures();
  ai_circuit_config.sync_key = 0;
}


void log_measures() {
  static uint32_t adc_age = 0;
  static uint32_t ind = 0;
  uint32_t age = hdl_adc_ms5194t_age(&mod_adc);
  if(adc_age != age) {
    adc_log[ind].valid = 0;
    adc_log[ind].ain1 = hdl_adc_ms5194t_get(&mod_adc, 0);
    adc_log[ind].ain2 = hdl_adc_ms5194t_get(&mod_adc, 1);
    adc_log[ind].ain3 = hdl_adc_ms5194t_get(&mod_adc, 2);
    adc_log[ind].ain4 = hdl_adc_ms5194t_get(&mod_adc, 3);
    adc_log[ind].ain5 = hdl_adc_ms5194t_get(&mod_adc, 4);
    adc_log[ind].ain6 = hdl_adc_ms5194t_get(&mod_adc, 5);
    adc_log[ind].timestamp = age;
    adc_log[ind].valid = MEASURE_VALID;
    ind++;
    if(ind >= APP_ADC_LOG_SIZE) {
      ind = 0;
    }
    adc_age = age;      
  }
}

uint8_t bldl_som_link_read_byte_cb(uint16_t addr) {
  if((addr >= MAP_ADDR_ADC_LOG) && (addr < (MAP_ADDR_ADC_LOG + sizeof(adc_log)))) {
    return ((uint8_t*)&adc_log)[addr - MAP_ADDR_ADC_LOG];
  }
  else if((addr >= MAP_ADDR_CIRCUIT_CONFIG) && (addr < (MAP_ADDR_CIRCUIT_CONFIG + sizeof(ai_circuit_config)))) {
    return ((uint8_t*)&ai_circuit_config)[addr - MAP_ADDR_CIRCUIT_CONFIG];
  }
  else if((addr >= MAP_ADDR_ADC_CONFIG) && (addr < (MAP_ADDR_ADC_CONFIG + sizeof(adc_config)))) {
    return ((uint8_t*)&adc_config)[addr - MAP_ADDR_ADC_CONFIG];
  }
  return 0;
}

void bldl_som_link_write_byte_cb(uint16_t addr, uint8_t data) {
  if((addr >= (MAP_ADDR_CIRCUIT_CONFIG + offsetof(app_circuit_config_t, user))) && (addr < (MAP_ADDR_CIRCUIT_CONFIG + sizeof(ai_circuit_config)))) {
    ((uint8_t*)&ai_circuit_config)[addr - MAP_ADDR_CIRCUIT_CONFIG] = data;
  }
  else {
    if((addr >= (MAP_ADDR_ADC_CONFIG + offsetof(app_adc_config_t, src_user))) && (addr < (MAP_ADDR_ADC_CONFIG + sizeof(adc_config)))) {
      ((uint8_t*)&adc_config)[addr - MAP_ADDR_ADC_CONFIG] = data;
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
  adc_config.adc_mode_active = adc_config.adc_mode_user;
  mod_adc_cnf.io_reg = adc_config.adc_io_active;
  mod_adc_cnf.mode_reg = adc_config.adc_mode_active;  
  reset_measures();
  adc_config.sync_key = 0;
}

#define APP_STATE_ENABLE_ADC  0
#define APP_STATE_AWAIT_ADC   1
#define APP_STATE_WORK_ADC    2
#define APP_STATE_RESET       3

uint8_t data[512] = {};
hdl_eeprom_i2c_message_t ee_msg = {
  .buffer = data,
  .count = 512,
  .offset = 0,
  .state = HDL_EEPROM_MSG_OPTION_READ,
};


void main() {

  uint8_t state = APP_STATE_ENABLE_ADC;

  hdl_enable(&mod_app);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  for(int i = 0; i < 128; i++) {
    data[i] = 0xff;
  }

  uint32_t time = 0;
  uint32_t now = hdl_time_counter_get(&mod_timer_ms);
  while(!TIME_ELAPSED(0, 5000, now)) {
    now = hdl_time_counter_get(&mod_timer_ms);
  }

  hdl_eeprom_i2c_transfer(&mod_eeprom, &ee_msg);

  bldl_som_link_init();
  while (1) {
    cooperative_scheduler(false);
    if(ai_circuit_config.sync_key == CIRCUIT_CONFIG_SYNC_KEY) {
      set_circuit_config();
    }
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
  }
}
