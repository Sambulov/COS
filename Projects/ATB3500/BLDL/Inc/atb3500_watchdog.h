#ifndef ATB3500_WATCHDOG_H_
#define ATB3500_WATCHDOG_H_

#define ATB3500_WATCHDOG_RESET_MAGIC           ((uint32_t)(0x17AF61EF))
#define ATB3500_WATCHDOG_CONFIG_MAGIC          ((uint32_t)(0xBADFEED0))
#define ATB3500_WATCHDOG_PRV_SIZE              40

#define ATB3500_WATCHDOG_OVN                   ((uint32_t)(0x12345678))

typedef struct {
	uint32_t delay_ms;
	uint32_t magic_value_config;
	uint32_t magic_value_reset;
} atb3500_watchdog_proto_rx_t;

typedef struct {
	uint32_t ovn;
	uint32_t delay_ms;
	uint32_t counter;
} atb3500_watchdog_proto_tx_t;


/*
  depends on:
  communication
  hdl_timer_event 
*/
typedef struct {
    hdl_module_t module;
    PRIVATE(bldl, ATB3500_WATCHDOG_PRV_SIZE);
} atb3500_watchdog_t;

hdl_module_state_t atb3500_watchdog(void *desc, uint8_t enable);
hdl_module_state_t atb3500_watchdog_event_subscribe(atb3500_watchdog_t *desc, hdl_event_handler_t handler, void *context);
atb3500_watchdog_proto_tx_t *atb3500_watchdog_update(atb3500_watchdog_t *desc, atb3500_watchdog_proto_rx_t *rx_data);

#endif //ATB3500_WATCHDOG_H_
