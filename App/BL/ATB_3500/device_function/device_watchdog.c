#include "device_logic.h"



hdl_module_t watchdog = {
    .dependencies = hdl_module_dependencies(&mod_watchdog_timer.module)
};

void device_watchdog_event_handler(uint32_t event_trigger, void *sender, void *context) {
    if(context != NULL) {
        object_dictionary_t *od = (object_dictionary_t *)context;
        od->sb.sb_watchdog_event_occur = 1;
        hdl_gpio_toggle(&mod_do_led_1_0);
    }
}

void watchdog_init() {
    static hdl_delegate_t watchdog_smarc_delegate = {
        .context = NULL,
        .handler = NULL,
    };
    hdl_enable(&watchdog);
    watchdog_smarc_delegate.context = NULL;
    watchdog_smarc_delegate.handler = &device_watchdog_event_handler;
    hdl_event_subscribe(&mod_watchdog_timer.event, &watchdog_smarc_delegate);
}

void watchdog_reset() {
    hdl_timer_event_reset(&mod_watchdog_timer);
}
