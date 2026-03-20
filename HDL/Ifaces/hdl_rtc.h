#ifndef HDL_RTC_H_
#define HDL_RTC_H_

typedef datetime_t hdl_datetime_t;

typedef enum {
  HDL_RTC_DATE_TIME_READED,
  HDL_RTC_DATE_TIME_SETED,
  HDL_RTC_DATE_TIME_READ_FAULT,
  HDL_RTC_DATE_TIME_SET_FAULT
} hdl_rtc_status_t;

typedef struct {
  hdl_rtc_status_t sync_status;
  hdl_datetime_t date_time;
} hdl_rtc_event_trigger_t;

typedef uint8_t (*hdl_rtc_sync_t)(const void *desc, const hdl_datetime_t *dt);

typedef struct {
  hdl_module_initializer_t init;
  hdl_event_subscribtion_t subscribe;
  hdl_rtc_sync_t sync;
} hdl_rtc_iface_t;

hdl_module_new_t(hdl_rtc_t, 0, void*, hdl_rtc_iface_t);

__STATIC_INLINE void hdl_rtc_subscribe(const void *desc, hdl_delegate_t *delegate) {
  MODULE_ASSERT(desc, );
  return ((hdl_rtc_iface_t *)((hdl_module_base_t *)desc)->iface)->subscribe(desc, delegate);
}

/* dt NULL to read time, !NULL to setup clock */
__STATIC_INLINE uint8_t hdl_rtc_sync(const void *desc, const hdl_datetime_t *dt) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_rtc_iface_t *)((hdl_module_base_t *)desc)->iface)->sync(desc, dt);
}

#endif /* HDL_RTC_H_ */
