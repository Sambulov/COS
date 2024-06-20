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

void device_page_transfer_proc(object_dictionary_t *od, atb_3500_page_transfer_read_sector_t *read_sector, atb_3500_page_transfer_write_sector_t *write_sector) {
    uint8_t new_rx_data_available = 0;
    if(new_rx_data_available) {
        /* LTE modem */
        if(read_sector->do_lte_modem_reset)
            hdl_gpio_write(&mod_do_lte_reset, HDL_GPIO_ON_WRAP(mod_do_lte_reset));
        else 
            hdl_gpio_write(&mod_do_lte_reset, HDL_GPIO_OFF_WRAP(mod_do_lte_reset));
        /* PCI switch */
        if(read_sector->do_pci_switch)
            hdl_gpio_write(&mod_do_pci_switch, HDL_GPIO_ON_WRAP(mod_do_pci_switch));
        else 
            hdl_gpio_write(&mod_do_pci_switch, HDL_GPIO_OFF_WRAP(mod_do_pci_switch));
        /* External outputs */
        if(read_sector->do_external_output_1)
            hdl_gpio_write(&mod_do_external_output_1, HDL_GPIO_ON_WRAP(mod_do_external_output_1));
        else 
            hdl_gpio_write(&mod_do_external_output_1, HDL_GPIO_OFF_WRAP(mod_do_external_output_1));
        if(read_sector->do_external_output_2)
            hdl_gpio_write(&mod_do_external_output_2, HDL_GPIO_ON_WRAP(mod_do_external_output_2));
        else 
            hdl_gpio_write(&mod_do_external_output_2, HDL_GPIO_OFF_WRAP(mod_do_external_output_2));
        /* LEDs */
        if(read_sector->do_led_0_0)
            hdl_gpio_write(&mod_do_led_0_0, HDL_GPIO_ON_WRAP(mod_do_led_0_0));
        else 
            hdl_gpio_write(&mod_do_led_0_0, HDL_GPIO_OFF_WRAP(mod_do_led_0_0));
        if(read_sector->do_led_0_1)
            hdl_gpio_write(&mod_do_led_0_1, HDL_GPIO_ON_WRAP(mod_do_led_0_1));
        else 
            hdl_gpio_write(&mod_do_led_0_1, HDL_GPIO_OFF_WRAP(mod_do_led_0_1));
        if(read_sector->do_led_0_2)
            hdl_gpio_write(&mod_do_led_0_2, HDL_GPIO_ON_WRAP(mod_do_led_0_2));
        else 
            hdl_gpio_write(&mod_do_led_0_2, HDL_GPIO_OFF_WRAP(mod_do_led_0_2));
        if(read_sector->do_led_1_0)
            hdl_gpio_write(&mod_do_led_1_0, HDL_GPIO_ON_WRAP(mod_do_led_1_0));
        else 
            hdl_gpio_write(&mod_do_led_1_0, HDL_GPIO_OFF_WRAP(mod_do_led_1_0));
        if(read_sector->do_led_1_1)
            hdl_gpio_write(&mod_do_led_1_1, HDL_GPIO_ON_WRAP(mod_do_led_1_1));
        else 
            hdl_gpio_write(&mod_do_led_1_1, HDL_GPIO_OFF_WRAP(mod_do_led_1_1));
        if(read_sector->do_led_1_2)
            hdl_gpio_write(&mod_do_led_1_2, HDL_GPIO_ON_WRAP(mod_do_led_1_2));
        else 
            hdl_gpio_write(&mod_do_led_1_2, HDL_GPIO_OFF_WRAP(mod_do_led_1_2));
        if(read_sector->do_led_2_0)
            hdl_gpio_write(&mod_do_led_2_0, HDL_GPIO_ON_WRAP(mod_do_led_2_0));
        else 
            hdl_gpio_write(&mod_do_led_2_0, HDL_GPIO_OFF_WRAP(mod_do_led_2_0));
        if(read_sector->do_led_2_1)
            hdl_gpio_write(&mod_do_led_2_1, HDL_GPIO_ON_WRAP(mod_do_led_2_1));
        else 
            hdl_gpio_write(&mod_do_led_2_1, HDL_GPIO_OFF_WRAP(mod_do_led_2_1));
        if(read_sector->do_led_2_2)
            hdl_gpio_write(&mod_do_led_2_2, HDL_GPIO_ON_WRAP(mod_do_led_2_2));
        else 
            hdl_gpio_write(&mod_do_led_2_2, HDL_GPIO_OFF_WRAP(mod_do_led_2_2));
        /* Watch dog */
        watchdog_reset();
    }
    uint8_t tx_data_update;
    if(tx_data_update) {
        write_sector->di_module_addres_1 = 
            ( (hdl_gpio_read(&mod_di_module_address_1)) == (HDL_GPIO_ON_WRAP(mod_di_module_address_1)) ) ? 1 : 0;
        write_sector->di_module_addres_2 = 
            ( (hdl_gpio_read(&mod_di_module_address_2)) == (HDL_GPIO_ON_WRAP(mod_di_module_address_2)) ) ? 1 : 0;
        write_sector->di_module_addres_3 = 
            ( (hdl_gpio_read(&mod_di_module_address_3)) == (HDL_GPIO_ON_WRAP(mod_di_module_address_3)) ) ? 1 : 0;
        write_sector->di_module_addres_4 = 
            ( (hdl_gpio_read(&mod_di_module_address_4)) == (HDL_GPIO_ON_WRAP(mod_di_module_address_4)) ) ? 1 : 0;
        write_sector->di_module_addres_5 = 
            ( (hdl_gpio_read(&mod_di_module_address_5)) == (HDL_GPIO_ON_WRAP(mod_di_module_address_5)) ) ? 1 : 0;
        write_sector->do_lte_modem_reset = 
            ( (hdl_gpio_read_output(&mod_do_lte_reset)) == (HDL_GPIO_ON_WRAP(mod_do_lte_reset)) ) ? 1 : 0;
        write_sector->do_pci_switch =
            ( (hdl_gpio_read_output(&mod_do_pci_switch)) == (HDL_GPIO_ON_WRAP(mod_do_pci_switch)) ) ? 1 : 0;
        write_sector->do_external_output_1 =
            ( (hdl_gpio_read_output(&mod_do_external_output_1)) == (HDL_GPIO_ON_WRAP(mod_do_external_output_1)) ) ? 1 : 0;
        write_sector->do_external_output_2 =
            ( (hdl_gpio_read_output(&mod_do_external_output_2)) == (HDL_GPIO_ON_WRAP(mod_do_external_output_2)) ) ? 1 : 0;
        write_sector->do_led_0_0 =
            ( (hdl_gpio_read_output(&mod_do_led_0_0)) == (HDL_GPIO_ON_WRAP(mod_do_led_0_0)) ) ? 1 : 0;
        write_sector->do_led_0_1 =
            ( (hdl_gpio_read_output(&mod_do_led_0_1)) == (HDL_GPIO_ON_WRAP(mod_do_led_0_1)) ) ? 1 : 0;
        write_sector->do_led_0_2 =
            ( (hdl_gpio_read_output(&mod_do_led_0_2)) == (HDL_GPIO_ON_WRAP(mod_do_led_0_1)) ) ? 1 : 0;
        write_sector->do_led_1_0 =
            ( (hdl_gpio_read_output(&mod_do_led_1_0)) == (HDL_GPIO_ON_WRAP(mod_do_led_1_0)) ) ? 1 : 0;
        write_sector->do_led_1_1 =
            ( (hdl_gpio_read_output(&mod_do_led_1_1)) == (HDL_GPIO_ON_WRAP(mod_do_led_1_1)) ) ? 1 : 0;
        write_sector->do_led_1_2 =
            ( (hdl_gpio_read_output(&mod_do_led_1_2)) == (HDL_GPIO_ON_WRAP(mod_do_led_1_2)) ) ? 1 : 0;
        write_sector->do_led_2_0 =
            ( (hdl_gpio_read_output(&mod_do_led_2_0)) == (HDL_GPIO_ON_WRAP(mod_do_led_2_0)) ) ? 1 : 0;
        write_sector->do_led_2_1 =
            ( (hdl_gpio_read_output(&mod_do_led_2_1)) == (HDL_GPIO_ON_WRAP(mod_do_led_2_1)) ) ? 1 : 0;
        write_sector->do_led_2_2 =
            ( (hdl_gpio_read_output(&mod_do_led_2_2)) == (HDL_GPIO_ON_WRAP(mod_do_led_2_2)) ) ? 1 : 0;
    }
}
