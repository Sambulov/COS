#include "device_logic.h"

// #define MAX_BOOT_BOOT_RETRY              ((int8_t)5)

/* SMARC RESET DELAY */
#define DELAY_MS_SMARC_RESET_AFTER_POWER_ISSUE  ((uint32_t)200)
#define DELAY_MS_SECONDARY_POWER_STABLE         ((uint32_t)5000)

hdl_module_state_t smarc(void *desc, uint8_t enable);

hdl_button_t power_button = {
    .module.init = &hdl_button,
    .module.dependencies = hdl_module_dependencies(&mod_do_smarc_power_btn.module, &mod_systick_timer_ms.module ),
    .debounce_delay = 50,
    .hold_delay = 0,
};

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
}hdl_smarc_t;

hdl_smarc_t mod_smarc = {
    .module.init = &smarc,
    .module.dependencies = hdl_module_dependencies(
        &hdl_null_module /* power good */,
        &power_button.module /* power button in */,
        &power_button.module /* power button out */,
        &hdl_null_module /* carrier_power_on */,
        &hdl_null_module /* carrier_stand_by */,
        &mod_do_smarc_reset_in.module, 
        &mod_di_smarc_reset_out.module,
        &mod_systick_timer_ms.module,
        &mod_do_smarc_boot_0.module, 
        &mod_do_smarc_boot_1.module, 
        &mod_do_smarc_boot_2.module),
    
};

static void _on_power_button_in(uint32_t event_trigger, void *sender, void *context) {
    hdl_button_t *pow_btn_out = (hdl_button_t *)mod_smarc.module.dependencies[2];
    if(!HDL_IS_NULL_MODULE(pow_btn_out)) {
        if(event_trigger == HDL_BTN_EVENT_CLICK)
            hdl_btn_sw_click(pow_btn_out);
        //if(event_trigger == HDL_BTN_EVENT_HOLD)
        //    TODO: force shutdown
    }
}

__weak void smarc_standby_circuits_cb(void *context) {

}

__weak void smarc_runtime_circuits_cb(void *context) {

}

__weak void smarc_runtime_cb(void *context) {

}

static uint8_t _smarc_worker(coroutine_desc_t this, uint8_t cancel, void *arg) {
    hdl_smarc_t *smarc = (hdl_smarc_t *)arg;
    hdl_gpio_pin_t *carrier_pwr_on = (hdl_gpio_pin_t *)mod_smarc.module.dependencies[3];
    if((HDL_IS_NULL_MODULE(carrier_pwr_on) || HDL_GPIO_IS_ACTIVE(carrier_pwr_on)) && !smarc->carrier_power_on) {
        smarc->carrier_power_on = 1;
        smarc_standby_circuits_cb(smarc->bl_context);
    } 

    hdl_gpio_pin_t *carrier_stby = (hdl_gpio_pin_t *)mod_smarc.module.dependencies[4];
    if((HDL_IS_NULL_MODULE(carrier_stby) || HDL_GPIO_IS_ACTIVE(carrier_stby)) && !smarc->carrier_stby) {
        smarc->carrier_stby = 1;
        smarc_runtime_circuits_cb(smarc->bl_context);
    }

    hdl_gpio_pin_t *reset_out = (hdl_gpio_pin_t *)mod_smarc.module.dependencies[6];
    if(!HDL_GPIO_IS_ACTIVE(reset_out) && !smarc->reset_out) {
        smarc->reset_out = 1;
        smarc_runtime_cb(smarc->bl_context);
    }
    // TODO: other smarc states
    return cancel;
}

hdl_module_state_t smarc(void *desc, uint8_t enable) {
    static coroutine_desc_static_t smarc_task_buf;
    static hdl_delegate_t pow_btn_delegate;
    if(enable) {
        hdl_gpio_pin_t *pow_bad_pin = (hdl_gpio_pin_t *)mod_smarc.module.dependencies[0];
        if(!HDL_IS_NULL_MODULE(pow_bad_pin)) {
            HDL_GPIO_SET_ACTIVE(pow_bad_pin);
        }
        HDL_GPIO_SET_ACTIVE(&mod_do_smarc_reset_in);
        hdl_button_t *pow_btn_in = (hdl_button_t *)mod_smarc.module.dependencies[1];
        if(!HDL_IS_NULL_MODULE(pow_btn_in)) {
            pow_btn_delegate.context = desc;
            pow_btn_delegate.handler = &_on_power_button_in;
            hdl_event_subscribe(&pow_btn_in->event, &pow_btn_delegate);
        }
        
        hdl_gpio_pin_t *reset_out = (hdl_gpio_pin_t *)mod_smarc.module.dependencies[6];
        coroutine_add_static(&smarc_task_buf, &_smarc_worker, desc);
        return HDL_MODULE_INIT_OK;
    }
    return HDL_MODULE_DEINIT_OK;
}

void smarc_init(void* context) {
    hdl_enable(&mod_smarc.module);
    mod_smarc.bl_context = context;
}

void smarc_boot_select(smarc_boot_select_e boot) {
    hdl_gpio_pin_t *boot0 = (hdl_gpio_pin_t *)mod_smarc.module.dependencies[8];
    hdl_gpio_pin_t *boot1 = (hdl_gpio_pin_t *)mod_smarc.module.dependencies[9];
    hdl_gpio_pin_t *boot2 = (hdl_gpio_pin_t *)mod_smarc.module.dependencies[10];
    if(!HDL_IS_NULL_MODULE(boot0)) {
        hdl_gpio_write(boot0, (boot & SMARC_BOOT0)? !boot0->inactive_default: boot0->inactive_default);
    }
    if(!HDL_IS_NULL_MODULE(boot1)) {
        hdl_gpio_write(boot1, (boot & SMARC_BOOT1)? !boot1->inactive_default: boot1->inactive_default);
    }
    if(!HDL_IS_NULL_MODULE(boot2)) {
        hdl_gpio_write(boot2, (boot & SMARC_BOOT2)? !boot2->inactive_default: boot2->inactive_default);
    }
}

void smarc_carrier_redy() {
    HDL_GPIO_SET_INACTIVE(&mod_do_smarc_reset_in);
}

void smarc_power_good() {
    hdl_gpio_pin_t *pow_bad_pin = (hdl_gpio_pin_t *)mod_smarc.module.dependencies[0];
    if(!HDL_IS_NULL_MODULE(pow_bad_pin)) {
        HDL_GPIO_SET_INACTIVE(pow_bad_pin);
    }
}
