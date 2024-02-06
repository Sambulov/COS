#ifndef POWER_CYCLE_H_
#define POWER_CYCLE_H_

#include "app.h"

#define SMARCV2P1

typedef struct
{
    void (*carrier_pwr_on_set)(uint8_t state);
    void (*carrier_stby_n_set)(uint8_t state);
    void (*reset_out_n_set)(uint8_t state);

    uint8_t (*vin_pwr_bad_n_get)();
    uint8_t (*reset_in_n_get)();
} smarc_pc_handle_t;

void power_cycle(smarc_pc_handle_t *handle);

#endif // POWER_CYCLE_H_