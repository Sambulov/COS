#include "app.h"


void bldl_som_boot_dev_en(const bldl_som_boot_lock_t *desc, const bldl_boot_dev_t dev) {
//   static uint8_t init = 1;
//   if(desc == NULL)
//     return;
//   if(init) {
//     hdl_gpio(desc->lock_pin_sd, HDL_TRUE);
//     hdl_gpio(desc->lock_pin_emmc, HDL_TRUE);
//     hdl_gpio(desc->lock_pin_qspi, HDL_TRUE);
//     init = 0;
//   }
//   hdl_gpio_write(desc->lock_pin_sd, ((dev & BLDL_BOOT_DEV_SD) != 0)? !desc->active_state_lock_sd: desc->active_state_lock_sd);
//   hdl_gpio_write(desc->lock_pin_emmc, ((dev & BLDL_BOOT_DEV_EMMC) != 0)? !desc->active_state_lock_emmc: desc->active_state_lock_emmc);
//   hdl_gpio_write(desc->lock_pin_qspi, ((dev & BLDL_BOOT_DEV_SPI) != 0)? !desc->active_state_lock_qspi: desc->active_state_lock_qspi);
}
