#ifndef MIG_ATB3500_H_
#define MIG_ATB3500_H_


/***********************************************************
 *                          TIMER
 ***********************************************************/
extern hdl_time_counter_t mod_systick_timer_ms;
/**************************************************************
 *                        SPI
 *************************************************************/
extern hdl_spi_server_dma_t mod_spi3_server_dma;
/**************************************************************
 *                        BLDL
 *************************************************************/
extern atb3500_power_rail_t rail_24v;
extern atb3500_power_rail_t rail_24vpoe;
extern atb3500_power_rail_t rail_5v;
extern atb3500_power_rail_t rail_3v3;
extern atb3500_power_rail_t rail_2v5;
extern atb3500_power_rail_t rail_1v8;
extern hdl_smarc_carrier_t mod_smarc;
extern hdl_button_t power_button;
extern atb3500_io_t mod_carrier_io;
extern atb3500_watchdog_t mod_watchdog;
extern atb3500_flash_serial_t mod_serial;
extern atb3500_i2c_gateway_t mod_i2c_gateway;
extern hdl_module_t power_domain;
extern hdl_module_t app_module;

extern hdl_gpio_pin_t mod_do_rs485_dir;
extern hdl_gpio_pin_t mod_di_rs485_tx;
extern hdl_interrupt_t rs485_tx_int;
extern hdl_interrupt_t rs485_timer_int;
extern hdl_tick_counter_t mod_rs485_tick_counter;

extern hdl_interrupt_controller_t mod_int_ctrlr;

extern const uint32_t RS485_TIMER_RELOAD_EVENT;

#endif // MIG_ATB3500_H_