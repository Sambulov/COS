#ifndef BLDL_SMARC_CARRIER_H_
#define BLDL_SMARC_CARRIER_H_

typedef enum {
    SMARC_CARRIER_BOOT0 = 0x01,
    SMARC_CARRIER_BOOT1 = 0x02,
    SMARC_CARRIER_BOOT2 = 0x04
} smarc_carrier_boot_select_e;

typedef enum {
    SMARC_CARRIER_EVENT_STBY_CIRCUITS,
    SMARC_CARRIER_EVENT_RUNTIME_CIRCUITS,
    SMARC_CARRIER_EVENT_RUNTIME
} smarc_carrier_event_e;

/* depends on:
  gpio power bad (O)
  hdl_button pwr_in
  hdl_button pwr_out
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
    uint8_t carrier_power_on : 1,
            carrier_stby     : 1,
            reset_out        : 1;
    void *bl_context;
} bldl_smarc_carrier_t;

hdl_module_state_t bldl_smarc_carrier(void *desc, uint8_t enable);

uint8_t smarc_carrier_event_subscribe(bldl_smarc_carrier_t *desc, event_handler_t handler, void *context);
void smarc_carrier_boot_select(bldl_smarc_carrier_t *desc, smarc_carrier_boot_select_e select);
void smarc_carrier_redy(bldl_smarc_carrier_t *desc);
void smarc_carrier_power_good(bldl_smarc_carrier_t *desc);

#endif //BLDL_SMARC_CARRIER_H_
