#ifndef SM_POWER_H
#define SM_POWER_H

#include "app.h"

typedef enum {
    SOC_STATE_INIT,
    SOC_STATE_OFF,
    SOC_STATE_COLD_START,
    SOC_STATE_START,
    SOC_STATE_RESET,
    SOC_STATE_ON,
} soc_state_t;

typedef struct {
    void (*delay)(uint32_t ms);
    uint32_t (*millis)();

    void (*set_btn_pwron)(uint8_t state);
    void (*set_pmic_rst)(uint8_t state);
    void (*set_reset_out)(uint8_t state);

    void (*enable_sd)(uint8_t state);
    void (*enable_emmc)(uint8_t state);
    void (*enable_qspi)(uint8_t state);

    uint8_t (*get_boot_sel0)();
    uint8_t (*get_boot_sel1)();
    uint8_t (*get_boot_sel2)();
    uint8_t (*get_force_recov)();
    uint8_t (*get_x1_power_btn)();
    uint8_t (*get_reset_in)();

    soc_state_t soc_state;
} sm_power_handle_t;

void sm_power_task(sm_power_handle_t *handle);

#endif // SM_POWER_H
