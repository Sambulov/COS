#ifndef DUMMY_ETH_PHY_H
#define DUMMY_ETH_PHY_H

#include "hdl_eth_phy.h"

#define HDL_DUMMY_ETH_PHY_VAR_SIZE     12

typedef struct {
  eth_phy_mode_t mode;
  eth_phy_link_t link;
} hdl_dummy_eth_phy_config_t;

hdl_module_new_t(hdl_dummy_eth_phy_t, HDL_DUMMY_ETH_PHY_VAR_SIZE, hdl_dummy_eth_phy_config_t *, hdl_eth_phy_iface_t);

extern const hdl_eth_phy_iface_t hdl_dummy_eth_phy_iface;

#endif /* DUMMY_ETH_PHY_H */
