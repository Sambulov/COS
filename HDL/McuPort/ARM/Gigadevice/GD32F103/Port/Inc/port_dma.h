#ifndef PORT_DMA_H_
#define PORT_DMA_H_

typedef struct {
  rcu_periph_enum rcu;
} hdl_dma_config_t;

typedef struct {
  uint32_t periph_width;          /* DMA_MEMORY_WIDTH_8BIT, DMA_MEMORY_WIDTH_16BIT, DMA_MEMORY_WIDTH_32BIT */
  uint32_t memory_width;          /* DMA_MEMORY_WIDTH_8BIT, DMA_MEMORY_WIDTH_16BIT, DMA_MEMORY_WIDTH_32BIT */
  uint32_t priority;              /* DMA_PRIORITY_LOW DMA_PRIORITY_MEDIUM DMA_PRIORITY_HIGH DMA_PRIORITY_ULTRA_HIGH */
  uint8_t direction;              /* DMA_PERIPHERAL_TO_MEMORY, DMA_MEMORY_TO_PERIPHERAL, use DMA_PERIPHERAL_TO_MEMORY for m2m */
  uint8_t periph_inc    : 1,
          memory_inc    : 1,
          m2m_direction : 1,
          circular      : 1;
} hdl_dma_channel_config_t;

#endif