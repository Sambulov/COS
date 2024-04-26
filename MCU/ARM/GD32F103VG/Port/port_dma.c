#include "hdl_portable.h"

typedef enum{
    GD_DMA_CONVERSION_PROCEEDING,
    GD_DMA_CONVERSION_COMPLETE,
} gd_dma_coversion_status_e;

static const uint32_t gd_dma_memory_width_array[] = {DMA_MEMORY_WIDTH_8BIT, DMA_MEMORY_WIDTH_16BIT, DMA_MEMORY_WIDTH_32BIT};
static const uint32_t gd_dma_peripheral_width_array[] = {DMA_PERIPHERAL_WIDTH_8BIT, DMA_PERIPHERAL_WIDTH_16BIT, DMA_PERIPHERAL_WIDTH_32BIT};

/* DMA initialization and deinitialization */
hdl_module_state_t hdl_dma(void *desc, uint8_t enable){
  hdl_dma_t *hdl_dma = (hdl_dma_t *)desc;
  rcu_periph_enum rcu;
  switch ((uint32_t)hdl_dma->module.reg) {
    case DMA0:
      rcu = RCU_DMA0;
      break;
    case DMA1:
      rcu = RCU_DMA1;
      break;
    default:
      return HDL_MODULE_INIT_FAILED;
  }
  if (enable){
      rcu_periph_clock_enable(rcu);
      return HDL_MODULE_INIT_OK;
  }
  else{
      rcu_periph_clock_disable(rcu);
      return HDL_MODULE_DEINIT_OK;
  }
}

/* Config DMA channel */
uint8_t hdl_dma_config(hdl_dma_t *dma, hdl_dma_config_t *config, hdl_dma_channel_t channel) {
  uint32_t dma_periph = (uint32_t)dma->module.reg;
  dma_deinit(dma_periph, channel);
  dma_periph_address_config(dma_periph, channel, config->periph_addr);
  dma_memory_address_config(dma_periph, channel, config->memory_addr);
  dma_transfer_number_config(dma_periph, channel, config->amount);
  dma_priority_config(dma_periph, channel, config->priority);
  dma_memory_width_config(dma_periph, channel, gd_dma_memory_width_array[config->memory_width]);
  dma_periph_width_config(dma_periph, channel, gd_dma_peripheral_width_array[config->periph_width]);
  if(config->memory_inc)
      dma_memory_increase_enable(dma_periph, channel);
  if(config->periph_inc)
      dma_periph_increase_enable(dma_periph, channel);
  if(config->direction == HDL_DMA_DIRECTION_P2M)
      dma_transfer_direction_config(dma_periph, channel, DMA_PERIPHERAL_TO_MEMORY);
  else if(config->direction == HDL_DMA_DIRECTION_M2P)
      dma_transfer_direction_config(dma_periph, channel, DMA_MEMORY_TO_PERIPHERAL);
  else if(config->direction == HDL_DMA_DIRECTION_M2M)
  {
      /* In this case source address it is peripheral address, destiantion address it is memory address */
      dma_transfer_direction_config(dma_periph, channel, DMA_PERIPHERAL_TO_MEMORY);
      dma_memory_to_memory_enable(dma_periph, channel);
  }
  if (config->dma_mode == HDL_DMA_MODE_SINGLE)
      dma_circulation_disable(dma_periph, channel);
  else
      dma_circulation_enable(dma_periph, channel);
  return HDL_TRUE;
}

/* Enable DMA channel */
uint8_t hdl_dma_channel_enable(hdl_dma_t *dma, hdl_dma_channel_t channel){
  uint32_t dma_periph = (uint32_t)dma->module.reg;
  dma_channel_enable(dma_periph, channel);
  return HDL_TRUE;
}

/* Disable DMA channel */
uint8_t hdl_dma_channel_disable(hdl_dma_t *dma, hdl_dma_channel_t channel){
  uint32_t dma_periph = (uint32_t)dma->module.reg;
  dma_channel_disable(dma_periph, channel);
  return HDL_TRUE;
}


/* Get DMA channel status */
hdl_dma_status_e hdl_dma_status(hdl_dma_t *dma, hdl_dma_channel_t channel){
  uint32_t dma_periph = (uint32_t)dma->module.reg;
    hdl_dma_status_e rez = HDL_DMA_STATUS_NONE;
    if((DMA_CHCTL(dma_periph, channel) & (1 << 0)) == SET)
        rez |= HDL_DMA_STATUS_CHANNEL_ENABLE;
    if(dma_flag_get(dma_periph, channel, DMA_FLAG_HTF) == SET)
        rez |= HDL_DMA_STATUS_HALF_TRANSFER;
    if(dma_flag_get(dma_periph, channel, DMA_FLAG_FTF) == SET)
        rez |= HDL_DMA_STATUS_FULL_TRANSFER;
    if(dma_flag_get(dma_periph, channel, DMA_FLAG_ERR) == RESET)
        rez |= HDL_DMA_STATUS_ERROR_TRANSFER;
    return rez;
}

/* Get DMA transfer counter value */
uint32_t hdl_dma_counter(hdl_dma_t *dma, hdl_dma_channel_t channel){
  uint32_t dma_periph = (uint32_t)dma->module.reg;
  return dma_transfer_number_get(dma_periph, channel);
}
