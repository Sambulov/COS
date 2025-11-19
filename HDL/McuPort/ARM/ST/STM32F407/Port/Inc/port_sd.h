#ifndef PORT_SD_H_
#define PORT_SD_H_

#include "hdl_nvm.h"

#define HDL_SDIO_MCU_VAR_SIZE 24

typedef struct {
  uint32_t phy;
  uint32_t clock_edge;            /*!< Specifies the clock transition on which the bit capture is made.
                                       [SDIO_CLOCK_EDGE_RISING, SDIO_CLOCK_EDGE_FALLING]                       */
  uint32_t clock_bypass;          /*!< Specifies whether the SDMMC Clock divider bypass is enabled or disabled.
                                       [SDIO_CLOCK_BYPASS_DISABLE, SDIO_CLOCK_BYPASS_ENABLE]                   */
  uint32_t clock_power_save;      /*!< Specifies whether SDMMC Clock output is enabled or disabled when the bus is idle.
                                       [SDIO_CLOCK_POWER_SAVE_DISABLE, SDIO_CLOCK_POWER_SAVE_ENABLE]           */
  uint32_t bus_wide;              /*!< Specifies the SDMMC bus width.
                                       [SDIO_BUS_WIDE_1B, SDIO_BUS_WIDE_4B, SDIO_BUS_WIDE_8B]                  */
  uint32_t hardware_flow_control; /*!< Specifies whether the SDMMC hardware flow control is enabled or disabled.
                                       [SDIO_HARDWARE_FLOW_CONTROL_DISABLE, SDIO_HARDWARE_FLOW_CONTROL_ENABLE] */
  uint32_t clock_div;             /*!< Specifies the clock frequency of the SDMMC controller.
                                       [0, 255] */
} hdl_sdio_mcu_config_t;

//typedef uint8_t (* hdl_nvm_cancel_t)(const void *desc);
//typedef uint8_t (* hdl_nvm_transfer_t)(const void *desc, hdl_nvm_message_t *message);
//typedef uint8_t (* hdl_nvm_info_get_t)(const void *desc, hdl_nvm_info_t *out_info);

typedef struct{
  hdl_module_initializer_t init;
  //hdl_nvm_info_get_t info;
  //hdl_nvm_transfer_t read;
  //hdl_nvm_transfer_t write;
  //hdl_nvm_transfer_t erase;
  //hdl_nvm_cancel_t cancel;
} hdl_sdio_mcu_iface_t;

/* depends on:
  hdl_clock_t APB2
  hdl_gpio_t clk, cmd, io1 [io2 ... io8]
*/
hdl_module_new_t(hdl_sdio_mcu_t, 0, hdl_sdio_mcu_config_t *, hdl_sdio_mcu_iface_t);

extern const hdl_sdio_mcu_iface_t hdl_sdio_mcu_iface;

/* depends on:
  hdl_sdio_mcu_t
*/
hdl_module_new_t(hdl_sd_card_t, 0, void *, hdl_nvm_iface_t);

extern const hdl_nvm_iface_t hdl_sd_mcu_iface;

#endif /* PORT_SD_H_ */
