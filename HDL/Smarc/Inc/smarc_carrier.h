#ifndef BLDL_SMARC_CARRIER_H_
#define BLDL_SMARC_CARRIER_H_

#define BLDL_SMARC_CARRIER_PRV_SIZE    (12)

typedef enum {
    SMARC_CARRIER_BOOT0 = 0x01,
    SMARC_CARRIER_BOOT1 = 0x02,
    SMARC_CARRIER_BOOT2 = 0x04
} smarc_carrier_boot_select_e;

typedef enum {
    SMARC_EVENT_CARRIER_POWER_GOOD,
    SMARC_EVENT_CARRIER_SLEEP_TO_STBY_CIRCUITS,
    SMARC_EVENT_CARRIER_STBY_TO_RUNTIME_CIRCUITS,
    SMARC_EVENT_CARRIER_BOOT_READY,
    SMARC_EVENT_CARRIER_MODULE_RUNTIME,
    SMARC_EVENT_CARRIER_MODULE_RESET,
    SMARC_EVENT_CARRIER_RUNTIME_TO_STBY_CIRCUITS,
    SMARC_EVENT_CARRIER_STBY_TO_SLEEP_CIRCUITS,
    SMARC_EVENT_CARRIER_FAULT,
    SMARC_EVENT_CARRIER_POWER_FAULT,
    SMARC_EVENT_CARRIER_MODULE_BAD_STATE,
} smarc_carrier_event_e;

typedef enum {
    INITIAL          = 0x00,
    POWER_GOOD       = 0x01,
    BOOT_TRIGGER     = POWER_GOOD,
    CARRIER_POWER_ON = 0x04,
    CARRIER_STANDBY  = 0x08,
    CARRIER_READY    = 0x10,
    RESET_IN         = CARRIER_READY,
    RESET_OUT        = 0x40,
    RUNTIME          = RESET_OUT
} smarc_carrier_sate_t;

#define POWER_BAD_PIN               0
#define CARRIER_POWER_ON_PIN        1
#define CARRIER_STBY_PIN            2
#define RESET_IN_PIN                3
#define RESET_OUT_PIN               4
#define BOOT0_PIN                   5
#define BOOT1_PIN                   6
#define BOOT2_PIN                   7

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
    PRIVATE(bldl, BLDL_SMARC_CARRIER_PRV_SIZE);
} bldl_smarc_carrier_t;

hdl_module_state_t bldl_smarc_carrier(void *desc, uint8_t enable);

uint8_t smarc_carrier_event_subscribe(bldl_smarc_carrier_t *desc, hdl_event_handler_t handler, void *context);
void smarc_carrier_boot_select(bldl_smarc_carrier_t *desc, smarc_carrier_boot_select_e select);


void smarc_carrier_set_target_state(bldl_smarc_carrier_t *desc, smarc_carrier_sate_t state);
smarc_carrier_sate_t smarc_carrier_get_target_state(bldl_smarc_carrier_t *desc);
smarc_carrier_sate_t smarc_carrier_get_current_state(bldl_smarc_carrier_t *desc);
void smarc_carrier_force_state(bldl_smarc_carrier_t *desc, smarc_carrier_sate_t state, uint8_t active);

static inline void smarc_carrier_boot(bldl_smarc_carrier_t *desc) {
    smarc_carrier_set_target_state(desc, RUNTIME);
}

static inline void smarc_carrier_shutdown(bldl_smarc_carrier_t *desc) {
    smarc_carrier_set_target_state(desc, POWER_GOOD);
}

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
       while RESET_OUT# inactive - SMARC_EVENT_CARRIER_MODULE_BAD_STATE event will raise.
       while RESET_OUT# active - SMARC_EVENT_CARRIER_RUNTIME_TO_STBY_CIRCUITS event will raise. Switch to standby state.       
    3) CARRIER_PWR_ON became inactive, SMARC_CARRIER_EVENT_STBY_TO_SLEEP_CIRCUITS event will raise. 
       while RESET_OUT# inactive - SMARC_EVENT_CARRIER_MODULE_BAD_STATE event will raise.
       while RESET_OUT# active - SMARC_EVENT_CARRIER_STBY_TO_SLEEP_CIRCUITS event will raise. Switch to sleep state.
       if target state equals to or bigger than POWER_GOOD than POWER_BAD# pin will stay untouched 
       if target state lower than POWER_GOOD, POWER_BAD# will become active.
    4) if POWER_GOOD state resets while outher states active, POWER_BAD# pin set active, SMARC_EVENT_CARRIER_POWER_FAULT event will raise. 
       App notifies module about power fault state.
*/

#endif //BLDL_SMARC_CARRIER_H_