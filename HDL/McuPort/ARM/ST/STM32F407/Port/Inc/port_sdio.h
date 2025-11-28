#ifndef PORT_SDIO_H_
#define PORT_SDIO_H_

#include "hdl_sdio.h"

#define HDL_SDIO_MCU_VAR_SIZE        36

typedef struct {
  uint32_t phy;                   /* SDIO */
  uint32_t rcu;                   /*  */
  uint32_t clock_edge;            /* Specifies the clock transition on which the bit capture is made.
                                     [SDIO_CLOCK_EDGE_RISING, SDIO_CLOCK_EDGE_FALLING]                       */
  uint32_t clock_bypass;          /* Specifies whether the HDL_SDIO Clock divider bypass is enabled or disabled.
                                     [SDIO_CLOCK_BYPASS_DISABLE, SDIO_CLOCK_BYPASS_ENABLE]                   */
  uint32_t clock_power_save;      /* Specifies whether HDL_SDIO Clock output is enabled or disabled when the bus is idle.
                                     [SDIO_CLOCK_POWER_SAVE_DISABLE, SDIO_CLOCK_POWER_SAVE_ENABLE]           */
  uint32_t hardware_flow_control; /* Specifies whether the HDL_SDIO hardware flow control is enabled or disabled.
                                     [SDIO_HARDWARE_FLOW_CONTROL_DISABLE, SDIO_HARDWARE_FLOW_CONTROL_ENABLE] */
  uint32_t init_speed;            /* Use default 400000 */
} hdl_sdio_mcu_config_t;

/* depends on:
  hdl_clock_t APB2, PLL_Q
  hdl_gpio_t clk, cmd, io1 [io2 ... io8]
*/
hdl_module_new_t(hdl_sdio_mcu_t, HDL_SDIO_MCU_VAR_SIZE, hdl_sdio_mcu_config_t *, hdl_sdio_iface_t);

extern const hdl_sdio_iface_t hdl_sdio_mcu_iface;

#endif /* PORT_SDIO_H_ */
