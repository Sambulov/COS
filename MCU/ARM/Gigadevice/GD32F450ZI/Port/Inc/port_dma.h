#ifndef PORT_DMA_H_
#define PORT_DMA_H_

#define HDL_DMA_PRV_SIZE  0

typedef enum {
    HDL_DMA_PRIORITY_LOW = (uint32_t)DMA_PRIORITY_LOW,
    HDL_DMA_PRIORITY_MEDIUM = (uint32_t)DMA_PRIORITY_MEDIUM,
    HDL_DMA_PRIORITY_HIGH = (uint32_t)DMA_PRIORITY_HIGH,
    HDL_DMA_PRIORITY_ULTRA_HIGH = (uint32_t)DMA_PRIORITY_ULTRA_HIGH,
} hdl_dma_priority_e;

/*                            
                                                            DMA 0
                            | Channel 0 | Channel 1 | Channel 2 | Channel 3 | Channel 4 | Channel 5 | Channel 6 | Channel 7 |
HDL_DMA_CHANNEL_PERIPHERY_0 | SPI2_RX   |     ●     | SPI2_RX   | SPI1_RX   | SPI1_TX   |  SPI2_TX  |    ●      |  SPI2_TX  |
HDL_DMA_CHANNEL_PERIPHERY_1 |  I2C0_RX  |     ●     | TIMER6_UP |   ●       | TIMER6_UP |  I2C0_RX  |  I2C0_TX  |  I2C0_TX  |
HDL_DMA_CHANNEL_PERIPHERY_2 |TIMER3_CH0 |     ●     |I2S2_ADD_RX| TIMER3_CH1|I2S1_ADD_TX|I2S2_ADD_TX| TIMER3_UP | TIMER3_CH2|
HDL_DMA_CHANNEL_PERIPHERY_3 |I2S2_ADD_RX| TIMER1_UP | I2C2_RX   |I2S1_ADD_RX|  I2C2_TX  |TIMER1_CH0 | TIMER1_CH1| TIMER1_UP |
                                        | TIMER1_CH2|                                               | TIMER1_CH3| TIMER1_CH3|
HDL_DMA_CHANNEL_PERIPHERY_4 | UART4_RX  | USART2_RX | UART3_RX  | USART2_TX | UART3_TX  | USART1_RX | USART1_TX | UART4_TX  |
HDL_DMA_CHANNEL_PERIPHERY_5 | UART7_TX  | UART6_TX  | TIMER2_CH3| UART6_RX  |TIMER2_CH0 | TIMER2_CH1| UART7_RX  | TIMER2_CH2|
                                                    | TIMER2_UP |           | TIMER2_TG |
HDL_DMA_CHANNEL_PERIPHERY_6 | TIMER4_CH2|TIMER4_CH3 | TIMER4_TG |TIMER4_CH3 | TIMER4_CH1|    ●      | TIMER4_UP |     ●     |
                            | TIMER4_UP |           | TIMER4_CH0| TIMER4_TG
HDL_DMA_CHANNEL_PERIPHERY_7 |    ●      | TIMER5_UP |  I2C1_RX  | I2C1_RX   | USART2_TX |  DAC0     |  DAC1     |   I2C1_TX |

                                                            DMA 1
                            | Channel 0 | Channel 1 | Channel 2 | Channel 3 | Channel 4 | Channel 5 | Channel 6 | Channel 7 |
HDL_DMA_CHANNEL_PERIPHERY_0 |   ADC0    |    ●      |TIMER7_CH0 |      ●    |   ADC0    |     ●     |TIMER0_CH0 |    ●      |
                                                    |TIMER7_CH1 |                                   |TIMER0_CH1 |
                                                    |TIMER7_CH2 |                                   |TIMER0_CH2 |
HDL_DMA_CHANNEL_PERIPHERY_1 |     ●     |   DCI     |   ADC1    |    ADC1   |     ●     |   SPI5_TX |  SPI5_RX  |   DCI     |
HDL_DMA_CHANNEL_PERIPHERY_2 |   ADC2    |   ADC2    |     ●     |   SPI4_RX |  SPI4_TX  |     ●     |     ●     |    ●      |
HDL_DMA_CHANNEL_PERIPHERY_3 |  SPI0_RX  |      ●    |   SPI0_RX |  SPI0_TX  |     ●     |  SPI0_TX  |    ●      |     ●     |
HDL_DMA_CHANNEL_PERIPHERY_4 |  SPI3_RX  |  SPI3_TX  | USART0_RX |   SDIO    |     ●     | USART0_RX |   SDIO    | USART0_TX |
HDL_DMA_CHANNEL_PERIPHERY_5 |     ●     | USART5_RX | USART5_RX |   SPI3_RX | SPI3_TX   |   ●       | USART5_TX | USART5_TX |
HDL_DMA_CHANNEL_PERIPHERY_6 | TIMER0_TG | TIMER0_CH0| TIMER0_CH1| TIMER0_CH0| TIMER0_CH3| TIMER0_UP | TIMER0_CH2|     ●     |
                                                                            | TIMER0_TG |
                                                                            |TIMER0_CMT |
HDL_DMA_CHANNEL_PERIPHERY_7 |      ●    | TIMER7_UP | TIMER7_CH0| TIMER7_CH1| TIMER7_CH2|   SPI4_RX |  SPI4_TX   | TIMER7_CH3|
                                                                                                                 | TIMER7_TG |
                                                                                                                 | TIMER7_CMT|
*/
typedef enum {
  HDL_DMA_CHANNEL_PERIPHERY_0 = DMA_SUBPERI0,
  HDL_DMA_CHANNEL_PERIPHERY_1 = DMA_SUBPERI1,
  HDL_DMA_CHANNEL_PERIPHERY_2 = DMA_SUBPERI2,
  HDL_DMA_CHANNEL_PERIPHERY_3 = DMA_SUBPERI3,
  HDL_DMA_CHANNEL_PERIPHERY_4 = DMA_SUBPERI4,
  HDL_DMA_CHANNEL_PERIPHERY_5 = DMA_SUBPERI5,
  HDL_DMA_CHANNEL_PERIPHERY_6 = DMA_SUBPERI6,
  HDL_DMA_CHANNEL_PERIPHERY_7 = DMA_SUBPERI7,
} hdl_dma_channel_periphery_e;

typedef struct {
  hdl_module_t module;                   /* Depends on dma module*/
  hdl_dma_size_of_memory_e periph_width;
  hdl_dma_size_of_memory_e memory_width;
  hdl_dma_increment_e periph_inc;        /* HDL_DMA_INCREMENT_OFF, HDL_DMA_INCREMENT_ON */
  hdl_dma_increment_e memory_inc;        /* HDL_DMA_INCREMENT_OFF, HDL_DMA_INCREMENT_ON */
  hdl_dma_direction_e direction;         /* HDL_DMA_DIRECTION_P2M, HDL_DMA_DIRECTION_M2P, HDL_DMA_DIRECTION_M2M */
  hdl_dma_mode_e mode;                   /* HDL_DMA_MODE_SINGLE_CONVERSION, HDL_DMA_MODE_CIRCULAR_CONVERSION */
  hdl_dma_priority_e priority;           /* hdl_dma_priority_e */
  hdl_dma_channel_periphery_e channel_periphery;
} hdl_dma_channel_t;

#endif