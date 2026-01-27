#ifndef PORT_MAC_H_
#define PORT_MAC_H_

#include "hdl_mac.h"

#define HDL_MAC_MCU_VAR_SIZE        120

typedef struct {
  const uint8_t mac_addr[6];      /* default MAC */
  uint8_t rmii_mode       :1,     /* 0: MII; 1: RMII */
          pass_multicast  :1;     /* use for IPv6 */
} hdl_mac_mcu_config_t;

/* depends on:
  hdl_clock_t AHB, 
  clock_couter,
  hdl_eth_phy_t,
  hdl_gpio_t ETH_REF_CLK, ETH_CRS_DV, ETH_RXD0, ETH_RXD1, ETH_TX_EN, ETH_TXD0, ETH_TXD1
*/
hdl_module_new_t(hdl_mac_mcu_t, HDL_MAC_MCU_VAR_SIZE, hdl_mac_mcu_config_t *, hdl_mac_iface_t);

extern const hdl_mac_iface_t hdl_mac_mcu_iface;

#endif /* PORT_MAC_H_ */
