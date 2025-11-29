#ifndef HDL_DMA_H_
#define HDL_DMA_H_

typedef enum {
  HDL_DMA_P2M = 0,
  HDL_DMA_M2P = 1,
  HDL_DMA_M2M = 2
} hdl_dma_direction_t;

typedef enum {
  HDL_DMA_SATE_RUN  = 0x01,
  HDL_DMA_SATE_ERR  = 0x80,
} hdl_dma_status_t;

typedef struct {
  uint32_t transferred;
  hdl_dma_direction_t dir;
  hdl_dma_status_t status;
} hdl_dma_state_t;

typedef uint8_t (*hdl_dma_run_t)(void const *desc, hdl_dma_direction_t dir, uint32_t periph_addr, uint32_t mem_addr, uint32_t amount);
typedef uint8_t (*hdl_dma_stop_t)(void const *desc);
typedef uint8_t (*hdl_dma_get_state_t)(void const *desc, hdl_dma_state_t *out_state);

typedef struct {
  hdl_module_initializer_t init;
  hdl_dma_run_t run;
  hdl_dma_stop_t stop;
  hdl_dma_get_state_t get_state;
} hdl_dma_channel_iface_t;

hdl_module_new_t(hdl_dma_t, 0, void*, hdl_module_base_iface_t);
hdl_module_new_t(hdl_dma_channel_t, 0, void*, hdl_dma_channel_iface_t);

__STATIC_INLINE uint8_t hdl_dma_channel_run_m2m(void const *desc, void *periph_addr, void *mem_addr, uint32_t amount) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_dma_channel_iface_t *)((hdl_module_base_t *)desc)->iface)->run(desc, HDL_DMA_M2M, (uint32_t)periph_addr, (uint32_t)mem_addr, amount);
}

__STATIC_INLINE uint8_t hdl_dma_channel_run_p2m(void const *desc, void *periph_addr, void *mem_addr, uint32_t amount) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_dma_channel_iface_t *)((hdl_module_base_t *)desc)->iface)->run(desc, HDL_DMA_P2M, (uint32_t)periph_addr, (uint32_t)mem_addr, amount);
}

__STATIC_INLINE uint8_t hdl_dma_channel_run_m2p(void const *desc, void *periph_addr, void *mem_addr, uint32_t amount) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_dma_channel_iface_t *)((hdl_module_base_t *)desc)->iface)->run(desc, HDL_DMA_M2P, (uint32_t)periph_addr, (uint32_t)mem_addr, amount);
}

__STATIC_INLINE uint8_t hdl_dma_channel_stop(void const *desc) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_dma_channel_iface_t *)((hdl_module_base_t *)desc)->iface)->stop(desc);
}

__STATIC_INLINE uint8_t hdl_dma_channel_state(void const *desc, hdl_dma_state_t *out_state) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_dma_channel_iface_t *)((hdl_module_base_t *)desc)->iface)->get_state(desc, out_state);
}

__STATIC_INLINE uint32_t hdl_dma_channel_get_counter(void const *desc) {
  MODULE_ASSERT(desc, 0);
  hdl_dma_state_t out_state;
  if(hdl_dma_channel_state(desc, &out_state))
    return out_state.transferred;
  return 0;
}

__STATIC_INLINE uint8_t hdl_dma_channel_error(void const *desc) {
  MODULE_ASSERT(desc, HDL_TRUE);
  hdl_dma_state_t out_state;
  if(hdl_dma_channel_state(desc, &out_state))
    return (out_state.status & HDL_DMA_SATE_ERR) != 0;
  return HDL_TRUE;
}

#endif