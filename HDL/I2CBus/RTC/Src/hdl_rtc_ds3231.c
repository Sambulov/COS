#include "hdl_iface.h"

typedef struct {
  uint8_t sec      :7, /* 00-59 seconds BCD */
          res0     :1; /* reserved */
  uint8_t min      :7, /* 00-59 minutes BCD */
          res1     :1; /* reserved */
  uint8_t hour     :6, /* 00-23 hours BCD */
          hmode    :1, /* 12/#24 */
          res2     :1; /* reserved */
  uint8_t dow      :3, /* week day 1-7 */
          res3     :5; /* reserved */
  uint8_t day      :6, /* 01-31 days BCD */
          res4     :2; /* reserved */
  uint8_t month    :5, /* 01-12 month BCD */
          res5     :2, /* reserved */
          cent     :1; /* centure 0:20xx; 1:19xx */
  uint8_t year;        /* 00-99 year BCD */
} ds3231_data_t;

typedef struct {
  coroutine_t worker;
  hdl_event_t event;
  ds3231_data_t dt;
  uint8_t sync      : 1,
          sync_read : 1;
} hdl_rtc_ds3231_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_rtc_ds3231_var_t, *((hdl_rtc_ds3231_t *)0)->obj_var, HDL_RTC_DS3231_VAR_SIZE, "hdl_rtc_ds3231.h");

#define RTC_REG_FIRST      0x00
#define RTC_REG_AMOUNT     0x07

static uint8_t _rtc_ds3231_build(ds3231_data_t *data, 
  uint16_t year, uint8_t month, uint8_t day, uint8_t week_day, uint8_t hour, uint8_t min, uint8_t sec) {
  if((data != NULL) && 
     (year >= 1900) && (year <= 2099) &&
     (month >= 1) && (month <= 12) &&
     (day >= 1) && (day <= 31) &&
     (week_day >= 1) && (week_day <= 7) &&
     (hour <= 23) &&
     (min < 60) &&
     (sec < 60)) {
    data->year = CL_TO_BCD_BYTE(year);
    data->cent = (((year / 100) & 3) != 0);
    data->month = CL_TO_BCD_BYTE(month);
    data->day = CL_TO_BCD_BYTE(day);
    data->dow = week_day % 7;
    data->hour = CL_TO_BCD_BYTE(hour);
    data->hmode = 0; /* 24h */
    data->min = CL_TO_BCD_BYTE(min);
    data->sec = CL_TO_BCD_BYTE(sec);
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static uint8_t _ds3231_worker(coroutine_t *this, uint8_t cancel, void *arg);

static uint8_t _ds3231_responce_awaiting(coroutine_t *this, uint8_t cancel, void *arg) {
  hdl_rtc_ds3231_t *ds3231 = (hdl_rtc_ds3231_t *)arg;
  hdl_i2c_mem_t *mem = (hdl_i2c_mem_t *)ds3231->dependencies[0];
  hdl_rtc_ds3231_var_t *ds3231_var = (hdl_rtc_ds3231_var_t *)ds3231->obj_var;
  hdl_i2c_mem_state_t state = hdl_i2c_mem_state(mem);
  if(!(state & HDL_I2C_MEM_BUSY)) {
    hdl_rtc_event_trigger_t trigger = {
      .date_time = {
        .day = ds3231_var->dt.day,
        .hours = ds3231_var->dt.hour,
        .minutes = ds3231_var->dt.min,
        .month = ds3231_var->dt.month,
        .seconds = ds3231_var->dt.sec,
        .time_zone_hours = 0,
        .time_zone_minutes = 0,
        .week_day = ds3231_var->dt.dow,
        .year = ds3231_var->dt.year
      },
      .sync_status = HDL_RTC_DATE_TIME_READED
    };
    trigger.date_time.year += (ds3231_var->dt.cent? 1900 :2000);
    if(state & HDL_I2C_MEM_XFER_FAIL) 
      trigger.sync_status = ds3231_var->sync_read? HDL_RTC_DATE_TIME_READ_FAULT: HDL_RTC_DATE_TIME_SET_FAULT;
    else if(!ds3231_var->sync_read)
      trigger.sync_status = HDL_RTC_DATE_TIME_SETED;
    ds3231_var->sync = HDL_FALSE;
    hdl_module_give(mem, ds3231);
    hdl_event_raise(&ds3231_var->event, ds3231, &trigger);
    coroutine_set_handler(this, _ds3231_worker);
  }
  return cancel;
}

static uint8_t _ds3231_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  hdl_rtc_ds3231_t *ds3231 = (hdl_rtc_ds3231_t *)arg;
  hdl_i2c_mem_t *mem = (hdl_i2c_mem_t *)ds3231->dependencies[0];
  hdl_rtc_ds3231_var_t *ds3231_var = (hdl_rtc_ds3231_var_t *)ds3231->obj_var;
  if(ds3231_var->sync) {
      if(hdl_module_take(mem, ds3231)) {
        uint8_t requested = HDL_FALSE;
        if(ds3231_var->sync_read)
          requested = hdl_i2c_mem_read_r1(mem, ds3231->config, RTC_REG_FIRST, (uint8_t *)&ds3231_var->dt, RTC_REG_AMOUNT);
        else
          requested = hdl_i2c_mem_write_r1(mem, ds3231->config, RTC_REG_FIRST, (uint8_t *)&ds3231_var->dt, RTC_REG_AMOUNT);
        if(requested)
          coroutine_set_handler(this, _ds3231_responce_awaiting);
        else
          hdl_module_give(mem, ds3231);
      }
  }
  return cancel;
}

static hdl_module_state_t _hdl_rtc_ds3231(const void *desc, uint8_t enable) {
  hdl_rtc_ds3231_t *ds3231 = (hdl_rtc_ds3231_t *)desc;
  hdl_rtc_ds3231_var_t *ds3231_var = (hdl_rtc_ds3231_var_t *)ds3231->obj_var;
  if(enable) {
    ds3231_var->sync = HDL_FALSE;
    coroutine_add(&ds3231_var->worker, &_ds3231_worker, (void*)ds3231);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&ds3231_var->worker);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_rtc_sync(const void *desc, const hdl_datetime_t *dt) {
  hdl_rtc_ds3231_t *ds3231 = (hdl_rtc_ds3231_t *)desc;
  hdl_rtc_ds3231_var_t *ds3231_var = (hdl_rtc_ds3231_var_t *)ds3231->obj_var;
  if(ds3231_var->sync) return HDL_FALSE;
  ds3231_var->sync = ds3231_var->sync_read = (dt == NULL);
  if(!ds3231_var->sync_read) {
    ds3231_var->sync = _rtc_ds3231_build(&ds3231_var->dt, 
      dt->year, dt->month, dt->day, dt->week_day, dt->hours, dt->minutes, dt->seconds);
  }
  return ds3231_var->sync;
}

static void _hdl_event_subscribtion(const void *desc, hdl_delegate_t *delegate) {
  hdl_rtc_ds3231_t *ds3231 = (hdl_rtc_ds3231_t *)desc;
  hdl_rtc_ds3231_var_t *ds3231_var = (hdl_rtc_ds3231_var_t *)ds3231->obj_var;
  hdl_event_subscribe(&ds3231_var->event, delegate);
}

const hdl_rtc_iface_t hdl_rtc_ds3231_iface = {
  .init = &_hdl_rtc_ds3231,
  .sync = &_hdl_rtc_sync,
  .subscribe = &_hdl_event_subscribtion
};
