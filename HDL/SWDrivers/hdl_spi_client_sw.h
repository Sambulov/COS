#ifndef HDL_SPI_CLIENT_SW_H_
#define HDL_SPI_CLIENT_SW_H_

#define HDL_SPI_CLIENT_SW_VAR_SIZE           12
#define HDL_SPI_CLIENT_CH_SW_VAR_SIZE        28

typedef enum {
  HDL_SPI_SW_BIG_ENDIAN,
  HDL_SPI_SW_LITTLE_ENDIAN,
} hdl_spi_sw_endianness_t;

/*             __    __    __    __    CPOL=0
Sck      _____/  \__/  \__/  \__/  \   EDGE=1
            __    __    __    __
         __/  \__/  \__/  \__/  \__/   EDGE=2
         _____    __    __    __       CPOL=1
              \__/  \__/  \__/  \__/   EDGE=1   
         __    __    __    __    __    
           \__/  \__/  \__/  \__/  \   EDGE=2
Smapling      |     |     |     |
Data     ___/XX\__/XX\__/XX\__/XX\__
*/
typedef enum {
  HDL_SPI_SW_CPOL0       = 0x01,
  HDL_SPI_SW_CPOL1       = 0x02,
  HDL_SPI_SW_EDGE1       = 0x10,
  HDL_SPI_SW_EDGE2       = 0x20,
  HDL_SPI_SW_CPOL0_1EDGE = HDL_SPI_SW_CPOL0 | HDL_SPI_SW_EDGE1,
  HDL_SPI_SW_CPOL0_2EDGE = HDL_SPI_SW_CPOL0 | HDL_SPI_SW_EDGE2,
  HDL_SPI_SW_CPOL1_1EDGE = HDL_SPI_SW_CPOL1 | HDL_SPI_SW_EDGE1,
  HDL_SPI_SW_CPOL1_2EDGE = HDL_SPI_SW_CPOL1 | HDL_SPI_SW_EDGE2,
} hdl_spi_sw_polarity_t;

typedef struct {
  hdl_spi_sw_endianness_t endian;
  hdl_spi_sw_polarity_t polarity;
  uint32_t signal_min_delay;
} hdl_spi_client_sw_config_t;

/* hdl_spi_client_t depends on:
  gpio mosi
  gpio miso
  gpio sck
  hdl_tick_counter
 */
hdl_module_new_t(hdl_spi_client_sw_t, HDL_SPI_CLIENT_SW_VAR_SIZE, hdl_spi_client_sw_config_t, hdl_module_base_iface_t);

extern const hdl_module_base_iface_t hdl_spi_client_sw_iface;

typedef struct {
  uint32_t cs_min_delay;  // ticks
} hdl_spi_client_ch_sw_config_t;

/* hdl_spi_client_ch_t depends on:
  hdl_spi_client_t
  gpio cs
  hdl_tick_counter
 */
hdl_module_new_t(hdl_spi_client_ch_sw_t, HDL_SPI_CLIENT_CH_SW_VAR_SIZE, hdl_spi_client_ch_sw_config_t, hdl_spi_client_ch_iface_t);

extern const hdl_spi_client_ch_iface_t hdl_spi_client_ch_sw_iface;

#endif /* HDL_SPI_CLIENT_SW_H_ */
