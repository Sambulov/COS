#ifndef PORT_SPI_H_
#define PORT_SPI_H_

#define HDL_SPI_MESSAGE_PRV_SIZE           4
#define SPI_PRIVATE_SIZE                   20
#define SPI_CH_PRIVATE_SIZE                20

typedef enum {
  HDL_SPI_CLIENT = SPI_CTL0_MSTMOD,
  HDL_SPI_SERVER  = SPI_SLAVE
} hdl_spi_mode_t;

typedef enum {
  HDL_SPI_ENDIAN_MSB = SPI_ENDIAN_MSB,
  HDL_SPI_ENDIAN_LSB = SPI_ENDIAN_LSB
} hdl_spi_endianness_t;

typedef enum {
  HDL_SPI_CS_SOFT = SPI_NSS_SOFT,
  HDL_SPI_CS_HARD = SPI_NSS_HARD
} hdl_spi_cs_ctrl_t;

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


/**************** vvv  SPI slave vvv  ******************/

typedef struct {
  hdl_spi_cs_ctrl_t cs_mode;
  hdl_spi_endianness_t endian;
  hdl_spi_polarity_t polarity;
  hdl_spi_prescale_t prescale;
} hdl_spi_server_config_t;

/* depends on:
  gpio mosi
  gpio miso  
  gpio sck
  gpio nss
  apb2_bus for SPI 5, 4, 3, 0; apb1_bus for SPI 1, 2
  interrupt controller (nvic)
  hdl_timer_t
 */
typedef struct {
  hdl_module_t module;
  hdl_spi_server_config_t *config;
  hdl_nvic_irq_n_t spi_iterrupt;
  hdl_nvic_irq_n_t nss_iterrupt; /* if HDL_SPI_CS_SOFT in HDL_SPI_SERVER mode */
  uint8_t __private[SPI_PRIVATE_SIZE];
} hdl_spi_server_t;

/**************** vvv  SPI master vvv  ******************/
typedef struct {
  hdl_spi_endianness_t endian;
  hdl_spi_polarity_t polarity;
  hdl_spi_prescale_t prescale;
} hdl_spi_client_config_t;

/* depends on:
  gpio mosi
  gpio miso  
  gpio sck
  apb2_bus for SPI 5, 4, 3, 0; apb1_bus for SPI 1, 2
  interrupt controller (nvic)
  hdl_timer_t
 */
typedef struct {
  hdl_module_t module;
  hdl_spi_client_config_t *config;
  hdl_nvic_irq_n_t spi_iterrupt;
  uint8_t __private[SPI_PRIVATE_SIZE];
} hdl_spi_client_t;

/* depends on:
  hdl_spi_t
  gpio cs
 */
typedef struct {
  hdl_module_t module;
  uint8_t __private[SPI_PRIVATE_SIZE];
} hdl_spi_ch_t;

#endif // PORT_SPI_H_
