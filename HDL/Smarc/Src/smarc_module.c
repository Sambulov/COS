#include "hdl.h"
#include "CodeLib.h"

// typedef enum {
//   /* input */
//   HDL_SMARC_MODULE_SIGNAL_POWER_GOOD      = 0x01,
//   HDL_SMARC_MODULE_SIGNAL_POW_BTN_CLICK   = 0x02,
//   HDL_SMARC_MODULE_SIGNAL_POW_BTN_HOLD    = 0x04,
//   HDL_SMARC_MODULE_SIGNAL_RESET_IN        = 0x08,

//   /* output */
//   HDL_SMARC_MODULE_SIGNAL_CARRIER_PWR_ON  = 0x10,
//   HDL_SMARC_MODULE_SIGNAL_CARRIER_STANDBY = 0x20,
//   HDL_SMARC_MODULE_SIGNAL_RESET_OUT       = 0x40,
//   HDL_SMARC_MODULE_SIGNAL_POW_BTN_HOLD    = 0x80,

// } hdl_smarc_module_signals_t;


// typedef struct {
//   hdl_module_t module;
//   const hdl_smarc_module_config_t *config;
//   hdl_event_t event;
//   struct {
//     hdl_smarc_module_signals_t signal;
//     hdl_smarc_module_sate_t state;
//     hdl_delegate_t btn_delegate;
//     void *event_context;
//     coroutine_t worker;
//   } private;
// } hdl_smarc_module_private_t;

// HDL_ASSERRT_STRUCTURE_CAST(hdl_smarc_module_private_t, hdl_smarc_module_t, HDL_SMARC_MODULE_PRV_SIZE, hdl_smarc_module.h);

// static hdl_gpio_pin_t *_smarc_get_ctrl_pin(hdl_smarc_module_private_t *module, uint32_t pin_no) {
//   hdl_gpio_pin_t *pin = (hdl_gpio_pin_t *)module->module.dependencies[pin_no];
//   return HDL_IS_NULL_MODULE(pin)? NULL: pin;
// }

// static void _smarc_pow_btn_handler(uint32_t event_trigger, void *sender, void *context) {
//   hdl_smarc_module_private_t *smarc = (hdl_smarc_module_private_t *)context;
//   if(event_trigger == HDL_BTN_EVENT_CLICK) {
//     smarc->private.new_state |= HDL_SMARC_MODULE_SIGNAL_POW_BTN_HOLD;
//   }
//   if(event_trigger == HDL_BTN_EVENT_HOLD) {
//     smarc->private.new_state |= HDL_SMARC_MODULE_SIGNAL_POW_BTN_CLICK;    
//   }
// }

// static uint8_t _smarc_module_work(coroutine_t *this, uint8_t cancel, void *arg) {
//   hdl_smarc_module_private_t *module = (hdl_smarc_module_private_t *)arg;

//   hdl_gpio_pin_t *power_bad = _smarc_get_ctrl_pin(module, HDL_SMARC_MODULE_DEPENDENCY_POWER_BAD_PIN);
//   hdl_gpio_pin_t *carrier_pwr_on = _smarc_get_ctrl_pin(module, HDL_SMARC_MODULE_DEPENDENCY_CARRIER_POWER_ON_PIN);
//   hdl_gpio_pin_t *carrier_stby = _smarc_get_ctrl_pin(module, HDL_SMARC_MODULE_DEPENDENCY_CARRIER_STBY_PIN);
//   hdl_gpio_pin_t *reset_in = _smarc_get_ctrl_pin(module, HDL_SMARC_MODULE_DEPENDENCY_RESET_IN_PIN);
//   hdl_gpio_pin_t *reset_out = _smarc_get_ctrl_pin(module, HDL_SMARC_MODULE_DEPENDENCY_RESET_OUT_PIN);

//   if(reset_in != NULL) {
//     if(hdl_gpio_is_inactive(reset_in)) module->private.new_state |= HDL_SMARC_MODULE_SIGNAL_RESET;
//     else module->private.new_state &= ~HDL_SMARC_MODULE_SIGNAL_RESET;
//   }

//   if(power_bad != NULL) {
//     if(hdl_gpio_is_inactive(power_bad)) module->private.new_state |= HDL_SMARC_MODULE_SIGNAL_POWER_GOOD;
//     else module->private.new_state &= ~HDL_SMARC_MODULE_SIGNAL_POWER_GOOD;
//   }

//   if(module->private.new_state != module->private.state) {
    
//     //TODO: set gpio

//     if(HDL_REG_DIF(module->private.new_state, HDL_SMARC_MODULE_STATE_RUNTIME, module->private.state)) {
//       if(HDL_REG_CHECK(module->private.new_state, HDL_SMARC_MODULE_STATE_RUNTIME)) {
//         hdl_event_raise(&module->event, module, HDL_SMARC_MODULE_EVENT_RUNTIME);

//         module->private.state |= HDL_SMARC_MODULE_STATE_RUNTIME;
//       }
//       else {
//         //reset out
//         module->private.state &= ~HDL_SMARC_MODULE_STATE_RUNTIME;
//         hdl_event_raise(&module->event, module, HDL_SMARC_MODULE_EVENT_RESET);
//       }
//     }


//     if(HDL_REG_DIF(module->private.new_state, HDL_SMARC_MODULE_SIGNAL_RESET | HDL_SMARC_MODULE_STATE_READY, module->private.state)) {
//       if(HDL_REG_CHECK(module->private.new_state, HDL_SMARC_MODULE_SIGNAL_RESET | HDL_SMARC_MODULE_STATE_READY)) {
//         //TODO: delay 100ms from carrier power on
//         module->private.state |= HDL_SMARC_MODULE_SIGNAL_RESET | HDL_SMARC_MODULE_STATE_READY;
//         //reset out

//       }
//       else {

//       }
//     }

//     if(HDL_REG_DIF(module->private.new_state, HDL_SMARC_MODULE_STATE_MODULE_STANDBY, module->private.state)) {
//       if(HDL_REG_CHECK(module->private.new_state, HDL_SMARC_MODULE_STATE_MODULE_STANDBY)) {
//         hdl_event_raise(&module->event, module, HDL_SMARC_MODULE_EVENT_STBY_TO_RUNTIME_CIRCUITS);
//         module->private.state |= HDL_SMARC_MODULE_STATE_MODULE_STANDBY;
//       }
//       else {
//         module->private.state &= ~HDL_SMARC_MODULE_STATE_MODULE_STANDBY;
//         hdl_event_raise(&module->event, module, HDL_SMARC_MODULE_EVENT_RUNTIME_TO_STBY_CIRCUITS);
//       }
//     }

//     if(HDL_REG_DIF(module->private.new_state, HDL_SMARC_MODULE_SIGNAL_POW_BTN_HOLD, module->private.state)) {
//       module->private.new_state &= ~HDL_SMARC_MODULE_SIGNAL_POW_BTN_HOLD;
//       if(module->private.new_state & HDL_SMARC_MODULE_STATE_MODULE_SLEEP) {
//         hdl_event_raise(&module->event, module, HDL_SMARC_MODULE_EVENT_TERMINATE);
//       }
//     }

//     if(HDL_REG_DIF(module->private.new_state, HDL_SMARC_MODULE_STATE_MODULE_POWER_ON, module->private.state)) {
//       if(HDL_REG_CHECK(module->private.new_state, HDL_SMARC_MODULE_STATE_MODULE_POWER_ON)) {
//         hdl_event_raise(&module->event, module, HDL_SMARC_MODULE_EVENT_SLEEP_TO_STBY_CIRCUITS);
//         module->private.state |= HDL_SMARC_MODULE_STATE_MODULE_POWER_ON;
//       }
//       else {
//         module->private.state &= ~HDL_SMARC_MODULE_STATE_MODULE_POWER_ON;
//         hdl_event_raise(&module->event, module, HDL_SMARC_MODULE_EVENT_STBY_TO_SLEEP_CIRCUITS);
//       }
//     }

//     if(HDL_REG_DIF(module->private.new_state, HDL_SMARC_MODULE_STATE_POW_BTN_CLICK, module->private.state)) {
//       module->private.new_state &= ~HDL_SMARC_MODULE_STATE_POW_BTN_CLICK;
//       if(HDL_REG_CHECK(module->private.new_state, HDL_SMARC_MODULE_STATE_RUNTIME)) {
//         hdl_event_raise(&module->event, module, HDL_SMARC_MODULE_EVENT_SHUTDOWN);
//       }
//       if(HDL_REG_CHECK(module->private.new_state, HDL_SMARC_MODULE_STATE_POWER_GOOD)) {
//         module->private.new_state |= HDL_SMARC_MODULE_STATE_MODULE_POWER_ON;
//       }
//     }

//     if(HDL_REG_DIF(module->private.new_state, HDL_SMARC_MODULE_STATE_POWER_GOOD, module->private.state)) {
//       if(HDL_REG_CHECK(module->private.new_state, HDL_SMARC_MODULE_STATE_POWER_GOOD)) {
//         hdl_event_raise(&module->event, module, HDL_SMARC_MODULE_EVENT_POWER_GOOD);
//         module->private.state |= HDL_SMARC_MODULE_STATE_POWER_GOOD;
//       }
//       else {
//         module->private.state &= ~HDL_SMARC_MODULE_STATE_POWER_GOOD;
//         hdl_event_raise(&module->event, module, HDL_SMARC_MODULE_EVENT_POWER_FAULT);
//       }
//     }
//   }

//   return cancel;
// }

// hdl_module_state_t hdl_smarc_module(void *desc, uint8_t enable) {
//   hdl_smarc_module_private_t *smarc = (hdl_smarc_module_private_t *)desc;
//   if(enable) {
//     hdl_gpio_pin_t *pow_on_pin = _smarc_get_ctrl_pin(smarc, HDL_SMARC_MODULE_DEPENDENCY_CARRIER_POWER_ON_PIN);
//     hdl_gpio_pin_t *stby_pin = _smarc_get_ctrl_pin(smarc, HDL_SMARC_MODULE_DEPENDENCY_CARRIER_STBY_PIN);
//     hdl_gpio_pin_t *rst_out_pin = _smarc_get_ctrl_pin(smarc, HDL_SMARC_MODULE_DEPENDENCY_RESET_OUT_PIN);

//     hdl_button_t *pow_btn = smarc->module.dependencies[HDL_SMARC_MODULE_DEPENDENCY_POWER_BTN];
//     pow_btn = HDL_IS_NULL_MODULE(pow_btn)? NULL: pow_btn;

//     if(pow_on_pin != NULL) hdl_gpio_set_inactive(pow_on_pin);
//     if(stby_pin != NULL) hdl_gpio_set_inactive(stby_pin);
//     if(rst_out_pin != NULL) hdl_gpio_set_inactive(rst_out_pin);
//     if(pow_btn != NULL) {
//       smarc->private.btn_delegate.handler = &_smarc_pow_btn_handler;
//       smarc->private.btn_delegate.context = (void *)smarc;
//       hdl_event_subscribe(&pow_btn->event, &smarc->private.btn_delegate);
//     }
//     smarc->private.state = HDL_SMARC_MODULE_STATE_OFF;
//     smarc->private.signal = HDL_SMARC_MODULE_STATE_OFF;
//     coroutine_add(&smarc->private.worker, &_smarc_module_work, (void *)desc);
//     return HDL_MODULE_ACTIVE;
//   }
//   return HDL_MODULE_UNLOADED;
// }
