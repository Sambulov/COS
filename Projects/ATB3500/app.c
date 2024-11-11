/*!
    \file app.c
    \author a.bodin
    
    \version 2024-06-04, V1.0.0, This file describe major device logic
 */

#include "app.h"
#include "CodeLib.h"

void clear_rts(uint32_t event_trigger, void *sender, void *context) {
    hdl_gpio_set_inactive(&mod_do_rs485_dir);
    hdl_tick_counter_stop(&mod_rs485_tick_counter);
}

void set_rts(uint32_t event_trigger, void *sender, void *context) {
	if(event_trigger & (uint32_t)mod_di_rs485_tx.module.reg) {
        hdl_gpio_set_active(&mod_do_rs485_dir);
        hdl_tick_counter_set(&mod_rs485_tick_counter, 0, mod_rs485_tick_counter.config.timer->period);
	}
}

void carrier_shutdown(dev_context_t *dev) {
    /* This pins aren`t provided, we have to notify driver about this */
    hdl_smarc_carrier_force_state(&mod_smarc, HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON, HDL_FALSE); 
    hdl_smarc_carrier_force_state(&mod_smarc, HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY, HDL_FALSE);
    atb3500_power_rail_set(&rail_1v8, HDL_FALSE);
    atb3500_power_rail_set(&rail_2v5, HDL_FALSE);
    atb3500_power_rail_set(&rail_3v3, HDL_FALSE);
    atb3500_power_rail_set(&rail_5v, HDL_FALSE);
    smarc_carrier_shutdown(&mod_smarc);
    dev->restart_delay = 3000;
    dev->time_stamp = hdl_time_counter_get(&mod_systick_timer_ms);
}

void carrier_stby(dev_context_t *dev) {
    atb3500_power_rail_set(&rail_3v3, HDL_TRUE);
    atb3500_power_rail_set(&rail_2v5, HDL_TRUE);
    atb3500_power_rail_set(&rail_1v8, HDL_TRUE);
}

void power_domain_1v8_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    dev->power_domain_1v8_is_stable = (event_trigger == PD_STATE_STABLE);
}

void power_domain_3v3_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    dev->power_domain_3v3_is_stable = (event_trigger == PD_STATE_STABLE);
}

void power_domain_2v5_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    dev->power_domain_2v5_is_stable = (event_trigger == PD_STATE_STABLE);
}

void power_domain_5v_rail(uint32_t event_trigger, void *sender, void *context) {
    if(event_trigger == PD_STATE_STABLE) {
        hdl_smarc_carrier_force_state(&mod_smarc, HDL_SMARC_CARRIER_STATE_POWER_GOOD, HDL_TRUE);
        /* Smarc pins aren`t provided, we have to notify driver about this states */
        hdl_smarc_carrier_force_state(&mod_smarc, HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON, HDL_TRUE); 
        hdl_smarc_carrier_force_state(&mod_smarc, HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY, HDL_TRUE);
    }
//    else 
//        carrier_shutdown((dev_context_t *)context);
}

void power_domain_24v_rail(uint32_t event_trigger, void *sender, void *context) {
    if(event_trigger == PD_STATE_STABLE) {
        atb3500_power_rail_set(&rail_24vpoe, HDL_TRUE);
        atb3500_power_rail_set(&rail_5v, HDL_TRUE);
    } 
}

void power_domain_24vpoe_rail(uint32_t event_trigger, void *sender, void *context) {
    if(event_trigger == PD_STATE_FAULT)
        atb3500_power_rail_set(&rail_24vpoe, HDL_FALSE);
}

void watchdog_event_handler(uint32_t event_trigger, void *sender, void *context) {
    carrier_shutdown((dev_context_t *)context);
}

void smarc_carrier_event_handler(uint32_t event_trigger, void *sender, void *context) {
    switch (event_trigger) {
        case HDL_SMARC_CARRIER_EVENT_SLEEP_TO_STBY_CIRCUITS: carrier_stby((dev_context_t *)context); break;
        case HDL_SMARC_CARRIER_EVENT_MODULE_RESET: carrier_shutdown((dev_context_t *)context); break;
        default: break;
    }
}

#define RX_BUFFER_SIZE    8192

void main() {
    static uint8_t buf[RX_BUFFER_SIZE];
    static dev_context_t context = {.spi_buffer.size = RX_BUFFER_SIZE, .spi_buffer.data = buf};
    static hdl_delegate_t clear_rts_delegate = {
        .handler = &clear_rts,
        .context = NULL
    };
    static hdl_delegate_t set_rts_delegate = {
        .handler = &set_rts,
        .context = NULL
    };

    hdl_enable(&app_module);
    while (!hdl_init_complete()) {
        cooperative_scheduler(false);
    }

    hdl_event_subscribe(&rs485_timer_int.event, &clear_rts_delegate);
    hdl_interrupt_request(&mod_int_ctrlr, &rs485_timer_int);

    hdl_event_subscribe(&rs485_tx_int.event, &set_rts_delegate);
    hdl_interrupt_request(&mod_int_ctrlr, &rs485_tx_int);

    atb3500_power_rail_event_subscribe(&rail_24v, &power_domain_24v_rail, &context);
    atb3500_power_rail_event_subscribe(&rail_24vpoe, &power_domain_24vpoe_rail, &context);
    atb3500_power_rail_event_subscribe(&rail_5v, &power_domain_5v_rail, &context);
    atb3500_power_rail_event_subscribe(&rail_3v3, &power_domain_3v3_rail, &context);
    atb3500_power_rail_event_subscribe(&rail_2v5, &power_domain_2v5_rail, &context);
    atb3500_power_rail_event_subscribe(&rail_1v8, &power_domain_1v8_rail, &context);

    hdl_smarc_carrier_event_subscribe(&mod_smarc, &smarc_carrier_event_handler, &context);
    hdl_spi_server_dma_set_handler(&mod_spi3_server_dma, &spi_event_handler, &context);
    atb3500_watchdog_event_subscribe(&mod_watchdog, &watchdog_event_handler, &context);
    hdl_spi_server_dma_set_rx_buffer(&mod_spi3_server_dma, &context.spi_buffer);
    atb3500_power_rail_set(&rail_24v, HDL_TRUE);

    hdl_smarc_carrier_boot_select(&mod_smarc, 0);
    hdl_smarc_carrier_boot(&mod_smarc);
    while (1) {
        cooperative_scheduler(false);
        uint8_t pow_stable = context.power_domain_1v8_is_stable && context.power_domain_2v5_is_stable && context.power_domain_3v3_is_stable;
        if(!context.ready && pow_stable)
            hdl_smarc_carrier_force_state(&mod_smarc, HDL_SMARC_CARRIER_STATE_CARRIER_READY, HDL_TRUE);
        else if(context.ready && !pow_stable) 
            hdl_smarc_carrier_force_state(&mod_smarc, HDL_SMARC_CARRIER_STATE_CARRIER_READY, HDL_FALSE);

        if(context.restart_delay != 0) {
            uint32_t time_now = hdl_time_counter_get(&mod_systick_timer_ms);
            if (TIME_ELAPSED(context.time_stamp, context.restart_delay, time_now)) {
                context.restart_delay = 0;
                atb3500_power_rail_set(&rail_5v, HDL_TRUE);
                hdl_smarc_carrier_boot(&mod_smarc);
            }
        }
    }
}
