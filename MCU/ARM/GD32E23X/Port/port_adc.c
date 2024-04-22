#include "hdl_portable.h"
#include "Macros.h"

typedef enum {
    GD_ADC_STATE_MACHINE_DISABLE,
    GD_ADC_STATE_MACHINE_CALIBRATE,
    GD_ADC_STATE_MACHINE_OK,
    GD_ADC_STATE_MACHINE_FAULT,
} gd_adc_state_machine_e;

typedef struct 
{
    uint32_t time_stamp;
    uint8_t empty[2];
    uint8_t 
    flag_get_time           : 1,
    flag_delay_after_start  : 1,
    flag_reset_calibrate    : 1,
    flag_enable_calibrate   : 1,
    empty1                  : 4;

    uint8_t state_machine;      /* gd_adc_state_machine_e */
} gd_adc_private_t;

_Static_assert(sizeof(gd_adc_private_t) == (ADC_PRIVATE_FIELD_SIZE), "Private struct size have to be equal defined size");


hdl_module_state_t _gd_adc_calibrate(hdl_timer_t *timer, gd_adc_private_t *private, uint32_t timeout){
    static const uint16_t adc_short_delay_after_start = 14 * 3;  /* There must be 14 CK_ADC tact (info from manual) */
    if(!private->flag_get_time){
        private->flag_get_time = 1;
        private->time_stamp = hdl_timer_get(timer);
    }
    if(!private->flag_delay_after_start)
    {
        /* There must be 14 CK_ADC tact */
        private->flag_delay_after_start = 1;
        for(uint16_t i = 0; i < adc_short_delay_after_start; i++)
            __NOP();
    }
    /* reset the selected ADC calibration register */
    if(!private->flag_reset_calibrate){
        private->flag_reset_calibrate = 1;
        ADC_CTL1 |= (uint32_t)ADC_CTL1_RSTCLB;
    }
    if ((ADC_CTL1 & ADC_CTL1_RSTCLB)){
        if (TIME_ELAPSED(private->time_stamp, timeout, hdl_timer_get(timer)))
            return HDL_MODULE_INIT_FAILED;
        else
            return HDL_MODULE_INIT_ONGOING;
    }
    /* Enable calibrate */
    if(!private->flag_enable_calibrate){
        private->flag_enable_calibrate = 1;
        ADC_CTL1 |= ADC_CTL1_CLB;
    }
    /* enable ADC calibration process */
    if ((ADC_CTL1 & ADC_CTL1_CLB)){
        if (TIME_ELAPSED(private->time_stamp, timeout, hdl_timer_get(timer)))
            return HDL_MODULE_INIT_FAILED;
        else
            return HDL_MODULE_INIT_ONGOING;
    }
    return HDL_MODULE_INIT_OK;
}

void _gd_adc_dma_stuct_fill(hdl_dma_config_t* dma_struct, void* memmory_address, uint32_t amount_transfer){
    dma_struct->memory_addr = (uint32_t)memmory_address;
    dma_struct->memory_width = HDL_DMA_SIZE_OF_MEMORY_16_BIT;
    dma_struct->memory_inc = HDL_DMA_INCREMENT_ON;
    dma_struct->periph_addr = (uint32_t)(&ADC_RDATA);
    dma_struct->periph_inc = HDL_DMA_INCREMENT_OFF;
    dma_struct->periph_width = HDL_DMA_SIZE_OF_MEMORY_16_BIT;
    dma_struct->amount = amount_transfer;
    dma_struct->direction = HDL_DMA_DIRECTION_P2M;
    dma_struct->dma_mode = HDL_DMA_MODE_CIRCULAR;
    dma_struct->priority = 0;
}


hdl_module_state_t hdl_adc(void *desc, uint8_t enable){
    hdl_adc_t *hdl_adc = (hdl_adc_t *)desc;
    gd_adc_private_t *private = (gd_adc_private_t *)hdl_adc->__private;
    hdl_dma_t *hdl_dma = NULL;
    if(hdl_adc->module.reg == NULL || hdl_adc->module.dependencies == NULL || hdl_adc->module.dependencies[0] == NULL ||
        hdl_adc->module.dependencies[1] == NULL || hdl_adc->module.dependencies[2] == NULL || hdl_adc->sources == NULL || 
        hdl_adc->sources[0] == NULL)
            return HDL_MODULE_INIT_FAILED;

    /* We can find timer in our dependencies */
    hdl_timer_t *hdl_timer = (hdl_timer_t *)hdl_adc->module.dependencies[1];
    /* TODO: SEE ADC_REGULAR_INSERTED_CHANNEL */
    if(enable) {
        switch (private->state_machine){
            case GD_ADC_STATE_MACHINE_DISABLE: {
                rcu_periph_clock_enable(RCU_ADC);
                adc_special_function_config(ADC_SCAN_MODE, ENABLE);
                if(hdl_adc->mode == ADC_OPERATION_MODE_CONTINUOS_SCAN)
                    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);
                /* pointer points to the first element in the array */
                hdl_adc_source_t **adc_channel_source = hdl_adc->sources;
                int16_t channel_element_number = 0;
                while (*adc_channel_source != NULL) {
                    /* Config routine sequence */
                    adc_regular_channel_config((uint8_t)channel_element_number, (uint8_t)(*adc_channel_source)->channel_number, (uint32_t)(*adc_channel_source)->channel_sample_time);
                    adc_channel_source++;
                    channel_element_number++;
                }
                /* routine sequence lenght */
                adc_channel_length_config(ADC_REGULAR_CHANNEL, channel_element_number);
                //adc_discontinuous_mode_config(ADC_REGULAR_CHANNEL, channel_element_number);
                adc_data_alignment_config(hdl_adc->data_alignment);
                adc_resolution_config((uint32_t)hdl_adc->resolution);
                if(hdl_adc->start_triger == HDL_ADC_TRIGER_SOFTWARE)
                    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
                adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, hdl_adc->start_triger);
                
                
                adc_enable();

                private->state_machine = GD_ADC_STATE_MACHINE_CALIBRATE;
                private->flag_get_time = 0;
                private->flag_delay_after_start = 0;
                private->flag_enable_calibrate = 0;
                private->flag_reset_calibrate = 0;
            }
            case GD_ADC_STATE_MACHINE_CALIBRATE: {
                hdl_module_state_t res = _gd_adc_calibrate(hdl_timer, private, hdl_adc->init_timeout);
                if(res == HDL_MODULE_INIT_ONGOING)
                    return HDL_MODULE_INIT_ONGOING;
                
                if(res == HDL_MODULE_INIT_OK){
                    adc_dma_mode_enable();
                    private->state_machine = GD_ADC_STATE_MACHINE_OK;
                    return HDL_MODULE_INIT_OK;  
                }
                else{
                    private->state_machine = GD_ADC_STATE_MACHINE_FAULT;
                    return HDL_MODULE_INIT_FAILED;
                }
            }
            case GD_ADC_STATE_MACHINE_OK:
                return HDL_MODULE_INIT_OK;   
            case GD_ADC_STATE_MACHINE_FAULT:
                return HDL_MODULE_INIT_FAILED;
            default:
                return HDL_MODULE_INIT_FAILED;
        }
    }
    else{
        private->state_machine = GD_ADC_STATE_MACHINE_DISABLE;
        adc_disable();
        adc_dma_mode_disable();
        rcu_periph_clock_disable(RCU_ADC);
        return HDL_MODULE_DEINIT_OK;
    }
}

hdl_adc_status_e hdl_adc_status(hdl_adc_t *hdl_adc){
    // gd_adc_private_t *private = NULL;
    // hdl_dma_t* hdl_dma = NULL;
    // if(hdl_adc == NULL)
    //     return HDL_ADC_STATUS_FAILED;
    // private = (gd_adc_private_t *)hdl_adc->__private;
    // hdl_dma = (hdl_dma_t *)hdl_adc->module.dependencies[2];
    // if (private->state_machine != GD_ADC_STATE_MACHINE_OK)
    //     return HDL_ADC_STATUS_FAILED;

    // if(hdl_adc->mode == ADC_OPERATION_MODE_CONTINUOS_SCAN)
    //     return HDL_ADC_STATUS_OK;
    // else if(hdl_adc->mode == ADC_OPERATION_MODE_SINGLE_SCAN){
    //     if(adc_flag_get(ADC_FLAG_EOC) == SET)
    //         return HDL_ADC_STATUS_OK;
    //     else
    //         return HDL_ADC_STATUS_ONGOING;
    // }
    return HDL_ADC_STATUS_ONGOING;
}

hdl_adc_status_e hdl_adc_start(hdl_adc_t *hdl_adc, void *buff){
    gd_adc_private_t *private = NULL;
    hdl_dma_t* hdl_dma = NULL;
    if(hdl_adc == NULL || buff == NULL)
        return HDL_ADC_STATUS_FAILED;
    private = (gd_adc_private_t *)hdl_adc->__private;
    hdl_dma = (hdl_dma_t *)hdl_adc->module.dependencies[2];
    if (private->state_machine != GD_ADC_STATE_MACHINE_OK)
        return HDL_ADC_STATUS_FAILED;

    if(hdl_adc->mode == ADC_OPERATION_MODE_CONTINUOS_SCAN){
        /* Config and start DMA */
        if((DMA_CHCTL(hdl_adc->dma_channel) & (1 << 0)) == RESET){
            /* pointer points to the first element in the array */
            hdl_adc_source_t **adc_channel_source = hdl_adc->sources;
            int16_t channel_element_number = 0;
            while (*adc_channel_source != NULL){
                adc_channel_source++;
                channel_element_number++;
            }
            hdl_dma_config_t config;
            _gd_adc_dma_stuct_fill(&config, buff, channel_element_number);
            hdl_dma_config(hdl_dma, &config, hdl_adc->dma_channel);
            /* TODO: if channel == 1 */
            /* TODO: if channel !=0 || !=1  return FALSE*/
            dma_channel_enable(hdl_adc->dma_channel);
            if(hdl_adc->start_triger == HDL_ADC_TRIGER_SOFTWARE)
                adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
            else
                adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
        }
        return HDL_ADC_STATUS_OK;
    }
    else if(hdl_adc->mode == ADC_OPERATION_MODE_SINGLE_SCAN){
        if(dma_flag_get(0, DMA_FLAG_FTF) || (DMA_CHCTL(hdl_adc->dma_channel) & (1 << 0)) == RESET){
            /* pointer points to the first element in the array */
            hdl_adc_source_t **adc_channel_source = hdl_adc->sources;
            int16_t channel_element_number = 0;
            while (*adc_channel_source != NULL){
                adc_channel_source++;
                channel_element_number++;
            }
            hdl_dma_config_t config;
            _gd_adc_dma_stuct_fill(&config, buff, channel_element_number);
            hdl_dma_config(hdl_dma, &config, hdl_adc->dma_channel);
            /* TODO: if channel == 1 */
            /* TODO: if channel !=0 || !=1  return FALSE*/
            dma_channel_enable(hdl_adc->dma_channel);
            if(hdl_adc->start_triger == HDL_ADC_TRIGER_SOFTWARE)
                adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
            else
                adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);

            return HDL_ADC_STATUS_OK;
        }
        else
            return HDL_ADC_STATUS_ONGOING;
    }
    else
        return HDL_ADC_STATUS_FAILED;
}

