#include "hdl_portable.h"

#define dma_ch_no(ch)     ((dma_channel_enum)ch->module.reg)

/* DMA initialization and deinitialization */
hdl_module_state_t hdl_dma(void *desc, uint8_t enable) {
  hdl_dma_t *dma = (hdl_dma_t *)desc;
  if(enable) {
    rcu_periph_clock_enable(dma->config->rcu);
    return HDL_MODULE_INIT_OK;
  }
  rcu_periph_clock_disable(dma->config->rcu);
  return HDL_MODULE_DEINIT_OK;
}

// hdl_dma_status_e hdl_dma_status(hdl_dma_channel_t *channel) {
//   hdl_dma_status_e rez = HDL_DMA_STATUS_NONE;
//   hdl_dma_t *dma = (hdl_dma_t *)channel->module.dependencies[0];
//   if(dma != NULL) {
//     if((DMA_CHCTL((uint32_t)dma->module.reg, dma_ch_no(channel)) & (1 << 0)) == SET)
//       rez |= HDL_DMA_STATUS_CHANNEL_ENABLE;
//     if(dma_flag_get((uint32_t)dma->module.reg, dma_ch_no(channel), DMA_FLAG_HTF) == SET)
//       rez |= HDL_DMA_STATUS_HALF_TRANSFER;
//     if(dma_flag_get((uint32_t)dma->module.reg, dma_ch_no(channel), DMA_FLAG_FTF) == SET)
//       rez |= HDL_DMA_STATUS_FULL_TRANSFER;
//     if(dma_flag_get((uint32_t)dma->module.reg, dma_ch_no(channel), DMA_FLAG_ERR) == RESET)
//       rez |= HDL_DMA_STATUS_ERROR_TRANSFER;
//   }
//   return rez;
// }

/* Get DMA transfer counter value */
uint32_t hdl_dma_get_counter(hdl_dma_channel_t *channel) {
  hdl_dma_t *dma = (hdl_dma_t *)channel->module.dependencies[0];
  if(dma != NULL)
    return dma_transfer_number_get((uint32_t)dma->module.reg, dma_ch_no(channel));
  return 0;
}

hdl_module_state_t hdl_dma_ch(void *desc, uint8_t enable) {
  hdl_dma_channel_t *channel = (hdl_dma_channel_t *)desc;
  hdl_dma_t *dma = (hdl_dma_t *)channel->module.dependencies[0];
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

uint8_t hdl_dma_stop(hdl_dma_channel_t *channel) {
  hdl_dma_t *dma = (hdl_dma_t *)channel->module.dependencies[0];
  if(dma != NULL) {
    dma_deinit((uint32_t)dma->module.reg, dma_ch_no(channel));
  }
  return HDL_FALSE;
}
