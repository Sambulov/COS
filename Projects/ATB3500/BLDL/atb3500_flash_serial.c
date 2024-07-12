#include "app.h"
#include "CodeLib.h"

#define ATB3500_SERIAL_ADDRESS       (__flash_end__ - (ATB3500_SERIAL_SIZE - 1))

typedef struct {
  hdl_module_t module;
  atb3500_flash_serial_proto_tx_t tx_buf;
} atb3500_flash_serial_private_t;

_Static_assert(sizeof(atb3500_flash_serial_private_t) == sizeof(atb3500_flash_serial_t), "In atb3500_flash_serial.h data structure size of atb3500_flash_serial_t doesn't match, check ATB3500_FLASH_SERIAL_PRV_SIZE");

hdl_module_state_t atb3500_flash_serial(void *desc, uint8_t enable) {
  if(enable) {
    return HDL_MODULE_INIT_OK;
  }
  return HDL_MODULE_DEINIT_OK;
}

atb3500_flash_serial_proto_tx_t *atb3500_flash_serial_update(atb3500_flash_serial_t *desc, atb3500_flash_serial_proto_rx_t *data) {
  atb3500_flash_serial_private_t *serial = (atb3500_flash_serial_private_t *)desc;
  if(serial != NULL) {
    if((data->programm_key1 == ATB3500_SERIAL_PROGRAMM_KEY1) && (data->programm_key2 == ATB3500_SERIAL_PROGRAMM_KEY2)) {
      // flash new
    }
    serial->tx_buf.status = (*(uint8_t *)(ATB3500_SERIAL_ADDRESS-1) == 0xff)? SERIAL_EMPTY: SERIAL_VALID;
    mem_cpy(serial->tx_buf.serial, (uint8_t*)ATB3500_SERIAL_ADDRESS, ATB3500_SERIAL_SIZE);
    return &serial->tx_buf;
  }
  return NULL;
}
