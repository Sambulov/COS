#ifndef PORT_DMA_H_
#define PORT_DMA_H_

typedef enum {
    HDL_DMA_PRIORITY_LOW = (uint32_t)DMA_PRIORITY_LOW,
    HDL_DMA_PRIORITY_MEDIUM = (uint32_t)DMA_PRIORITY_MEDIUM,
    HDL_DMA_PRIORITY_HIGH = (uint32_t)DMA_PRIORITY_HIGH,
    HDL_DMA_PRIORITY_ULTRA_HIGH = (uint32_t)DMA_PRIORITY_ULTRA_HIGH,
} hdl_dma_priority_e;

typedef struct {
  hdl_module_t module;                   /* Depends on dma module*/
  hdl_dma_size_of_memory_e periph_width;
  hdl_dma_size_of_memory_e memory_width;
  hdl_dma_increment_e periph_inc;        /* HDL_DMA_INCREMENT_OFF, HDL_DMA_INCREMENT_ON */
  hdl_dma_increment_e memory_inc;        /* HDL_DMA_INCREMENT_OFF, HDL_DMA_INCREMENT_ON */
  hdl_dma_direction_e direction;         /* HDL_DMA_DIRECTION_P2M, HDL_DMA_DIRECTION_M2P, HDL_DMA_DIRECTION_M2M */
  hdl_dma_mode_e mode;                   /* HDL_DMA_MODE_SINGLE_CONVERSION, HDL_DMA_MODE_CIRCULAR_CONVERSION */
  hdl_dma_priority_e priority;           /* hdl_dma_priority_e */
} hdl_dma_channel_t;

#endif
