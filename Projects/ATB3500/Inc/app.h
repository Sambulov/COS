#ifndef APP_H_
#define APP_H_

#include "hdl.h"
#include "atb3500_power_domain.h"
#include "atb3500_io.h"
#include "atb3500_watchdog.h"
#include "atb3500_flash_serial.h"
#include "atb3500_i2c_gateway.h"
#include "atb3500_rs485_ctrl.h"
#include "mig.h"

typedef struct {
  uint8_t
     power_domain_3v3_is_stable : 1,
     power_domain_2v5_is_stable : 1,
     power_domain_1v8_is_stable : 1,
     ready                      : 1;
  uint32_t time_stamp;
  uint32_t restart_delay;
  hdl_basic_buffer_t spi_buffer;
  hdl_basic_buffer_t spi_tx_buffer;
} dev_context_t;

void spi_event_handler(uint32_t event_trigger, void *sender, void *context);

void main(void);

#endif /* APP_H_ */
