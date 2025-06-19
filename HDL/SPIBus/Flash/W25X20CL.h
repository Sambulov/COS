#ifndef HDL_W25X20CL_H_
#define HDL_W25X20CL_H_

#define HDL_W25X20CL_VAR_SIZE  60

typedef struct {
  uint32_t sector_size;
  uint16_t page_size;
  uint16_t write_time;
  uint32_t size;
} hdl_w25x20cl_config_t;

/* depends on
  hdl_spi_client_ch
  hdl_time_counter
  hdl_gpio  (WP)
  hdl_gpio  (HOLD)
*/
hdl_module_new_t(hdl_w25x20cl_t, HDL_W25X20CL_VAR_SIZE, hdl_w25x20cl_config_t*, hdl_nvm_iface_t);

extern const hdl_nvm_iface_t hdl_w25x20cl_iface;

#endif /* HDL_W25X20CL_H_ */
