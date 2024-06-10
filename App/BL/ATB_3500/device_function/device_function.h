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
uint32_t get_ms_time_from(uint32_t time_stamp_ms);

/*!
    \brief          Get current ms counter value
 */
uint32_t get_ms_time (void);

/*!
    \brief          Reset all field to default value
    \param[in]      h - pointer to struct
    \return         void
 */
void device_object_distonary_init_default(object_dictionary_t *od);

/*!
    \brief          Init all hardware module
    \param[in]      h - pointer to struct
    \return         
      \retval         DL_STATUS_SUCCES
      \retval         DL_STATUS_ERROR
 */
device_logic_status_e device_hardware_init(object_dictionary_t *od);

/*!
    \brief          Update and filter adc value
 */
void device_adc_proc(object_dictionary_t* h);

/*!
    \brief          This function check power status
 */
void device_check_power_status(object_dictionary_t* h);

/*!
    \brief          This function describe relay control
 */
void device_relay_control(object_dictionary_t *od);

uint8_t device_choose_smarc_boot(object_dictionary_t *od);
#endif /* ATB_3500 */
#endif /* ATB_3500_DEVICE_FUNCTION_H_ */