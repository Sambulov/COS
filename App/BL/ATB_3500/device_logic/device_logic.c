/*!
    \file device_logic.c
    \author a.bodin
    
    \version 2024-06-04, V1.0.0, This file describe major device logic
 */
#if defined(ATB_3500)

#include "device_logic.h"
#include "app.h"
#include "CodeLib.h"

atb3500_power_rail_t rail_24v = {
    .module.init = &atb3500_power_rail,
    .module.dependencies = hdl_module_dependencies(
        &mod_systick_timer_ms.module,
        &mod_adc.module,
        &hdl_null_module,
        &hdl_null_module),
    .adc_scale = POWER_RAIL_ADC_SCALE_24V,
    .adc_src = &mod_adc_source_0_adc_24v,
    .uv_threshold = POWER_RAIL_UV_TRHESHOLD_24V,
    .ov_threshold = POWER_RAIL_OV_TRHESHOLD_24V,
    .raise_delay = POWER_RAIL_RAISE_DELAY_24V,
    .stabilization_delay = POWER_RAIL_STAB_DELAY_24V,
};

atb3500_power_rail_t rail_24vpoe = {
    .module.init = &atb3500_power_rail,
    .module.dependencies = hdl_module_dependencies(
        &mod_systick_timer_ms.module,
        &mod_adc.module,
        &rail_24v.module,
        &mod_do_24v_poe_power_on.module /*, 
        &mod_di_24v_poe_power_fault.module, 
        &mod_di_24v_poe_power_good.module */),
    .adc_scale = POWER_RAIL_ADC_SCALE_24V,
    .adc_src = &mod_adc_source_0_adc_24v,
    .uv_threshold = POWER_RAIL_UV_TRHESHOLD_24V,
    .ov_threshold = POWER_RAIL_OV_TRHESHOLD_24V,
    .raise_delay = POWER_RAIL_RAISE_DELAY_24V,
    .stabilization_delay = POWER_RAIL_STAB_DELAY_24V,
};

atb3500_power_rail_t rail_5v = {
    .module.init = &atb3500_power_rail,
    .module.dependencies = hdl_module_dependencies(
        &mod_systick_timer_ms.module,
        &mod_adc.module,
        &rail_24v.module,
        &mod_do_5v_power_on.module),
    .adc_scale = POWER_RAIL_ADC_SCALE_5V,
    .adc_src = &mod_adc_source_2_adc_5v,
    .uv_threshold = POWER_RAIL_UV_TRHESHOLD_5V,
    .ov_threshold = POWER_RAIL_OV_TRHESHOLD_5V,
    .raise_delay = POWER_RAIL_RAISE_DELAY_5V,
    .stabilization_delay = POWER_RAIL_STAB_DELAY_5V,
};

atb3500_power_rail_t rail_3v3 = {
    .module.init = &atb3500_power_rail,
    .module.dependencies = hdl_module_dependencies(
        &mod_systick_timer_ms.module,
        &mod_adc.module,
        &rail_5v.module,
        &hdl_null_module),
    .adc_scale = POWER_RAIL_ADC_SCALE_3V3,
    .adc_src = &mod_adc_source_3_adc_3v3,
    .uv_threshold = POWER_RAIL_UV_TRHESHOLD_3V3,
    .ov_threshold = POWER_RAIL_OV_TRHESHOLD_3V3,
    .raise_delay = POWER_RAIL_RAISE_DELAY_3V3,
    .stabilization_delay = POWER_RAIL_STAB_DELAY_3V3,
};

atb3500_power_rail_t rail_2v5 = {
    .module.init = &atb3500_power_rail,
    .module.dependencies = hdl_module_dependencies(
        &mod_systick_timer_ms.module,
        &mod_adc.module,
        &rail_5v.module,
        &hdl_null_module),
    .adc_scale = POWER_RAIL_ADC_SCALE_2V5,
    .adc_src = &mod_adc_source_4_adc_2v5,
    .uv_threshold = POWER_RAIL_UV_TRHESHOLD_2V5,
    .ov_threshold = POWER_RAIL_OV_TRHESHOLD_2V5,
    .raise_delay = POWER_RAIL_RAISE_DELAY_2V5,
    .stabilization_delay = POWER_RAIL_STAB_DELAY_2V5,
};

atb3500_power_rail_t rail_1v8 = {
    .module.init = &atb3500_power_rail,
    .module.dependencies = hdl_module_dependencies(
        &mod_systick_timer_ms.module,
        &mod_adc.module,
        &rail_5v.module,
        &hdl_null_module),
    .adc_scale = POWER_RAIL_ADC_SCALE_1V8,
    .adc_src = &mod_adc_source_5_adc_1v8,
    .uv_threshold = POWER_RAIL_UV_TRHESHOLD_1V8,
    .ov_threshold = POWER_RAIL_OV_TRHESHOLD_1V8,
    .raise_delay = POWER_RAIL_RAISE_DELAY_1V8,
    .stabilization_delay = POWER_RAIL_STAB_DELAY_1V8,
};

hdl_button_t power_button = {
    .module.init = &hdl_button,
    .module.dependencies = hdl_module_dependencies(&mod_do_smarc_power_btn.module, &mod_systick_timer_ms.module ),
    .debounce_delay = 50,
    .hold_delay = 0,
};

bldl_smarc_carrier_t mod_smarc = {
    .module.init = &bldl_smarc_carrier,
    .module.dependencies = hdl_module_dependencies(
        &hdl_null_module /* power good */,
        &hdl_null_module /* carrier_power_on */,
        &hdl_null_module /* carrier_stand_by */,
        &mod_do_smarc_reset_in.module, 
        &mod_di_smarc_reset_out.module,
        &mod_do_smarc_boot_0.module, 
        &mod_do_smarc_boot_1.module, 
        &mod_do_smarc_boot_2.module,
        &mod_systick_timer_ms.module),
};

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
    if(dev->power_domain_1v8_is_stable && dev->power_domain_2v5_is_stable && dev->power_domain_3v3_is_stable)
        smarc_carrier_ready(&mod_smarc, HDL_TRUE);
    else
        smarc_carrier_ready(&mod_smarc, HDL_FALSE);
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
        smarc_carrier_boot_select(&mod_smarc, SMARC_CARRIER_BOOT0 | SMARC_CARRIER_BOOT1 | SMARC_CARRIER_BOOT2);
        atb3500_power_rail_event_subscribe(&rail_3v3, &power_domain_3v3_rail, context);
        atb3500_power_rail_event_subscribe(&rail_2v5, &power_domain_2v5_rail, context);
        atb3500_power_rail_event_subscribe(&rail_1v8, &power_domain_1v8_rail, context);
        atb3500_power_rail_set(&rail_3v3, HDL_TRUE);
        atb3500_power_rail_set(&rail_2v5, HDL_TRUE);
        atb3500_power_rail_set(&rail_1v8, HDL_TRUE);
    }
    else if(event_trigger == SMARC_EVENT_CARRIER_STBY_TO_RUNTIME_CIRCUITS) {

    }
    else if(event_trigger == SMARC_EVENT_CARRIER_MODULE_RUNTIME) {
        /* Start runtime */
    }
    else if(event_trigger == SMARC_EVENT_CARRIER_MODULE_RESET) {
        atb3500_power_rail_set(&rail_1v8, HDL_FALSE);
        atb3500_power_rail_set(&rail_2v5, HDL_FALSE);
        atb3500_power_rail_set(&rail_3v3, HDL_FALSE);
        atb3500_power_rail_set(&rail_5v, HDL_FALSE);
    }
}

void power_domain_5v_rail(uint32_t event_trigger, void *sender, void *context) {
    dev_context_t *dev = (dev_context_t*)context;
    if(event_trigger == PD_STATE_STABLE) {
        smarc_carrier_power_good(&mod_smarc, HDL_TRUE);
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
    __NOP();
}

atb3500_io_t mod_carrier_io = {
    .module.init = atb3500_io,
    .module.dependencies = hdl_module_dependencies(
    /***********************************************************
    *                      LED
    ***********************************************************/
        &mod_do_led_0_0.module, &mod_do_led_0_1.module, &mod_do_led_0_2.module,
        &mod_do_led_1_0.module, &mod_do_led_1_1.module, &mod_do_led_1_2.module, 
        &mod_do_led_2_0.module, &mod_do_led_2_1.module, &mod_do_led_2_2.module, 
    /***********************************************************
    *                          RELAY
    ***********************************************************/
        &mod_do_relay1.module, &mod_do_relay2.module,
        
    /***********************************************************
    *                        Other
    ***********************************************************/
        &mod_do_pci_switch.module, &mod_do_lte_reset.module,
    /***********************************************************
    *                        X1 Connector
    ***********************************************************/
        &mod_di_module_address_1.module, &mod_di_module_address_2.module, &mod_di_module_address_3.module,
        &mod_di_module_address_4.module, &mod_di_module_address_5.module, &mod_di_external_periph_irq_1.module,
        &mod_di_external_periph_irq_2.module, &mod_do_external_output_1.module, &mod_do_external_output_2.module
    )
    
};

atb3500_watchdog_t mod_watchdog = {
    .module.init = &atb3500_watchdog,
    .module.dependencies = hdl_module_dependencies(&mod_watchdog_timer.module)
};

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
            hdl_spi_server_dma_set_tx_data(&mod_spi_server_dma, &cont->spi_tx_buffer);
        }
        if(cmd == IO_CMD) {
            atb3500_io_proto_tx_t *res = atb3500_io_update(&mod_carrier_io, (atb3500_io_proto_rx_t *)(cont->spi_buffer.data+4));
            cont->spi_tx_buffer.data = (uint8_t *)res;
            cont->spi_tx_buffer.size = sizeof(atb3500_io_proto_tx_t);
            hdl_spi_server_dma_set_tx_data(&mod_spi_server_dma, &cont->spi_tx_buffer);
        }
    }
    
}

#define NEW_SPI_DMA

#ifdef NEW_SPI_DMA

hdl_module_t power_domain = {
  .dependencies = hdl_module_dependencies( 
    &rail_24v.module,
    &rail_24vpoe.module,
    &rail_5v.module,
    &rail_3v3.module,
    &rail_2v5.module,
    &rail_1v8.module)
};

hdl_module_t app_module = {
    .dependencies = hdl_module_dependencies(
        &power_domain,
        &mod_smarc.module,
        &mod_carrier_io.module,
        &mod_watchdog.module,
        &mod_spi_server_dma.module
   )
};

void device_logic(void) {
    static uint8_t buf[128];
    static dev_context_t context = {.spi_buffer.size = 128, .spi_buffer.data = buf};
    hdl_enable(&app_module);
    while (!hdl_init_complete()) {
        cooperative_scheduler(false);
    }
    atb3500_power_rail_event_subscribe(&rail_24v, &power_domain_24v_rail, &context);
    smarc_carrier_event_subscribe(&mod_smarc, &smarc_carrier_event_handler, &context);
    hdl_spi_server_dma_set_handler(&mod_spi_server_dma, &spi_event_handler, &context);
    atb3500_watchdog_event_subscribe(&mod_watchdog, &watchdog_event_handler, &context);
    hdl_spi_server_dma_set_rx_buffer(&mod_spi_server_dma, &context.spi_buffer);
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

#else
hdl_module_t app_module = {
    .dependencies = hdl_module_dependencies(
        &mod_power_domain.module,
        &mod_smarc.module,
        &mod_carrier_io.module,
        &mod_watchdog.module
    )
};

void device_logic(void) {
    static dev_context_t context = {0};
    hdl_enable(&app_module);
    hdl_enable(&mod_spi_server_dma.module);
    power_domain_event_subscribe(&mod_power_domain, ATB3500_PD_24V, &power_domain_24v_rail, &context);
    smarc_carrier_event_subscribe(&mod_smarc, &smarc_carrier_event_handler, &context);
    atb3500_watchdog_event_subscribe(&mod_watchdog, &watchdog_event_handler, &context);
    /* proto map */
    proto_map_mem_t io_tx = { .offset = 0, .size = atb3500_io_proto_tx_size() };
    proto_map_mem_t io_rx = { .offset = 0, .size = atb3500_io_proto_rx_size() };
    proto_map_mem_t watchdog_tx = { .offset = 16, .size = atb3500_io_proto_tx_size() };
    proto_map_mem_t watchdog_rx = { .offset = 16, .size = atb3500_io_proto_rx_size() };

    communication_map_tx(&smarc_comm, &io_tx);
    communication_map_rx(&smarc_comm, &io_rx);
    communication_map_tx(&smarc_comm, &watchdog_tx);
    communication_map_rx(&smarc_comm, &watchdog_rx);

    atb3500_io_proto_set_map_tx(&mod_carrier_io, &io_tx);
    atb3500_io_proto_set_map_rx(&mod_carrier_io, &io_rx);
    atb3500_watchdog_proto_set_map_tx(&mod_watchdog, &watchdog_tx);
    atb3500_watchdog_proto_set_map_rx(&mod_watchdog, &watchdog_rx);

    //connector_init();
    //watchdog_init();
    while (!hdl_init_complete()) {
        cooperative_scheduler(false);
    }
    //TODO: smarc enable;
    while (1) {
        cooperative_scheduler(false);
        if(context.restart_delay != 0) {
            uint32_t time_now = hdl_timer_get(&mod_systick_timer_ms);
            if (TIME_ELAPSED(context.time_stamp, context.restart_delay, time_now)) {
                context.restart_delay = 0;
                power_domain_set(&mod_power_domain, ATB3500_PD_5V, HDL_TRUE);
            }
        }
    }
}
#endif

#endif /* ATB_3500 */
