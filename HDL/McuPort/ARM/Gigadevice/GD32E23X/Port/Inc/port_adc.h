#ifndef PORT_ADC_H_
#define PORT_ADC_H_

#include "hdl_adc.h"

#define HDL_ADC_VAR_SIZE  56

// typedef enum {
//     ADC_OPERATION_MODE_SINGLE_SCAN,    /* Single SCAN, user must launched every conversion with some triger */
//     ADC_OPERATION_MODE_CONTINUOS_SCAN, /* Continuous scan, each next conversion after first triger will be do automaticly */
// } hdl_adc_operation_mode_e;

typedef struct {
  uint32_t phy;                    /* ADC */
  hdl_interrupt_t *adc_interrupt;
  uint32_t resolution;             /* ADC_RESOLUTION_[6, 8, 10, 12]B */
  uint32_t data_alignment;         /* ADC_DATAALIGN_RIGHT, ADC_DATAALIGN_LEFT */
  uint32_t regular_ext_trg;        /* ADC_EXTTRIG_REGULAR_[NONE, T0_CH(0,1,2), T2_TRGO, T14_CH0, EXTI_11] */
  uint32_t injected_ext_trg;       /* ADC_EXTTRIG_INSERTED_[NONE, T0_TRGO, T0_CH3, T2_CH3, T14_TRGO, EXTI_15] */
  uint32_t init_timeout;
  uint32_t *adc_slots;             /* buffer, static_malloc(4*x) where x is number of regular channels */
} hdl_adc_config_t;

typedef enum {
  HDL_ADC_CH_RANK_REGULAR1      = 0x01,
  HDL_ADC_CH_RANK_REGULAR2      = 0x02,
  HDL_ADC_CH_RANK_REGULAR3      = 0x03,
  HDL_ADC_CH_RANK_REGULAR4      = 0x04,
  HDL_ADC_CH_RANK_REGULAR5      = 0x05,
  HDL_ADC_CH_RANK_REGULAR6      = 0x06,
  HDL_ADC_CH_RANK_REGULAR7      = 0x07,
  HDL_ADC_CH_RANK_REGULAR8      = 0x08,
  HDL_ADC_CH_RANK_REGULAR9      = 0x09,
  HDL_ADC_CH_RANK_REGULAR10     = 0x0a,
  HDL_ADC_CH_RANK_REGULAR11     = 0x0b,
  HDL_ADC_CH_RANK_REGULAR12     = 0x0c,
  HDL_ADC_CH_RANK_REGULAR13     = 0x0d,
  HDL_ADC_CH_RANK_REGULAR14     = 0x0e,
  HDL_ADC_CH_RANK_REGULAR15     = 0x0f,
  HDL_ADC_CH_RANK_REGULAR16     = 0x10,
  HDL_ADC_CH_RANK_INJECTED_MASK = 0x80,
  HDL_ADC_CH_RANK_INJECTED1     = 0x81,
  HDL_ADC_CH_RANK_INJECTED2     = 0x82,
  HDL_ADC_CH_RANK_INJECTED3     = 0x83,
  HDL_ADC_CH_RANK_INJECTED4     = 0x84
} hdl_adc_ch_rank_t;

typedef struct {
  uint32_t channel;       /* ADC_CHANNEL_[0, ..., 9, 16 (Temperature sensor), 17 (TSVREFE channel), 18 (VBAT channel)] */
  uint32_t sample_time;   /* ADC_SAMPLETIME_[1, 7, 3, 28, 41, 55, 71, 239]POINT5 */
  hdl_adc_ch_rank_t rank;
} hdl_adc_ch_config_t;

/* depends on
  hdl_clock_t
  hdl_time_counter_t
  hdl_dma_channel_t
  hdl_interrupt_controller_t
*/
hdl_module_new_t(hdl_adc_mcu_t, HDL_ADC_VAR_SIZE, hdl_adc_config_t*, hdl_module_base_iface_t);

extern const hdl_module_base_iface_t hdl_adc_iface;

/* depends on:
  hdl_adc_mcu_t
 */
hdl_module_new_t(hdl_adc_ch_mcu_t, 0, hdl_adc_ch_config_t*, hdl_adc_ch_iface_t);

extern const hdl_adc_ch_iface_t hdl_adc_ch_iface;

#endif