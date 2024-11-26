#include "app.h"
#include "CodeLib.h"

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

    switch (state) {
      case APP_STATE_ENABLE_ADC:
        state = APP_STATE_AWAIT_ADC;
        break;
      case APP_STATE_AWAIT_ADC:
        state = APP_STATE_WORK_ADC;
        break;
      case APP_STATE_WORK_ADC:
        // log_measures(0);
        // if(adc_config.sync_key == ADC_CONFIG_SYNC_KEY) {
        //   hdl_kill(&mod_adc.module);
        //   state = APP_STATE_RESET;
        // }
        break;
      case APP_STATE_RESET:
        // if(hdl_state(&mod_adc.module) == HDL_MODULE_UNLOADED) {
        //   state = APP_STATE_ENABLE_ADC;
        // }
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

