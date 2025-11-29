#pragma once

#include "hdl_nvm.h"

#define HDL_SD_VAR_SIZE   128

typedef enum {
  HDL_SD_OCR_VW_2V7_2V8 = 0x00008000UL,
  HDL_SD_OCR_VW_2V8_2V9 = 0x00010000UL,
  HDL_SD_OCR_VW_2V9_3V0 = 0x00020000UL,
  HDL_SD_OCR_VW_3V0_3V1 = 0x00040000UL,
  HDL_SD_OCR_VW_3V1_3V2 = 0x00080000UL,
  HDL_SD_OCR_VW_3V2_3V3 = 0x00100000UL,
  HDL_SD_OCR_VW_3V3_3V4 = 0x00200000UL,
  HDL_SD_OCR_VW_3V4_3V5 = 0x00400000UL,
  HDL_SD_OCR_VW_3V5_3V6 = 0x00800000UL
} hdl_sd_ocr_vw_t;

typedef struct {
  uint32_t power_up_delay;
  uint32_t init_retry_delay;
  hdl_sd_ocr_vw_t voltage_window;
} hdl_sd_config_t;

/* depends on:
  hdl_sdio_t
  hdl_time_counter
  hdl_gpio_t       (card detect)
  hdl_gpio_t       (power en)
*/
hdl_module_new_t(hdl_sd_t, HDL_SD_VAR_SIZE, hdl_sd_config_t *, hdl_nvm_iface_t);

extern const hdl_nvm_iface_t hdl_sd_mcu_iface;
