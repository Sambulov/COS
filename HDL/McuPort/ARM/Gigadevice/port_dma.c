#include "hdl_iface.h"

static hdl_module_state_t _hdl_dma(const void *desc, uint8_t enable) {
  hdl_dma_t *dma = (hdl_dma_t *)desc;
  hdl_dma_config_t *cnf = (hdl_dma_config_t *)dma->config;
  if(enable) {
    rcu_periph_clock_enable(cnf->rcu);
    return HDL_MODULE_ACTIVE;
  }
  rcu_periph_clock_disable(cnf->rcu);
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_dma_ch(const void *desc, uint8_t enable) {
  hdl_dma_channel_t *channel = ((hdl_dma_channel_t *)desc);
  hdl_dma_t *dma = (hdl_dma_t *)channel->dependencies[0];
  hdl_dma_channel_config_t *ch_cnf = (hdl_dma_channel_config_t *)channel->config;
  hdl_dma_config_t *dma_cnf = (hdl_dma_config_t *)dma->config;
  if(dma != NULL) {
    if(enable) {
      return HDL_MODULE_ACTIVE;
    }
    dma_deinit(dma_cnf->phy, ch_cnf->ch_no);
    dma_channel_disable(dma_cnf->phy, ch_cnf->ch_no);
  }
  return HDL_MODULE_UNLOADED;
}

static uint32_t _hdl_dma_get_counter(const void *desc) {
  hdl_dma_channel_t *channel = ((hdl_dma_channel_t *)desc);
  hdl_dma_t *dma = (hdl_dma_t *)channel->dependencies[0];
  hdl_dma_channel_config_t *ch_cnf = (hdl_dma_channel_config_t *)channel->config;
  hdl_dma_config_t *dma_cnf = (hdl_dma_config_t *)dma->config;
  return dma_transfer_number_get(dma_cnf->phy, ch_cnf->ch_no);
}

static uint8_t _hdl_dma_stop(const void *desc) {
  hdl_dma_channel_t *channel = ((hdl_dma_channel_t *)desc);
  hdl_dma_t *dma = (hdl_dma_t *)channel->dependencies[0];
  hdl_dma_channel_config_t *ch_cnf = (hdl_dma_channel_config_t *)channel->config;
  hdl_dma_config_t *dma_cnf = (hdl_dma_config_t *)dma->config;
  dma_deinit(dma_cnf->phy, ch_cnf->ch_no);
  return HDL_TRUE;
}

const hdl_module_base_iface_t dma_iface = {
  .init = &_hdl_dma
};

extern uint8_t __hdl_dma_run(const void *desc, uint32_t periph_addr, uint32_t memory_addr, uint32_t amount);

const hdl_dma_channel_iface_t dma_channel_iface = {
  .init = &_hdl_dma_ch,
  .get_counter = &_hdl_dma_get_counter,
  .run = &__hdl_dma_run,
  .stop = &_hdl_dma_stop
};
