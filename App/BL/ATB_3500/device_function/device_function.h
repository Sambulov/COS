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

/* Corresponds to array adc index */



void indicator_init();
void device_relay_control(object_dictionary_t *od);


void connector_init();
void connector_lte_reset(uint8_t enable);


void watchdog_init();
void watchdog_reset();


void communication_init();


#endif /* ATB_3500 */
#endif /* ATB_3500_DEVICE_FUNCTION_H_ */