#include "hdl_iface.h"

typedef struct {
  __IO uint32_t ISR;   /*!< DMA interrupt status register */
  __IO uint32_t Reserved0;
  __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
} dma_stream_registers_t;

static const uint8_t dma_stream_bitshift[8U] = {0U, 6U, 16U, 22U, 0U, 6U, 16U, 22U};

static hdl_module_state_t _hdl_dma(const void *desc, uint8_t enable) {
  hdl_dma_t *dma = (hdl_dma_t *)desc;
  if(dma == NULL) return HDL_MODULE_FAULT;
  hdl_dma_config_t *cnf = (hdl_dma_config_t *)dma->config;
  if(cnf->phy == (uint32_t)DMA1) CL_REG_SET(RCC->AHB1RSTR, RCC_AHB1RSTR_DMA1RST);
  else if(cnf->phy == (uint32_t)DMA2) CL_REG_SET(RCC->AHB1RSTR, RCC_AHB1RSTR_DMA2RST);
  else return HDL_MODULE_FAULT;
  CL_REG_CLEAR(RCC->AHB1RSTR, RCC_AHB1RSTR_DMA1RST | RCC_AHB1RSTR_DMA2RST);
  if(enable) {
    CL_REG_SET(RCC->AHB1ENR, cnf->rcc);
    return HDL_MODULE_ACTIVE;
  }
  CL_REG_CLEAR(RCC->AHB1ENR, cnf->rcc);
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_dma_ch(const void *desc, uint8_t enable) {
  hdl_dma_channel_mcu_t *channel = ((hdl_dma_channel_mcu_t *)desc);
  if(channel == NULL) return HDL_MODULE_FAULT;
  if((channel->dependencies == NULL) || (channel->dependencies[0] == NULL)) return HDL_MODULE_FAULT;
  hdl_dma_t *dma = (hdl_dma_t *)channel->dependencies[0];
  hdl_dma_channel_config_t *ch_cnf = (hdl_dma_channel_config_t *)channel->config;
  hdl_dma_config_t *dma_cnf = (hdl_dma_config_t *)dma->config;
  DMA_Stream_TypeDef *stream = (DMA_Stream_TypeDef *)(dma_cnf->phy + sizeof(DMA_TypeDef) + (sizeof(DMA_Stream_TypeDef) * ch_cnf->stream));
  if(enable) {

    stream->CR &= ~DMA_SxCR_EN;
    uint32_t time = 10000;
    while((stream->CR & DMA_SxCR_EN) && time--);
    if(!time) return HDL_MODULE_FAULT;
    stream->FCR = (uint32_t)0x00000021U;
    stream->CR =  ch_cnf->channel_periphery | ch_cnf->priority |
                  ch_cnf->memory_width | ch_cnf->periph_width |
                  (ch_cnf->periph_inc? DMA_SxCR_PINC: 0) |
                  (ch_cnf->memory_inc? DMA_SxCR_MINC: 0) |
                  (ch_cnf->circular? DMA_SxCR_CIRC: 0) |
                  (ch_cnf->periph_ctrl? DMA_SxCR_PFCTRL: 0);

    dma_stream_registers_t *stream_regs = (dma_stream_registers_t *) ((ch_cnf->stream > HDL_DMA_STREAM_3)? 
      (((uint32_t)dma_cnf->phy & (uint32_t)(~0x3FFU)) + 4U) /* return pointer to HISR and HIFCR */:
      ((uint32_t)dma_cnf->phy & (uint32_t)(~0x3FFU))) /* return pointer to LISR and LIFCR */;
    stream_regs->IFCR = 0x3FU << dma_stream_bitshift[ch_cnf->stream];
    return HDL_MODULE_ACTIVE;
  }
  stream->CR = 0;
  return HDL_MODULE_UNLOADED;
}

uint8_t port_dma_ch_transfer_complete(const void *desc) {
  hdl_dma_channel_mcu_t *channel = ((hdl_dma_channel_mcu_t *)desc);
  if(hdl_is_null_module(desc)) return HDL_FALSE;
  hdl_dma_t *dma = (hdl_dma_t *)channel->dependencies[0];
  hdl_dma_channel_config_t *ch_cnf = (hdl_dma_channel_config_t *)channel->config;
  hdl_dma_config_t *dma_cnf = (hdl_dma_config_t *)dma->config;
  dma_stream_registers_t *stream_regs = (dma_stream_registers_t *) ((ch_cnf->stream > HDL_DMA_STREAM_3)? 
    (((uint32_t)dma_cnf->phy & (uint32_t)(~0x3FFU)) + 4U) /* return pointer to HISR and HIFCR */:
    ((uint32_t)dma_cnf->phy & (uint32_t)(~0x3FFU))) /* return pointer to LISR and LIFCR */;
  return (stream_regs->ISR & (DMA_LISR_TCIF0 << dma_stream_bitshift[ch_cnf->stream]))? HDL_TRUE: HDL_FALSE;
}

static uint32_t _hdl_dma_get_counter(const void *desc) {
  hdl_dma_channel_mcu_t *channel = ((hdl_dma_channel_mcu_t *)desc);
  hdl_dma_t *dma = (hdl_dma_t *)channel->dependencies[0];
  hdl_dma_channel_config_t *ch_cnf = (hdl_dma_channel_config_t *)channel->config;
  hdl_dma_config_t *dma_cnf = (hdl_dma_config_t *)dma->config;
  DMA_Stream_TypeDef *stream = (DMA_Stream_TypeDef *)(dma_cnf->phy + sizeof(DMA_TypeDef) + (sizeof(DMA_Stream_TypeDef) * ch_cnf->stream));
  return stream->NDTR;
}

static uint8_t _hdl_dma_stop(const void *desc) {
  hdl_dma_channel_mcu_t *channel = ((hdl_dma_channel_mcu_t *)desc);
  hdl_dma_t *dma = (hdl_dma_t *)channel->dependencies[0];
  hdl_dma_channel_config_t *ch_cnf = (hdl_dma_channel_config_t *)channel->config;
  hdl_dma_config_t *dma_cnf = (hdl_dma_config_t *)dma->config;
  DMA_Stream_TypeDef *stream = (DMA_Stream_TypeDef *)(dma_cnf->phy + sizeof(DMA_TypeDef) + (sizeof(DMA_Stream_TypeDef) * ch_cnf->stream));
  stream->CR &= ~DMA_SxCR_EN;
  uint32_t time = 10000;
  while((stream->CR & DMA_SxCR_EN) && time--);
  return time != 0;
}

static uint8_t _hdl_dma_run(const void *desc, hdl_dma_direction_t dir, uint32_t periph_addr, uint32_t mem_addr, uint32_t amount) {
  hdl_dma_channel_mcu_t *channel = ((hdl_dma_channel_mcu_t *)desc);
  hdl_dma_t *dma = (hdl_dma_t *)channel->dependencies[0];
  hdl_dma_channel_config_t *ch_cnf = (hdl_dma_channel_config_t *)channel->config;
  hdl_dma_config_t *dma_cnf = (hdl_dma_config_t *)dma->config;
  DMA_Stream_TypeDef *stream = (DMA_Stream_TypeDef *)(dma_cnf->phy + sizeof(DMA_TypeDef) + (sizeof(DMA_Stream_TypeDef) * ch_cnf->stream));
  if(stream->CR & DMA_SxCR_EN) return HDL_FALSE;
  stream->CR &= (uint32_t)(~DMA_SxCR_DBM);
  stream->NDTR = amount;
  stream->PAR = periph_addr;
  stream->M0AR = mem_addr;
  switch (dir) {
    case HDL_DMA_M2M:
      CL_REG_MODIFY(stream->CR, DMA_SxCR_DIR, DMA_MEMORY_TO_MEMORY);
      break;
    case HDL_DMA_P2M:
      CL_REG_MODIFY(stream->CR, DMA_SxCR_DIR, DMA_PERIPH_TO_MEMORY);
      break;
    case HDL_DMA_M2P:
      CL_REG_MODIFY(stream->CR, DMA_SxCR_DIR, DMA_MEMORY_TO_PERIPH);
      break;
    default:
      return HDL_FALSE;
  } 
  stream->CR |= DMA_SxCR_EN;
  return HDL_TRUE;  
}

const hdl_module_base_iface_t hdl_dma_iface = {
  .init = &_hdl_dma
};

const hdl_dma_channel_iface_t hdl_dma_channel_iface = {
  .init = &_hdl_dma_ch,
  .get_counter = &_hdl_dma_get_counter,
  .run = &_hdl_dma_run,
  .stop = &_hdl_dma_stop
};
