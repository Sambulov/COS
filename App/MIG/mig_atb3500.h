#ifndef MIG_ATB3500_H_
#define MIG_ATB3500_H_

/***********************************************************
 *                          TIMER
 ***********************************************************/
extern hdl_timer_t mod_systick_timer_ms;
extern hdl_timer_event_t mod_watchdog_timer;
/**************************************************************
 *                        SPI
 *************************************************************/
extern hdl_spi_server_dma_t mod_spi_server_dma;
/***********************************************************
 *               SMARC POWER UP and SMARC GPIO
***********************************************************/
extern hdl_gpio_pin_t mod_do_smarc_reset_in;
extern hdl_gpio_pin_t mod_di_smarc_reset_out;
extern hdl_gpio_pin_t mod_do_smarc_boot_0;
extern hdl_gpio_pin_t mod_do_smarc_boot_1;
extern hdl_gpio_pin_t mod_do_smarc_boot_2;
extern hdl_gpio_pin_t mod_do_smarc_power_btn;
extern hdl_gpio_pin_t mod_do_smarc_irq_1;
extern hdl_gpio_pin_t mod_do_smarc_irq_2;
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
/***********************************************************
 *                  POWER ADJUST
***********************************************************/
extern hdl_gpio_pin_t mod_do_5v_power_on;
extern hdl_gpio_pin_t mod_do_24v_poe_power_on;
extern hdl_gpio_pin_t mod_di_24v_poe_power_fault;
extern hdl_gpio_pin_t mod_di_24v_poe_power_good;
/***********************************************************
 *                          RELAY
***********************************************************/
extern hdl_gpio_pin_t mod_do_relay1;
extern hdl_gpio_pin_t mod_do_relay2;
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
/***********************************************************
 *                        Other
***********************************************************/
extern hdl_gpio_pin_t mod_do_pci_switch;
extern hdl_gpio_pin_t mod_do_lte_reset;
/**************************************************************
 *                        ADC
 *************************************************************/

extern hdl_gpio_pin_t mod_adc_pin_24v;
extern hdl_gpio_pin_t mod_adc_pin_24v_poe;
extern hdl_gpio_pin_t mod_adc_pin_5v;
extern hdl_gpio_pin_t mod_adc_pin_3v3;
extern hdl_gpio_pin_t mod_adc_pin_2v5;
extern hdl_gpio_pin_t mod_adc_pin_1v8;

extern hdl_adc_source_t mod_adc_source_0_adc_24v;
extern hdl_adc_source_t mod_adc_source_1_adc_24v_poe;
extern hdl_adc_source_t mod_adc_source_2_adc_5v;
extern hdl_adc_source_t mod_adc_source_3_adc_3v3;
extern hdl_adc_source_t mod_adc_source_4_adc_2v5;
extern hdl_adc_source_t mod_adc_source_5_adc_1v8;
extern hdl_adc_t mod_adc;

#endif // MIG_ATB3500_H_