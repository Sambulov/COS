/*!
    \file app.c
    \author a.bodin
    
    \version 2024-06-04, V1.0.0, This file describe major device logic
 */

#include "app.h"
#include "CodeLib.h"

typedef struct {
  uint8_t
     power_domain_3v3_is_stable : 1,
     power_domain_2v5_is_stable : 1,
     power_domain_1v8_is_stable : 1;
  uint32_t time_stamp;
  uint32_t restart_delay;
  hdl_basic_buffer_t spi_buffer;
  hdl_basic_buffer_t spi_tx_buffer;
} dev_context_t;

void power_domain_rails_stable(dev_context_t *dev) {
    if(dev->power_domain_1v8_is_stable && dev->power_domain_2v5_is_stable && dev->power_domain_3v3_is_stable) {
        smarc_carrier_boot_select(&mod_smarc, SMARC_CARRIER_BOOT0 | SMARC_CARRIER_BOOT1 | SMARC_CARRIER_BOOT2);
        smarc_carrier_ready(&mod_smarc, HDL_TRUE);
    }
    else {
        smarc_carrier_ready(&mod_smarc, HDL_FALSE);
    }
}

void power_domain_1v8_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    if(event_trigger == PD_STATE_STABLE) {
        dev->power_domain_1v8_is_stable = 1;
    }
    else if(event_trigger == PD_STATE_OFF)
        dev->power_domain_1v8_is_stable = 0;
    power_domain_rails_stable(dev);
}

void power_domain_3v3_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    if(event_trigger == PD_STATE_STABLE) {
        dev->power_domain_3v3_is_stable = 1;
    }
    else if(event_trigger == PD_STATE_OFF)
        dev->power_domain_3v3_is_stable = 0;
    power_domain_rails_stable(dev);
}

void power_domain_2v5_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    if(event_trigger == PD_STATE_STABLE) {
        dev->power_domain_2v5_is_stable = 1;
    }
    else if(event_trigger == PD_STATE_OFF)
        dev->power_domain_2v5_is_stable = 0;
    power_domain_rails_stable(dev);
}


void smarc_carrier_event_handler(uint32_t event_trigger, void *sender, void *context) {
    if(event_trigger == SMARC_EVENT_CARRIER_SLEEP_TO_STBY_CIRCUITS) {
        atb3500_power_rail_event_subscribe(&rail_3v3, &power_domain_3v3_rail, context);
        atb3500_power_rail_event_subscribe(&rail_2v5, &power_domain_2v5_rail, context);
        atb3500_power_rail_event_subscribe(&rail_1v8, &power_domain_1v8_rail, context);
        atb3500_power_rail_set(&rail_3v3, HDL_TRUE);
        atb3500_power_rail_set(&rail_2v5, HDL_TRUE);
        atb3500_power_rail_set(&rail_1v8, HDL_TRUE);
    }
    else if(event_trigger == SMARC_EVENT_CARRIER_MODULE_RESET) {
        /* This pins aren`t provided, we have to notify driver about this */
        smarc_carrier_force_state(&mod_smarc, CARRIER_POWER_ON, HDL_FALSE); 
        smarc_carrier_force_state(&mod_smarc, CARRIER_STANDBY, HDL_FALSE);
        atb3500_power_rail_set(&rail_1v8, HDL_FALSE);
        atb3500_power_rail_set(&rail_2v5, HDL_FALSE);
        atb3500_power_rail_set(&rail_3v3, HDL_FALSE);
        atb3500_power_rail_set(&rail_5v, HDL_FALSE);
        smarc_carrier_shutdown(&mod_smarc);
    }
}

void power_domain_5v_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    if(event_trigger == PD_STATE_STABLE) {
        smarc_carrier_boot(&mod_smarc);
        smarc_carrier_power_good(&mod_smarc, HDL_TRUE);
        /* This pins aren`t provided, we have to notify driver about this */
        smarc_carrier_force_state(&mod_smarc, CARRIER_POWER_ON, HDL_TRUE); 
        smarc_carrier_force_state(&mod_smarc, CARRIER_STANDBY, HDL_TRUE);
    }
    else if(event_trigger == PD_STATE_OFF) {
        smarc_carrier_power_good(&mod_smarc, HDL_FALSE);
        atb3500_power_rail_set(&rail_1v8, HDL_FALSE);
        atb3500_power_rail_set(&rail_2v5, HDL_FALSE);
        atb3500_power_rail_set(&rail_3v3, HDL_FALSE);
        dev->restart_delay = 3000;
        dev->time_stamp = hdl_timer_get(&mod_systick_timer_ms);
    }
}

void power_domain_24v_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    if(event_trigger == PD_STATE_STABLE) {
        atb3500_power_rail_event_subscribe(&rail_5v, &power_domain_5v_rail, context);
        atb3500_power_rail_set(&rail_5v, HDL_TRUE);
    }
}

void watchdog_event_handler(uint32_t event_trigger, void *sender, void *context) {
    /* This pins aren`t provided, we have to notify driver about this */
    smarc_carrier_force_state(&mod_smarc, CARRIER_POWER_ON, HDL_FALSE); 
    smarc_carrier_force_state(&mod_smarc, CARRIER_STANDBY, HDL_FALSE);
    atb3500_power_rail_set(&rail_1v8, HDL_FALSE);
    atb3500_power_rail_set(&rail_2v5, HDL_FALSE);
    atb3500_power_rail_set(&rail_3v3, HDL_FALSE);
    atb3500_power_rail_set(&rail_5v, HDL_FALSE);
    smarc_carrier_shutdown(&mod_smarc);
}


#define WDT_CMD    0x00000000ED
#define IO_CMD     0x00000000D0

void spi_event_handler(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *cont = (dev_context_t *)context;
    if(event_trigger > 0) {
        uint32_t cmd = *(uint32_t *)cont->spi_buffer.data;
        if(cmd == WDT_CMD) {
            atb3500_watchdog_proto_tx_t *res = atb3500_watchdog_update(&mod_watchdog, (atb3500_watchdog_proto_rx_t *)(cont->spi_buffer.data+4));
            cont->spi_tx_buffer.data = (uint8_t *)res;
            cont->spi_tx_buffer.size = sizeof(atb3500_watchdog_proto_tx_t);
            hdl_spi_server_dma_set_tx_data(&mod_spi3_server_dma, &cont->spi_tx_buffer);
        }
        if(cmd == IO_CMD) {
            atb3500_io_proto_tx_t *res = atb3500_io_update(&mod_carrier_io, (atb3500_io_proto_rx_t *)(cont->spi_buffer.data+4));
            cont->spi_tx_buffer.data = (uint8_t *)res;
            cont->spi_tx_buffer.size = sizeof(atb3500_io_proto_tx_t);
            hdl_spi_server_dma_set_tx_data(&mod_spi3_server_dma, &cont->spi_tx_buffer);
        }
    }
    
}

void main() {
    static uint8_t buf[128];
    static dev_context_t context = {.spi_buffer.size = 128, .spi_buffer.data = buf};
    hdl_enable(&app_module);
    while (!hdl_init_complete()) {
        cooperative_scheduler(false);
    }
    atb3500_power_rail_event_subscribe(&rail_24v, &power_domain_24v_rail, &context);
    smarc_carrier_event_subscribe(&mod_smarc, &smarc_carrier_event_handler, &context);
    hdl_spi_server_dma_set_handler(&mod_spi3_server_dma, &spi_event_handler, &context);
    atb3500_watchdog_event_subscribe(&mod_watchdog, &watchdog_event_handler, &context);
    hdl_spi_server_dma_set_rx_buffer(&mod_spi3_server_dma, &context.spi_buffer);
    atb3500_power_rail_set(&rail_24v, HDL_TRUE);

    while (1) {
        cooperative_scheduler(false);
        if(context.restart_delay != 0) {
            uint32_t time_now = hdl_timer_get(&mod_systick_timer_ms);
            if (TIME_ELAPSED(context.time_stamp, context.restart_delay, time_now)) {
                context.restart_delay = 0;
                atb3500_power_rail_set(&rail_5v, HDL_TRUE);

            }
        }
    }
}
