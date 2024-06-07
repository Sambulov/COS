/*!
    \file device_logic.h
    \author a.bodin
    
    \version 2024-06-04, V1.0.0, This file describe major device logic
 */
#ifndef ATB_3500_DEVICE_LOGIC_H_
#define ATB_3500_DEVICE_LOGIC_H_

#ifdef ATB_3500
#include "stdint.h"
#include "bldl.h"
#include "CodeLib.h"
#include <string.h>
#include "filter_mean.h"

#define HDL_MODULE_WRAP(x)       (&(x->module))
#define HDL_GPIO_ON_WRAP(x)      (!(x->inactive_default))
#define HDL_GPIO_OFF_WRAP(x)      ((x->inactive_default))

#define ADC_REFERENCE_VOLTAGE_MV      ((uint32_t)2500)
#define ADC_RESOLUTION                ((uint32_t)4096)
#define ADC_CHANNEL_0_VOLTAGE_DIVIDER ((uint32_t)11)
#define ADC_CHANNEL_1_VOLTAGE_DIVIDER ((uint32_t)11)
#define ADC_CHANNEL_2_VOLTAGE_DIVIDER ((uint32_t)3)
#define ADC_CHANNEL_3_VOLTAGE_DIVIDER ((uint32_t)2)
#define ADC_CHANNEL_4_VOLTAGE_DIVIDER ((uint32_t)2)
#define ADC_CHANNEL_5_VOLTAGE_DIVIDER ((uint32_t)1)


#define ADC_CHANNEL_AMOUNT  6
/* Corresponds to array adc index */
typedef enum {
    ATB3500_ADC_24V = (uint8_t)0,
    ATB3500_ADC_24V_POE = (uint8_t)1,
    ATB3500_ADC_5V = (uint8_t)2,
    ATB3500_ADC_3V3 = (uint8_t)3,
    ATB3500_ADC_2V5 = (uint8_t)4,
    ATB3500_ADC_1V8 = (uint8_t)5,
} atb3500_adc_channel_name_e;


typedef enum{
    DL_STATUS_SUCCESS = 0,
    DL_STATUS_ERROR = !DL_STATUS_SUCCESS,
} device_logic_status_e;

typedef enum {
  DL_STATE_MACHINE_INITIAL,
  DL_STATE_MACHINE_SMARC_POWER_UP,
  DL_STATE_MACHINE_POWER_MONITOR,
  DL_STATE_MACHINE_POWER_RESET,  
} device_logic_state_machine_e;

typedef struct {
    uint8_t state;      /* device_logic_state_machine_e */
    uint8_t sub_state;
} device_logic_state_machine_t;

typedef struct {
    uint32_t
    sb_all_hdl_module_is_init   : 1, /* This bit indicates that all hdl module have initialized */
    sb_adc_include_valid_data   : 1, /* This bit indicates that adc include valid data */
    sb_24v_poe_is_enable        : 1, /* This bit indicates that 24v poe is active */
    sb_watchdog_event_occur     : 1, /* This bit indicates that watchdog event occur */
    dummy                       : 28;
} device_logic_state_bits_t;

typedef struct {
    uint32_t
    dl_error_poe_not_good       : 1,  /* POE status don`t equal GOOD */
    dl_error_poe_fault          : 1,  /* POE status fault */
    dummy                       : 31;
} device_logic_error_t;

typedef struct {
    device_logic_state_machine_t state_machine;     /* State machime */
    device_logic_state_bits_t sb;                   /* State bits */
    device_logic_error_t error;
    filter_mean_t adc_filter[ADC_CHANNEL_AMOUNT];
    int8_t counter_of_boot_retry;                  /* retains number of remaining boot attempt */       
} object_dictionary_t;

void device_logic(void);

#include "device_function.h"
#include "device_tree.h"
#include "atb_3500_page_transfer.h"

#endif /* ATB_3500 */
#endif /* ATB_3500_DEVICE_LOGIC_H_ */