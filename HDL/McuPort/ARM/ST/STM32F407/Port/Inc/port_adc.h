#ifndef PORT_ADC_H_
#define PORT_ADC_H_

#include "hdl_adc.h"

#define HDL_ADC_VAR_SIZE  36

typedef struct {
  uint32_t phy;                    /* ADC */
  uint32_t rcc;                    /* RCC_APB2ENR_ADC1EN */
  uint32_t prescaler;              /* Can be 2, 4, 6, 8 */
  uint32_t multi_mode;             /* ADC_MODE_INDEPENDENT, (ADC_DUALMODE, ADC_TRIPLEMODE)_[REGSIMULT_INJECSIMULT, REGSIMULT_ALTERTRIG, INJECSIMULT, REGSIMULT, INTERL,ALTERTRIG], ADC_TRIPLEMODE_] */
} hdl_adc_common_config_t;

typedef struct {
  uint32_t phy;                    /* ADC1, ADC2, ADC3*/
  uint32_t rcc;                    /* RCC_APB2ENR_ADC1EN, RCC_APB2ENR_ADC2EN, RCC_APB2ENR_ADC3EN */
  hdl_interrupt_t *adc_interrupt;
  uint32_t resolution;             /* ADC_RESOLUTION_12B, ADC_RESOLUTION_10B, ADC_RESOLUTION_8B, ADC_RESOLUTION_6B */
  uint32_t data_alignment;         /* ADC_DATAALIGN_RIGHT, ADC_DATAALIGN_LEFT */
  uint32_t regular_ext_trg;        /* ADC_EXTERNALTRIGCONV_[T1_CC1(CC2/CC3), T2_CC2(CC3/CC4/TRGO), T3_CC1(TRGO), T4_CC4, T5_CC1(CC2/CC3), T8_CC1(TRGO), Ext_IT11] */
  uint32_t regular_trg_edge;       /* ADC_EXTERNALTRIGCONVEDGE_[NONE, RISING, FALLING, RISINGFALLING] */
  uint32_t injected_ext_trg;       /* ADC_EXTERNALTRIGINJECCONV_[T1_(CC4,TRGO), T2_(CC1,TRGO), T3_(CC2, CC4), T4_(CC1, CC2, CC3,TRGO), T5_(CC4,TRGO), T8_(CC2, CC3, CC4), Ext_IT15 */
  uint32_t injected_trg_edge;      /* ADC_EXTERNALTRIGINJECCONVEDGE_[NONE, RISING, FALLING, RISINGFALLING] */
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
  uint32_t channel;     /* ADC_CHANNEL_[0, ..., 9, 16 (Temperature sensor), 17 (TSVREFE channel), 18 (VBAT channel)] */
  uint32_t sample_time; /* ADC_SAMPLETIME_[3, 15, 28, 56, 84, 112, 144, 480]CYCLES */
  hdl_adc_ch_rank_t rank;         
} hdl_adc_ch_config_t;

/* depends on:
  hdl_clock_t
 */
hdl_module_new_t(hdl_adc_common_mcu_t, 0, hdl_adc_common_config_t*, hdl_module_base_iface_t);

extern const hdl_module_base_iface_t hdl_adc_common_iface;

/* depends on:
  hdl_adc_common_mcu
  hdl_dma_channel_t
  hdl_timertick_counter_t
 */
hdl_module_new_t(hdl_adc_mcu_t, HDL_ADC_VAR_SIZE, hdl_adc_config_t*, hdl_module_base_iface_t);

extern const hdl_module_base_iface_t hdl_adc_iface;

/* depends on:
  hdl_adc_mcu_t
 */
hdl_module_new_t(hdl_adc_ch_mcu_t, 0, hdl_adc_ch_config_t*, hdl_adc_ch_iface_t);

extern const hdl_adc_ch_iface_t hdl_adc_ch_iface;

#endif
