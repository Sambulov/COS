#include "app.h"
#include "CodeLib.h"

#define PRIMARY_FILTER_SIZE  16
#define SECONDARY_FILTER_SIZE  8
#define ADC_CHANNELS_AMOUNT   6
#define MAP_ADDR_SECONDARY_FILTER_ADC_VAL      0x950
#define MAP_ADDR_PRIMARY_FILTER_ADC_VAL      0x900

app_adc_measure_t primary_filter_val;
app_adc_measure_t secondary_filter_val;


void log_measures(uint8_t reset) {
  static uint32_t primary_filter_ain[ADC_CHANNELS_AMOUNT][PRIMARY_FILTER_SIZE];
  static uint32_t secondary_filter_ain[ADC_CHANNELS_AMOUNT][SECONDARY_FILTER_SIZE];

  static median_filter_t primary_filter[ADC_CHANNELS_AMOUNT] = { 
    {.data = primary_filter_ain[0], .size = PRIMARY_FILTER_SIZE},
    {.data = primary_filter_ain[1], .size = PRIMARY_FILTER_SIZE},
    {.data = primary_filter_ain[2], .size = PRIMARY_FILTER_SIZE},
    {.data = primary_filter_ain[3], .size = PRIMARY_FILTER_SIZE},
    {.data = primary_filter_ain[4], .size = PRIMARY_FILTER_SIZE},
    {.data = primary_filter_ain[5], .size = PRIMARY_FILTER_SIZE},
  };

  static median_filter_t secondary_filter[ADC_CHANNELS_AMOUNT] = { 
    {.data = secondary_filter_ain[0], .size = SECONDARY_FILTER_SIZE},
    {.data = secondary_filter_ain[1], .size = SECONDARY_FILTER_SIZE},
    {.data = secondary_filter_ain[2], .size = SECONDARY_FILTER_SIZE},
    {.data = secondary_filter_ain[3], .size = SECONDARY_FILTER_SIZE},
    {.data = secondary_filter_ain[4], .size = SECONDARY_FILTER_SIZE},
    {.data = secondary_filter_ain[5], .size = SECONDARY_FILTER_SIZE},
  };

  static uint32_t adc_age = 0;
  static uint32_t ind = 0;
  if(reset) {
    ind = 0;
    for(uint8_t i = 0; i < APP_ADC_LOG_SIZE; i++) {
      adc_log[i].valid = 0;
    }
    for(uint8_t i = 0; i < ADC_CHANNELS_AMOUNT; i++) {
      median_filter_init(&primary_filter[i], HDL_ADC_MS5194T_INVALID_VALUE, &primary_filter_val.ain[i]);
      median_filter_init(&secondary_filter[i], HDL_ADC_MS5194T_INVALID_VALUE, &secondary_filter_val.ain[i]);
    }
    adc_age = hdl_adc_ms5194t_age(&mod_adc);
    return;
  }
  uint32_t age = hdl_adc_ms5194t_age(&mod_adc);
  if(adc_age != age) {
    adc_log[ind].valid = 0;
    for(uint8_t i = 0; i < ADC_CHANNELS_AMOUNT; i++) {
      adc_log[ind].ain[i] = hdl_adc_ms5194t_get(&mod_adc, i);
      if(median_filter_feed(&primary_filter[i], adc_log[ind].ain[i])) {
        median_filter_feed(&secondary_filter[i], primary_filter_val.ain[i]);
      }
    }
    adc_log[ind].timestamp = age;
    primary_filter_val.timestamp = age;
    secondary_filter_val.timestamp = age;
    adc_log[ind].valid = MEASURE_VALID;
    secondary_filter_val.valid = MEASURE_VALID;
    primary_filter_val.valid = MEASURE_VALID;

    ind++;
    if(ind >= APP_ADC_LOG_SIZE) {
      ind = 0;
    }
    adc_age = age;
  }
}

void update_circuit_config() {
  if(ai_circuit_config.sync_key == CIRCUIT_CONFIG_SYNC_KEY) {
    bldl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port1, ai_circuit_config.user.ai1);
    ai_circuit_config.active.ai1 = ai_circuit_config.user.ai1;
    bldl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port2, ai_circuit_config.user.ai2);
    ai_circuit_config.active.ai2 = ai_circuit_config.user.ai2;
    bldl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port3, ai_circuit_config.user.ai3);
    ai_circuit_config.active.ai3 = ai_circuit_config.user.ai3;
    bldl_uspd_ain_port_set_circuit(&hdl_uspd_ain_port4, ai_circuit_config.user.ai4);
    ai_circuit_config.active.ai4 = ai_circuit_config.user.ai4;
    log_measures(1);
    ai_circuit_config.sync_key = 0;
  }
}

uint8_t bldl_som_link_read_byte_cb(uint16_t addr) {
  if((addr >= MAP_ADDR_ADC_LOG) && (addr < (MAP_ADDR_ADC_LOG + sizeof(adc_log)))) {
    return ((uint8_t*)&adc_log)[addr - MAP_ADDR_ADC_LOG];
  }
  if((addr >= MAP_ADDR_PRIMARY_FILTER_ADC_VAL) && (addr < (MAP_ADDR_PRIMARY_FILTER_ADC_VAL + sizeof(primary_filter_val)))) {
    return ((uint8_t*)&primary_filter_val)[addr - MAP_ADDR_PRIMARY_FILTER_ADC_VAL];
  }
  if((addr >= MAP_ADDR_SECONDARY_FILTER_ADC_VAL) && (addr < (MAP_ADDR_SECONDARY_FILTER_ADC_VAL + sizeof(secondary_filter_val)))) {
    return ((uint8_t*)&secondary_filter_val)[addr - MAP_ADDR_SECONDARY_FILTER_ADC_VAL];
  }
  else if((addr >= MAP_ADDR_CIRCUIT_CONFIG) && (addr < (MAP_ADDR_CIRCUIT_CONFIG + sizeof(ai_circuit_config)))) {
    return ((uint8_t*)&ai_circuit_config)[addr - MAP_ADDR_CIRCUIT_CONFIG];
  }
  else if((addr >= MAP_ADDR_ADC_CONFIG) && (addr < (MAP_ADDR_ADC_CONFIG + sizeof(adc_config)))) {
    return ((uint8_t*)&adc_config)[addr - MAP_ADDR_ADC_CONFIG];
  }
  else if((addr >= MAP_ADDR_ADC_PRESET_CONFIG) && (addr < (MAP_ADDR_ADC_PRESET_CONFIG + sizeof(app_adc_preset_config)))) {
    return ((uint8_t*)&app_adc_preset_config)[addr - MAP_ADDR_ADC_PRESET_CONFIG];
  }
  return 0;
}

void bldl_som_link_write_byte_cb(uint16_t addr, uint8_t data) {
  if((addr >= (MAP_ADDR_CIRCUIT_CONFIG + offsetof(app_circuit_config_t, user))) && (addr < (MAP_ADDR_CIRCUIT_CONFIG + sizeof(ai_circuit_config)))) {
    ((uint8_t*)&ai_circuit_config)[addr - MAP_ADDR_CIRCUIT_CONFIG] = data;
  }
  else if((addr >= (MAP_ADDR_ADC_CONFIG + offsetof(app_adc_config_t, src_user))) && (addr < (MAP_ADDR_ADC_CONFIG + sizeof(adc_config)))) {
    ((uint8_t*)&adc_config)[addr - MAP_ADDR_ADC_CONFIG] = data;
  }
  else if((addr >= MAP_ADDR_ADC_PRESET_CONFIG) && (addr < (MAP_ADDR_ADC_PRESET_CONFIG + sizeof(app_adc_preset_config)))) {
    ((uint8_t*)&app_adc_preset_config)[addr - MAP_ADDR_ADC_PRESET_CONFIG] = data;
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
  adc_config.sync_key = 0;
}

void update_adc_preset_config() {
  if(app_adc_preset_config.sync_key == PRESET_CONFIG_SYNC_KEY) {
    //adc_config.adc_io_user = MS5194T_IO_REG_DEFAULT;
    //adc_config.adc_mode_user = MS5194T_MODE_REG_MS_PWR_DWN | !MS5194T_MODE_REG_PSW | !MS5194T_MODE_REG_AMP_CM | 
    //                   MS5194T_MODE_REG_CLK_INT64K | !MS5194T_MODE_REG_CHOP_DIS | MS5194T_MODE_REG_FILTER_RATE(5);

    if(app_adc_preset_config.port_preset_selection[0] < ADC_PRESETS_AMOUNT) {
      adc_config.src_user[5].config_reg = app_adc_presets[app_adc_preset_config.port_preset_selection[0]]->ch_config.config_reg | MS5194T_CONFIG_REG_CH_SEL(5);
      adc_config.src_user[5].options = app_adc_presets[app_adc_preset_config.port_preset_selection[0]]->ch_config.options;
      ai_circuit_config.user.ai1 = app_adc_presets[app_adc_preset_config.port_preset_selection[0]]->circuit_config;
    }
    if(app_adc_preset_config.port_preset_selection[0] < ADC_PRESETS_AMOUNT) {
      adc_config.src_user[0].config_reg = app_adc_presets[app_adc_preset_config.port_preset_selection[1]]->ch_config.config_reg | MS5194T_CONFIG_REG_CH_SEL(0);
      adc_config.src_user[0].options = app_adc_presets[app_adc_preset_config.port_preset_selection[1]]->ch_config.options;
      ai_circuit_config.user.ai2 = app_adc_presets[app_adc_preset_config.port_preset_selection[1]]->circuit_config;
    }
    if(app_adc_preset_config.port_preset_selection[0] < ADC_PRESETS_AMOUNT) {
      adc_config.src_user[1].config_reg = app_adc_presets[app_adc_preset_config.port_preset_selection[2]]->ch_config.config_reg | MS5194T_CONFIG_REG_CH_SEL(1);
      adc_config.src_user[1].options = app_adc_presets[app_adc_preset_config.port_preset_selection[2]]->ch_config.options;
      ai_circuit_config.user.ai3 = app_adc_presets[app_adc_preset_config.port_preset_selection[2]]->circuit_config;
    }
    if(app_adc_preset_config.port_preset_selection[0] < ADC_PRESETS_AMOUNT) {
      adc_config.src_user[2].config_reg = app_adc_presets[app_adc_preset_config.port_preset_selection[3]]->ch_config.config_reg | MS5194T_CONFIG_REG_CH_SEL(2);
      adc_config.src_user[2].options = app_adc_presets[app_adc_preset_config.port_preset_selection[3]]->ch_config.options;
      ai_circuit_config.user.ai4 = app_adc_presets[app_adc_preset_config.port_preset_selection[3]]->circuit_config;
    }
    ai_circuit_config.sync_key = CIRCUIT_CONFIG_SYNC_KEY;
    adc_config.sync_key = ADC_CONFIG_SYNC_KEY;
    app_adc_preset_config.sync_key = 0;
  }
}

#define APP_STATE_ENABLE_ADC  0
#define APP_STATE_AWAIT_ADC   1
#define APP_STATE_WORK_ADC    2
#define APP_STATE_RESET       3

uint8_t test_data[512] = {};
hdl_eeprom_i2c_message_t ee_msg_test = {
  .buffer = test_data,
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

  for(int i = 0; i < 512; i++) {
    test_data[i] = 0;
  }

  uint32_t time = 0;
  uint32_t now = hdl_time_counter_get(&mod_timer_ms);
  while(!TIME_ELAPSED(0, 2000, now)) {
    now = hdl_time_counter_get(&mod_timer_ms);
  }

  hdl_eeprom_i2c_transfer(&mod_eeprom, &ee_msg_test);

  bldl_som_link_init();
  while (1) {
    cooperative_scheduler(false);
    update_circuit_config();
    update_adc_preset_config();

    switch (state) {
      case APP_STATE_ENABLE_ADC:
        log_measures(1);
        set_adc_config();
        hdl_enable(&mod_adc.module);
        state = APP_STATE_AWAIT_ADC;
        break;
      case APP_STATE_AWAIT_ADC:
        if(hdl_state(&mod_adc.module) == HDL_MODULE_ACTIVE)
          state = APP_STATE_WORK_ADC;
        break;
      case APP_STATE_WORK_ADC:
        log_measures(0);
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








// #define _GNU_SOURCE

// #include <stdint.h>
// #include <errno.h>
// #include <fcntl.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <errno.h>
// #include <string.h>

// // -50 ~ 50 C
// uint32_t pt1000_arr[] = {
// /* -50 */
//    8030628,  8070334,  8110026,  8149704,  8189368,  8229018,  8268654,  830827>
//    8427065,  8466634,  8506190,  8545733,  8585262,  8624779,  8664282,  870377>
//    8822166,  8861605,  8901031,  8940444,  8979845,  9019234,  9058610,  909797>
//    9215990,  9255304,  9294606,  9333896,  9373174,  9412439,  9451693,  949093>
//    9608588,  9647782,  9686964,  9726134,  9765293,  9804440,  9843575,  988269>
// /* 0 */
//   10000000, 10039077, 10078143, 10117197, 10156240, 10195271, 10234290, 1027329>
//   10390252, 10429214, 10468164, 10507103, 10546030, 10584946, 10623850, 1066274>
//   10779350, 10818196, 10857031, 10895854, 10934666, 10973466, 11012254, 1101225>
//   11167292, 11206023, 11244742, 11283450, 11322146, 11360831, 11399504, 1143816>
//   11554080, 11592695, 11631299, 11669891, 11708472, 11747041, 11785598, 1182414>
// /* 50 */
//   11939713
// };

// // port1 = 5, port3 = 1, port4 = 2
// #define ADC_CH  2

// uint32_t convert(uint32_t type, uint32_t val, uint32_t ch) {
//   val *= 3;
//   val >>= 1;
//   val *= 100;
//   val >>= 7;
//   val *= 100;
//   val >>= 7;
//   val *= 100;
//   val >>= 9; // val = (3V*ADC)/(2^24) = U (uV)
//   if(type == 1) {
//     val *= 143;
//     val /= 43;
//   }
//   else if(type == 2) {
//     val *= 1000;
//     val /= 150; // val = I = U/R nA
//   }
//   if((type == 3) || (type == 4)) {
//     // val = R = (U(uV) * 1000000)/I*(nA) Ohm
//     uint64_t tmp = (uint64_t)val * 1000000;
//     val = tmp / ((ch == 5)? 998941: (ch == 1)? 1005011: (ch == 2)? 1001753: 0);
//   }
//   if(type == 4) {
//     val *= 10;
//     for(int i = 1; i < (sizeof(pt1000_arr)/sizeof(uint32_t)); i++) {
//       if(pt1000_arr[i] > val) {
//         val = (i * 1000) + (((val  - pt1000_arr[i-1]) * 1000)/(pt1000_arr[i] - pt1000_arr[i - 1]));
//         val = (val * 10000
//         //((ch == 5)? 9856: (ch == 1)? 9562: (ch == 2)? 9718: 0)
//         ) / 10000;
//         val -= 50000;
//         break;
//       }
//     }
//   }
//   return val;
// }

// int main4(){
//   int i2c_fd = open("/sys/bus/i2c/devices/4-0050/eeprom", O_RDONLY);

//   printf("opening... ");
//   uint8_t data[32];
//   if(i2c_fd != -1){
//     printf("ok\r\n");
//     uint32_t current_ts = 0;
//     for(int i = 0; ; i++) {
//       uint32_t offset = (i%64)*32;
//       offset = 0x0900;
//       lseek(i2c_fd, offset, SEEK_SET);
//       uint32_t readed = read(i2c_fd, data, 32);
//       uint32_t valid = data[31]<<24 | data[30]<<16 | data[29]<<8 | data[28];
//       uint32_t ts = data[27]<<24 | data[26]<<16 | data[25]<<8 | data[24];
//       if((readed > 0) && (valid == 0xCAFEFEED) && (ts > current_ts)) {
//         printf("%d;", i);
//         uint32_t ch = data[3+(ADC_CH*4)] << 24 | data[2+(ADC_CH*4)] << 16 |
//                       data[1+(ADC_CH*4)] <<  8 | data[0+(ADC_CH*4)];
//         //ch = convert(4, ch);
//         //printf("%d;", ch);

//         offset = 0x0950;
//         //lseek(i2c_fd, offset, SEEK_SET);
//         //read(i2c_fd, data, 32);
//         ch = data[3+(ADC_CH*4)] << 24 | data[2+(ADC_CH*4)] << 16 |
//              data[1+(ADC_CH*4)] <<  8 | data[0+(ADC_CH*4)];
//         //ch = convert(4, ch);
//         //printf("%d;", ch);


//         ch = data[3+(5*4)] << 24 | data[2+(5*4)] << 16 |
//              data[1+(5*4)] <<  8 | data[0+(5*4)];
//         ch = convert(3, ch, 5);
//         printf("%d;", ch);


//         ch = data[3+(1*4)] << 24 | data[2+(1*4)] << 16 |
//              data[1+(1*4)] <<  8 | data[0+(1*4)];
//         ch = convert(3, ch, 1);
//         printf("%d;", ch);

//         ch = data[3+(2*4)] << 24 | data[2+(2*4)] << 16 |
//              data[1+(2*4)] <<  8 | data[0+(2*4)];
//         ch = convert(3, ch, 2);
//         printf("%d;", ch);

//         //offset = 0x950;
//         //lseek(i2c_fd, offset, SEEK_SET);
//         //read(i2c_fd, data, 32);
//         //ch = data[3+(2*4)] << 24 | data[2+(2*4)] << 16 |
//         //     data[1+(2*4)] <<  8 | data[0+(2*4)];
//         //ch = convert(4, ch);
//         //printf("%d;", ch);

//         printf("%d;\r\n", ts);
//         current_ts = ts;
//       }
//       else {
//         i--;
//         usleep(100000);
//       }
//       //usleep(5000000);
//     }
//     close(i2c_fd);
//   }
//   else {
//     printf("fail\r\n");
//     printf("Something went wrong! %s\n", strerror(errno));
//   }
//   return 0;
// }

// int main2(){
//   int i2c_fd = open("/sys/bus/i2c/devices/4-0050/eeprom", O_RDONLY);

//   printf("opening... ");
//   uint8_t data[32];
//   if(i2c_fd != -1){
//     printf("ok\r\n");
//     uint32_t current_ts = 0;
//     for(int i = 0; ; i++) {
//       uint32_t offset = (i%64)*32;
//       lseek(i2c_fd, offset, SEEK_SET);
//       uint32_t readed = read(i2c_fd, data, 32);
//       uint32_t valid = data[31]<<24 | data[30]<<16 | data[29]<<8 | data[28];
//       uint32_t ts = data[27]<<24 | data[26]<<16 | data[25]<<8 | data[24];
//       if((readed > 0) && (valid == 0xCAFEFEED) && (ts > current_ts)) {
//         printf("%d;", i);
//         uint32_t ch = data[3+(2*4)] << 24 | data[2+(2*4)] << 16 |
//                       data[1+(2*4)] <<  8 | data[0+(2*4)];
//         ch *= 3;
//         ch >>= 1;
//         ch *= 100;
//         ch >>= 7;
//         ch *= 100;
//         ch >>= 7;
//         ch *= 100;
//         ch >>= 9; // val = (3V*ADC)/(2^24) = U
//         //ch = (ch * 10) / 3;
//         ch *= 143;
//         ch /= 43;
//         //ch *= 1000;
//         //ch /= 150; // val = I = U/R nA
//         printf("%d;", ch);

//         offset = 0x900;
//         lseek(i2c_fd, offset, SEEK_SET);
//         read(i2c_fd, data, 32);
//         ch = data[3+(2*4)] << 24 | data[2+(2*4)] << 16 |
//              data[1+(2*4)] <<  8 | data[0+(2*4)];
//         ch *= 3;
//         ch >>= 1;
//         ch *= 100;
//         ch >>= 7;
//         ch *= 100;
//         ch >>= 7;
//         ch *= 100;
//         ch >>= 9; // val = (3V*ADC)/(2^24) = U
//         //ch = (ch * 10) / 3;
//         ch *= 143;
//         ch /= 43;

//         printf("%d;", ch);

//         printf("%d;\r\n", ts);
//         current_ts = ts;
//       }
//       else {
//         i--;
//         usleep(100000);
//       }
//     }
//     close(i2c_fd);
//   }
//   else {
//     printf("fail\r\n");
//     printf("Something went wrong! %s\n", strerror(errno));
//   }
//   return 0;
// }



// int main1(){
//   int i2c_fd = open("/sys/bus/i2c/devices/4-0050/eeprom", O_RDONLY);

//   printf("opening... ");
//   uint8_t data[32];
//   if(i2c_fd != -1){
//     printf("ok\r\n");
//     uint32_t current_ts = 0;
//     for(int i = 0; ; i++) {
//       uint32_t offset = (i%64)*32;
//       //offset = 0x900;
//       lseek(i2c_fd, offset, SEEK_SET);
//       uint32_t readed =  read(i2c_fd, data, 32);
//       uint32_t valid = data[31]<<24 | data[30]<<16 | data[29]<<8 | data[28];
//       uint32_t ts = data[27]<<24 | data[26]<<16 | data[25]<<8 | data[24];
//       if((readed > 0) && (valid == 0xCAFEFEED) && (ts > current_ts)) {
//         printf("%d;", i);
//         for(int j = 0; j < 6; j++) {
//           uint32_t ch = data[3+(j*4)] << 24 | data[2+(j*4)] << 16 |
//                         data[1+(j*4)] <<  8 | data[0+(j*4)];
//           if(ch != -1) {
//             ch *= 3;
//             ch >>= 1;
//             ch *= 100;
//             ch >>= 7;
//             ch *= 100;
//             ch >>= 7;
//             ch *= 100;
//             ch >>= 9; // val = (3V*ADC)/(2^24) = U
//             //ch = (ch * 10) / 3;
//             //ch *= 143;
//             //ch /= 43;
//             ch *= 1000;
//             ch /= 150; // val = I = U/R nA
//             printf("%d;", ch);
//           }
//           else
//             printf("NA;");
//         }
//         printf("%d;\r\n", ts);
//         current_ts = ts;
//       }
//       else {
//         i--;
//         usleep(100000);
//       }
//     }
//     close(i2c_fd);
//   }
//   else {
//     printf("fail\r\n");
//     printf("Something went wrong! %s\n", strerror(errno));
//   }
//   return 0;
// }

