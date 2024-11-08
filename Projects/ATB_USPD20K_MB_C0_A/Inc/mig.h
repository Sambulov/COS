#ifndef ATB_USPD20K_H_
#define ATB_USPD20K_H_

extern hdl_time_counter_t mod_timer_ms;

extern hdl_gpio_pin_t uspd20k_ai1_ntc_pu;       // (PA8)
extern hdl_gpio_pin_t uspd20k_ai1_1k_pd;        // (PA9)
extern hdl_gpio_pin_t uspd20k_ai1_cur_scr;      // (PB3)
extern hdl_gpio_pin_t uspd20k_ai1_op_amp;       // (PA11)
extern hdl_gpio_pin_t uspd20k_ai1_150r_pd;      // (PA10)
extern hdl_gpio_pin_t uspd20k_ai1_4K3_pd;       // (PA15)

extern hdl_gpio_pin_t uspd20k_ai2_ntc_pu;       // (PC13) 
extern hdl_gpio_pin_t uspd20k_ai2_1k_pd;        // (PB4)
extern hdl_gpio_pin_t uspd20k_ai2_cur_scr;      // (PB15)
extern hdl_gpio_pin_t uspd20k_ai2_op_amp;       // (PB6)
extern hdl_gpio_pin_t uspd20k_ai2_150r_pd;      // (PB5) 
extern hdl_gpio_pin_t uspd20k_ai2_4K3_pd;       // (PB7)

extern hdl_gpio_pin_t uspd20k_ai3_ntc_pu;       // (PB14)
extern hdl_gpio_pin_t uspd20k_ai3_1k_pd;        // (PB13)
extern hdl_gpio_pin_t uspd20k_ai3_cur_scr;      // (PB2)
extern hdl_gpio_pin_t uspd20k_ai3_op_amp;       // (PB11)
extern hdl_gpio_pin_t uspd20k_ai3_150r_pd;      // (PB12)
extern hdl_gpio_pin_t uspd20k_ai3_4K3_pd;       // (PB10)

extern hdl_gpio_pin_t uspd20k_ai4_ntc_pu;       // (PB1)
extern hdl_gpio_pin_t uspd20k_ai4_1k_pd;        // (PB0)
extern hdl_gpio_pin_t uspd20k_ai4_cur_scr;      // (PA3)
extern hdl_gpio_pin_t uspd20k_ai4_op_amp;       // (PA1)
extern hdl_gpio_pin_t uspd20k_ai4_150r_pd;      // (PA0) =====
extern hdl_gpio_pin_t uspd20k_ai4_4K3_pd;       // (PA2) =====

extern hdl_gpio_pin_t uspd20k_adc_rdy;          // (PA6)
extern hdl_spi_client_ch_t uspd20k_adc_spi;     // (PA4->CS),(PA5->SCK),(PA6->MISO),(PA7->MOSI)

extern hdl_i2c_t uspd20k_i2c_som;               // (PF6->SCL),(PF7->SDA)
extern hdl_gpio_pin_t uspd20k_som_int;          // (PA12)

extern hdl_i2c_t uspd20k_i2c_eeprom;            // (PB10->SCL), (PB11->SDA)

extern hdl_adc_ms5194t_t mod_adc;

#endif // ATB_USPD20K_H_
