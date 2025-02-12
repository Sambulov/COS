#ifndef PORT_SPI_H_
#define PORT_SPI_H_

#define SPI_ERROR_MASK      (uint32_t)(SPI_STAT_FERR | SPI_STAT_RXORERR | SPI_STAT_CONFERR | SPI_STAT_CRCERR | SPI_STAT_TXURERR)

typedef enum {
  HDL_SPI_CLIENT = SPI_CTL0_MSTMOD,
  HDL_SPI_SERVER  = SPI_SLAVE
} hdl_spi_mode_t;

typedef enum {
  HDL_SPI_ENDIAN_MSB = SPI_ENDIAN_MSB,
  HDL_SPI_ENDIAN_LSB = SPI_ENDIAN_LSB
} hdl_spi_endianness_t;

typedef enum {
  HDL_SPI_CK_PL_LOW_PH_1EDGE  = SPI_CK_PL_LOW_PH_1EDGE,
  HDL_SPI_CK_PL_HIGH_PH_1EDGE = SPI_CK_PL_HIGH_PH_1EDGE,
  HDL_SPI_CK_PL_LOW_PH_2EDGE  = SPI_CK_PL_LOW_PH_2EDGE,
  HDL_SPI_CK_PL_HIGH_PH_2EDGE = SPI_CK_PL_HIGH_PH_2EDGE,
} hdl_spi_polarity_t;

typedef enum {
  HDL_SPI_PSC_2   = SPI_PSC_2,
  HDL_SPI_PSC_4   = SPI_PSC_4,
  HDL_SPI_PSC_8   = SPI_PSC_8,
  HDL_SPI_PSC_16  = SPI_PSC_16,
  HDL_SPI_PSC_32  = SPI_PSC_32,
  HDL_SPI_PSC_64  = SPI_PSC_64,
  HDL_SPI_PSC_128 = SPI_PSC_128,
  HDL_SPI_PSC_256 = SPI_PSC_256
} hdl_spi_prescale_t;

void hdl_spi_reset_status(uint32_t spi_module_reg);

/**************** vvv  SPI slave vvv  ******************/
#define HDL_SPI_SERVER_VAR_SIZE            52

typedef struct {
  uint32_t phy;
  rcu_periph_enum rcu;
  hdl_spi_endianness_t endian;
  hdl_spi_polarity_t polarity;
  hdl_interrupt_t *spi_interrupt;
  hdl_interrupt_t *nss_interrupt;
} hdl_spi_server_config_t;

/* hdl_spi_server_t depends on:
  gpio mosi
  gpio miso  
  gpio sck
  gpio nss
  apb2_bus for SPI 5, 4, 3, 0; apb1_bus for SPI 1, 2
  interrupt controller (nvic)
 */

/**************** vvv  SPI slave DMA vvv  ******************/
#define HDL_SPI_SERVER_DMA_VAR_SIZE        84

/* hdl_spi_server_dma_t depends on:
  gpio mosi
  gpio miso  
  gpio sck
  gpio nss
  apb2_bus for SPI 5, 4, 3, 0; apb1_bus for SPI 1, 2
  interrupt controller (nvic)
  hdl_dma_channel rx
  hdl_dma_channel tx
  timer
*/

/**************** vvv  SPI master vvv  ******************/
#define HDL_SPI_CLIENT_VAR_SIZE           32
#define HDL_SPI_CLIENT_CH_VAR_SIZE        28

typedef struct {
  uint32_t phy;
  rcu_periph_enum rcu;
  hdl_spi_endianness_t endian;
  hdl_spi_polarity_t polarity;
  hdl_spi_prescale_t prescale;
  hdl_interrupt_t *spi_interrupt;
} hdl_spi_client_config_t;

/* hdl_spi_client_t depends on:
  gpio mosi
  gpio miso  
  gpio sck
  apb2_bus for SPI 5, 4, 3, 0; apb1_bus for SPI 1, 2
  interrupt controller (nvic)
 */

typedef struct {
  uint32_t cs_min_delay;  // ticks
} hdl_spi_client_ch_config_t;

/* hdl_spi_client_ch_t depends on:
  hdl_spi_client_t
  gpio cs
  hdl_tick_counter
 */

#endif // PORT_SPI_H_
