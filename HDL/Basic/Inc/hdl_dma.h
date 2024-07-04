#ifndef HDL_DMA_H_
#define HDL_DMA_H_

#define DMA_PRIVATE_SIZE  (4)

typedef enum {
    HDL_DMA_DIRECTION_P2M,  /* DMA direction peripheral to memory */
    HDL_DMA_DIRECTION_M2P,  /* DMA direction memory to peripheral */
    HDL_DMA_DIRECTION_M2M,  /* DMA direction memory to memory */ 
} hdl_dma_direction_e;

typedef enum {
    HDL_DMA_MODE_SINGLE, /* DMA single conversion mode, each next conversion have to started by event */
    HDL_DMA_MODE_CIRCULAR /* DMA circular mode, each next conversion will be started automatically */
} hdl_dma_mode_e;

typedef enum {
    HDL_DMA_INCREMENT_OFF,  /* Automatically address increment is off */
    HDL_DMA_INCREMENT_ON,   /* Automatically address increment is on */
} hdl_dma_increment_e;

typedef enum {
    HDL_DMA_SIZE_OF_MEMORY_8_BIT  = 0,
    HDL_DMA_SIZE_OF_MEMORY_16_BIT = 1,
    HDL_DMA_SIZE_OF_MEMORY_32_BIT = 2,
} hdl_dma_size_of_memory_e;

typedef dma_channel_enum hdl_dma_channel_number_t;

typedef struct {
  hdl_module_t module;
  PRIVATE(hdl, DMA_PRIVATE_SIZE);
} hdl_dma_t;

#include "port_dma.h"

typedef enum {
    HDL_DMA_STATUS_NONE = (0),
    HDL_DMA_STATUS_FULL_TRANSFER = (1),
    HDL_DMA_STATUS_HALF_TRANSFER = (2),
    HDL_DMA_STATUS_ERROR_TRANSFER = (4),
    HDL_DMA_STATUS_CHANNEL_ENABLE = (8)
} hdl_dma_status_e;

hdl_module_state_t hdl_dma(void *desc, uint8_t enable);
hdl_module_state_t hdl_dma_ch(void *desc, uint8_t enable);

uint8_t hdl_dma_run(hdl_dma_channel_t *channel, uint32_t periph_addr, uint32_t memory_addr, uint32_t amount);
uint8_t hdl_dma_stop(hdl_dma_channel_t *channel);
hdl_dma_status_e hdl_dma_status(hdl_dma_channel_t *channel);
uint32_t hdl_dma_get_counter(hdl_dma_channel_t *channel);

#if defined ( GD32E23X )

#endif

#endif