/*!
    \file device_tree.h
    \author a.bodin
    
    \version 2024-06-04, V1.0.0, This file describe all hardware unit and soft object, which using, it this project
 */
#ifndef ATB_3500_DEVICE_TREE
#define ATB_3500_DEVICE_TREE

#ifdef ATB_3500

/***********************************************************
 *               TIMERS
***********************************************************/
hdl_timer_t* get_object_timer_ms(void);
hdl_timer_event_t* get_object_watchdog_smarc_timer(void);
hdl_delegate_t* get_object_watchdog_smarc_delegate(void);
/***********************************************************
 *               SPI
***********************************************************/
hdl_spi_mem_server_t* get_object_spi3(void);
/***********************************************************
 *               SMARC POWER UP and SMARC GPIO
***********************************************************/
hdl_gpio_pin_t* get_object_do_smarc_reset(void);
hdl_gpio_pin_t* get_object_di_smarc_reset_feedback(void);
hdl_gpio_pin_t* get_object_do_smarc_boot_0(void);
hdl_gpio_pin_t* get_object_do_smarc_boot_1(void);
hdl_gpio_pin_t* get_object_do_smarc_boot_2(void);
hdl_gpio_pin_t* get_object_do_smarc_button(void);
hdl_gpio_pin_t* get_object_do_smarc_irq_1(void);
hdl_gpio_pin_t* get_object_do_smarc_irq_2(void);
/***********************************************************
 *                      LED 
***********************************************************/
hdl_gpio_pin_t* get_object_do_led_0_0(void);
hdl_gpio_pin_t* get_object_do_led_0_1(void);
hdl_gpio_pin_t* get_object_do_led_0_2(void);
hdl_gpio_pin_t* get_object_do_led_1_0(void);
hdl_gpio_pin_t* get_object_do_led_1_1(void);
hdl_gpio_pin_t* get_object_do_led_1_2(void);
hdl_gpio_pin_t* get_object_do_led_2_0(void);
hdl_gpio_pin_t* get_object_do_led_2_1(void);
hdl_gpio_pin_t* get_object_do_led_2_2(void);
/***********************************************************
 *                  POWER ADJUST
***********************************************************/
hdl_gpio_pin_t* get_object_do_5v_power_on(void);
hdl_gpio_pin_t* get_object_do_24v_power_on(void);
hdl_gpio_pin_t* get_object_di_power_fault(void);
hdl_gpio_pin_t* get_object_di_power_good(void);
/***********************************************************
 *                          RELAY
***********************************************************/
hdl_gpio_pin_t* get_object_do_relay_1(void);
hdl_gpio_pin_t* get_object_do_relay_2(void);
/***********************************************************
 *                        X1 Connector
***********************************************************/
hdl_gpio_pin_t* get_object_di_module_address_1(void);
hdl_gpio_pin_t* get_object_di_module_address_2(void);
hdl_gpio_pin_t* get_object_di_module_address_3(void);
hdl_gpio_pin_t* get_object_di_module_address_4(void);
hdl_gpio_pin_t* get_object_di_module_address_5(void);
hdl_gpio_pin_t* get_object_di_external_periph_irq_1(void);
hdl_gpio_pin_t* get_object_di_external_periph_irq_2(void);
hdl_gpio_pin_t* get_object_do_external_output_1(void);
hdl_gpio_pin_t* get_object_do_external_output_2(void);
/***********************************************************
 *                        Other
***********************************************************/
hdl_gpio_pin_t* get_object_do_pci_switch(void);
hdl_gpio_pin_t* get_object_do_lte_reset(void);
/***********************************************************
 *                  ADC
***********************************************************/
hdl_gpio_pin_t *get_object_gpio_ain_0(void);
hdl_gpio_pin_t *get_object_gpio_ain_1(void);
hdl_gpio_pin_t *get_object_gpio_ain_2(void);
hdl_gpio_pin_t *get_object_gpio_ain_3(void);
hdl_gpio_pin_t *get_object_gpio_ain_4(void);
hdl_gpio_pin_t *get_object_gpio_ain_5(void);

hdl_adc_t *get_object_adc(void);
hdl_adc_source_t *get_object_adc_24v(void);
hdl_adc_source_t *get_object_adc_24v_poe(void);
hdl_adc_source_t *get_object_adc_5v(void);
hdl_adc_source_t *get_object_adc_3v3(void);
hdl_adc_source_t *get_object_adc_2v5(void);
hdl_adc_source_t *get_object_adc_1v8(void);

#endif /* ATB_3500 */
#endif /* ATB_3500_DEVICE_TREE */