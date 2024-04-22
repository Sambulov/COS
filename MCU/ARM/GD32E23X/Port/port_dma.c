#include "hdl_portable.h"

typedef enum{
    GD_DMA_CONVERSION_PROCEEDING,
    GD_DMA_CONVERSION_COMPLETE,
} gd_dma_coversion_status_e;

static const uint32_t gd_dma_memory_width_array[] = {DMA_MEMORY_WIDTH_8BIT, DMA_MEMORY_WIDTH_16BIT, DMA_MEMORY_WIDTH_32BIT};
static const uint32_t gd_dma_peripheral_width_array[] = {DMA_PERIPHERAL_WIDTH_8BIT, DMA_PERIPHERAL_WIDTH_16BIT, DMA_PERIPHERAL_WIDTH_32BIT};

static gd_dma_coversion_status_e _gd_dma_get_conversion_status(uint32_t channel);

/* Get DMA conversion status */
static gd_dma_coversion_status_e _gd_dma_get_conversion_status(uint32_t channel){
    if((DMA_CHCTL(channel) & (1 << 0)) && dma_flag_get(channel, DMA_FLAG_FTF) == RESET)
        return GD_DMA_CONVERSION_PROCEEDING;
    else
        return GD_DMA_CONVERSION_COMPLETE;
}

/* DMA initialization and deinitialization */
hdl_module_state_t hdl_dma(void *desc, uint8_t enable){
    hdl_dma_t *hdl_dma = NULL;
    /* Try casting void pointer to hdl_dma_t */
    hdl_dma = (hdl_dma_t *)desc;
    if (hdl_dma->module.reg == NULL || (uint32_t)hdl_dma->module.reg != DMA_BASE)
        return HDL_MODULE_INIT_FAILED;
    if (enable){
        rcu_periph_clock_enable(RCU_DMA);
        return HDL_MODULE_INIT_OK;
    }
    else{
        rcu_periph_clock_disable(RCU_DMA);
        return HDL_MODULE_DEINIT_OK;
    }
}

/* Config DMA channel */
uint8_t hdl_dma_config(hdl_dma_t *h, hdl_dma_config_t *config, hdl_dma_channel_t channel)
{
    dma_deinit(channel);
    dma_periph_address_config(channel, config->periph_addr);
    dma_memory_address_config(channel, config->memory_addr);
    dma_transfer_number_config(channel, config->amount);
    dma_priority_config(channel, config->priority);
    dma_memory_width_config(channel, gd_dma_memory_width_array[config->memory_width]);
    dma_periph_width_config(channel, gd_dma_peripheral_width_array[config->periph_width]);

    //dma_interrupt_enable(channel, DMA_INT_FTF); 
    //dma_interrupt_enable(channel, DMA_INT_HTF);


    if(config->memory_inc)
        dma_memory_increase_enable(channel);
    if(config->periph_inc)
        dma_periph_increase_enable(channel);
    if(config->direction == HDL_DMA_DIRECTION_P2M)
        dma_transfer_direction_config(channel, DMA_PERIPHERAL_TO_MEMORY);
    else if(config->direction == HDL_DMA_DIRECTION_M2P)
        dma_transfer_direction_config(channel, DMA_MEMORY_TO_PERIPHERAL);
    else if(config->direction == HDL_DMA_DIRECTION_M2M)
    {
        /* In this case source address it is peripheral address, destiantion address it is memory address */
        dma_transfer_direction_config(channel, DMA_PERIPHERAL_TO_MEMORY);
        dma_memory_to_memory_enable(channel);
    }
    if (config->dma_mode == HDL_DMA_MODE_SINGLE)
        dma_circulation_disable(channel);
    else
        dma_circulation_enable(channel);
    
    return HDL_TRUE;
}

/* Software triger for starting transfer */
uint8_t hdl_dma_sw_triger(hdl_dma_t *h, hdl_dma_channel_t channel){
    if (_gd_dma_get_conversion_status(channel) == GD_DMA_CONVERSION_COMPLETE){
        dma_channel_enable(channel);
        return HDL_TRUE;
    }
    return HDL_FALSE;
}
/* Get DMA channel status */
hdl_dma_status_e hdl_dma_status(hdl_dma_t *h, hdl_dma_channel_t channel){
    hdl_dma_status_e rez = HDL_DMA_STATUS_NONE;
    if((DMA_CHCTL(channel) & (1 << 0)) && dma_flag_get(channel, DMA_FLAG_HTF) == RESET)
        rez = HDL_DMA_STATUS_HALF_TRANSFER;
    if((DMA_CHCTL(channel) & (1 << 0)) && dma_flag_get(channel, DMA_FLAG_FTF) == RESET)
        rez = HDL_DMA_STATUS_FULL_TRANSFER;
    if((DMA_CHCTL(channel) & (1 << 0)) && dma_flag_get(channel, DMA_FLAG_ERR) == RESET)
        rez = HDL_DMA_STATUS_ERROR_TRANSFER;
    
    return rez;
}
/* Get DMA transfer counter value */
uint32_t hdl_dma_counter(hdl_dma_t *h, hdl_dma_channel_t channel){
    return dma_transfer_number_get(channel);
}
/* Subscribe to IRQ */
uint8_t hdl_dma_subscribe(hdl_dma_t *h, hdl_dma_event_t *event);