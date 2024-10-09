#include "hdl_portable.h"

#define dma_ch_no(ch)     ((dma_channel_enum)ch->module.reg)

uint8_t hdl_dma_run(hdl_dma_channel_t *channel, uint32_t periph_addr, uint32_t memory_addr, uint32_t amount) {
  hdl_dma_t *dma = (hdl_dma_t *)channel->module.dependencies[0];
  if(dma != NULL) {
    dma_deinit((uint32_t)dma->module.reg, dma_ch_no(channel));
    dma_channel_subperipheral_select((uint32_t)dma->module.reg, dma_ch_no(channel), channel->config->channel_periphery);
    dma_periph_address_config((uint32_t)dma->module.reg, dma_ch_no(channel), periph_addr);
    dma_memory_address_config((uint32_t)dma->module.reg, dma_ch_no(channel), DMA_MEMORY_0, memory_addr);
    dma_transfer_number_config((uint32_t)dma->module.reg, dma_ch_no(channel), amount);
    dma_priority_config((uint32_t)dma->module.reg, dma_ch_no(channel), channel->config->priority);
    dma_memory_width_config((uint32_t)dma->module.reg, dma_ch_no(channel), channel->config->memory_width);
    dma_periph_width_config((uint32_t)dma->module.reg, dma_ch_no(channel), channel->config->periph_width);
    if(channel->config->memory_inc) dma_memory_address_generation_config((uint32_t)dma->module.reg, dma_ch_no(channel), DMA_MEMORY_INCREASE_ENABLE);
    if(channel->config->periph_inc) dma_peripheral_address_generation_config((uint32_t)dma->module.reg, dma_ch_no(channel), DMA_PERIPH_INCREASE_ENABLE);
    dma_transfer_direction_config((uint32_t)dma->module.reg, dma_ch_no(channel), channel->config->direction);
    if (channel->config) dma_circulation_enable((uint32_t)dma->module.reg, dma_ch_no(channel));
    dma_channel_enable((uint32_t)dma->module.reg, dma_ch_no(channel));
    return HDL_TRUE;
  }
  return HDL_FALSE;
}
