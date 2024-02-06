#include "power_cycle.h"

#ifdef SMARCV2P1

void power_cycle(smarc_pc_handle_t *handle) {
    handle->carrier_pwr_on_set(0);
    handle->carrier_stby_n_set(1);
    handle->reset_out_n_set(0);

    while (handle->vin_pwr_bad_n_get() == 0) ;

    handle->carrier_pwr_on_set(1);
    handle->carrier_stby_n_set(1);

    while (handle->reset_in_n_get() == 0) ;

    handle->reset_out_n_set(1);
}
#endif
