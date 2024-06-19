/*!
    \file device_function.h
    \author a.bodin
    
    \version 2024-06-04, V1.0.0, This file describe functions, which using, in the business logic
 */

#ifndef ATB_3500_DEVICE_FUNCTION_H_
#define ATB_3500_DEVICE_FUNCTION_H_

#if defined(ATB_3500)
/*!
    \brief          Function returns difference between timestamp and current ms counters
    \param[in]      time_stamp_ms
    \return         difference in ms
 */
__STATIC_INLINE uint32_t get_ms_time_from(uint32_t time_stamp_ms) {
    return hdl_timer_get(&mod_systick_timer_ms) - time_stamp_ms;
}
/*!
    \brief          Get current ms counter value
 */
__STATIC_INLINE uint32_t get_ms_time (void) {
  return hdl_timer_get(&mod_systick_timer_ms);
}



#define ADC_CHANNEL_AMOUNT  6
/* Corresponds to array adc index */
typedef enum {
    ATB3500_PD_24V     = (uint8_t) 0,
    ATB3500_PD_24V_POE = (uint8_t) 1,
    ATB3500_PD_5V      = (uint8_t) 2,
    ATB3500_PD_3V3     = (uint8_t) 3,
    ATB3500_PD_2V5     = (uint8_t) 4,
    ATB3500_PD_1V8     = (uint8_t) 5,
} atb3500_power_domain_e;

typedef enum {
    PD_STATE_OFF,
    PD_STATE_ENABLE,
    PD_STATE_STABLE_DELAY,
    PD_STATE_STABLE,
    PD_STATE_FAULT
} power_state_e;

//void power_domain_init();

void power_domain_init(void);
void power_domain_set(atb3500_power_domain_e domain, uint8_t enable);
uint8_t power_domain_is_stable(atb3500_power_domain_e domain);
uint8_t power_domain_event_subscribe(atb3500_power_domain_e domain, hdl_delegate_t *callback);


typedef enum {
    SMARC_BOOT0 = 0x01,
    SMARC_BOOT1 = 0x02,
    SMARC_BOOT2 = 0x04
} smarc_boot_select_e;

void smarc_init();
void smarc_boot_select(smarc_boot_select_e boot);
void smarc_standby_circuits_cb();
void smarc_runtime_circuits_cb();
void smarc_runtime_cb();

void indicator_init();
void device_relay_control(object_dictionary_t *od);


void connector_init();
void connector_lte_reset(uint8_t enable);


void watchdog_init();
void watchdog_reset();


void communication_init();


#endif /* ATB_3500 */
#endif /* ATB_3500_DEVICE_FUNCTION_H_ */