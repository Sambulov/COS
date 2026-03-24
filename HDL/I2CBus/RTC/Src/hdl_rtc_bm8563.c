#include "hdl_iface.h"

typedef struct {
  uint8_t sec   :7, /* 00-59 seconds BCD */
          vl    :1; /* reliable clock/calendar information is 0: guaranteed; 1: no longer guaranteed */
  uint8_t min   :7, /* 00-59 minutes BCD */
          res0  :1; /* reserved */
  uint8_t hour  :6, /* 00-23 hours BCD */
          res1  :2; /* reserved */
  uint8_t day   :6, /* 01-31 days BCD */
          res2  :2; /* reserved */
  uint8_t dow   :3, /* Sun 1-7 Sat day of week BCD */
          res3  :5; /* reserved */
  uint8_t month :5, /* 01-12 month BCD */
          res4  :2, /* reserved */
          cent  :1; /* centure 0:20xx; 1:19xx */
  uint8_t year;     /* 00-99 year BCD */
} bm8563_data_t;

typedef struct {
  coroutine_t worker;
  hdl_event_t event;
  bm8563_data_t dt;
  uint8_t sync      : 1,
          sync_read : 1;
} hdl_rtc_bm8563_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_rtc_bm8563_var_t, *((hdl_rtc_bm8563_t *)0)->obj_var, HDL_RTC_BM8563_VAR_SIZE, "hdl_rtc_bm8563.h");

#define RTC_REG_FIRST     0x02
#define RTC_REG_AMOUNT    0x07

#define RTC_REG_SECONDS_VL 0x80  /* power reset flag */

static uint8_t _rtc_bm8563_build(bm8563_data_t *data, 
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
    data->min = CL_TO_BCD_BYTE(min);
    data->sec = CL_TO_BCD_BYTE(sec);
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static uint8_t _bm8563_worker(coroutine_t *this, uint8_t cancel, void *arg);

static uint8_t _bm8563_responce_awaiting(coroutine_t *this, uint8_t cancel, void *arg) {
  hdl_rtc_bm8563_t *bm8563 = (hdl_rtc_bm8563_t *)arg;
  hdl_i2c_mem_t *mem = (hdl_i2c_mem_t *)bm8563->dependencies[0];
  hdl_rtc_bm8563_var_t *bm8563_var = (hdl_rtc_bm8563_var_t *)bm8563->obj_var;
  hdl_i2c_mem_state_t state = hdl_i2c_mem_state(mem);
  if(!(state & HDL_I2C_MEM_BUSY)) {
    hdl_rtc_event_trigger_t trigger = {
      .date_time = {
        .day = CL_FROM_BCD_BYTE(bm8563_var->dt.day),
        .hours = CL_FROM_BCD_BYTE(bm8563_var->dt.hour),
        .minutes = CL_FROM_BCD_BYTE(bm8563_var->dt.min),
        .month = CL_FROM_BCD_BYTE(bm8563_var->dt.month),
        .seconds = CL_FROM_BCD_BYTE(bm8563_var->dt.sec),
        .time_zone_hours = 0,
        .time_zone_minutes = 0,
        .week_day = CL_FROM_BCD_BYTE(bm8563_var->dt.dow),
        .year = CL_FROM_BCD_BYTE(bm8563_var->dt.year)
      },
      .sync_status = HDL_RTC_DATE_TIME_READED
    };
    trigger.date_time.year += (bm8563_var->dt.cent? 1900 :2000);
    if(state & HDL_I2C_MEM_XFER_FAIL) 
      trigger.sync_status = bm8563_var->sync_read? HDL_RTC_DATE_TIME_READ_FAULT: HDL_RTC_DATE_TIME_SET_FAULT;
    else if(!bm8563_var->sync_read)
      trigger.sync_status = HDL_RTC_DATE_TIME_SETED;
    bm8563_var->sync = HDL_FALSE;
    hdl_module_give(mem, bm8563);
    hdl_event_raise(&bm8563_var->event, bm8563, &trigger);
    coroutine_set_handler(this, _bm8563_worker);
  }
  return cancel;
}

static uint8_t _bm8563_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  hdl_rtc_bm8563_t *bm8563 = (hdl_rtc_bm8563_t *)arg;
  hdl_i2c_mem_t *mem = (hdl_i2c_mem_t *)bm8563->dependencies[0];
  hdl_rtc_bm8563_var_t *bm8563_var = (hdl_rtc_bm8563_var_t *)bm8563->obj_var;
  if(bm8563_var->sync) {
      if(hdl_module_take(mem, bm8563)) {
        uint8_t requested = HDL_FALSE;
        if(bm8563_var->sync_read)
          requested = hdl_i2c_mem_read_r1(mem, bm8563->config, RTC_REG_FIRST, (uint8_t *)&bm8563_var->dt, RTC_REG_AMOUNT);
        else
          requested = hdl_i2c_mem_write_r1(mem, bm8563->config, RTC_REG_FIRST, (uint8_t *)&bm8563_var->dt, RTC_REG_AMOUNT);
        if(requested)
          coroutine_set_handler(this, _bm8563_responce_awaiting);
        else
          hdl_module_give(mem, bm8563);
      }
  }
  return cancel;
}

static hdl_module_state_t _hdl_rtc_bm8563(const void *desc, uint8_t enable) {
  hdl_rtc_bm8563_t *bm8563 = (hdl_rtc_bm8563_t *)desc;
  hdl_rtc_bm8563_var_t *bm8563_var = (hdl_rtc_bm8563_var_t *)bm8563->obj_var;
  if(enable) {
    bm8563_var->sync = HDL_FALSE;
    coroutine_add(&bm8563_var->worker, &_bm8563_worker, (void*)bm8563);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&bm8563_var->worker);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_rtc_sync(const void *desc, const hdl_datetime_t *dt) {
  hdl_rtc_bm8563_t *bm8563 = (hdl_rtc_bm8563_t *)desc;
  hdl_rtc_bm8563_var_t *bm8563_var = (hdl_rtc_bm8563_var_t *)bm8563->obj_var;
  if(bm8563_var->sync || (hdl_module_state(desc) != HDL_MODULE_ACTIVE)) 
    return HDL_FALSE;
  bm8563_var->sync = bm8563_var->sync_read = (dt == NULL);
  if(!bm8563_var->sync_read) {
    bm8563_var->sync = _rtc_bm8563_build(&bm8563_var->dt, 
      dt->year, dt->month, dt->day, dt->week_day, dt->hours, dt->minutes, dt->seconds);
  }
  return bm8563_var->sync;
}

static void _hdl_event_subscribtion(const void *desc, hdl_delegate_t *delegate) {
  hdl_rtc_bm8563_t *bm8563 = (hdl_rtc_bm8563_t *)desc;
  hdl_rtc_bm8563_var_t *bm8563_var = (hdl_rtc_bm8563_var_t *)bm8563->obj_var;
  hdl_event_subscribe(&bm8563_var->event, delegate);
}

const hdl_rtc_iface_t hdl_rtc_bm8563_iface = {
  .init = &_hdl_rtc_bm8563,
  .sync = &_hdl_rtc_sync,
  .subscribe = &_hdl_event_subscribtion
};
