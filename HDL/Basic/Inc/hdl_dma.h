#ifndef HDL_DMA_H_
#define HDL_DMA_H_

#include "port_dma.h"

typedef struct {
  hdl_module_t module;
  const hdl_dma_config_t *config;
} hdl_dma_t;

/* Depends on dma module */
typedef struct {
  hdl_module_t module;
  const hdl_dma_channel_config_t* config;
} hdl_dma_channel_t;

// typedef enum {
//     HDL_DMA_STATUS_NONE = (0),
//     HDL_DMA_STATUS_FULL_TRANSFER = (1),
//     HDL_DMA_STATUS_HALF_TRANSFER = (2),
//     HDL_DMA_STATUS_ERROR_TRANSFER = (4),
//     HDL_DMA_STATUS_CHANNEL_ENABLE = (8)
// } hdl_dma_status_e;

hdl_module_state_t hdl_dma(void *desc, uint8_t enable);
hdl_module_state_t hdl_dma_ch(void *desc, uint8_t enable);

uint8_t hdl_dma_run(hdl_dma_channel_t *channel, uint32_t periph_addr, uint32_t memory_addr, uint32_t amount);
uint8_t hdl_dma_stop(hdl_dma_channel_t *channel);
// hdl_dma_status_e hdl_dma_status(hdl_dma_channel_t *channel);
uint32_t hdl_dma_get_counter(hdl_dma_channel_t *channel);

#endif