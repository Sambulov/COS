#include "sm_power.h"

typedef enum {
  BOOT_DEV_NONE = 0x00,
  BOOT_DEV_SATA = 0x01,
  BOOT_DEV_SD   = 0x02,
  BOOT_DEV_EMMC = 0x04,
  BOOT_DEV_SPI  = 0x08,
  BOOT_DEV_ALL  = BOOT_DEV_SATA | BOOT_DEV_SD | BOOT_DEV_EMMC |BOOT_DEV_SPI
} boot_dev_enum_t;

void enable_boot_devices(sm_power_handle_t *handle, boot_dev_enum_t dev) {
  if(handle->enable_sd != NULL)
    handle->enable_sd((dev & BOOT_DEV_SD) != 0);
  if(handle->enable_emmc != NULL)
    handle->enable_emmc((dev & BOOT_DEV_EMMC) != 0);
  if(handle->enable_qspi != NULL)
    handle->enable_qspi((dev & BOOT_DEV_SPI) != 0);
}

static boot_dev_enum_t bootsel_to_boot_dev(sm_power_handle_t *handle) {
  if((handle->get_force_recov != NULL) &&
     (handle->get_boot_sel2 != NULL) &&
     (handle->get_boot_sel1 != NULL) &&
     (handle->get_boot_sel0 != NULL)) {
    switch ((handle->get_force_recov() << 3) | (handle->get_boot_sel2() << 2) | (handle->get_boot_sel1() << 1) | (handle->get_boot_sel0() << 0)) {
      case 0x09: /* SD:       FR(1) BS2(0) BS1(0) BS0(1) */
        return BOOT_DEV_SD;
      case 0x0E: /* EMMC:     FR(1) BS2(1) BS1(1) BS0(0) */
        return BOOT_DEV_EMMC;
      case 0x0f: /* SPI:      FR(1) BS2(1) BS1(1) BS0(1) */
        return BOOT_DEV_SPI;
      case 0x08: /* SATA:     FR(1) BS2(0) BS1(0) BS0(0) */
      case 0x00: /* RECOVERY: FR(0) BS2(0) BS1(0) BS0(0) */
      default: 
        break;
    }
  }
  return BOOT_DEV_NONE;
}

static void btn_process(sm_power_handle_t *handle) {
  #define DEBOUNCE_DELAY 50
  #define TIME_LONG_CLICK 5000

    static uint8_t state_prev = 1;
    static uint32_t time_start = 0;

    // Проброс кнопки
    handle->set_btn_pwron(!handle->get_x1_power_btn());

    uint8_t state_current = handle->get_x1_power_btn();

    if (state_prev == 1 && state_current == 0) // press
    {
        time_start = handle->millis();
    }
    else if (state_prev == 0 && state_current == 1) // release
    {
        uint32_t diff = handle->millis() - time_start;

        if (diff >= DEBOUNCE_DELAY && diff < TIME_LONG_CLICK)
        {
            if (handle->soc_state == SOC_STATE_OFF)
            {
                handle->soc_state = SOC_STATE_START;
            }
        }
        else if (diff >= TIME_LONG_CLICK)
        {
            handle->soc_state = SOC_STATE_OFF;
        }
    }

    state_prev = state_current;
}

static void reset_in_process(sm_power_handle_t *handle) {
  static uint8_t state_prev = 1;
  uint8_t state_current = handle->get_reset_in();
  if (state_prev == 0 && state_current == 1) {
    if (handle->soc_state == SOC_STATE_RESET) {
      handle->soc_state = SOC_STATE_START;
    }
  }
  else if (state_prev == 1 && state_current == 0) {
    if (handle->soc_state == SOC_STATE_ON) {
      handle->soc_state = SOC_STATE_RESET;
    }
  }
  state_prev = state_current;
}

static void exec_start(sm_power_handle_t *handle) {
  if (handle->get_force_recov()) {
    enable_boot_devices(handle, BOOT_DEV_ALL);
    handle->set_pmic_rst(0);
  }
  else {
    enable_boot_devices(handle, BOOT_DEV_SD);
    handle->set_pmic_rst(0);
    handle->delay(5000);
    enable_boot_devices(handle, BOOT_DEV_ALL);
  }
}

static void exec_reset(sm_power_handle_t *handle) {
  handle->set_pmic_rst(1);
  enable_boot_devices(handle, BOOT_DEV_NONE);
  handle->set_reset_out(0);
}

static void exec_cold_start(sm_power_handle_t *handle) {
  handle->set_reset_out(1);
  if (handle->get_force_recov()) {
    handle->set_btn_pwron(1);
    handle->delay(500);
    handle->set_btn_pwron(0);
    enable_boot_devices(handle, BOOT_DEV_ALL);
    handle->delay(500);
    handle->set_pmic_rst(0);
  }
  else {
    enable_boot_devices(handle, BOOT_DEV_SD);
    handle->set_btn_pwron(1);
    handle->delay(500);
    handle->set_btn_pwron(0);
    handle->delay(500);
    handle->set_pmic_rst(0);
    handle->delay(5000);
    enable_boot_devices(handle, BOOT_DEV_ALL);
  }
}

static void exec_off(sm_power_handle_t *handle) {
  handle->set_pmic_rst(1);
  enable_boot_devices(handle, BOOT_DEV_NONE);
}

void sm_power_task(sm_power_handle_t *handle) {
  static soc_state_t prev_state = SOC_STATE_INIT;
  if(prev_state == SOC_STATE_INIT) {
    handle->soc_state = SOC_STATE_OFF;
  }
  if(handle->soc_state != prev_state) {
    switch (handle->soc_state) {
      case SOC_STATE_OFF:
        exec_off(handle);
        break;
      case SOC_STATE_COLD_START:
        exec_cold_start(handle);
        handle->soc_state = SOC_STATE_ON;
        break;
      case SOC_STATE_START:
        exec_start(handle);
        handle->soc_state = SOC_STATE_ON;
        break;
      case SOC_STATE_RESET:
        exec_reset(handle);
        break;
      default:
        break;
    }
  }
  prev_state = handle->soc_state;
  btn_process(handle);
  reset_in_process(handle);
}
