#ifndef PORT_SPI_H_
#define PORT_SPI_H_

#include "hdl_spi.h"

#define   SPI_ERROR_MASK      (uint32_t)(SPI_SR_FRE | SPI_SR_OVR | SPI_SR_MODF | SPI_SR_UDR | SPI_SR_CRCERR)

// typedef enum {
//   HDL_SPI_CLIENT = SPI_CTL0_MSTMOD,
//   HDL_SPI_SERVER = SPI_SLAVE
// } hdl_spi_mode_t;

typedef enum {
  HDL_SPI_ENDIAN_MSB = 0,
  HDL_SPI_ENDIAN_LSB = SPI_CR1_LSBFIRST
} hdl_spi_endianness_t;

typedef enum {
  /* CK to 0 when idle, the first clock transition is the first data capture edge */
  HDL_SPI_CK_PL_LOW_PH_1EDGE  = 0,
  /* CK to 1 when idle, the first clock transition is the first data capture edge */
  HDL_SPI_CK_PL_HIGH_PH_1EDGE = SPI_CR1_CPOL,
  /* CK to 0 when idle, the second clock transition is the first data capture edge */
  HDL_SPI_CK_PL_LOW_PH_2EDGE  = SPI_CR1_CPHA,
  /* CK to 1 when idle, the second clock transition is the first data capture edge */
  HDL_SPI_CK_PL_HIGH_PH_2EDGE = SPI_CR1_CPOL | SPI_CR1_CPHA,
} hdl_spi_polarity_t;

typedef enum {
  HDL_SPI_PSC_2   = 0,
  HDL_SPI_PSC_4   = SPI_CR1_BR_0,
  HDL_SPI_PSC_8   = SPI_CR1_BR_1,
  HDL_SPI_PSC_16  = SPI_CR1_BR_1 | SPI_CR1_BR_0,
  HDL_SPI_PSC_32  = SPI_CR1_BR_2,
  HDL_SPI_PSC_64  = SPI_CR1_BR_2 | SPI_CR1_BR_0,
  HDL_SPI_PSC_128 = SPI_CR1_BR_2 | SPI_CR1_BR_1,
  HDL_SPI_PSC_256 = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0
} hdl_spi_prescale_t;

void hdl_spi_reset_status(SPI_TypeDef *phy);


/**************** vvv  SPI master vvv  ******************/
#define HDL_SPI_CLIENT_VAR_SIZE           32
#define HDL_SPI_CLIENT_CH_VAR_SIZE        28

typedef struct {
  uint32_t phy;  /* SPI1, SPI2 ... */
  uint32_t rcu;  /* RCC_APB2ENR_SPI1EN .. RCC_APB1ENR_SPI2EN .. */
  hdl_spi_endianness_t endian;
  hdl_spi_polarity_t polarity;
  hdl_spi_prescale_t prescale;
  hdl_interrupt_t *interrupt;
} hdl_spi_client_config_t;

/* hdl_spi_client_t depends on:
  gpio mosi
  gpio miso  
  gpio sck
  apb2_bus for SPI 5, 4, 3, 0; apb1_bus for SPI 1, 2
  interrupt controller (nvic)
 */
hdl_module_new_t(hdl_spi_client_mcu_t, HDL_SPI_CLIENT_VAR_SIZE, hdl_spi_client_config_t, hdl_module_base_iface_t);

extern const hdl_module_base_iface_t hdl_spi_client_iface;

typedef struct {
  uint32_t cs_min_delay;  // ticks
} hdl_spi_client_ch_config_t;

/* hdl_spi_client_ch_t depends on:
  hdl_spi_client_t
  gpio cs
  hdl_tick_counter
 */
hdl_module_new_t(hdl_spi_client_ch_mcu_t, HDL_SPI_CLIENT_CH_VAR_SIZE, hdl_spi_client_ch_config_t, hdl_spi_client_ch_iface_t);

extern const hdl_spi_client_ch_iface_t hdl_spi_client_ch_iface;

#endif // PORT_SPI_H_
