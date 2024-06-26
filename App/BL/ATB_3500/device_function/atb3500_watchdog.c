#include "device_logic.h"
#include "CodeLib.h"

typedef struct {
	uint32_t delay_ms;
	uint32_t magic_value_config;
	uint32_t magic_value_reset;
} atb3500_watchdog_proto_rx_t;

typedef struct {
	uint32_t delay_ms;
	uint32_t magic_value_config;
} atb3500_watchdog_proto_tx_t;

typedef struct {
    hdl_module_t module;
    /* private */
    proto_map_mem_t *map_rx;
    proto_map_mem_t *map_tx;
    uint32_t magic_value_config;
    hdl_delegate_t watchdog_delegate;
    uint32_t epoch;
} atb3500_watchdog_private_t;

_Static_assert(sizeof(atb3500_watchdog_private_t) == sizeof(atb3500_watchdog_t), "In atb3500_watchdog.h data structure size of atb3500_watchdog_t doesn't match, check ATB3500_WATCHDOG_PRV_SIZE");

uint32_t atb3500_watchdog_proto_rx_size(void) {
    return sizeof(atb3500_watchdog_proto_rx_t);
}

void atb3500_watchdog_proto_set_map_rx(atb3500_watchdog_t *desc, proto_map_mem_t *map) {
    atb3500_watchdog_private_t *wdt = (atb3500_watchdog_private_t*)desc;
    wdt->map_rx = map;
}

uint32_t atb3500_watchdog_proto_tx_size(void) {
    return sizeof(atb3500_watchdog_proto_tx_t);
}

static void _watchdog_update_tx(atb3500_watchdog_private_t *wdt) {
    bldl_communication_t *comm = (bldl_communication_t *)wdt->module.dependencies[0];
    hdl_timer_event_t *timer = (hdl_timer_event_t *)wdt->module.dependencies[1];
    atb3500_watchdog_proto_tx_t tx_data;
    tx_data.delay_ms = timer->delay;
    tx_data.magic_value_config = wdt->magic_value_config;
    communication_put(comm, wdt->map_tx, (void*)&tx_data);
}

void atb3500_watchdog_proto_set_map_tx(atb3500_watchdog_t *desc, proto_map_mem_t *map) {
    if((desc != NULL) && (map != NULL)) {
        // if init ok
        atb3500_watchdog_private_t *wdt = (atb3500_watchdog_private_t*)desc;
        wdt->map_tx = map;
        _watchdog_update_tx(wdt);
    }
}

static uint32_t _wd_gen_new_magic(uint32_t last) {
    return last + 1;
}

static uint8_t _watchdog_work(coroutine_desc_t this, uint8_t cancel, void *arg) {
    atb3500_watchdog_private_t *wdt = (atb3500_watchdog_private_t*)arg;
    bldl_communication_t *comm = (bldl_communication_t *)wdt->module.dependencies[0];
    hdl_timer_event_t *timer = (hdl_timer_event_t *)wdt->module.dependencies[1];
    uint32_t comm_ts = communication_epoch(comm);
    atb3500_watchdog_proto_rx_t rx_data;
    uint8_t rx_valid = (wdt->epoch != comm_ts)? communication_get(comm, wdt->map_rx, (void*)&rx_data): HDL_FALSE;
    if(rx_valid) {
        if(rx_data.magic_value_reset == ATB3500_WATCHDOG_RESET_MAGIC) {
            hdl_timer_event_reset(timer);
        }
        if(rx_data.magic_value_config == wdt->magic_value_config) {
            wdt->magic_value_config = _wd_gen_new_magic(wdt->magic_value_config);
            timer->delay = rx_data.delay_ms;
            if(rx_data.delay_ms)
                hdl_timer_event_run_once(timer);
            else
                hdl_timer_event_stop(timer);
            _watchdog_update_tx(wdt);
        }
        wdt->epoch = comm_ts;
    }
    return cancel;
}

hdl_module_state_t atb3500_watchdog(void *desc, uint8_t enable) {
    static coroutine_desc_static_t watchdog_task_buf;
    if(enable) {
        atb3500_watchdog_private_t *wdt = (atb3500_watchdog_private_t*)desc;
        hdl_timer_event_t *timer = (hdl_timer_event_t *)wdt->module.dependencies[1];
        wdt->watchdog_delegate.context = NULL;
        wdt->watchdog_delegate.handler = NULL;
        wdt->magic_value_config = _wd_gen_new_magic(0);
        hdl_event_subscribe(&timer->event, &wdt->watchdog_delegate);
        coroutine_add_static(&watchdog_task_buf, &_watchdog_work, (void *)desc);
        return HDL_MODULE_INIT_OK;
    }
    return HDL_MODULE_DEINIT_OK;
}

hdl_module_state_t atb3500_watchdog_event_subscribe(atb3500_watchdog_t *desc, event_handler_t handler, void *context) {
    atb3500_watchdog_private_t *wdt = (atb3500_watchdog_private_t*)desc;
    wdt->watchdog_delegate.context = context;
    wdt->watchdog_delegate.handler = handler;
}
