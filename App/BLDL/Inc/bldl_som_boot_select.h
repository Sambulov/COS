#ifndef BLDL_SMARC_BOOT_SELECT_H_
#define BLDL_SMARC_BOOT_SELECT_H_

typedef enum {
    BLDL_BOOT_SEL_NONE = 0x00,
    BLDL_BOOT_SEL_0    = 0x01,
    BLDL_BOOT_SEL_1    = 0x02,
    BLDL_BOOT_SEL_2    = 0x04,
    BLDL_BOOT_FR       = 0x08
} bldl_boot_select_t;

typedef struct {
  const hdl_gpio_t *gpio_fr;
  const hdl_gpio_t *gpio_bs2;
  const hdl_gpio_t *gpio_bs1;
  const hdl_gpio_t *gpio_bs0;
} bldl_som_boot_sel_t;

bldl_boot_select_t bldl_som_boot_sel_get(const bldl_som_boot_sel_t *desc);

bldl_boot_dev_t bldl_bootsel2bootdev(const bldl_boot_select_t bs);

#endif // BLDL_SMARC_BOOT_SELECT_H_
