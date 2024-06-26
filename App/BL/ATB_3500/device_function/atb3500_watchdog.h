#ifndef ATB3500_WATCHDOG_H_
#define ATB3500_WATCHDOG_H_

#define ATB3500_WATCHDOG_RESET_MAGIC           ((uint32_t)(0x17AF61EF))
#define ATB3500_WATCHDOG_PRV_SIZE              40

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

uint32_t atb3500_watchdog_proto_rx_size(void);
void atb3500_watchdog_proto_set_map_rx(atb3500_watchdog_t *desc, proto_map_mem_t *map);
uint32_t atb3500_watchdog_proto_tx_size(void);
void atb3500_watchdog_proto_set_map_tx(atb3500_watchdog_t *desc, proto_map_mem_t *map);

hdl_module_state_t atb3500_watchdog_event_subscribe(atb3500_watchdog_t *desc, event_handler_t handler, void *context);


#endif //ATB3500_WATCHDOG_H_
