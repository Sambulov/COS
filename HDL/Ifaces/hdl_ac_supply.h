#ifndef HDL_AC_SUPPLY_H_
#define HDL_AC_SUPPLY_H_

typedef enum {
  HDL_AC_SUPPLY_STATE_IDLE                = 0x00,
  HDL_AC_SUPPLY_STATE_VF                  = 0x01, /* off-grid or grid itself */
  HDL_AC_SUPPLY_STATE_PQ                  = 0x03, /* on-grid mode, power injection */
} hdl_ac_supply_state_t;

typedef enum {
  HDL_AC_SUPPLY_STATUS_UNKNOWN            = 0x00,
  HDL_AC_SUPPLY_STATUS_OUTPUT_ENABLED     = 0x01,
  HDL_AC_SUPPLY_STATUS_VF_CAPABILITY      = 0x02, /* Invertor mode supported */
  HDL_AC_SUPPLY_STATUS_PQ_CAPABILITY      = 0x04, /* Injertor mode supported */
  HDL_AC_SUPPLY_STATUS_NON_ADJUSTABLE     = 0x08, /* Grid itself */
  
  HDL_AC_SUPPLY_STATUS_OK                 = 0x40,
  HDL_AC_SUPPLY_STATUS_GENERAL_FAULT      = 0x80,
} hdl_ac_supply_status_t;

typedef struct {
  decimal32_t voltage_min;
  decimal32_t voltage_max;
  decimal32_t current_min;
  decimal32_t current_max;
  decimal32_t frequency_min;
  decimal32_t frequency_max;
  decimal32_t power_max;
} hdl_ac_supply_limitations_t;

typedef struct {
  decimal32_t active_power_limit;
  decimal32_t reactive_power_limit;
  decimal32_t current_limit;
} hdl_ac_supply_pq_setpoint_t;

typedef struct {
  decimal32_t voltage;
  decimal32_t frequency;
  decimal32_t current_limit;
} hdl_ac_supply_vf_setpoint_t;

typedef struct {
  decimal32_t cut_off_voltage;
  decimal32_t ramp_up_rate;
  decimal32_t ramp_down_rate;
} hdl_ac_supply_guards_t;

typedef struct {
  hdl_ac_supply_status_t status;
  hdl_ac_supply_state_t state;
  const hdl_ac_supply_limitations_t *limitations;
  const hdl_ac_supply_vf_setpoint_t *vf_setpoint;
  const hdl_ac_supply_pq_setpoint_t *pq_setpoint;
  const hdl_ac_supply_guards_t *guards;
} hdl_ac_supply_event_trigger_t;

typedef uint8_t (* hdl_ac_supply_trigger_t)(const void *desc);
typedef uint8_t (* hdl_ac_supply_set_state_t)(const void *desc, hdl_ac_supply_state_t state);
typedef uint8_t (* hdl_ac_supply_set_vf_t)(const void *desc, const hdl_ac_supply_vf_setpoint_t *setpoint);
typedef uint8_t (* hdl_ac_supply_set_pq_t)(const void *desc, const hdl_ac_supply_pq_setpoint_t *setpoint);
typedef uint8_t (* hdl_ac_supply_set_guards_t)(const void *desc, const hdl_ac_supply_guards_t *setpoint);

typedef struct {
  hdl_module_initializer_t init;
  hdl_ac_supply_set_state_t set_state;
  hdl_ac_supply_set_vf_t set_vf;
  hdl_ac_supply_set_pq_t set_pq;
  hdl_ac_supply_set_guards_t set_guards;
  hdl_ac_supply_trigger_t trigger_event;
  hdl_event_subscribtion_t subscribe; /* hdl_ac_supply_event_trigger_t */
} hdl_ac_supply_iface_t;

hdl_module_new_t(hdl_ac_supply_t, 0, void *, hdl_ac_supply_iface_t);

__STATIC_INLINE uint8_t hdl_ac_supply_trigger(const void *desc) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_ac_supply_iface_t *)((hdl_module_base_t *)desc)->iface)->trigger_event(desc);
}

__STATIC_INLINE uint8_t hdl_ac_supply_set_state(const void *desc, hdl_ac_supply_state_t state) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_ac_supply_iface_t *)((hdl_module_base_t *)desc)->iface)->set_state(desc, state);
}

__STATIC_INLINE uint8_t hdl_ac_supply_set_vf(const void *desc, const hdl_ac_supply_vf_setpoint_t *setpoint) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_ac_supply_iface_t *)((hdl_module_base_t *)desc)->iface)->set_vf(desc, setpoint);
}

__STATIC_INLINE uint8_t hdl_ac_supply_set_pq(const void *desc, const hdl_ac_supply_pq_setpoint_t *setpoint) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_ac_supply_iface_t *)((hdl_module_base_t *)desc)->iface)->set_pq(desc, setpoint);
}

__STATIC_INLINE uint8_t hdl_ac_supply_set_guards(const void *desc, const hdl_ac_supply_guards_t *guards) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_ac_supply_iface_t *)((hdl_module_base_t *)desc)->iface)->set_guards(desc, guards);
}

__STATIC_INLINE void hdl_ac_supply_subscribe(const void *desc, hdl_delegate_t *delegate) {
  MODULE_ASSERT(desc, );
  ((hdl_ac_supply_iface_t *)((hdl_module_base_t *)desc)->iface)->subscribe(desc, delegate);
}

#endif /* HDL_AC_SUPPLY_H_ */
