#ifndef PORT_SPI_H_
#define PORT_SPI_H_


void hdl_spi_reset_status(uint32_t spi_module_reg);

/**************** vvv  SPI slave vvv  ******************/
#define HDL_SPI_SERVER_PRIVATE_SIZE          0
typedef struct {

} hdl_spi_server_config_t;

/* depends on:
  gpio mosi
  gpio miso  
  gpio sck
  gpio nss
  apb2_bus for SPI 5, 4, 3, 0; apb1_bus for SPI 1, 2
  interrupt controller (nvic)
 */
//typedef struct {

//} hdl_spi_server_t;

/**************** vvv  SPI slave DMA vvv  ******************/
#define HDL_SPI_SERVER_DMA_PRIVATE_SIZE      0

/* depends on:
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
//typedef struct {

//} hdl_spi_server_dma_t;

/**************** vvv  SPI master vvv  ******************/
#define HDL_SPI_CLIENT_PRIVATE_SIZE          0
#define HDL_SPI_CLIENT_CH_PRIVATE_SIZE       0

typedef struct {

} hdl_spi_client_config_t;

/* depends on:
  gpio mosi
  gpio miso  
  gpio sck
  apb2_bus for SPI 5, 4, 3, 0; apb1_bus for SPI 1, 2
  interrupt controller (nvic)
 */
//typedef struct {

//} hdl_spi_client_t;

/* depends on:
  hdl_spi_t
  gpio cs
 */
//typedef struct {

//} hdl_spi_client_ch_t;

typedef struct {

} hdl_spi_client_ch_config_t;

#endif // PORT_SPI_H_
