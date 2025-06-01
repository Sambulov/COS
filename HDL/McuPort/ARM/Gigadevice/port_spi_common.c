#include "hdl_portable.h"

#ifndef SPI_STAT_FERR
#define SPI_STAT_FERR   0
#endif

void hdl_spi_reset_status (uint32_t spi_reg) {
  __IO uint32_t tmpreg = SPI_DATA(spi_reg);
  tmpreg = SPI_STAT(spi_reg);
  SPI_CTL0(spi_reg) = SPI_CTL0(spi_reg);
  SPI_STAT(spi_reg) &= ~(SPI_STAT_CRCERR | SPI_STAT_FERR);
  (void)tmpreg;
}