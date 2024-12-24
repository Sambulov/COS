#ifndef HDL_SMARC_MODULE_H_
#define HDL_SMARC_MODULE_H_

/*
    Smarc module driver usage
    Prepare:
    Enable module, wait while it ready, subscribe to event [smarc_module_event_subscribe], set target state RUNTIME [smarc_module_set_target_state]
    Boot seq:
    1) Wait SMARC_MODULE_EVENT_POWER_GOOD event, it will signalize POWER_BAD# became inactive. Start sleep circuits. if corresponding 
       pin is not provided, force MODULE_POWER_GOOD state[smarc_module_force_state] from app.
    2) Wait SMARC_MODULE_EVENT_POWER_BTN event, it will signalize POWER_BTN# triggers. if corresponding pin is not provided, force 
       MODULE_POWER_ON state[smarc_module_force_state] from app. Start stand by circuits. MODULE_PWR_ON sets active.
    3) Set MODULE_STAND_BY state[smarc_module_force_state] when stand by circuits ready. It will set MODULE_STBY# inactive.
    4) Wait SMARC_MODULE_EVENT_STAND_BY event, it will signalize MODULE_STBY# is inactive. Prepare runtme circuits.
    5) Wait SMARC_MODULE_EVENT_MODULE_READY event, it will signalize RESET_IN is active. if corresponding pin is not provided, force 
       MODULE_TAKE_OFF state[smarc_module_force_state] from app. Now module awaits delay 100ms from MODULE_POWER_ON state and sets
       RESET_OUT# inactive.
    6) Wait SMARC_MODULE_EVENT_RUNTIME event, it will signalize RESET_OUT# is inactive. Boot CPU, run service routines.

    Other states:
    1) RESET_IN became inactive, SMARC_MODULE_EVENT_RESET event will raise. Set CPU in reset state, stop service routines. 
       RESET_OUT# sets active.
    2) POWER_BTN# became active. SMARC_MODULE_EVENT_POWER_BTN event will raise.
    3) POWER_BTN# active for a while. SMARC_MODULE_EVENT_POWER_BTN_HOLD event will raise.
    4) POWER_BAD# became active. SMARC_MODULE_EVENT_POWER_FAULT event will raise.

*/

#define HDL_SMARC_MODULE_PRV_SIZE    (36)

typedef struct {
  //uint32_t power_btn_hold_delay;
} hdl_smarc_module_config_t;

#define HDL_SMARC_MODULE_DEPENDENCY_POWER_BAD_PIN               0
#define HDL_SMARC_MODULE_DEPENDENCY_POWER_BTN                   1
#define HDL_SMARC_MODULE_DEPENDENCY_CARRIER_POWER_ON_PIN        2
#define HDL_SMARC_MODULE_DEPENDENCY_CARRIER_STBY_PIN            3
#define HDL_SMARC_MODULE_DEPENDENCY_RESET_IN_PIN                4
#define HDL_SMARC_MODULE_DEPENDENCY_RESET_OUT_PIN               5
#define HDL_SMARC_MODULE_DEPENDENCY_BOOT0_PIN                   6
#define HDL_SMARC_MODULE_DEPENDENCY_BOOT1_PIN                   7
#define HDL_SMARC_MODULE_DEPENDENCY_BOOT2_PIN                   8

/* depends on:
  gpio power bad (I)
  hdl_btn power btn
  gpio MODULE_power_on (O)
  gpio MODULE_stand_by (O)
  gpio reset_in (I)
  gpio reset_out (O)
  gpio boot 0 (I)
  gpio boot 1 (I)
  gpio boot 2 (I)
  timer
*/
typedef struct {
  hdl_module_t module;
  const hdl_smarc_module_config_t *config;
  hdl_event_t event;
  PRIVATE(hdl, HDL_SMARC_MODULE_PRV_SIZE);
} hdl_smarc_module_t;

typedef enum {
  HDL_SMARC_MODULE_EVENT_POWER_GOOD,
  HDL_SMARC_MODULE_EVENT_SLEEP_TO_STBY_CIRCUITS,
  HDL_SMARC_MODULE_EVENT_STBY_TO_RUNTIME_CIRCUITS,
  HDL_SMARC_MODULE_EVENT_BOOT_READY,
  HDL_SMARC_MODULE_EVENT_RUNTIME,
  HDL_SMARC_MODULE_EVENT_RESET,
  HDL_SMARC_MODULE_EVENT_RUNTIME_TO_STBY_CIRCUITS,
  HDL_SMARC_MODULE_EVENT_STBY_TO_SLEEP_CIRCUITS,
  HDL_SMARC_MODULE_EVENT_FAULT,
  HDL_SMARC_MODULE_EVENT_POWER_FAULT,
  HDL_SMARC_MODULE_EVENT_SHUTDOWN,
  HDL_SMARC_MODULE_EVENT_TERMINATE
} hdl_smarc_module_event_t;

typedef enum {
  HDL_SMARC_MODULE_STATE_OFF              = 0x00,
  HDL_SMARC_MODULE_STATE_MODULE_SLEEP     = 0x01,
  HDL_SMARC_MODULE_STATE_MODULE_STANDBY   = 0x02,
  HDL_SMARC_MODULE_STATE_READY            = 0x03,
  HDL_SMARC_MODULE_STATE_RUNTIME          = 0x04,
} hdl_smarc_module_sate_t;


hdl_module_state_t hdl_smarc_module(void *desc, uint8_t enable);

//void hdl_smarc_module_boot_select(hdl_smarc_module_t *desc, hdl_smarc_module_boot_select_t select);

hdl_smarc_module_sate_t hdl_smarc_module_get_current_state(hdl_smarc_module_t *desc);
void hdl_smarc_module_force_state(hdl_smarc_module_t *desc, hdl_smarc_module_sate_t state, uint8_t active);

#endif /* HDL_SMARC_MODULE_H_ */
