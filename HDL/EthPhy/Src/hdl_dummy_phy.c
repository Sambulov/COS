#include "hdl_iface.h"

typedef struct {
  hdl_event_t event;
  eth_phy_state_t state;
} hdl_dummy_eth_phy_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_dummy_eth_phy_var_t, *((hdl_dummy_eth_phy_t *)0)->obj_var, HDL_DUMMY_ETH_PHY_VAR_SIZE, "hdl_dummy_phy.h");

static uint8_t _hdl_eth_phy_set_link(const void *desc, eth_phy_link_t link) {
  hdl_dummy_eth_phy_t *dummy_eth_phy = (hdl_dummy_eth_phy_t *)desc;
  hdl_dummy_eth_phy_var_t *dummy_eth_phy_var = (hdl_dummy_eth_phy_var_t *)dummy_eth_phy->obj_var;
  uint8_t mask = ETH_PHY_SPEED_10M | ETH_PHY_SPEED_100M | ETH_PHY_DUPLEX | ETH_PHY_CROSSOVER;
  if(link & ~mask) return HDL_FALSE;
    if((dummy_eth_phy_var->state.link & mask) ^ link) {
    dummy_eth_phy_var->state.link &= ~mask;
    dummy_eth_phy_var->state.link |= link;
    hdl_event_raise(&dummy_eth_phy_var->event, dummy_eth_phy, &dummy_eth_phy_var->state);
  }
  return HDL_TRUE;
}

static uint8_t _hdl_eth_phy_set_mode(const void *desc, eth_phy_mode_t mode) {
  hdl_dummy_eth_phy_t *dummy_eth_phy = (hdl_dummy_eth_phy_t *)desc;
  hdl_dummy_eth_phy_var_t *dummy_eth_phy_var = (hdl_dummy_eth_phy_var_t *)dummy_eth_phy->obj_var;
  uint8_t mask = ETH_PHY_MODE_POWER_DOWN | ETH_PHY_MODE_ISOLATE | ETH_PHY_MODE_LOOPBACK |
    ETH_PHY_MODE_AUTO_MDIX | ETH_PHY_MODE_AUTONEGO;
  if(mode & ~mask) return HDL_FALSE;
  if((dummy_eth_phy_var->state.mode & mask) ^ mode) {
    dummy_eth_phy_var->state.mode &= ~mask;
    dummy_eth_phy_var->state.mode |= mode;
    hdl_event_raise(&dummy_eth_phy_var->event, dummy_eth_phy, &dummy_eth_phy_var->state);
  }
  return HDL_TRUE;
}

static hdl_module_state_t _hdl_dummy_eth_phy(const void *desc, uint8_t enable) {
  hdl_dummy_eth_phy_t *dummy_eth_phy = (hdl_dummy_eth_phy_t *)desc;
  hdl_dummy_eth_phy_var_t *dummy_eth_phy_var = (hdl_dummy_eth_phy_var_t *)dummy_eth_phy->obj_var;
  if(enable) {
    dummy_eth_phy_var->state.config = HDL_FALSE;
    return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_UNLOADED;
}

static void void_hdl_eth_phy_subscribe(const void *desc, hdl_delegate_t *delegate) {
  hdl_dummy_eth_phy_t *dummy_eth_phy = (hdl_dummy_eth_phy_t *)desc;
  hdl_dummy_eth_phy_var_t *dummy_eth_phy_var = (hdl_dummy_eth_phy_var_t *)dummy_eth_phy->obj_var;
  hdl_event_subscribe(&dummy_eth_phy_var->event, delegate);
  hdl_event_raise(&dummy_eth_phy_var->event, dummy_eth_phy, &dummy_eth_phy_var->state);
}

const hdl_eth_phy_iface_t hdl_dummy_eth_phy_iface = {
  .init = &_hdl_dummy_eth_phy,
  .set_link = &_hdl_eth_phy_set_link,
  .set_mode = &_hdl_eth_phy_set_mode,
  .subscribe = &void_hdl_eth_phy_subscribe
};
