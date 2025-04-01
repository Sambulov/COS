#ifndef PORT_DMA_H_
#define PORT_DMA_H_

/*                            
                                                            DMA 0
              | Channel 0 | Channel 1 | Channel 2 | Channel 3 | Channel 4 | Channel 5 | Channel 6 | Channel 7 |
DMA_SUBPERI_0 | SPI2_RX   |     ●     | SPI2_RX   | SPI1_RX   | SPI1_TX   |  SPI2_TX  |    ●      |  SPI2_TX  |
DMA_SUBPERI_1 |  I2C0_RX  |     ●     | TIMER6_UP |   ●       | TIMER6_UP |  I2C0_RX  |  I2C0_TX  |  I2C0_TX  |
DMA_SUBPERI_2 |TIMER3_CH0 |     ●     |I2S2_ADD_RX| TIMER3_CH1|I2S1_ADD_TX|I2S2_ADD_TX| TIMER3_UP | TIMER3_CH2|
DMA_SUBPERI_3 |I2S2_ADD_RX| TIMER1_UP | I2C2_RX   |I2S1_ADD_RX|  I2C2_TX  |TIMER1_CH0 | TIMER1_CH1| TIMER1_UP |
                          | TIMER1_CH2|                                               | TIMER1_CH3| TIMER1_CH3|
DMA_SUBPERI_4 | UART4_RX  | USART2_RX | UART3_RX  | USART2_TX | UART3_TX  | USART1_RX | USART1_TX | UART4_TX  |
DMA_SUBPERI_5 | UART7_TX  | UART6_TX  | TIMER2_CH3| UART6_RX  |TIMER2_CH0 | TIMER2_CH1| UART7_RX  | TIMER2_CH2|
                                      | TIMER2_UP |           | TIMER2_TG |
DMA_SUBPERI_6 | TIMER4_CH2|TIMER4_CH3 | TIMER4_TG |TIMER4_CH3 | TIMER4_CH1|    ●      | TIMER4_UP |     ●     |
              | TIMER4_UP |           | TIMER4_CH0| TIMER4_TG
DMA_SUBPERI_7 |    ●      | TIMER5_UP |  I2C1_RX  | I2C1_RX   | USART2_TX |  DAC0     |  DAC1     |   I2C1_TX |

                                                            DMA 1
              | Channel 0 | Channel 1 | Channel 2 | Channel 3 | Channel 4 | Channel 5 | Channel 6 | Channel 7 |
DMA_SUBPERI_0 |   ADC0    |    ●      |TIMER7_CH0 |      ●    |   ADC0    |     ●     |TIMER0_CH0 |    ●      |
                                      |TIMER7_CH1 |                                   |TIMER0_CH1 |
                                      |TIMER7_CH2 |                                   |TIMER0_CH2 |
DMA_SUBPERI_1 |     ●     |   DCI     |   ADC1    |    ADC1   |     ●     |   SPI5_TX |  SPI5_RX  |   DCI     |
DMA_SUBPERI_2 |   ADC2    |   ADC2    |     ●     |   SPI4_RX |  SPI4_TX  |     ●     |     ●     |    ●      |
DMA_SUBPERI_3 |  SPI0_RX  |      ●    |   SPI0_RX |  SPI0_TX  |     ●     |  SPI0_TX  |    ●      |     ●     |
DMA_SUBPERI_4 |  SPI3_RX  |  SPI3_TX  | USART0_RX |   SDIO    |     ●     | USART0_RX |   SDIO    | USART0_TX |
DMA_SUBPERI_5 |     ●     | USART5_RX | USART5_RX |   SPI3_RX | SPI3_TX   |   ●       | USART5_TX | USART5_TX |
DMA_SUBPERI_6 | TIMER0_TG | TIMER0_CH0| TIMER0_CH1| TIMER0_CH0| TIMER0_CH3| TIMER0_UP | TIMER0_CH2|     ●     |
                                                              | TIMER0_TG |
                                                              |TIMER0_CMT |
DMA_SUBPERI_7 |      ●    | TIMER7_UP | TIMER7_CH0| TIMER7_CH1| TIMER7_CH2|   SPI4_RX |  SPI4_TX   | TIMER7_CH3|
                                                                                                   | TIMER7_TG |
                                                                                                   | TIMER7_CMT|
*/

#include "hdl_dma.h"

typedef struct {
  rcu_periph_enum rcu;
  uint32_t phy;
} hdl_dma_config_t;

typedef struct {
  uint32_t priority;                        /* DMA_PRIORITY_LOW DMA_PRIORITY_MEDIUM DMA_PRIORITY_HIGH DMA_PRIORITY_ULTRA_HIGH */
  uint32_t periph_width;                    /* DMA_MEMORY_WIDTH_8BIT, DMA_MEMORY_WIDTH_16BIT, DMA_MEMORY_WIDTH_32BIT */
  uint32_t memory_width;                    /* DMA_MEMORY_WIDTH_8BIT, DMA_MEMORY_WIDTH_16BIT, DMA_MEMORY_WIDTH_32BIT */
  uint32_t direction;                       /* DMA_MEMORY_TO_MEMORY, DMA_MEMORY_TO_PERIPH, DMA_PERIPH_TO_MEMORY */
  dma_subperipheral_enum channel_periphery; /* DMA_SUBPERI0, DMA_SUBPERI1, DMA_SUBPERI2, DMA_SUBPERI3, DMA_SUBPERI4, DMA_SUBPERI5, DMA_SUBPERI6, DMA_SUBPERI7 */
  uint8_t periph_inc    : 1,
          memory_inc    : 1,
          circular      : 1;
  dma_channel_enum ch_no;
} hdl_dma_channel_config_t;

hdl_module_new_t(hdl_dma_mcu_t, 0, hdl_dma_config_t, hdl_module_base_iface_t);

/* Depends on 
  dma module 
*/
hdl_module_new_t(hdl_dma_channel_mcu_t, 0, hdl_dma_channel_config_t, hdl_dma_channel_iface_t);

extern const hdl_module_base_iface_t hdl_dma_iface;
extern const hdl_dma_channel_iface_t hdl_dma_channel_iface;

#endif
