#ifndef HDL_DMA_H_
#define HDL_DMA_H_

#define DMA_PRIVATE_SIZE  (4)
#define EVENT_DMA_PRIVATE_SIZE  (4)

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
    HDL_DMA_SIZE_OF_MEMORY_8_BIT,   /* 8 bit size of memory transfer */
    HDL_DMA_SIZE_OF_MEMORY_16_BIT,  /* 16 bit size of memory transfer */
    HDL_DMA_SIZE_OF_MEMORY_32_BIT,  /* 32 bit size of memory transfer */
} hdl_dma_size_of_memory_e;

typedef dma_channel_enum hdl_dma_channel_t;

typedef struct {
    uint8_t __private[EVENT_DMA_PRIVATE_SIZE];
    hdl_dma_channel_t channel;
    event_handler_t transfer_full;
    event_handler_t transfer_half;
    event_handler_t transfer_error;
} hdl_dma_event_t;

typedef struct {
    hdl_module_t module;
    uint8_t __private[DMA_PRIVATE_SIZE];
} hdl_dma_t;

typedef struct{
    /* Tranfer mode, size, address */
    uint32_t periph_addr;                  /* peripheral base address or source addrees, when DMA direction memory to memory */
    hdl_dma_size_of_memory_e periph_width; /* HDL_DMA_SIZE_OF_MEMORY_8_BIT, HDL_DMA_SIZE_OF_MEMORY_16_BIT, HDL_DMA_SIZE_OF_MEMORY_32_BIT */
    uint32_t memory_addr;                  /* memory base address or destination, when DMA direction memory to memory */
    hdl_dma_size_of_memory_e memory_width; /* HDL_DMA_SIZE_OF_MEMORY_8_BIT, HDL_DMA_SIZE_OF_MEMORY_16_BIT, HDL_DMA_SIZE_OF_MEMORY_32_BIT */
    uint32_t amount;                       /* amount transfers */
    uint32_t priority;                     /* channel priority level */
    hdl_dma_increment_e periph_inc;        /* HDL_DMA_INCREMENT_OFF, HDL_DMA_INCREMENT_ON */
    hdl_dma_increment_e memory_inc;        /* HDL_DMA_INCREMENT_OFF, HDL_DMA_INCREMENT_ON */
    hdl_dma_direction_e direction;         /* HDL_DMA_DIRECTION_P2M, HDL_DMA_DIRECTION_M2P, HDL_DMA_DIRECTION_M2M */
    /* DMA mode */
    hdl_dma_mode_e dma_mode;                /* HDL_DMA_MODE_SINGLE_CONVERSION, HDL_DMA_MODE_CIRCULAR_CONVERSION */
} hdl_dma_config_t;

typedef enum {
    HDL_DMA_STATUS_FULL_TRANSFER = (1),
    HDL_DMA_STATUS_HALF_TRANSFER = (2),
    HDL_DMA_STATUS_ERROR_TRANSFER = (4),
} hdl_dma_status_e;


/*!
    \brief          DMA initialization and deinitialization
    \param[in]      desc - can be casting to hdl_dma_t*
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return 
      \retval         HDL_MODULE_INIT_FAILED
      \retval         HDL_MODULE_INIT_OK
      \retval         HDL_MODULE_DEINIT_OK
 */
hdl_module_state_t hdl_dma(void *desc, uint8_t enable);
/*!
    \brief          Config DMA channel
    \note           After that DMA will be waiting start trigger for starting transfer
    \param[in]      h - DMA handler
    \param[in]      config - config struct
    \param[in]      channel - channel number
    \return         
      \retval         HDL_TRUE - OK
      \retval         HDL_FALSE - ERROR
 */
uint8_t hdl_dma_config(hdl_dma_t *h, hdl_dma_config_t *config, hdl_dma_channel_t channel);
/*!
    \brief          Software triger for starting transfer
    \param[in]      h - DMA handler
    \param[in]      channel - DMA channel
    \return         
      \retval         HDL_TRUE - OK
      \retval         HDL_FALSE - ERROR
 */
uint8_t hdl_dma_sw_triger(hdl_dma_t *h, hdl_dma_channel_t channel); 
/*!
    \brief          Get DMA channel status
    \param[in]      h - DMA handler
    \param[in]      channel - DMA channel
    \return         hdl_dma_status_e
 */
hdl_dma_status_e hdl_dma_status(hdl_dma_t *h, hdl_dma_channel_t channel);
/*!
    \brief          Get DMA transfer counter value
    \param[in]      h - DMA handler
    \param[in]      channel - DMA channel
    \return         Counter value
 */
uint32_t hdl_dma_counter(hdl_dma_t *h, hdl_dma_channel_t channel);
/* Subscribe to IRQ */
uint8_t hdl_dma_subscribe(hdl_dma_t *h, hdl_dma_event_t *event);

#if defined ( GD32E23X )

#endif

#endif