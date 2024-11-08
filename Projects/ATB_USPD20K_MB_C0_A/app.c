#include "app.h"
#include "CodeLib.h"

void log_measures(uint8_t reset) {
  static uint32_t primary_filter_ain[APP_ADC_CHANNELS_AMOUNT][PRIMARY_FILTER_SIZE];
  static uint32_t secondary_filter_ain[APP_ADC_CHANNELS_AMOUNT][SECONDARY_FILTER_SIZE];

  static median_filter_t primary_filter[APP_ADC_CHANNELS_AMOUNT] = { 
    {.data = primary_filter_ain[0], .size = PRIMARY_FILTER_SIZE},
    {.data = primary_filter_ain[1], .size = PRIMARY_FILTER_SIZE},
    {.data = primary_filter_ain[2], .size = PRIMARY_FILTER_SIZE},
    {.data = primary_filter_ain[3], .size = PRIMARY_FILTER_SIZE},
    {.data = primary_filter_ain[4], .size = PRIMARY_FILTER_SIZE},
    {.data = primary_filter_ain[5], .size = PRIMARY_FILTER_SIZE},
  };

  static median_filter_t secondary_filter[APP_ADC_CHANNELS_AMOUNT] = { 
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
    for(uint8_t i = 0; i < APP_ADC_CHANNELS_AMOUNT; i++) {
      median_filter_init(&primary_filter[i], HDL_ADC_MS5194T_INVALID_VALUE, &primary_filter_val.ain[i]);
      median_filter_init(&secondary_filter[i], HDL_ADC_MS5194T_INVALID_VALUE, &secondary_filter_val.ain[i]);
    }
    adc_age = hdl_adc_ms5194t_age(&mod_adc);
    return;
  }

  static uint8_t log_state = APP_ADC_CHANNELS_AMOUNT + 1;
  static uint32_t curr_age;
  switch (log_state)
  {
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
    if(median_filter_feed(&primary_filter[log_state], adc_log[ind].ain[log_state])) {
      median_filter_feed(&secondary_filter[log_state], primary_filter_val.ain[log_state]);
    }
    log_state++;
    break;

  case APP_ADC_CHANNELS_AMOUNT:
    primary_filter_val.timestamp = curr_age;
    secondary_filter_val.timestamp = curr_age;    
    secondary_filter_val.valid = MEASURE_VALID;
    primary_filter_val.valid = MEASURE_VALID;
    ind++;
    if(ind >= APP_ADC_LOG_SIZE) {
      ind = 0;
    }
    log_state++;
  break;

  case APP_ADC_CHANNELS_AMOUNT + 1:
    curr_age = hdl_adc_ms5194t_age(&mod_adc);
    if(adc_age != curr_age) {
      adc_log[ind].valid = 0;
      for(uint8_t i = 0; i < APP_ADC_CHANNELS_AMOUNT; i++) {
        adc_log[ind].ain[i] =  hdl_adc_ms5194t_get(&mod_adc, i);
      }
      adc_log[ind].timestamp = curr_age;
      adc_log[ind].valid = MEASURE_VALID;
      adc_age = curr_age;
      primary_filter_val.valid = 0;
      secondary_filter_val.valid = 0;
      log_state = 0;
    }
    break;
  default:
    break;
  }
}

void update_circuit_config() {
  if(ai_circuit_config.sync_key == CIRCUIT_CONFIG_SYNC_KEY) {
    for(uint8_t i = 0; i < APP_AIN_PORTS_AMOUNT; i++) {
      bldl_uspd_ain_port_set_circuit(&app_uspd_ain_port[i], ai_circuit_config.user.ai[i]);
      ai_circuit_config.active.ai[i] = ai_circuit_config.user.ai[i];
    }
    log_measures(1);
    ai_circuit_config.sync_key = 0;
  }
}

uint8_t bldl_mem_map_rw(const app_mem_map_item_t map[], uint8_t map_size, uint16_t map_addr, uint8_t *w_data) {
  static uint8_t current = 0;
  for(uint8_t i = 0; i < map_size; i++) {
    if(current >= map_size)
      current = 0;
    if((map_addr >= map[current].map_addr_start) && (map_addr <= map[current].map_addr_end)) {
      uint8_t *data = &(map[current].data_ptr[map_addr - map[current].map_addr_start]);
      if(w_data != NULL) *data = *w_data;
      return *data;
    }
    current++;
  }
  return 0;
}

uint8_t bldl_som_link_read_byte_cb(uint16_t addr) {
  return bldl_mem_map_rw(app_r_mem_map, APP_R_MEM_MAP_SIZE, addr, NULL);
}

void bldl_som_link_write_byte_cb(uint16_t addr, uint8_t data) {
  bldl_mem_map_rw(app_w_mem_map, APP_W_MEM_MAP_SIZE, addr, &data);
}

void set_adc_config() {
  for(uint8_t i = 0; i < APP_ADC_CHANNELS_AMOUNT; i++) {
    adc_config.src_active[i] = adc_config.src_user[i];
  }
  adc_config.adc_io_active = adc_config.adc_io_user;
  adc_config.adc_mode_active = adc_config.adc_mode_user;
  mod_adc_cnf.io_reg = adc_config.adc_io_active;
  mod_adc_cnf.mode_reg = adc_config.adc_mode_active;  
  adc_config.sync_key = 0;
}

void update_adc_preset_config() {
  if(app_adc_preset_config.sync_key == PRESET_CONFIG_SYNC_KEY) {
    for(uint8_t i = 0; i < APP_AIN_PORTS_AMOUNT; i++) {
      if(app_adc_preset_config.port_preset_selection[i] < APP_ADC_PRESETS_AMOUNT) {
        adc_config.src_user[app_uspd_ain_port_to_adc_ch_map[i]].config_reg = 
          app_adc_presets[app_adc_preset_config.port_preset_selection[i]]->ch_config.config_reg | 
          MS5194T_CONFIG_REG_CH_SEL(app_uspd_ain_port_to_adc_ch_map[i]);
        adc_config.src_user[app_uspd_ain_port_to_adc_ch_map[i]].options = 
          app_adc_presets[app_adc_preset_config.port_preset_selection[i]]->ch_config.options;
        ai_circuit_config.user.ai[i] = app_adc_presets[app_adc_preset_config.port_preset_selection[i]]->circuit_config;
      }
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

void main() {
  uint8_t state = APP_STATE_ENABLE_ADC;

  hdl_enable(&mod_app);

  while (!hdl_init_complete()) {
    cooperative_scheduler(false);
  }

  // uint32_t time = 0;
  // uint32_t now = hdl_time_counter_get(&mod_timer_ms);
  // while(!TIME_ELAPSED(0, 2000, now)) {
  //   now = hdl_time_counter_get(&mod_timer_ms);
  // }


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

