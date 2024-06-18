#include "device_logic.h"

hdl_module_t indicators = {
    .dependencies = hdl_module_dependencies(
    /***********************************************************
    *                      LED
    ***********************************************************/
        &mod_do_led_0_0.module, &mod_do_led_0_1.module, &mod_do_led_0_2.module,
        &mod_do_led_1_0.module, &mod_do_led_1_1.module, &mod_do_led_1_2.module, &mod_do_led_2_0.module,
        &mod_do_led_2_1.module, &mod_do_led_2_2.module, 
    /***********************************************************
    *                          RELAY
    ***********************************************************/
        &mod_do_relay1.module, &mod_do_relay2.module)
};

void indicator_init() {
    hdl_enable(&indicators);
}

/*!
    \brief          This function describe relay control
 */
void device_relay_control(object_dictionary_t *od) {
    if(od->error.dl_error_poe_fault || od->error.dl_error_poe_not_good) {
        hdl_gpio_write(&mod_do_relay1, HDL_GPIO_ON_WRAP(mod_do_relay1));
        hdl_gpio_write(&mod_do_relay2, HDL_GPIO_ON_WRAP(mod_do_relay1));
    }
    else {
        /* TODO: CHECK */
        hdl_gpio_write(&mod_do_relay1, HDL_GPIO_OFF_WRAP(mod_do_relay1));
        hdl_gpio_write(&mod_do_relay2, HDL_GPIO_ON_WRAP(mod_do_relay1));
    }
}