/*!
    \file device_tree.c
    \author a.bodin
    \note Main idea is declare all hardware unit in platform depended files, in business logic 
        we only have to get a pointer to nessesasy pin. This approach allow get a flexible solve for business logic.

    \version 2024-06-04, V1.0.0, This file tie all hardware unit and soft object, which using, it this project
 */
#if defined(ATB_3500)

#include "device_logic.h"

/***********************************************************
 *               TIMERS
***********************************************************/
extern hdl_timer_t mod_systick_timer_ms;
extern hdl_timer_event_t mod_timer_event;

hdl_delegate_t watchdog_smarc_delegate = {
    .context = NULL,
    .handler = NULL,
};
hdl_timer_t* get_object_timer_ms(void) {
    return &mod_systick_timer_ms;
}
hdl_timer_event_t* get_object_watchdog_smarc_timer(void) {
    return &mod_timer_event;
}
hdl_delegate_t* get_object_watchdog_smarc_delegate(void) {
    return &watchdog_smarc_delegate;
}
/***********************************************************
 *               SPI
***********************************************************/
extern hdl_spi_mem_server_t mod_spi_3;

hdl_spi_mem_server_t* get_object_spi3(void) {
    return &mod_spi_3;
};

/***********************************************************
 *               SMARC POWER UP and SMARC GPIO
***********************************************************/
extern hdl_gpio_pin_t mod_do_smarc_reset;
extern hdl_gpio_pin_t mod_di_smarc_reset_feedback;
extern hdl_gpio_pin_t mod_do_smarc_boot_0;
extern hdl_gpio_pin_t mod_do_smarc_boot_1;
extern hdl_gpio_pin_t mod_do_smarc_boot_2;
extern hdl_gpio_pin_t mod_do_smarc_button;
extern hdl_gpio_pin_t mod_do_smarc_irq_1;
extern hdl_gpio_pin_t mod_do_smarc_irq_2;
hdl_gpio_pin_t* get_object_do_smarc_reset(void) {
    return &mod_do_smarc_reset;
}
hdl_gpio_pin_t* get_object_di_smarc_reset_feedback(void) {
    return &mod_di_smarc_reset_feedback;
}
hdl_gpio_pin_t* get_object_do_smarc_boot_0(void) {
    return &mod_do_smarc_boot_0;
}
hdl_gpio_pin_t* get_object_do_smarc_boot_1(void) {
    return &mod_do_smarc_boot_1;
}
hdl_gpio_pin_t* get_object_do_smarc_boot_2(void) {
    return &mod_do_smarc_boot_2;
}
hdl_gpio_pin_t* get_object_do_smarc_button(void) {
    return &mod_do_smarc_button;
}
hdl_gpio_pin_t* get_object_do_smarc_irq_1(void) {
    return &mod_do_smarc_irq_1;
}
hdl_gpio_pin_t* get_object_do_smarc_irq_2(void) {
    return &mod_do_smarc_irq_2;
}
/***********************************************************
 *                      LED 
***********************************************************/
extern hdl_gpio_pin_t mod_do_led_0_0;
extern hdl_gpio_pin_t mod_do_led_0_1;
extern hdl_gpio_pin_t mod_do_led_0_2;
extern hdl_gpio_pin_t mod_do_led_1_0;
extern hdl_gpio_pin_t mod_do_led_1_1;
extern hdl_gpio_pin_t mod_do_led_1_2;
extern hdl_gpio_pin_t mod_do_led_2_0;
extern hdl_gpio_pin_t mod_do_led_2_1;
extern hdl_gpio_pin_t mod_do_led_2_2;
hdl_gpio_pin_t* get_object_do_led_0_0(void) {
    return &mod_do_led_0_0;
}
hdl_gpio_pin_t* get_object_do_led_0_1(void) {
    return &mod_do_led_0_1;
}
hdl_gpio_pin_t* get_object_do_led_0_2(void) {
    return &mod_do_led_0_2;
}
hdl_gpio_pin_t* get_object_do_led_1_0(void) {
    return &mod_do_led_1_0;
}
hdl_gpio_pin_t* get_object_do_led_1_1(void) {
    return &mod_do_led_1_1;
}
hdl_gpio_pin_t* get_object_do_led_1_2(void) {
    return &mod_do_led_1_2;
}
hdl_gpio_pin_t* get_object_do_led_2_0(void) {
    return &mod_do_led_2_0;
}
hdl_gpio_pin_t* get_object_do_led_2_1(void) {
    return &mod_do_led_2_1;
}
hdl_gpio_pin_t* get_object_do_led_2_2(void) {
    return &mod_do_led_2_2;
}
/***********************************************************
 *                  POWER ADJUST
***********************************************************/
extern hdl_gpio_pin_t mod_do_5v_power_on;
extern hdl_gpio_pin_t mod_do_24v_power_on;
extern hdl_gpio_pin_t mod_di_power_fault;
extern hdl_gpio_pin_t mod_di_power_status;
hdl_gpio_pin_t* get_object_do_5v_power_on(void) {
    return &mod_do_5v_power_on;
}
hdl_gpio_pin_t* get_object_do_24v_power_on(void) {
    return &mod_do_24v_power_on;
}
hdl_gpio_pin_t* get_object_di_power_fault(void) {
    return &mod_di_power_fault;
}
hdl_gpio_pin_t* get_object_di_power_good(void) {
    return &mod_di_power_status;
}
/***********************************************************
 *                          RELAY
***********************************************************/
extern hdl_gpio_pin_t mod_do_relay1;
extern hdl_gpio_pin_t mod_do_relay2;
hdl_gpio_pin_t* get_object_do_relay_1(void) {
    return &mod_do_relay1;
}
hdl_gpio_pin_t* get_object_do_relay_2(void) {
    return &mod_do_relay2;
}
/***********************************************************
 *                        X1 Connector
***********************************************************/
extern hdl_gpio_pin_t mod_di_module_address_1;
extern hdl_gpio_pin_t mod_di_module_address_2;
extern hdl_gpio_pin_t mod_di_module_address_3;
extern hdl_gpio_pin_t mod_di_module_address_4;
extern hdl_gpio_pin_t mod_di_module_address_5;
extern hdl_gpio_pin_t mod_di_external_periph_irq_1;
extern hdl_gpio_pin_t mod_di_external_periph_irq_2;
extern hdl_gpio_pin_t mod_do_external_output_1;
extern hdl_gpio_pin_t mod_do_external_output_2;
hdl_gpio_pin_t* get_object_di_module_address_1(void) {
    return &mod_di_module_address_1;
}
hdl_gpio_pin_t* get_object_di_module_address_2(void) {
    return &mod_di_module_address_2;
}
hdl_gpio_pin_t* get_object_di_module_address_3(void) {
    return &mod_di_module_address_3;
}
hdl_gpio_pin_t* get_object_di_module_address_4(void) {
    return &mod_di_module_address_4;
}
hdl_gpio_pin_t* get_object_di_module_address_5(void) {
    return &mod_di_module_address_5;
}
hdl_gpio_pin_t* get_object_di_external_periph_irq_1(void) {
    return &mod_di_external_periph_irq_1;
}
hdl_gpio_pin_t* get_object_di_external_periph_irq_2(void) {
    return &mod_di_external_periph_irq_2;
}
hdl_gpio_pin_t* get_object_do_external_output_1(void) {
    return &mod_do_external_output_1;
}
hdl_gpio_pin_t* get_object_do_external_output_2(void) {
    return &mod_do_external_output_2;
}
/***********************************************************
 *                        Other
***********************************************************/
extern hdl_gpio_pin_t mod_do_pci_switch;
extern hdl_gpio_pin_t mod_do_lte_reset;
hdl_gpio_pin_t* get_object_do_pci_switch(void) {
    return &mod_do_pci_switch;
}
hdl_gpio_pin_t* get_object_do_lte_reset(void) {
    return &mod_do_lte_reset;
}
/**************************************************************
 *                        ADC
 *************************************************************/
extern hdl_gpio_pin_t mod_adc_in0;
extern hdl_gpio_pin_t mod_adc_in1;
extern hdl_gpio_pin_t mod_adc_in2;
extern hdl_gpio_pin_t mod_adc_in3;
extern hdl_gpio_pin_t mod_adc_in4;
extern hdl_gpio_pin_t mod_adc_in5;

extern hdl_adc_source_t mod_adc_source_0;
extern hdl_adc_source_t mod_adc_source_1;
extern hdl_adc_source_t mod_adc_source_2;
extern hdl_adc_source_t mod_adc_source_3;
extern hdl_adc_source_t mod_adc_source_4;
extern hdl_adc_source_t mod_adc_source_5;
extern hdl_adc_t mod_adc;


hdl_gpio_pin_t *get_object_gpio_ain_0(void) {
    return &mod_adc_in0;
}
hdl_gpio_pin_t *get_object_gpio_ain_1(void) {
    return &mod_adc_in1;
}
hdl_gpio_pin_t *get_object_gpio_ain_2(void) {
    return &mod_adc_in2;
}
hdl_gpio_pin_t *get_object_gpio_ain_3(void) {
    return &mod_adc_in3;
}
hdl_gpio_pin_t *get_object_gpio_ain_4(void) {
    return &mod_adc_in4;
}
hdl_gpio_pin_t *get_object_gpio_ain_5(void) {
    return &mod_adc_in5;
}
hdl_adc_t *get_object_adc(void) {
    return &mod_adc;
}
hdl_adc_source_t *get_object_adc_24v(void) {
    return &mod_adc_source_0;
}
hdl_adc_source_t *get_object_adc_24v_poe(void) {
    return &mod_adc_source_1;
}
hdl_adc_source_t *get_object_adc_5v(void) {
    return &mod_adc_source_2;
}
hdl_adc_source_t *get_object_adc_3v3(void) {
    return &mod_adc_source_3;
}
hdl_adc_source_t *get_object_adc_2v5(void) {
    return &mod_adc_source_4;
}
hdl_adc_source_t *get_object_adc_1v8(void) {
    return &mod_adc_source_5;
}

#endif