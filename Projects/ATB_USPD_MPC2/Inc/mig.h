#ifndef ATB_USPD_MPC2_H_
#define ATB_USPD_MPC2_H_

extern hdl_gpio_pin_t uspd_mpc_in1; //(PA3)
extern hdl_gpio_pin_t uspd_mpc_in2; //(PA2)
extern hdl_gpio_pin_t uspd_mpc_in3; //(PB1)
extern hdl_gpio_pin_t uspd_mpc_in4; //(PB0)
extern hdl_gpio_pin_t uspd_mpc_in5; //(PA10)
extern hdl_gpio_pin_t uspd_mpc_in6; //(PA9)
extern hdl_gpio_pin_t uspd_mpc_in7; //(PB9)
extern hdl_gpio_pin_t uspd_mpc_in8; //(PB8)

extern hdl_gpio_pin_t uspd_mpc_out1; //(PB7)
extern hdl_gpio_pin_t uspd_mpc_out2; //(PB6)
extern hdl_gpio_pin_t uspd_mpc_out3; //(PB5)
extern hdl_gpio_pin_t uspd_mpc_out4; //(PB4)
extern hdl_gpio_pin_t uspd_mpc_out5; //(PB3)
extern hdl_gpio_pin_t uspd_mpc_out6; //(PA15)
extern hdl_gpio_pin_t uspd_mpc_out7; //(PA12)
extern hdl_gpio_pin_t uspd_mpc_out8; //(PA11)

extern hdl_gpio_pin_t uspd_mpc_pps; //(PA0)
extern hdl_gpio_pin_t uspd_mpc_int; //(PA1)

#if EXT_ADC
extern hdl_spi_client_ch_t uspd_mpc_adc_spi; //(PA4->CS),(PA5->SCK),(PA6->MISO),(PA7->MOSI)
#else
extern hdl_adc_source_t mod_adc_ain1; //(PA4)
extern hdl_adc_source_t mod_adc_ain2; //(PA6)
extern hdl_adc_source_t mod_adc_ain3; //(PA7)
extern hdl_adc_source_t mod_adc_ain4; //(PA5)
extern hdl_adc_t mod_adc;
#endif

extern hdl_spi_client_ch_t uspd_mpc_gpio_expander_spi;  //(PA4->CS),(PA5->SCK),(PA6->MISO),(PA7->MOSI)

extern hdl_i2c_t uspd_mpc_i2c_if; //(PB10->SCL), (PB11->SDA) slave

extern hdl_i2c_t uspd_mpc_eeprom_i2c; //(PF6->SCL),(PF7->SDA) master
extern hdl_gpio_pin_t uspd_mpc_eeprom_wp; //(PC13->WP)


/*
Reg1:=>      Reg2:=>      Reg3:=>      Reg4:=>      Reg5:=>     Reg6:=>     Reg7:=> loopback
Q0->PD-14K3  Q0->PD-14K3  Q0->PD-14K3  Q0->PD-14K3  Q0->X       Q0->X       Q0->X
Q1->PD-1K    Q1->PD-1K    Q1->PD-1K    Q1->PD-1K    Q1->LED1    Q1->LED11   Q1->LED6
Q2->PD-150R  Q2->PD-150R  Q2->PD-150R  Q2->PD-150R  Q2->LED2    Q2->LED12   Q2->LED7
Q3->CUR-LOW  Q3->CUR-LOW  Q3->CUR-LOW  Q3->CUR-LOW  Q3->LED3    Q3->LED13   Q3->LED8
Q4->PU-12K4  Q4->PU-12K4  Q4->PU-12K4  Q4->PU-12K4  Q4->LED4    Q4->LED14   Q4->LED9
Q5->CUR-HIGH Q5->CUR-HIGH Q5->CUR-HIGH Q5->CUR-HIGH Q5->LED5    Q5->LED15   Q5->LED10
Q6->X        Q6->X        Q6->X        Q6->X        Q6->X       Q6->X       Q6->X
Q7->X        Q7->X        Q7->X        Q7->X        Q7->X       Q7->X       Q7->X
*/

#endif // ATB_USPD_MPC2_H_
