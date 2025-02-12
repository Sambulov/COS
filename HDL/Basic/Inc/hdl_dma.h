#ifndef HDL_DMA_H_
#define HDL_DMA_H_

hdl_module_new_t(hdl_dma_t, 0, void *, hdl_module_base_iface_t);

typedef uint8_t (*hdl_dma_run_t)(void const *desc, uint32_t periph_addr, uint32_t memory_addr, uint32_t amount);
typedef uint8_t (*hdl_dma_stop_t)(void const *desc);
typedef uint32_t (*hdl_dma_get_counter_t)(void const *desc);

typedef struct {
  hdl_module_initializer_t init;
  hdl_dma_run_t run;
  hdl_dma_stop_t stop;
  hdl_dma_get_counter_t get_counter;
} hdl_dma_channel_iface_t;

hdl_module_new_t(hdl_dma_channel_t, 0, void, hdl_dma_channel_iface_t);

__STATIC_INLINE uint8_t hdl_dma_channel_run(void const *desc, uint32_t periph_addr, uint32_t memory_addr, uint32_t amount) {
  return ((hdl_dma_channel_t *)desc)->iface->run(desc, periph_addr, memory_addr, amount);
}

__STATIC_INLINE uint8_t hdl_dma_channel_stop(void const *desc) {
  return ((hdl_dma_channel_t *)desc)->iface->stop(desc);
}

__STATIC_INLINE uint32_t hdl_dma_channel_get_counter(void const *desc) {
  return ((hdl_dma_channel_t *)desc)->iface->get_counter(desc);
}

#endif