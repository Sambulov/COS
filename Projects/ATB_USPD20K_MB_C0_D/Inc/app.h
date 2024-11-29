#ifndef APP_H_
#define APP_H_

#include "hdl.h"

#include "uspd_dio.h"
#include "som_link.h"
#include "mig.h"

#define DIO_CONFIG_SYNC_KEY            0x12345678

typedef struct {
  uint32_t input_state;
  uint32_t output_state;
  uint32_t input_disable;
  uint32_t input_hold_state;
  uint32_t sync_key;  
} app_dio_t;

#define MAP_ADDR_DIO                   0x1000

#define APP_R_MEM_MAP_SIZE       1
#define APP_W_MEM_MAP_SIZE       1

extern hdl_module_t mod_app;
extern app_dio_t app_dio;

void main();

#endif /* APP_H_ */
