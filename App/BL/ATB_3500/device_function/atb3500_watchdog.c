#include "device_logic.h"
#include "CodeLib.h"

typedef struct {
    hdl_module_t module;
    /* private */
    hdl_delegate_t watchdog_delegate;
    atb3500_watchdog_proto_tx_t tx_data;
} atb3500_watchdog_private_t;

_Static_assert(sizeof(atb3500_watchdog_private_t) == sizeof(atb3500_watchdog_t), "In atb3500_watchdog.h data structure size of atb3500_watchdog_t doesn't match, check ATB3500_WATCHDOG_PRV_SIZE");

atb3500_watchdog_proto_tx_t *atb3500_watchdog_update(atb3500_watchdog_t *desc, atb3500_watchdog_proto_rx_t *rx_data) {
    atb3500_watchdog_private_t *wdt = (atb3500_watchdog_private_t*)desc;
    hdl_timer_event_t *timer = (hdl_timer_event_t *)wdt->module.dependencies[0];
    if(rx_data->magic_value_reset == ATB3500_WATCHDOG_RESET_MAGIC) {
        hdl_timer_event_reset(timer);
    }
    if(rx_data->magic_value_config == ATB3500_WATCHDOG_CONFIG_MAGIC) {
        timer->delay = rx_data->delay_ms;
        if(rx_data->delay_ms)
            hdl_timer_event_run_once(timer);
        else
            hdl_timer_event_stop(timer);
    }
    wdt->tx_data.delay_ms = timer->delay;
    wdt->tx_data.counter = hdl_timer_event_get_timer(timer);
    return &wdt->tx_data;
}

static uint8_t _watchdog_work(coroutine_desc_t this, uint8_t cancel, void *arg) {
    atb3500_watchdog_private_t *wdt = (atb3500_watchdog_private_t*)arg;
    hdl_timer_event_t *timer = (hdl_timer_event_t *)wdt->module.dependencies[0];

    return cancel;
}

hdl_module_state_t atb3500_watchdog(void *desc, uint8_t enable) {
    if(enable) {
        atb3500_watchdog_private_t *wdt = (atb3500_watchdog_private_t*)desc;
        hdl_timer_event_t *timer = (hdl_timer_event_t *)wdt->module.dependencies[0];
        wdt->watchdog_delegate.context = NULL;
        wdt->watchdog_delegate.handler = NULL;
        wdt->tx_data.ovn = ATB3500_WATCHDOG_OVN;
        hdl_event_subscribe(&timer->event, &wdt->watchdog_delegate);
        return HDL_MODULE_INIT_OK;
    }
    return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t atb3500_watchdog_event_subscribe(atb3500_watchdog_t *desc, hdl_event_handler_t handler, void *context) {
    atb3500_watchdog_private_t *wdt = (atb3500_watchdog_private_t*)desc;
    hdl_timer_event_t *timer = (hdl_timer_event_t *)wdt->module.dependencies[1];
    wdt->watchdog_delegate.context = context;
    wdt->watchdog_delegate.handler = handler;
    hdl_event_subscribe(&timer->event, &wdt->watchdog_delegate);
}

