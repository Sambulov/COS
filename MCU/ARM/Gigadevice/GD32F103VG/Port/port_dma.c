#include "hdl_portable.h"

#define dma_ch_no(ch)     ((dma_channel_enum)ch->module.reg)

/* DMA initialization and deinitialization */
hdl_module_state_t hdl_dma(void *desc, uint8_t enable) {
  hdl_dma_t *dma = (hdl_dma_t *)desc;
  rcu_periph_enum rcu;
  switch ((uint32_t)dma->module.reg) {
    case DMA0:
      rcu = RCU_DMA0;
      break;
    case DMA1:
      rcu = RCU_DMA1;
      break;
    default:
      return HDL_MODULE_INIT_FAILED;
  }
  if(enable) {
    rcu_periph_clock_enable(rcu);
    return HDL_MODULE_INIT_OK;
  }
  rcu_periph_clock_disable(rcu);
  return HDL_MODULE_DEINIT_OK;
}

static hdl_dma_t *_hdl_get_dma(hdl_dma_channel_t *channel) {
  if((channel != NULL) && (channel->module.dependencies != NULL) &&
     (channel->module.dependencies[0] != NULL)) {
    return (hdl_dma_t *)channel->module.dependencies[0];
  }
  return NULL;
}

hdl_dma_status_e hdl_dma_status(hdl_dma_channel_t *channel) {
  hdl_dma_status_e rez = HDL_DMA_STATUS_NONE;
  hdl_dma_t *dma = _hdl_get_dma(channel);
  if(dma != NULL) {
    if((DMA_CHCTL((uint32_t)dma->module.reg, dma_ch_no(channel)) & (1 << 0)) == SET)
      rez |= HDL_DMA_STATUS_CHANNEL_ENABLE;
    if(dma_flag_get((uint32_t)dma->module.reg, dma_ch_no(channel), DMA_FLAG_HTF) == SET)
      rez |= HDL_DMA_STATUS_HALF_TRANSFER;
    if(dma_flag_get((uint32_t)dma->module.reg, dma_ch_no(channel), DMA_FLAG_FTF) == SET)
      rez |= HDL_DMA_STATUS_FULL_TRANSFER;
    if(dma_flag_get((uint32_t)dma->module.reg, dma_ch_no(channel), DMA_FLAG_ERR) == RESET)
      rez |= HDL_DMA_STATUS_ERROR_TRANSFER;
  }
  return rez;
}

/* Get DMA transfer counter value */
uint32_t hdl_dma_counter(hdl_dma_channel_t *channel) {
  hdl_dma_t *dma = _hdl_get_dma(channel);
  if(dma != NULL)
    return dma_transfer_number_get((uint32_t)dma->module.reg, dma_ch_no(channel));
  return 0;
}

hdl_module_state_t hdl_dma_ch(void *desc, uint8_t enable) {
  hdl_dma_channel_t *channel = (hdl_dma_channel_t *)desc;
  hdl_dma_t *dma = _hdl_get_dma(channel);
  if(dma != NULL) {
    if(enable) {
      //dma_channel_enable(dma_ch_no(channel));
      return HDL_MODULE_INIT_OK;
    }
    dma_deinit((uint32_t)dma->module.reg, dma_ch_no(channel));
    dma_channel_disable((uint32_t)dma->module.reg, dma_ch_no(channel));
  }
  return HDL_MODULE_DEINIT_OK;
}

uint8_t hdl_dma_run(hdl_dma_channel_t *channel, uint32_t periph_addr, uint32_t memory_addr, uint32_t amount) {
  hdl_dma_t *dma = _hdl_get_dma(channel);
  if(dma != NULL) {
    dma_deinit((uint32_t)dma->module.reg, dma_ch_no(channel));
    dma_periph_address_config((uint32_t)dma->module.reg, dma_ch_no(channel), periph_addr);
    dma_memory_address_config((uint32_t)dma->module.reg, dma_ch_no(channel), memory_addr);
    dma_transfer_number_config((uint32_t)dma->module.reg, dma_ch_no(channel), amount);
    dma_priority_config((uint32_t)dma->module.reg, dma_ch_no(channel), channel->priority);
    dma_memory_width_config((uint32_t)dma->module.reg, dma_ch_no(channel), CHCTL_MWIDTH(channel->memory_width));
    dma_periph_width_config((uint32_t)dma->module.reg, dma_ch_no(channel), CHCTL_PWIDTH(channel->periph_width));
    if(channel->memory_inc)
        dma_memory_increase_enable((uint32_t)dma->module.reg, dma_ch_no(channel));
    if(channel->periph_inc)
        dma_periph_increase_enable((uint32_t)dma->module.reg, dma_ch_no(channel));
    if(channel->direction == HDL_DMA_DIRECTION_P2M)
        dma_transfer_direction_config((uint32_t)dma->module.reg, dma_ch_no(channel), DMA_PERIPHERAL_TO_MEMORY);
    else if(channel->direction == HDL_DMA_DIRECTION_M2P)
        dma_transfer_direction_config((uint32_t)dma->module.reg, dma_ch_no(channel), DMA_MEMORY_TO_PERIPHERAL);
    else if(channel->direction == HDL_DMA_DIRECTION_M2M)
    {
        /* In this case source address it is peripheral address, destiantion address it is memory address */
        dma_transfer_direction_config((uint32_t)dma->module.reg, dma_ch_no(channel), DMA_PERIPHERAL_TO_MEMORY);
        dma_memory_to_memory_enable((uint32_t)dma->module.reg, dma_ch_no(channel));
    }
    if (channel->mode == HDL_DMA_MODE_SINGLE)
        dma_circulation_disable((uint32_t)dma->module.reg, dma_ch_no(channel));
    else
        dma_circulation_enable((uint32_t)dma->module.reg, dma_ch_no(channel));

    dma_channel_enable((uint32_t)dma->module.reg, dma_ch_no(channel));
    return HDL_TRUE;
  }
  return HDL_FALSE;
}
