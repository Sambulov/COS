#ifndef ATB3500_FLASH_SERIAL_H_
#define ATB3500_FLASH_SERIAL_H_

#define ATB3500_SERIAL_SIZE             20
#define ATB3500_FLASH_SERIAL_PRV_SIZE   24

#define ATB3500_SERIAL_PROGRAMM_KEY1    0x6706DC0A
#define ATB3500_SERIAL_PROGRAMM_KEY2    0x542B3A46

typedef struct {
  uint32_t programm_key1;
  uint8_t serial[ATB3500_SERIAL_SIZE];
  uint32_t programm_key2;
} atb3500_flash_serial_proto_rx_t;

typedef enum {
  SERIAL_VALID     = (uint32_t)0,
  SERIAL_BURNING   = (uint32_t)1,
  SERIAL_EMPTY     = (uint32_t)2
} atb3500_flash_serial_status_t;

typedef struct {
  uint32_t status;
  uint8_t serial[ATB3500_SERIAL_SIZE];
} atb3500_flash_serial_proto_tx_t;

typedef struct {
  hdl_module_t module;
  PRIVATE(hdl, ATB3500_FLASH_SERIAL_PRV_SIZE);
} atb3500_flash_serial_t;

hdl_module_state_t atb3500_flash_serial(void *desc, uint8_t enable);
atb3500_flash_serial_proto_tx_t *atb3500_flash_serial_update(atb3500_flash_serial_t *desc, atb3500_flash_serial_proto_rx_t *data);


#endif //ATB3500_IO_H_