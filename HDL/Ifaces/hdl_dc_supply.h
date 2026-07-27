#ifndef HDL_DC_SUPPLY_H_
#define HDL_DC_SUPPLY_H_

typedef enum {
  HDL_DC_SUPPLY_STATE_IDLE                = 0x00,
  HDL_DC_SUPPLY_STATE_POWER_OUT           = 0x01,
} hdl_dc_supply_state_t;

typedef enum {
  HDL_DC_SUPPLY_STATUS_UNKNOWN            = 0x00,
  HDL_DC_SUPPLY_STATUS_OUTPUT_ENABLED     = 0x01,
  HDL_DC_SUPPLY_STATUS_CV_CAPABILITY      = 0x02,
  HDL_DC_SUPPLY_STATUS_CC_CAPABILITY      = 0x04,
  HDL_DC_SUPPLY_STATUS_NON_ADJUSTABLE     = 0x08,

  HDL_DC_SUPPLY_STATUS_OK                 = 0x40,
  HDL_DC_SUPPLY_STATUS_GENERAL_FAULT      = 0x80,
} hdl_dc_supply_status_t;

typedef struct {
  decimal32_t voltage_min;
  decimal32_t voltage_max;
  decimal32_t current_min;
  decimal32_t current_max;
  decimal32_t power_max;
} hdl_dc_supply_limitations_t;

typedef struct {
  decimal32_t voltage_limit;
  decimal32_t current_limit;
} hdl_dc_supply_setpoint_t;

typedef struct {
  decimal32_t power_limit;
  decimal32_t voltage_ramp_rate;
  decimal32_t current_ramp_rate;
} hdl_dc_supply_guards_t;

typedef uint8_t (* hdl_dc_supply_trigger_t)(const void *desc);
typedef uint8_t (* hdl_dc_supply_set_state_t)(const void *desc, hdl_dc_supply_state_t state);
typedef uint8_t (* hdl_dc_supply_set_guards_t)(const void *desc, const hdl_dc_supply_guards_t *guards);
typedef uint8_t (* hdl_dc_supply_set_t)(const void *desc, const hdl_dc_supply_setpoint_t *setpoint);

typedef struct {
  hdl_dc_supply_status_t status;
  hdl_dc_supply_state_t state;
  const hdl_dc_supply_limitations_t *limitations;
  const hdl_dc_supply_setpoint_t *setpoint;
  const hdl_dc_supply_guards_t *guards;
} hdl_dc_supply_event_trigger_t;

typedef struct {
  hdl_module_initializer_t init;
  hdl_dc_supply_set_state_t set_state;
  hdl_dc_supply_set_guards_t set_guards;
  hdl_dc_supply_trigger_t trigger_event;
  hdl_dc_supply_set_t set;
  hdl_event_subscribtion_t subscribe; /* hdl_dc_supply_event_trigger_t */
} hdl_dc_supply_iface_t;

hdl_module_new_t(hdl_dc_supply_t, 0, void *, hdl_dc_supply_iface_t);

__STATIC_INLINE uint8_t hdl_dc_supply_trigger(const void *desc) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_dc_supply_iface_t *)((hdl_module_base_t *)desc)->iface)->trigger_event(desc);
}

__STATIC_INLINE uint8_t hdl_dc_supply_set_state(const void *desc, hdl_dc_supply_state_t state) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_dc_supply_iface_t *)((hdl_module_base_t *)desc)->iface)->set_state(desc, state);
}

__STATIC_INLINE uint8_t hdl_dc_supply_set(const void *desc, const hdl_dc_supply_setpoint_t *setpoint) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_dc_supply_iface_t *)((hdl_module_base_t *)desc)->iface)->set(desc, setpoint);
}

__STATIC_INLINE uint8_t hdl_dc_supply_set_guards(const void *desc, const hdl_dc_supply_guards_t *guards) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_dc_supply_iface_t *)((hdl_module_base_t *)desc)->iface)->set_guards(desc, guards);
}

__STATIC_INLINE void hdl_dc_supply_subscribe(const void *desc, hdl_delegate_t *delegate) {
  MODULE_ASSERT(desc, );
  ((hdl_dc_supply_iface_t *)((hdl_module_base_t *)desc)->iface)->subscribe(desc, delegate);
}

#endif /* HDL_DC_SUPPLY_H_ */
