#ifndef ATB_USPD20K_H_
#define ATB_USPD20K_H_

extern hdl_time_counter_t mod_timer_ms;

extern hdl_gpio_pin_t uspd20k_io1_di_disable;       // (PB14)
extern hdl_gpio_pin_t uspd20k_io1_di;               // (PB5)
extern hdl_gpio_pin_t uspd20k_io1_do;               // (PA6)

extern hdl_gpio_pin_t uspd20k_io2_di_disable;       // (PB13)
extern hdl_gpio_pin_t uspd20k_io2_di;               // (PA10)
extern hdl_gpio_pin_t uspd20k_io2_do;               // (PA5)

extern hdl_gpio_pin_t uspd20k_io3_di_disable;       // (PA8)
extern hdl_gpio_pin_t uspd20k_io3_di;               // (PB4)
extern hdl_gpio_pin_t uspd20k_io3_do;               // (PA3)

extern hdl_gpio_pin_t uspd20k_io4_di_disable;       // (PB15)
extern hdl_gpio_pin_t uspd20k_io4_di;               // (PA11)
extern hdl_gpio_pin_t uspd20k_io4_do;               // (PA4)

extern hdl_gpio_pin_t uspd20k_io5_di_disable;       // (PB12)
extern hdl_gpio_pin_t uspd20k_io5_di;               // (PB3)
extern hdl_gpio_pin_t uspd20k_io5_do;               // (PB1)

extern hdl_gpio_pin_t uspd20k_io6_di_disable;       // (PA9)
extern hdl_gpio_pin_t uspd20k_io6_di;               // (PA12)
extern hdl_gpio_pin_t uspd20k_io6_do;               // (PB2)

extern hdl_gpio_pin_t uspd20k_io7_di_disable;       // (PB10)
extern hdl_gpio_pin_t uspd20k_io7_di;               // (PA15)
extern hdl_gpio_pin_t uspd20k_io7_do;               // (PB0)

extern hdl_gpio_pin_t uspd20k_io8_di_disable;       // (PB11)
extern hdl_gpio_pin_t uspd20k_io8_di;               // (PB6)
extern hdl_gpio_pin_t uspd20k_io8_do;               // (PA7)

extern hdl_gpio_pin_t uspd20k_io_fault_all;         // (PA0)
extern hdl_gpio_pin_t uspd20k_io_fault_reset;       // (PA2)

extern hdl_gpio_pin_t uspd20k_som_int;              // (PA1)

extern hdl_i2c_t uspd20k_i2c_som;               // (PB8->SCL),(PB9->SDA)

#endif // ATB_USPD20K_H_
