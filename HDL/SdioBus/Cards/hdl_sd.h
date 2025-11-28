#pragma once

#include "hdl_nvm.h"

#define HDL_SD_VAR_SIZE   4200

typedef struct {
  uint32_t power_up_delay;
} hdl_sd_config_t;

/* depends on:
  hdl_sdio_t
  hdl_time_counter (ms)
  hdl_gpio_t       (card detect)
  hdl_gpio_t       (power en)
*/
hdl_module_new_t(hdl_sd_t, HDL_SD_VAR_SIZE, hdl_sd_config_t *, hdl_nvm_iface_t);

extern const hdl_nvm_iface_t hdl_sd_mcu_iface;
