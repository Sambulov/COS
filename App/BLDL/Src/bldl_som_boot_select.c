#include "bldl.h"

bldl_boot_select_t bldl_som_boot_sel_get(const bldl_som_boot_sel_t *desc) {
  static uint8_t init = 1;
  if(init) {
    
  }
  bldl_boot_select_t sel = BLDL_BOOT_SEL_NONE;
  if(hdl_gpio_read(desc->gpio_fr) == HDL_GPIO_LOW)
    sel |= BLDL_BOOT_FR;
  if(hdl_gpio_read(desc->gpio_bs0) == HDL_GPIO_LOW)
    sel |= BLDL_BOOT_SEL_0;
  if(hdl_gpio_read(desc->gpio_bs1) == HDL_GPIO_LOW)
    sel |= BLDL_BOOT_SEL_1;
  if(hdl_gpio_read(desc->gpio_bs2) == HDL_GPIO_LOW)
    sel |= BLDL_BOOT_SEL_2;
  return sel;
}

bldl_boot_dev_t bldl_bootsel2bootdev(const bldl_boot_select_t bs) {
  switch (bs) {
    case BLDL_BOOT_FR | BLDL_BOOT_SEL_0:                                     /* SD:       FR(on) BS2(off) BS1(off) BS0(on) */
      return BLDL_BOOT_DEV_SD;
    case BLDL_BOOT_FR | BLDL_BOOT_SEL_2 | BLDL_BOOT_SEL_1:                   /* EMMC:     FR(on) BS2(on) BS1(on) BS0(off) */
      return BLDL_BOOT_DEV_EMMC;
    case BLDL_BOOT_FR | BLDL_BOOT_SEL_2 | BLDL_BOOT_SEL_1 | BLDL_BOOT_SEL_0: /* SPI:      FR(on) BS2(on) BS1(on) BS0(on) */
      return BLDL_BOOT_DEV_SPI;
    case BLDL_BOOT_FR:                                                       /* SATA:     FR(on) BS2(off) BS1(off) BS0(off) */
    case BLDL_BOOT_SEL_NONE:                                                 /* RECOVERY: FR(off) BS2(off) BS1(off) BS0(off) */
    default:
      break;
  }
  return BLDL_BOOT_DEV_NONE;
}
