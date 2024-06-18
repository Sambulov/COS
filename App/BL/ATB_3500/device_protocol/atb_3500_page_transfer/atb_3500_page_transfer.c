/*!
    \file atb_3500_page_transfer.c
    \author a.bodin
    
    \version 2024-06-05, V1.0.0, This file describe major protocol
 */

#if defined(ATB_3500)
#include "device_logic.h"

void device_page_transfer_thread(atb3500_sector_transfer_handler_t *sector_handler,
                                const atb_3500_segment_struct_t *clent_sector,
                                atb_3500_segment_struct_t *server_sector) {
    switch (sector_handler->state_machine) {
        case ATB_3500_SECTOR_TRANSFER_SM_INITIAL: {
            if(clent_sector->destination_port != 0) {
                sector_handler->current_destination_port = clent_sector->destination_port;
                sector_handler->current_sequence_number = clent_sector->sequence_number;
                sector_handler->acknoledgment_number = clent_sector->acknoledgment_number;
                sector_handler->state_machine = ATB_3500_SECTOR_TRANSFER_SM_PROCCEDING;
                sector_handler->client_request(sector_handler, clent_sector->data, ATB_3500_SECTOR_DATA_SIZE);
            }
            break;
        } 
        case ATB_3500_SECTOR_TRANSFER_SM_PROCCEDING: {
            
            break;
        }
        case ATB_3500_SECTOR_TRNASFER_SM_COMPLETED: {
            
            break;
        }  
        case ATB_3500_SECTOR_TRNASFER_SM_FAILED: {
            
            break;
        }        
        default:
            break;
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
        hdl_timer_event_reset(&mod_timer_event);
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

#endif /* ATB_3500 */