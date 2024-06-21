/*!
    \file device_logic.h
    \author a.bodin
    
    \version 2024-06-04, V1.0.0, This file describe major device logic
 */
#ifndef ATB_3500_DEVICE_LOGIC_H_
#define ATB_3500_DEVICE_LOGIC_H_

#ifdef ATB_3500
#include "bldl.h"
#include "bldl_power_domain.h"
#include "bldl_smarc_carrier.h"
#include "bldl_communication.h"
#include "atb3500_io.h"
#include "CodeLib.h"
#include "mig_atb3500.h"

#define HDL_GPIO_ON_WRAP(x)      (!(x.inactive_default))
#define HDL_GPIO_OFF_WRAP(x)      ((x.inactive_default))

#define HDL_GPIO_SET_ACTIVE(gpio)     hdl_gpio_write(gpio, !((gpio)->inactive_default))
#define HDL_GPIO_SET_INACTIVE(gpio)   hdl_gpio_write(gpio, (gpio)->inactive_default)
#define HDL_GPIO_IS_ACTIVE(gpio)      (hdl_gpio_read(gpio) != (gpio)->inactive_default)

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
    int8_t counter_of_boot_retry;                  /* retains number of remaining boot attempt */       
} object_dictionary_t;

void device_logic(void);

#include "device_function.h"
#include "atb_3500_page_transfer.h"

#endif /* ATB_3500 */
#endif /* ATB_3500_DEVICE_LOGIC_H_ */
