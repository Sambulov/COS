#include "hdl_portable.h"

hdl_module_state_t hdl_dma(void *desc, uint8_t enable) {
  return HDL_MODULE_INIT_FAILED;
}

uint8_t hdl_dma_config(hdl_dma_t *dma, hdl_dma_config_t *config, hdl_dma_channel_t channel) {
  return HDL_FALSE;
}

uint8_t hdl_dma_sw_triger(hdl_dma_t *dma, hdl_dma_channel_t channel) {
  return HDL_FALSE;
}

hdl_dma_status_e hdl_dma_status(hdl_dma_t *dma, hdl_dma_channel_t channel){
    return HDL_DMA_STATUS_NONE;
}

uint32_t hdl_dma_counter(hdl_dma_t *dma, hdl_dma_channel_t channel){
    return 0;
}
