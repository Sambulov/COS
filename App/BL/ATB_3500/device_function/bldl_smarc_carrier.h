#ifndef BLDL_SMARC_CARRIER_H_
#define BLDL_SMARC_CARRIER_H_

#define BLDL_SMARC_CARRIER_PRV_SIZE    (16)

typedef enum {
    SMARC_CARRIER_BOOT0 = 0x01,
    SMARC_CARRIER_BOOT1 = 0x02,
    SMARC_CARRIER_BOOT2 = 0x04
} smarc_carrier_boot_select_e;

typedef enum {
    SMARC_EVENT_CARRIER_SLEEP_TO_STBY_CIRCUITS,
    SMARC_EVENT_CARRIER_STBY_TO_RUNTIME_CIRCUITS,
    SMARC_EVENT_CARRIER_MODULE_RUNTIME,
    SMARC_EVENT_CARRIER_MODULE_RESET,
    SMARC_EVENT_CARRIER_RUNTIME_TO_STBY_CIRCUITS,
    SMARC_EVENT_CARRIER_STBY_TO_SLEEP_CIRCUITS,
    SMARC_EVENT_CARRIER_FAULT,
    SMARC_EVENT_CARRIER_POWER_FAULT,
} smarc_carrier_event_e;

/* depends on:
  gpio power bad (O)
  hdl_button_pwr_out
  gpio carrier_power_on (I)
  gpio carrier_stand_by (I)
  gpio reset_in (O)
  gpio reset_out (O)
  timer
  gpio boot 0 (O)
  gpio boot 1 (O)
  gpio boot 2 (O)
*/
typedef struct {
    hdl_module_t module;
    uint32_t sw_power_btn_trig_delay;
    PRIVATE(bldl, BLDL_SMARC_CARRIER_PRV_SIZE);
} bldl_smarc_carrier_t;

hdl_module_state_t bldl_smarc_carrier(void *desc, uint8_t enable);

uint8_t smarc_carrier_event_subscribe(bldl_smarc_carrier_t *desc, event_handler_t handler, void *context);
void smarc_carrier_boot_select(bldl_smarc_carrier_t *desc, smarc_carrier_boot_select_e select);

void smarc_carrier_power_good(bldl_smarc_carrier_t *desc, uint8_t enable);
void smarc_carrier_ready(bldl_smarc_carrier_t *desc, uint8_t enable);

/*
    Smarc carrier driver usage
    Prepare:
    Enable module, subscribe to event [smarc_carrier_event_subscribe], wait while it ready
    Boot seq:
    1) Prepare sleep power rails, whait while it stable. Call [smarc_carrier_power_good], if POWER_BAD# pin provided it will triggers POWER_BAD# line to 
       inactive state and if sofware power button provided, triggers click. Now smarc carrier waits CARRIER_PWR_ON become active. if pin is not provided, 
       assume it active.
    2) Wait SMARC_CARRIER_EVENT_STBY_CIRCUITS event, it will signalize CARRIER_PWR_ON became active. Prepare carrier standby mode. Now smarc carrier waits 
       CARRIER_STBY# become inactive. if pin is not provided, assume it inactive.
    3) Wait SMARC_CARRIER_EVENT_RUNTIME_CIRCUITS event, it will signalize CARRIER_STBY# became inactive. Prepare carrier runtime mode.
    4) When carrier ready for module boot, call [smarc_carrier_ready], it will trigger RESET_IN line to active state. Now smarc carrier waits 
       RESET_OUT# become inactive. if pin is not provided, assume it inactive.
    5) Wait SMARC_CARRIER_EVENT_RUNTIME event, it will signalize RESET_OUT# became inactive. Module boots, run service routines.

    Shutdown seq:
    1) RESET_OUT# became active, SMARC_CARRIER_EVENT_MODULE_RESET event will raise. CPU in reset state. Stop service routines.
    2) RESET_IN became inactive, SMARC_EVENT_CARRIER_FAULT event will raise. App notifies module about fault state.
    3) CARRIER_STBY# became active, SMARC_CARRIER_EVENT_RUNTIME_TO_STBY_CIRCUITS event will raise. Switch carrier to standby mode.
    4) CARRIER_PWR_ON became inactive, SMARC_CARRIER_EVENT_STBY_TO_SLEEP_CIRCUITS event will raise. Switch carrier to sleep mode.
    5) POWER_BAD# became active, SMARC_EVENT_CARRIER_POWER_FAULT event will raise. App notifies module about power fault state.

AppInitState 
    POWER_GOOD           = 0x01,
    POWER_POWER_BTN_TRIG = 0x02,
    CARRIER_PWR_ON_SET   = 0x04,
    CARRIER_STBY_RESET   = 0x08,
    CARRIER_READY        = 0x10,
    RESET_IN_SET         = 0x20,
    RESET_OUT_SET        = 0x40,
    
*/

#endif //BLDL_SMARC_CARRIER_H_
