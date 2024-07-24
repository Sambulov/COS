#ifndef HDL_SMARC_CARRIER_H_
#define HDL_SMARC_CARRIER_H_

/*
    Smarc carrier driver usage
    Prepare:
    Enable module, wait while it ready, subscribe to event [smarc_carrier_event_subscribe], set target state RUNTIME [smarc_carrier_set_target_state]
    Boot seq:
    1) Prepare sleep power rails, whait while it stable. Set POWER_GOOD state[smarc_carrier_force_state], if POWER_BAD# pin provided it will 
       triggers POWER_BAD# line to inactive state. Now smarc carrier waits CARRIER_PWR_ON become active. if corresponding pin is not provided, 
       trigger state from app.
    2) Wait SMARC_CARRIER_EVENT_STBY_CIRCUITS event, it will signalize CARRIER_PWR_ON became active. Prepare carrier standby mode. Now smarc 
       carrier waits CARRIER_STBY# become inactive. if corresponding pin is not provided, trigger state from app.
    3) Wait SMARC_CARRIER_EVENT_RUNTIME_CIRCUITS event, it will signalize CARRIER_STBY# became inactive. Prepare carrier runtime mode.
    4) When carrier ready for module boot, set CARRIER_READY state[smarc_carrier_force_state], it will trigger RESET_IN line to active state. 
       Now smarc carrier waits RESET_OUT# become inactive. if corresponding pin is not provided, trigger state from app.
    5) Wait SMARC_CARRIER_EVENT_RUNTIME event, it will signalize RESET_OUT# became inactive. Module boots, run service routines.

    Other states:
    1) RESET_OUT# became active, SMARC_CARRIER_EVENT_MODULE_RESET event will raise. CPU in reset state. Stop service routines.
       if target state equals to or bigger than CARRIER_READY than RESET_IN will stay untouched 
       if target state lower than CARRIER_READY, RESET_IN will become inactive.
    2) CARRIER_STBY# became active:
       while RESET_OUT# inactive - SMARC_CARRIER_EVENT_MODULE_BAD_STATE event will raise.
       while RESET_OUT# active - SMARC_CARRIER_EVENT_RUNTIME_TO_STBY_CIRCUITS event will raise. Switch to standby state.       
    3) CARRIER_PWR_ON became inactive: 
       while RESET_OUT# inactive - SMARC_CARRIER_EVENT_MODULE_BAD_STATE event will raise.
       while RESET_OUT# active - SMARC_CARRIER_EVENT_STBY_TO_SLEEP_CIRCUITS event will raise. Switch to sleep state.
       if target state equals to or bigger than POWER_GOOD than POWER_BAD# pin will stay untouched 
       if target state lower than POWER_GOOD, POWER_BAD# will become active.
    4) if POWER_GOOD state resets while outher states active, POWER_BAD# pin set active, SMARC_CARRIER_EVENT_POWER_FAULT event will raise. 
       App notifies module about power fault state.
*/

#define HDL_SMARC_CARRIER_PRV_SIZE    (40)

typedef enum {
    HDL_SMARC_CARRIER_BOOT0 = 0x01,
    HDL_SMARC_CARRIER_BOOT1 = 0x02,
    HDL_SMARC_CARRIER_BOOT2 = 0x04
} hdl_smarc_carrier_boot_select_t;

typedef enum {
    HDL_SMARC_CARRIER_EVENT_POWER_GOOD,
    HDL_SMARC_CARRIER_EVENT_SLEEP_TO_STBY_CIRCUITS,
    HDL_SMARC_CARRIER_EVENT_STBY_TO_RUNTIME_CIRCUITS,
    HDL_SMARC_CARRIER_EVENT_BOOT_READY,
    HDL_SMARC_CARRIER_EVENT_MODULE_RUNTIME,
    HDL_SMARC_CARRIER_EVENT_MODULE_RESET,
    HDL_SMARC_CARRIER_EVENT_RUNTIME_TO_STBY_CIRCUITS,
    HDL_SMARC_CARRIER_EVENT_STBY_TO_SLEEP_CIRCUITS,
    HDL_SMARC_CARRIER_EVENT_FAULT,
    HDL_SMARC_CARRIER_EVENT_POWER_FAULT,
    HDL_SMARC_CARRIER_EVENT_MODULE_BAD_STATE,
} hdl_smarc_carrier_event_t;

typedef enum {
    HDL_SMARC_CARRIER_STATE_INITIAL          = 0x00,
    HDL_SMARC_CARRIER_STATE_POWER_GOOD       = 0x01,
    HDL_SMARC_CARRIER_STATE_BOOT_TRIGGER     = HDL_SMARC_CARRIER_STATE_POWER_GOOD,
    HDL_SMARC_CARRIER_STATE_CARRIER_POWER_ON = 0x04,
    HDL_SMARC_CARRIER_STATE_CARRIER_STANDBY  = 0x08,
    HDL_SMARC_CARRIER_STATE_CARRIER_READY    = 0x10,
    HDL_SMARC_CARRIER_STATE_RESET_IN         = HDL_SMARC_CARRIER_STATE_CARRIER_READY,
    HDL_SMARC_CARRIER_STATE_RESET_OUT        = 0x40,
    HDL_SMARC_CARRIER_STATE_RUNTIME          = HDL_SMARC_CARRIER_STATE_RESET_OUT
} hdl_smarc_carrier_sate_t;

#define HDL_SMARC_CARRIER_DEPENDENCY_POWER_BAD_PIN               0
#define HDL_SMARC_CARRIER_DEPENDENCY_CARRIER_POWER_ON_PIN        1
#define HDL_SMARC_CARRIER_DEPENDENCY_CARRIER_STBY_PIN            2
#define HDL_SMARC_CARRIER_DEPENDENCY_RESET_IN_PIN                3
#define HDL_SMARC_CARRIER_DEPENDENCY_RESET_OUT_PIN               4
#define HDL_SMARC_CARRIER_DEPENDENCY_BOOT0_PIN                   5
#define HDL_SMARC_CARRIER_DEPENDENCY_BOOT1_PIN                   6
#define HDL_SMARC_CARRIER_DEPENDENCY_BOOT2_PIN                   7

/* depends on:
  gpio power bad (O)
  gpio carrier_power_on (I)
  gpio carrier_stand_by (I)
  gpio reset_in (O)
  gpio reset_out (I)
  gpio boot 0 (O)
  gpio boot 1 (O)
  gpio boot 2 (O)
  timer
*/
typedef struct {
    hdl_module_t module;
    uint32_t sw_power_btn_trig_delay;
    PRIVATE(hdl, HDL_SMARC_CARRIER_PRV_SIZE);
} hdl_smarc_carrier_t;

hdl_module_state_t hdl_smarc_carrier(void *desc, uint8_t enable);

uint8_t hdl_smarc_carrier_event_subscribe(hdl_smarc_carrier_t *desc, hdl_event_handler_t handler, void *context);
void hdl_smarc_carrier_boot_select(hdl_smarc_carrier_t *desc, hdl_smarc_carrier_boot_select_t select);


void hdl_smarc_carrier_set_target_state(hdl_smarc_carrier_t *desc, hdl_smarc_carrier_sate_t state);
hdl_smarc_carrier_sate_t hdl_smarc_carrier_get_target_state(hdl_smarc_carrier_t *desc);
hdl_smarc_carrier_sate_t hdl_smarc_carrier_get_current_state(hdl_smarc_carrier_t *desc);
void hdl_smarc_carrier_force_state(hdl_smarc_carrier_t *desc, hdl_smarc_carrier_sate_t state, uint8_t active);

static inline void hdl_smarc_carrier_boot(hdl_smarc_carrier_t *desc) {
    hdl_smarc_carrier_set_target_state(desc, HDL_SMARC_CARRIER_STATE_RUNTIME);
}

static inline void smarc_carrier_shutdown(hdl_smarc_carrier_t *desc) {
    hdl_smarc_carrier_set_target_state(desc, HDL_SMARC_CARRIER_STATE_POWER_GOOD);
}

#endif /* HDL_SMARC_CARRIER_H_ */
