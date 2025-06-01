#ifndef PORT_DMA_H_
#define PORT_DMA_H_

#include "hdl_dma.h"

typedef struct {
  rcu_periph_enum rcu;
  uint32_t phy;
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
  dma_channel_enum ch_no;
} hdl_dma_channel_config_t;

hdl_module_new_t(hdl_dma_mcu_t, 0, hdl_dma_config_t, hdl_module_base_iface_t);

/* Depends on 
  dma module 
*/
hdl_module_new_t(hdl_dma_channel_mcu_t, 0, hdl_dma_channel_config_t, hdl_dma_channel_iface_t);

extern const hdl_module_base_iface_t hdl_dma_iface;
extern const hdl_dma_channel_iface_t hdl_dma_channel_iface;

#endif