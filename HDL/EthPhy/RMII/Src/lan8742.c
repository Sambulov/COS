#include "hdl_iface.h"
#include "lan8742_prv.h"

#define LAN8742_MAX_DEV_ADDR   ((uint32_t)31U)

typedef struct {
  coroutine_t worker;
  hdl_event_t event;
  eth_phy_state_t state;
  uint32_t timer;
  uint8_t force_event;
} hdl_lan8742_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_lan8742_var_t, *((hdl_lan8742_t *)0)->obj_var, HDL_LAN8742_VAR_SIZE, "lan8742.h");

static uint8_t detect_phy_addr(hdl_lan8742_t *lan8742) {
  hdl_mdio_t *mdio = (hdl_mdio_t *)lan8742->dependencies[0];
  hdl_mdio_message_t msg = {
    .dev_addr = 0,
    .reg_addr = LAN8742_SMR
  };
  for(; msg.dev_addr <= LAN8742_MAX_DEV_ADDR; msg.dev_addr++)
    if((hdl_mdio_read(mdio, &msg)) && ((msg.reg_val & LAN8742_SMR_PHY_ADDR) == msg.dev_addr))
      return msg.dev_addr;
  return 0;
}

static uint8_t _lan8742_modify_reg(hdl_lan8742_t *lan8742, uint8_t addr, uint16_t mask, uint16_t value) {
  hdl_lan8742_var_t *lan8742_var = (hdl_lan8742_var_t *)lan8742->obj_var;
  hdl_mdio_t *mdio = (hdl_mdio_t *)lan8742->dependencies[0];
  hdl_mdio_message_t msg = {
    .dev_addr = lan8742_var->state.addr,
    .reg_addr = addr,
  };
  if(hdl_mdio_read(mdio, &msg)) {
    CL_REG_MODIFY(msg.reg_val, mask, value);
    return hdl_mdio_write(mdio, &msg);
  }
  return HDL_FALSE;
}

static void _lan8742_sync_state(hdl_lan8742_t *lan8742) {
  hdl_lan8742_var_t *lan8742_var = (hdl_lan8742_var_t *)lan8742->obj_var;
  hdl_mdio_t *mdio = (hdl_mdio_t *)lan8742->dependencies[0];
  hdl_mdio_message_t msg = {
    .dev_addr = lan8742_var->state.addr,
    .reg_addr = LAN8742_BSR
  };

  uint8_t link_up = lan8742_var->state.link_up;
  uint8_t link = lan8742_var->state.link;
  uint8_t mode = lan8742_var->state.mode;
  uint8_t fault = lan8742_var->state.fault;

  do {
    /* Read Status register twice?! */
    fault = !hdl_mdio_read(mdio, &msg) || !hdl_mdio_read(mdio, &msg);
    if(fault) break;

    link_up = ((msg.reg_val & LAN8742_BSR_LINK_STATUS) != 0);
    msg.reg_addr = LAN8742_BCR;
    fault = !hdl_mdio_read(mdio, &msg);
    if(fault) break;

    mode &= ~ETH_PHY_MODE_AUTONEGO;
    if(msg.reg_val & LAN8742_BCR_AUTONEGO_EN) { 
      /* Auto Nego enabled */
      mode |= ETH_PHY_MODE_AUTONEGO;
      msg.reg_addr = LAN8742_PHYSCSR;
      fault = !hdl_mdio_read(mdio, &msg);
      if(fault) break;

      CL_REG_CLEAR(link, ETH_PHY_DUPLEX_FULL | ETH_PHY_SPEED);
      if(msg.reg_val & LAN8742_PHYSCSR_AUTONEGO_DONE) {
        if(msg.reg_val & LAN8742_PHYSCSR_FULL_DUPLEX)
          CL_REG_SET(link, ETH_PHY_DUPLEX_FULL);
        if(msg.reg_val & LAN8742_PHYSCSR_100BTX)
          CL_REG_SET(link, ETH_PHY_SPEED_100M);
        else
          CL_REG_SET(link, ETH_PHY_SPEED_10M);
      }
    }
    else {
      if(msg.reg_val & LAN8742_BCR_DUPLEX_MODE)
        CL_REG_SET(link, ETH_PHY_DUPLEX_FULL);
      if(msg.reg_val & LAN8742_BCR_SPEED_SELECT)
        CL_REG_SET(link, ETH_PHY_SPEED_100M);
      else
        CL_REG_SET(link, ETH_PHY_SPEED_10M);
    }
  } while (0);
  
  lan8742_var->force_event = lan8742_var->force_event ||
    (lan8742_var->state.link_up ^ link_up) ||
    (lan8742_var->state.link ^ link) ||
    (lan8742_var->state.mode ^ mode) ||
    (lan8742_var->state.fault ^ fault);
  lan8742_var->state.link_up = link_up;
  lan8742_var->state.link = link;
  lan8742_var->state.mode = mode;
  lan8742_var->state.fault = fault;
}

static uint8_t _lan8742_update_cnf(hdl_lan8742_t *lan8742) {
  hdl_lan8742_var_t *lan8742_var = (hdl_lan8742_var_t *)lan8742->obj_var;
  uint16_t bcr_mask = LAN8742_BCR_SPEED_SELECT | LAN8742_BCR_DUPLEX_MODE | LAN8742_BCR_POWER_DOWN | 
    LAN8742_BCR_ISOLATE | LAN8742_BCR_LOOPBACK | LAN8742_BCR_AUTONEGO_EN;
  uint16_t bcr_val = 0;
  uint16_t scsir_mask = LAN8742_SCSIR_CHANNEL_SELECT | LAN8742_SCSIR_AUTO_MDIX_ENABLE;
  uint16_t scsir_val = 0;
  if(lan8742_var->state.mode & ETH_PHY_MODE_POWER_DOWN) bcr_val |= LAN8742_BCR_POWER_DOWN;
  if(lan8742_var->state.mode & ETH_PHY_MODE_ISOLATE) bcr_val |= LAN8742_BCR_ISOLATE;
  if(lan8742_var->state.mode & ETH_PHY_MODE_LOOPBACK) bcr_val |= LAN8742_BCR_LOOPBACK;
  if(lan8742_var->state.mode & ETH_PHY_MODE_AUTONEGO) bcr_val |= LAN8742_BCR_AUTONEGO_EN;
  if(lan8742_var->state.link & ETH_PHY_SPEED_100M) bcr_val |= LAN8742_BCR_SPEED_SELECT;
  if(lan8742_var->state.link & ETH_PHY_DUPLEX_FULL) bcr_val |= LAN8742_BCR_DUPLEX_MODE;
  if(lan8742_var->state.link & ETH_PHY_CROSSOVER) scsir_val |= LAN8742_SCSIR_CHANNEL_SELECT;
  if(lan8742_var->state.mode & ETH_PHY_MODE_AUTO_MDIX) scsir_val |= LAN8742_SCSIR_AUTO_MDIX_ENABLE;
  return _lan8742_modify_reg(lan8742, LAN8742_SCSIR, scsir_mask, scsir_val) &&
    _lan8742_modify_reg(lan8742, LAN8742_BCR, bcr_mask, bcr_val);
}

static uint8_t _lan8742_worker(coroutine_t *this, uint8_t cancel, void *context) {
  (void)this;
  hdl_lan8742_t *lan8742 = (hdl_lan8742_t *)context;
  hdl_lan8742_var_t *lan8742_var = (hdl_lan8742_var_t *)lan8742->obj_var;
  hdl_time_counter_t *timer = (hdl_time_counter_t *)lan8742->dependencies[2];

  //todo int pin
  //hdl_mdio_message_t msg = {
  //  .dev_addr = lan8742_var->state.addr,
  //  .reg_addr = LAN8742_ISFR,
  //};
  //if(hdl_mdio_read(mdio, &msg)) {
  //  /* todo process it */
  //}
  uint8_t force_sync = HDL_FALSE;
  if(lan8742_var->state.config) {
    lan8742_var->state.config = !_lan8742_update_cnf(lan8742);
    force_sync = !lan8742_var->state.config;
  }
  uint32_t now = hdl_time_counter_get(timer);
  if(force_sync || CL_TIME_ELAPSED(lan8742_var->timer, lan8742->config->sync_state_delay, now)) {
    lan8742_var->timer = now;
    _lan8742_sync_state(lan8742);
  }
  if(lan8742_var->force_event) {
    hdl_event_raise(&lan8742_var->event, lan8742, &lan8742_var->state);
    lan8742_var->force_event = HDL_FALSE;
  }
  return cancel;
}

static uint8_t _hdl_eth_phy_set_link(const void *desc, eth_phy_link_t link) {
  hdl_lan8742_t *lan8742 = (hdl_lan8742_t *)desc;
  hdl_lan8742_var_t *lan8742_var = (hdl_lan8742_var_t *)lan8742->obj_var;
  uint8_t mask = ETH_PHY_SPEED_10M | ETH_PHY_SPEED_100M | ETH_PHY_DUPLEX | ETH_PHY_CROSSOVER;
  if(link & ~mask) return HDL_FALSE;
    if((lan8742_var->state.link & mask) ^ link) {
    lan8742_var->state.link &= ~mask;
    lan8742_var->state.link |= link;
    lan8742_var->state.config = HDL_TRUE;
  }
  return HDL_TRUE;
}

static uint8_t _hdl_eth_phy_set_mode(const void *desc, eth_phy_mode_t mode) {
  hdl_lan8742_t *lan8742 = (hdl_lan8742_t *)desc;
  hdl_lan8742_var_t *lan8742_var = (hdl_lan8742_var_t *)lan8742->obj_var;
  uint8_t mask = ETH_PHY_MODE_POWER_DOWN | ETH_PHY_MODE_ISOLATE | ETH_PHY_MODE_LOOPBACK |
    ETH_PHY_MODE_AUTO_MDIX | ETH_PHY_MODE_AUTONEGO;
  if(mode & ~mask) return HDL_FALSE;
  if((lan8742_var->state.mode & mask) ^ mode) {
    lan8742_var->state.mode &= ~mask;
    lan8742_var->state.mode |= mode;
    lan8742_var->state.config = HDL_TRUE;
  }
  return HDL_TRUE;
}

static hdl_module_state_t _hdl_lan8742(const void *desc, uint8_t enable) {
  hdl_lan8742_t *lan8742 = (hdl_lan8742_t *)desc;
  hdl_lan8742_var_t *lan8742_var = (hdl_lan8742_var_t *)lan8742->obj_var;
  if(enable) {
    lan8742_var->timer = hdl_time_counter_get(lan8742->dependencies[2]);
    lan8742_var->state.fault = HDL_TRUE;
    lan8742_var->state.addr = lan8742->config->dev_addr;
    if(lan8742->config->dev_addr == 255)
      lan8742_var->state.addr = detect_phy_addr(lan8742);
    if(lan8742_var->state.addr > LAN8742_MAX_DEV_ADDR) 
      return HDL_MODULE_FAULT;

    hdl_mdio_t *mdio = (hdl_mdio_t *)lan8742->dependencies[0];
    hdl_mdio_message_t msg = {
      .dev_addr = lan8742_var->state.addr,
      .reg_addr = LAN8742_PHYI1R
    };
    if(!hdl_mdio_read(mdio, &msg)) return HDL_MODULE_FAULT;
    lan8742_var->state.id = ((uint32_t)msg.reg_val) << 16;
    msg.reg_addr = LAN8742_PHYI2R;
    if(!hdl_mdio_read(mdio, &msg)) return HDL_MODULE_FAULT;
    lan8742_var->state.id |= ((uint32_t)msg.reg_val);

    uint16_t mask = LAN8742_WOL_IT | LAN8742_AUTONEGO_COMPLETE_IT | LAN8742_LINK_DOWN_IT | LAN8742_REMOTE_FAULT_IT |
      LAN8742_ENERGYON_IT | LAN8742_AUTONEGO_LP_ACK_IT | LAN8742_PARALLEL_DETECTION_FAULT_IT | LAN8742_AUTONEGO_PAGE_RECEIVED_IT;
    uint16_t it = LAN8742_WOL_IT | LAN8742_AUTONEGO_COMPLETE_IT | LAN8742_LINK_DOWN_IT | LAN8742_REMOTE_FAULT_IT;
    /* LAN8742_ENERGYON_IT | LAN8742_AUTONEGO_LP_ACK_IT | LAN8742_PARALLEL_DETECTION_FAULT_IT | LAN8742_AUTONEGO_PAGE_RECEIVED_IT */

    lan8742_var->state.fault = 
      !_lan8742_modify_reg(lan8742, LAN8742_IMR, mask, it) ||
      !_hdl_eth_phy_set_link(lan8742, lan8742->config->link) ||
      !_hdl_eth_phy_set_mode(lan8742, lan8742->config->mode) ||
      !_lan8742_update_cnf(lan8742);
    if(lan8742_var->state.fault) 
      return HDL_MODULE_FAULT;

    _lan8742_sync_state(lan8742);
    coroutine_add(&lan8742_var->worker, &_lan8742_worker, lan8742);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&lan8742_var->worker);
  return HDL_MODULE_UNLOADED;
}

static void void_hdl_eth_phy_subscribe(const void *desc, hdl_delegate_t *delegate) {
  if(delegate) {
    hdl_lan8742_t *lan8742 = (hdl_lan8742_t *)desc;
    hdl_lan8742_var_t *lan8742_var = (hdl_lan8742_var_t *)lan8742->obj_var;
    hdl_event_subscribe(&lan8742_var->event, delegate);
    lan8742_var->force_event = HDL_TRUE;
  }
}

const hdl_eth_phy_iface_t hdl_lan8742_iface = {
  .init = &_hdl_lan8742,
  .set_link = &_hdl_eth_phy_set_link,
  .set_mode = &_hdl_eth_phy_set_mode,
  .subscribe = &void_hdl_eth_phy_subscribe
};
