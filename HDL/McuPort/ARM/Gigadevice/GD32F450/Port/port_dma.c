#include "hdl_iface.h"

uint8_t __hdl_dma_run(hdl_dma_channel_mcu_t *channel, uint32_t periph_addr, uint32_t memory_addr, uint32_t amount) {
  hdl_dma_mcu_t *dma = (hdl_dma_mcu_t *)channel->dependencies[0];
  if(dma != NULL) {
    dma_deinit(dma->config->phy, channel->config->ch_no);
    dma_channel_subperipheral_select(dma->config->phy, channel->config->ch_no, channel->config->channel_periphery);
    dma_periph_address_config(dma->config->phy, channel->config->ch_no, periph_addr);
    dma_memory_address_config(dma->config->phy, channel->config->ch_no, DMA_MEMORY_0, memory_addr);
    dma_transfer_number_config(dma->config->phy, channel->config->ch_no, amount);
    dma_priority_config(dma->config->phy, channel->config->ch_no, channel->config->priority);
    dma_memory_width_config(dma->config->phy, channel->config->ch_no, channel->config->memory_width);
    dma_periph_width_config(dma->config->phy, channel->config->ch_no, channel->config->periph_width);
    if(channel->config->memory_inc) dma_memory_address_generation_config(dma->config->phy, channel->config->ch_no, DMA_MEMORY_INCREASE_ENABLE);
    if(channel->config->periph_inc) dma_peripheral_address_generation_config(dma->config->phy, channel->config->ch_no, DMA_PERIPH_INCREASE_ENABLE);
    dma_transfer_direction_config(dma->config->phy, channel->config->ch_no, channel->config->direction);
    if (channel->config) dma_circulation_enable(dma->config->phy, channel->config->ch_no);
    dma_channel_enable(dma->config->phy, channel->config->ch_no);
    return HDL_TRUE;
  }
  return HDL_FALSE;
}
