#include "app.h"

bldl_boot_select_t bldl_som_boot_sel_get(const bldl_som_boot_sel_t *desc) {
  static uint8_t init = 1;
  bldl_boot_select_t sel = BLDL_BOOT_SEL_NONE;
//   if(desc != NULL) {
//     if(init) {
//       hdl_gpio_init(desc->gpio_bs0);
//       hdl_gpio_init(desc->gpio_bs1);
//       hdl_gpio_init(desc->gpio_bs2);
//       hdl_gpio_init(desc->gpio_fr);
//       init = 0;
//     }
// /* NOTE: Force recovery pin shared with SWD interface on STM32L0XX and GD32E23X.
//    So this signal is unused in DEBUG mode */
// #if !defined(DEBUG) || (!defined(STM32L0XX) && !defined (GD32E23X))
//     if(hdl_gpio_read(desc->gpio_fr) == desc->active_state_fr)
//       sel |= BLDL_BOOT_FR;
// #endif
//     if(hdl_gpio_read(desc->gpio_bs0) == desc->active_state_bs0)
//       sel |= BLDL_BOOT_SEL_0;
//     if(hdl_gpio_read(desc->gpio_bs1) == desc->active_state_bs1)
//       sel |= BLDL_BOOT_SEL_1;
//     if(hdl_gpio_read(desc->gpio_bs2) == desc->active_state_bs2)
//       sel |= BLDL_BOOT_SEL_2;
//   }
  return sel;
}

bldl_boot_dev_t bldl_bootsel2bootdev(const bldl_boot_select_t bs) {
//   switch (bs) {
//     case BLDL_BOOT_FR | BLDL_BOOT_SEL_0:                                     /* SD:       FR(on) BS2(off) BS1(off) BS0(on) */
//       return BLDL_BOOT_DEV_SD;
//     case BLDL_BOOT_FR | BLDL_BOOT_SEL_2 | BLDL_BOOT_SEL_1:                   /* EMMC:     FR(on) BS2(on) BS1(on) BS0(off) */
//       return BLDL_BOOT_DEV_EMMC;
//     case BLDL_BOOT_FR | BLDL_BOOT_SEL_2 | BLDL_BOOT_SEL_1 | BLDL_BOOT_SEL_0: /* SPI:      FR(on) BS2(on) BS1(on) BS0(on) */
//       return BLDL_BOOT_DEV_SPI;
//     case BLDL_BOOT_FR:                                                       /* SATA:     FR(on) BS2(off) BS1(off) BS0(off) */
//     case BLDL_BOOT_SEL_NONE:                                                 /* RECOVERY: FR(off) BS2(off) BS1(off) BS0(off) */
//     default:
//       break;
//   }
  return BLDL_BOOT_DEV_NONE;
}
