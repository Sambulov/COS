#ifndef LAN8742_H
#define LAN8742_H

#include "hdl_eth_phy.h"

#define HDL_LAN8742_VAR_SIZE     44

typedef struct {
  uint8_t dev_addr;       /* max dev_addr: 31; 255 - autodetect */
  eth_phy_mode_t mode;
  eth_phy_link_t link;
  uint32_t sync_state_delay; /* sync state by timer */
} hdl_lan8742_config_t;

/* depends on:
  hdl_mdio_t
  hdl_gpio_t int
  hdl_time_counter_t
*/
hdl_module_new_t(hdl_lan8742_t, HDL_LAN8742_VAR_SIZE, hdl_lan8742_config_t *, hdl_eth_phy_iface_t);

extern const hdl_eth_phy_iface_t hdl_lan8742_iface;

#endif /* LAN8742_H */
