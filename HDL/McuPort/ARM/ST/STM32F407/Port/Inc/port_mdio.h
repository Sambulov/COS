#ifndef PORT_MDIO_H_
#define PORT_MDIO_H_

#include "hdl_mdio.h"

#define HDL_MDIO_MCU_VAR_SIZE        24

// typedef struct {
//   uint32_t phy;                   /* ETH */
//   uint32_t rcu;                   /*  */
// } hdl_mdio_mcu_config_t;

/* depends on:
  hdl_clock_t AHB, 
  clock_couter timer,
  hdl_gpio_t ETH_MDC, ETH_MDIO
*/
hdl_module_new_t(hdl_mdio_mcu_t, HDL_MDIO_MCU_VAR_SIZE, void *, hdl_mdio_iface_t);

extern const hdl_mdio_iface_t hdl_mdio_mcu_iface;

#endif /* PORT_MDIO_H_ */
