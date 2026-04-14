#ifndef HDL_METER_H_
#define HDL_METER_H_

typedef uint8_t (* hdl_meter_get_t)(const void *desc);

typedef enum {
  HDL_METER_MEASURE_OK,
  HDL_METER_MEASURE_FAULT,
} hdl_meter_measure_status_t;

typedef struct {
  hdl_meter_measure_status_t status;
  decimal32_t measure;
  uint32_t measure_id;
} hdl_meter_event_trigger_t;

typedef struct {
  hdl_module_initializer_t init;
  hdl_event_subscribtion_t subscribe; /* hdl_meter_event_trigger_t */
  hdl_meter_get_t trigger_measure;
} hdl_meter_iface_t;

hdl_module_new_t(hdl_meter_t, 0, void *, hdl_meter_iface_t);

__STATIC_INLINE void hdl_meter_subscribe(const void *desc, hdl_delegate_t *delegate) {
  MODULE_ASSERT(desc, );
  return ((hdl_meter_iface_t *)((hdl_module_base_t *)desc)->iface)->subscribe(desc, delegate);
}

__STATIC_INLINE uint8_t hdl_meter_trigger_measure(const void *desc) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_meter_iface_t *)((hdl_module_base_t *)desc)->iface)->trigger_measure(desc);
}

#endif /* HDL_METER_H_ */
