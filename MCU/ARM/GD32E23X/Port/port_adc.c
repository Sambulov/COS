#include "hdl_portable.h"
#include "Macros.h"

typedef enum {
    GD_ADC_STATE_MACHINE_DISABLE,
    GD_ADC_STATE_MACHINE_CALIBRATE,
    GD_ADC_STATE_MACHINE_OK,
    GD_ADC_STATE_MACHINE_FAULT,
} gd_adc_state_machine_e;

static uint8_t internal_dma_is_init = 0;

int8_t _gd_adc_calibrate(hdl_sys_timer_t *timer_ms, uint32_t time_stamp_ms, uint32_t timeout_ms){
    /* reset the selected ADC calibration register */
    ADC_CTL1 |= (uint32_t)ADC_CTL1_RSTCLB;
    if ((ADC_CTL1 & ADC_CTL1_RSTCLB)){
        if (TIME_ELAPSED(time_stamp_ms, 1000, millis(timer_ms)))
            return HDL_MODULE_INIT_FAILED;
        else
            return HDL_MODULE_INIT_ONGOING;
    }
    /* enable ADC calibration process */
    ADC_CTL1 |= ADC_CTL1_CLB;
    if ((ADC_CTL1 & ADC_CTL1_CLB)){
        if (TIME_ELAPSED(time_stamp_ms, 1000, millis(timer_ms)))
            return HDL_MODULE_INIT_FAILED;
        else
            return HDL_MODULE_INIT_ONGOING;
    }
    return HDL_MODULE_INIT_OK;
}

hdl_module_state_t hdl_adc(void *desc, uint8_t enable){
    static uint32_t time_stamp_ms = 0;
    static gd_adc_state_machine_e state_machine = GD_ADC_STATE_MACHINE_DISABLE;
    hdl_adc_t *hdl_adc = (hdl_adc_t *)desc;
    hdl_sys_timer_t *hdl_timer_ms = NULL;

    if(hdl_adc->module.reg == NULL || hdl_adc->module.dependencies == NULL || hdl_adc->module.dependencies[0] == NULL ||
        hdl_adc->module.dependencies[1] == NULL || hdl_adc->module.dependencies[2] == NULL || 
        hdl_adc->module.dependencies[2] == NULL || hdl_adc->channel_array == NULL || hdl_adc->channel_array[0] == NULL)
        return HDL_MODULE_INIT_FAILED;
    /* We can find timer in our dependencies */
    hdl_timer_ms = (hdl_sys_timer_t *)hdl_adc->module.dependencies[1];
    /* TODO: SEE ADC_REGULAR_INSERTED_CHANNEL */
    if(enable) {
        switch (state_machine){
            case GD_ADC_STATE_MACHINE_DISABLE:
                rcu_periph_clock_enable(RCU_ADC);
                /* pointer points to the first element in the array */
                hdl_adc_channel_source_t **adc_channel_source = hdl_adc->channel_array;
                int16_t channel_element_number = 0;
                while (*adc_channel_source != NULL){
                    /* Config routine sequence */
                    adc_regular_channel_config((uint8_t)channel_element_number, (uint8_t)(*adc_channel_source)->channel_number, (uint32_t)(*adc_channel_source)->channel_sample_time);
                    adc_channel_source++;
                    channel_element_number++;
                }
                /* routine sequence lenght */    
                adc_channel_length_config(ADC_REGULAR_CHANNEL, channel_element_number + 1);
                adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
                adc_resolution_config((uint32_t)hdl_adc->resolution);
                adc_special_function_config(ADC_SCAN_MODE, ENABLE);

                if (hdl_adc->mode == ADC_OPERATION_MODE_SINGLE_SCAN)
                    adc_special_function_config(ADC_SCAN_MODE, DISABLE);
                else if (hdl_adc->mode == ADC_OPERATION_MODE_CONTINUOS_SCAN)
                    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);

                /* Software trigger for start conversion */
                /* TODO: Issue with INSERTED channel */     
                //adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, (uint32_t)hdl_adc->start_triger);
                //adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
                adc_enable();
                state_machine = GD_ADC_STATE_MACHINE_CALIBRATE;
            case GD_ADC_STATE_MACHINE_CALIBRATE:
                if(time_stamp_ms == 0)
                    time_stamp_ms = hdl_sys_timer_get(hdl_timer_ms); 
                hdl_module_state_t res = _gd_adc_calibrate(hdl_timer_ms, time_stamp_ms, 3000);
                if(res == HDL_MODULE_INIT_OK){
                    adc_dma_mode_enable();
                    state_machine = GD_ADC_STATE_MACHINE_OK;
                    return HDL_MODULE_INIT_OK;
                }
                else if(res == HDL_MODULE_INIT_ONGOING){
                    return HDL_MODULE_INIT_ONGOING;
                }
                else{
                    state_machine = GD_ADC_STATE_MACHINE_FAULT;
                    return HDL_MODULE_INIT_FAILED;
                }
            case GD_ADC_STATE_MACHINE_FAULT:
                return HDL_MODULE_INIT_FAILED;
            case GD_ADC_STATE_MACHINE_OK:
                return HDL_MODULE_INIT_OK;
        }
    }
    else{
        time_stamp_ms = 0;
        internal_dma_is_init = 0;
        state_machine = GD_ADC_STATE_MACHINE_DISABLE;
        adc_disable();
        adc_dma_mode_disable();
        rcu_periph_clock_disable(RCU_ADC);
        return HDL_MODULE_DEINIT_OK;
    }
}

uint8_t hdl_adc_start(hdl_adc_t *hdl_adc, void *buff){
    if(!internal_dma_is_init){
        internal_dma_is_init = 1;
        /* pointer points to the first element in the array */
        hdl_adc_channel_source_t **adc_channel_source = hdl_adc->channel_array;
        int16_t channel_element_number = 0;
        while (*adc_channel_source != NULL){
            adc_channel_source++;
            channel_element_number++;
        }
        /* Config DMA struct */
        hdl_dma_config_t config;
        config.memory_addr = (uint32_t)buff;
        config.memory_width = HDL_DMA_SIZE_OF_MEMORY_16_BIT;
        config.memory_inc = HDL_DMA_INCREMENT_ON;
        config.periph_addr = (uint32_t)(&ADC_RDATA);
        config.periph_inc = HDL_DMA_INCREMENT_ON;
        config.periph_width = HDL_DMA_SIZE_OF_MEMORY_16_BIT;
        config.amount = channel_element_number;
        config.direction = HDL_DMA_DIRECTION_P2M;
        /* TODO: What is mode the best? */
        config.dma_mode = HDL_DMA_MODE_SINGLE_CONVERSION;
        config.priority = 0;
        hdl_dma_t* hdl_dma = (hdl_dma_t *)hdl_adc->module.dependencies[2];
        hdl_dma_config(hdl_dma, &config, 0);
    }
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, (uint32_t)hdl_adc->start_triger);
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
}