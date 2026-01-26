#ifndef HDL_ETH_PHY_H_
#define HDL_ETH_PHY_H_

typedef enum {
  ETH_PHY_SPEED            = 0x03,
  ETH_PHY_SPEED_10M        = 0x00,
  ETH_PHY_SPEED_100M       = 0x01,
  ETH_PHY_SPEED_1000M      = 0x02,
  ETH_PHY_SPEED_2500M      = 0x03,
  ETH_PHY_DUPLEX           = 0x04,
  ETH_PHY_DUPLEX_HALF      = 0x00,
  ETH_PHY_DUPLEX_FULL      = 0x04,
  ETH_PHY_CROSSOVER        = 0x08,
} eth_phy_link_t;

typedef enum {
  ETH_PHY_MODE_POWER_DOWN = 0x01,
  ETH_PHY_MODE_ISOLATE    = 0x02,
  ETH_PHY_MODE_LOOPBACK   = 0x04,
  ETH_PHY_MODE_AUTO_MDIX  = 0x40,
  ETH_PHY_MODE_AUTONEGO   = 0x80,
} eth_phy_mode_t;

typedef struct {
  uint8_t fault   : 1,
          config  : 1,
          link_up : 1;
  eth_phy_link_t link;
  eth_phy_mode_t mode;
  uint8_t addr;
  uint32_t id;
} eth_phy_state_t;

typedef uint8_t (*hdl_eth_phy_set_link_t)(const void *desc, eth_phy_link_t link);
typedef uint8_t (*hdl_eth_phy_set_mode_t)(const void *desc, eth_phy_mode_t mode);

typedef struct {
  hdl_module_initializer_t init;
  hdl_eth_phy_set_link_t set_link;
  hdl_eth_phy_set_mode_t set_mode;
  hdl_event_subscribtion_t subscribe; /* event_trigger: eth_phy_state_t */
} hdl_eth_phy_iface_t;

hdl_module_new_t(hdl_eth_phy_t, 0, void *, hdl_eth_phy_iface_t);

__STATIC_INLINE uint8_t hdl_eth_phy_set_link(const void *desc, eth_phy_link_t link) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_eth_phy_t *)desc)->iface->set_link(desc, link);
}

__STATIC_INLINE uint8_t hdl_eth_phy_set_mode(const void *desc, eth_phy_mode_t mode) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_eth_phy_t *)desc)->iface->set_mode(desc, mode);
}

__STATIC_INLINE void hdl_eth_phy_subscribe(const void *desc, hdl_delegate_t *delegate) {
  MODULE_ASSERT(desc, );
  ((hdl_eth_phy_t *)desc)->iface->subscribe(desc, delegate);
}

#endif /* HDL_ETH_PHY_H_ */
