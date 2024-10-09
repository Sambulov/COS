#include "app.h"
#include "CodeLib.h"

typedef struct {
    hdl_module_t module;
    /* private */
    hdl_delegate_t watchdog_delegate;
    uint32_t delay;
    atb3500_watchdog_proto_tx_t tx_data;
} atb3500_watchdog_private_t;

HDL_ASSERRT_STRUCTURE_CAST(atb3500_watchdog_private_t, atb3500_watchdog_t, ATB3500_WATCHDOG_PRV_SIZE, atb3500_watchdog.h);

atb3500_watchdog_proto_tx_t *atb3500_watchdog_update(atb3500_watchdog_t *desc, atb3500_watchdog_proto_rx_t *rx_data) {
    atb3500_watchdog_private_t *wdt = (atb3500_watchdog_private_t*)desc;
    hdl_timer_t *timer = (hdl_timer_t *)wdt->module.dependencies[0];
    if(rx_data->magic_value_reset == ATB3500_WATCHDOG_RESET_MAGIC) {
        rx_data->magic_value_reset = 0;
        if(hdl_timer_mode(timer) != HDL_TIMER_EVENT_IDLE)
            hdl_timer_set(timer, wdt->delay, HDL_TIMER_EVENT_SINGLE);
    }
    if(rx_data->magic_value_config == ATB3500_WATCHDOG_CONFIG_MAGIC) {
        rx_data->magic_value_config = 0;
        wdt->delay = rx_data->delay_ms;
        if(wdt->delay)
            hdl_timer_set(timer, wdt->delay, HDL_TIMER_EVENT_SINGLE);
        else
            hdl_timer_set(timer, 0, HDL_TIMER_EVENT_IDLE);
    }
    wdt->tx_data.delay_ms = wdt->delay;
    wdt->tx_data.counter = hdl_timer_left(timer);
    return &wdt->tx_data;
}

hdl_module_state_t atb3500_watchdog(void *desc, uint8_t enable) {
    if(enable) {
        atb3500_watchdog_private_t *wdt = (atb3500_watchdog_private_t*)desc;
        hdl_timer_t *timer = (hdl_timer_t *)wdt->module.dependencies[0];
        wdt->watchdog_delegate.context = NULL;
        wdt->watchdog_delegate.handler = NULL;
        wdt->tx_data.ovn = ATB3500_WATCHDOG_OVN;
        hdl_event_subscribe(&timer->event, &wdt->watchdog_delegate);
        return HDL_MODULE_ACTIVE;
    }
    return HDL_MODULE_UNLOADED;
}

hdl_module_state_t atb3500_watchdog_event_subscribe(atb3500_watchdog_t *desc, hdl_event_handler_t handler, void *context) {
    atb3500_watchdog_private_t *wdt = (atb3500_watchdog_private_t*)desc;
    hdl_timer_t *timer = (hdl_timer_t *)wdt->module.dependencies[0];
    wdt->watchdog_delegate.context = context;
    wdt->watchdog_delegate.handler = handler;
    hdl_event_subscribe(&timer->event, &wdt->watchdog_delegate);
}

