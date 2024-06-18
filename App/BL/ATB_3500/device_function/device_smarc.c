#include "device_logic.h"

// #define MAX_BOOT_BOOT_RETRY              ((int8_t)5)

/* SMARC RESET DELAY */
#define DELAY_MS_SMARC_RESET_AFTER_POWER_ISSUE  ((uint32_t)200)
#define DELAY_MS_SECONDARY_POWER_STABLE         ((uint32_t)5000)

hdl_module_state_t smarc(void *desc, uint8_t enable) {
    if(enable) {
        hdl_gpio_write(&mod_do_smarc_reset, HDL_GPIO_ON_WRAP(mod_do_smarc_reset));
        hdl_gpio_write(&mod_do_lte_reset, HDL_GPIO_ON_WRAP(mod_do_lte_reset));

        hdl_gpio_write(&mod_do_smarc_irq_1, HDL_GPIO_OFF_WRAP(mod_do_smarc_irq_1));
        hdl_gpio_write(&mod_do_smarc_irq_2, HDL_GPIO_OFF_WRAP(mod_do_smarc_irq_2));
        hdl_gpio_write(&mod_do_smarc_button,HDL_GPIO_OFF_WRAP(mod_do_smarc_button));
        hdl_gpio_write(&mod_do_smarc_boot_0, HDL_GPIO_OFF_WRAP(mod_do_smarc_boot_0));
        hdl_gpio_write(&mod_do_smarc_boot_1, HDL_GPIO_OFF_WRAP(mod_do_smarc_boot_1));
        hdl_gpio_write(&mod_do_smarc_boot_2, HDL_GPIO_OFF_WRAP(mod_do_smarc_boot_2));

        power_domain_set(ATB3500_PD_24V_POE, HDL_FALSE);
        power_domain_set(ATB3500_PD_5V, HDL_FALSE);
        return HDL_MODULE_INIT_OK;
    }
    return HDL_MODULE_DEINIT_OK;
}

hdl_module_t mod_smarc = {
    .init = &smarc,
    .dependencies = hdl_module_dependencies(
    /***********************************************************
     *               SMARC POWER UP and SMARC GPIO
     ***********************************************************/
        &mod_do_smarc_reset.module, &mod_di_smarc_reset_feedback.module,
        &mod_do_smarc_boot_0.module, &mod_do_smarc_boot_1.module, &mod_do_smarc_boot_2.module,
        &mod_do_smarc_button.module, &mod_do_smarc_irq_1.module, &mod_do_smarc_irq_2.module),
};

void smarc_init() {
    hdl_enable(&mod_smarc);
    //power_domain_init();
}

uint8_t smarc_boot_select(object_dictionary_t *od) {
    return SUCCESS;
}

void device_smarc_irq_proc(object_dictionary_t *od) {
    if(od->error.dl_error_poe_fault)
        hdl_gpio_write(&mod_do_smarc_irq_1, HDL_GPIO_ON_WRAP(mod_do_smarc_irq_1));
    else
        hdl_gpio_write(&mod_do_smarc_irq_1, HDL_GPIO_OFF_WRAP(mod_do_smarc_irq_1));
    if(od->error.dl_error_poe_not_good)
        hdl_gpio_write(&mod_do_smarc_irq_2, HDL_GPIO_ON_WRAP(mod_do_smarc_irq_2));
    else
        hdl_gpio_write(&mod_do_smarc_irq_2, HDL_GPIO_OFF_WRAP(mod_do_smarc_irq_2));
}

device_logic_state_machine_t state_machine;

void state_power_reset(void) {
    static uint32_t smarc_reset_time_stamp_ms;
    device_logic_state_machine_t *sm = &state_machine;
    switch (sm->sub_state) {
        /* Init major strcut */
        case 0: {
            hdl_gpio_write(&mod_do_smarc_reset, HDL_GPIO_ON_WRAP(mod_do_smarc_reset));
            sm->sub_state = 2;
            break;
        }
        case 1: {
            if(get_ms_time_from(smarc_reset_time_stamp_ms) >= DELAY_MS_SMARC_RESET_AFTER_POWER_ISSUE) {
                sm->sub_state = 2;
            }
            break;
        }
        case 2: {
            //_state_machine_switch(DL_STATE_MACHINE_SMARC_POWER_UP, 0);
            break;
        }
        default:
            break;
    }
}

void state_smarc_power_up(void) {
    static uint32_t time_stmap_ms = 0;
    device_logic_state_machine_t *sm = &state_machine;
    switch (sm->sub_state) {
        /* Choose smarc boot option */
        case 0: {
            //smarc_boot_select(&od);
            sm->sub_state = 1;
            break;
        }
        case 1: {
            /* Here we have to check CARRIER_POWER_ON (adc_ain_3) */
            hdl_gpio_write(&mod_do_smarc_reset, HDL_GPIO_OFF_WRAP(mod_do_smarc_reset));
            sm->sub_state = 2;
            time_stmap_ms = get_ms_time();
            break;
        }
        case 2: {
            if(get_ms_time_from(time_stmap_ms) >= DELAY_MS_SECONDARY_POWER_STABLE) {
                /* TODO: Here we cycling  */
                sm->sub_state = 3;
            }
            else {
                uint8_t power_is_stable = 
                    power_domain_is_stable(ATB3500_PD_3V3) &&
                    power_domain_is_stable(ATB3500_PD_2V5) &&
                    power_domain_is_stable(ATB3500_PD_1V8);
                if(power_is_stable)
                    sm->sub_state = 3;
            }
            break;
        }
        case 3: {
            power_domain_set(ATB3500_PD_24V_POE, HDL_TRUE);
            // TODO: call on event
            //connector_lte_reset(HDL_FALSE);

            //_state_machine_switch(DL_STATE_MACHINE_POWER_MONITOR, 0);
            break;
        }
        default:
            break;
    }
}
