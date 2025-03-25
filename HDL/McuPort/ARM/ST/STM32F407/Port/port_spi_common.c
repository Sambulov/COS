#include "hdl_portable.h"

void hdl_spi_reset_status (SPI_TypeDef *phy) {
  __IO uint32_t tmpreg = phy->DR;
  phy->SR &= ~SPI_SR_CRCERR;
  phy->CR1 = phy->CR1;
  (void)tmpreg;
}