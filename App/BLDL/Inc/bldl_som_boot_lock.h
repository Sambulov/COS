#ifndef BLDL_BOOT_LOCK_H_
#define BLDL_BOOT_LOCK_H_

typedef enum {
  BLDL_BOOT_DEV_NONE = 0x00,
  BLDL_BOOT_DEV_SATA = 0x01,
  BLDL_BOOT_DEV_SD   = 0x02,
  BLDL_BOOT_DEV_EMMC = 0x04,
  BLDL_BOOT_DEV_SPI  = 0x08,
  BLDL_BOOT_DEV_ALL  = BLDL_BOOT_DEV_SATA | BLDL_BOOT_DEV_SD | BLDL_BOOT_DEV_EMMC | BLDL_BOOT_DEV_SPI
} bldl_boot_dev_t;

typedef struct {
  const hdl_gpio_state active_state_lock_sd;
  const hdl_gpio_state active_state_lock_emmc;
  const hdl_gpio_state active_state_lock_qspi;
  const hdl_gpio_pin_t *lock_pin_sd;
  const hdl_gpio_pin_t *lock_pin_emmc;
  const hdl_gpio_pin_t *lock_pin_qspi;
} bldl_som_boot_lock_t;

void bldl_som_boot_dev_en(const bldl_som_boot_lock_t *desc, const bldl_boot_dev_t dev);

#endif // BLDL_BOOT_LOCK
