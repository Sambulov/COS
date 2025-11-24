#ifndef PORT_DMA_H_
#define PORT_DMA_H_

/*                            
                                                   DMA 1
          |  Stream 0 |  Stream 1 |  Stream 2 |  Stream 3 |  Stream 4 |  Stream 5 |  Stream 6 |  Stream 7 |
Channel 0 |  SPI3_RX  |     ●     |  SPI3_RX  |  SPI2_RX  |  SPI2_TX  |  SPI3_TX  |     ●     |  SPI3_TX  |
Channel 1 |  I2C1_RX  |     ●     | TIMER7_UP |     ●     | TIMER7_UP |  I2C1_RX  |  I2C1_TX  |  I2C1_TX  |
Channel 2 |TIMER4_CH0 |     ●     |I2S3_EXT_RX| TIMER4_CH1|I2S2_EXT_TX|I2S3_EXT_TX| TIMER4_UP | TIMER4_CH3|
Channel 3 |I2S3_EXT_RX| TIMER2_UP | I2C3_RX   |I2S2_EXT_RX|  I2C3_TX  |TIMER2_CH1 | TIMER1_CH2| TIMER2_UP |
          |           | TIMER2_CH3|           |           |           |           | TIMER1_CH4| TIMER2_CH4|
Channel 4 | UART5_RX  | USART3_RX | UART4_RX  | USART3_TX | UART4_TX  | USART2_RX | USART2_TX | UART5_TX  |
Channel 5 | UART8_TX  | UART7_TX  | TIMER3_CH4| UART7_RX  |TIMER3_CH1 | TIMER3_CH2| UART8_RX  | TIMER3_CH3|
          |           |           | TIMER3_UP |           |TIMER3_TRIG|           |           |           |
Channel 6 | TIMER5_CH3|TIMER5_CH4 | TIMER5_CH1|TIMER5_CH4 | TIMER5_CH2|     ●     | TIMER5_UP |     ●     |
          | TIMER5_UP |TIMER5_TRIG|           |TIMER5_TRIG|           |           |           |           |
Channel 7 |     ●     | TIMER6_UP |  I2C2_RX  | I2C2_RX   | USART3_TX |  DAC1     |  DAC2     |   I2C2_TX |

                                                   DMA 2
          |  Stream 0 |  Stream 1 |  Stream 2 |  Stream 3 |  Stream 4 |  Stream 5 |  Stream 6 |  Stream 7 |
Channel 0 |   ADC1    |   SAI1_A  |TIMER8_CH1 |   SAI1_A  |   ADC1    |   SAI1_B  |TIMER1_CH1 |     ●     |
          |           |           |TIMER8_CH2 |           |           |           |TIMER1_CH2 |           |
          |           |           |TIMER8_CH3 |           |           |           |TIMER1_CH3 |           |
Channel 1 |     ●     |   DCMI    |   ADC2    |    ADC2   |   SAI1_B  |  SPI6_TX  |  SPI6_RX  |   DCMI    |
Channel 2 |   ADC3    |   ADC3    |     ●     |  SPI5_RX  |  SPI5_TX  | CRYP_OUT  |  CRYP_IN  |  HASH_IN  |
Channel 3 |  SPI1_RX  |     ●     |  SPI1_RX  |  SPI1_TX  |     ●     |  SPI1_TX  |     ●     |     ●     |
Channel 4 |  SPI4_RX  |  SPI4_TX  | USART1_RX |   SDIO    |     ●     | USART1_RX |   SDIO    | USART1_TX |
Channel 5 |     ●     | USART6_RX | USART6_RX |  SPI4_RX  |  SPI4_TX  |     ●     | USART6_TX | USART6_TX |
Channel 6 |TIMER1_TRIG| TIMER1_CH1| TIMER1_CH2| TIMER1_CH1| TIMER1_CH4| TIMER1_UP | TIMER1_CH3|     ●     |
          |           |           |           |           |TIMER1_TRIG|           |           |           |
          |           |           |           |           |TIMER1_COM |           |           |           |
Channel 7 |     ●     | TIMER8_UP | TIMER8_CH1| TIMER8_CH2| TIMER8_CH3|   SPI5_RX |  SPI5_TX  | TIMER8_CH4|
          |           |           |           |           |           |           |           |TIMER8_TRIG|
          |           |           |           |           |           |           |           | TIMER8_COM|
*/
#include "hdl_dma.h"

typedef enum {
  HDL_DMA_STREAM_0 = 0,
  HDL_DMA_STREAM_1 = 1,
  HDL_DMA_STREAM_2 = 2,
  HDL_DMA_STREAM_3 = 3,
  HDL_DMA_STREAM_4 = 4,
  HDL_DMA_STREAM_5 = 5,
  HDL_DMA_STREAM_6 = 6,
  HDL_DMA_STREAM_7 = 7
} hdl_dma_stream_t;

typedef struct {
  uint32_t rcc;
  uint32_t phy;
} hdl_dma_config_t;

typedef struct {
  uint32_t priority;               /* DMA_PRIORITY_LOW, DMA_PRIORITY_MEDIUM, DMA_PRIORITY_HIGH, DMA_PRIORITY_VERY_HIGH */
  uint32_t periph_width;           /* DMA_PDATAALIGN_BYTE, DMA_PDATAALIGN_HALFWORD, DMA_PDATAALIGN_WORD */
  uint32_t memory_width;           /* DMA_MDATAALIGN_BYTE, DMA_MDATAALIGN_HALFWORD, DMA_MDATAALIGN_WORD */
  uint32_t periph_burst;           /* DMA_PBURST_SINGLE, DMA_PBURST_INC4, DMA_PBURST_INC8, DMA_PBURST_INC16 */
  uint32_t memory_burst;           /* DMA_MBURST_SINGLE, DMA_MBURST_INC4, DMA_MBURST_INC8, DMA_MBURST_INC16 */
  uint32_t channel_periphery;      /* DMA_CHANNEL_0 ... DMA_CHANNEL_7 */
  uint8_t periph_inc    : 1,
          memory_inc    : 1,
          periph_ctrl   : 1,
          circular      : 1;
  hdl_dma_stream_t stream;
} hdl_dma_channel_config_t;

/* Depends on 
  ahb clock 
*/
hdl_module_new_t(hdl_dma_mcu_t, 0, hdl_dma_config_t*, hdl_module_base_iface_t);

/* Depends on 
  dma module 
*/
hdl_module_new_t(hdl_dma_channel_mcu_t, 0, hdl_dma_channel_config_t*, hdl_dma_channel_iface_t);

extern const hdl_module_base_iface_t hdl_dma_iface;
extern const hdl_dma_channel_iface_t hdl_dma_channel_iface;

uint8_t port_dma_ch_transfer_complete(const void *desc);

#endif /* PORT_DMA_H_ */
