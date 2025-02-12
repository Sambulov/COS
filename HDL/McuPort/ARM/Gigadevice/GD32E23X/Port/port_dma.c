#include "hdl.h"

uint8_t __hdl_dma_run(const void *desc, uint32_t periph_addr, uint32_t memory_addr, uint32_t amount) {
  hdl_dma_channel_t *channel = ((hdl_dma_channel_t *)desc);
  hdl_dma_t *dma = (hdl_dma_t *)channel->dependencies[0];
  hdl_dma_channel_config_t *ch_cnf = (hdl_dma_channel_config_t *)channel->config;
  if(dma != NULL) {
    dma_deinit(0, ch_cnf->ch_no);
    dma_periph_address_config(ch_cnf->ch_no, periph_addr);
    dma_memory_address_config(ch_cnf->ch_no, memory_addr);
    dma_transfer_number_config(ch_cnf->ch_no, amount);
    dma_priority_config(ch_cnf->ch_no, ch_cnf->priority);
    dma_memory_width_config(ch_cnf->ch_no, ch_cnf->memory_width);
    dma_periph_width_config(ch_cnf->ch_no, ch_cnf->periph_width);
    if(ch_cnf->memory_inc) dma_memory_increase_enable( ch_cnf->ch_no);
    if(ch_cnf->periph_inc) dma_periph_increase_enable( ch_cnf->ch_no);
    dma_transfer_direction_config(ch_cnf->ch_no, ch_cnf->direction);
    if(ch_cnf->m2m_direction) dma_memory_to_memory_enable( ch_cnf->ch_no);
    if (ch_cnf->circular) dma_circulation_enable(ch_cnf->ch_no);
    dma_channel_enable(ch_cnf->ch_no);
    return HDL_TRUE;
  }
  return HDL_FALSE;
}
