#ifndef PORT_SPI_H_
#define PORT_SPI_H_

/**************** vvv  SPI slave vvv  ******************/
#define HDL_SPI_SERVER_PRIVATE_SIZE            0

typedef struct {

} hdl_spi_server_config_t;

/**************** vvv  SPI slave DMA vvv  ******************/
#define HDL_SPI_SERVER_DMA_PRIVATE_SIZE        0

/**************** vvv  SPI master vvv  ******************/
#define HDL_SPI_CLIENT_PRIVATE_SIZE            0
#define HDL_SPI_CLIENT_CH_PRIVATE_SIZE         0   

typedef struct {

} hdl_spi_client_config_t;

typedef struct {

} hdl_spi_client_ch_config_t;

#endif /* PORT_SPI_H_ */ 
