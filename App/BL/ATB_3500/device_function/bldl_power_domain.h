#ifndef BLDL_POWER_DOMAIN_H_
#define BLDL_POWER_DOMAIN_H_

#define POWER_DOMAIN_CHANNELS_AMOUNT        6
#define POWER_DOMAIN_ADC_FILTER_LENGH 		7

#define ADC_REFERENCE_VOLTAGE_MV      ((uint32_t)2500)
#define ADC_RESOLUTION                ((uint32_t)4096)
#define ADC_CHANNEL_0_VOLTAGE_DIVIDER ((uint32_t)11)
#define ADC_CHANNEL_1_VOLTAGE_DIVIDER ((uint32_t)11)
#define ADC_CHANNEL_2_VOLTAGE_DIVIDER ((uint32_t)3)
#define ADC_CHANNEL_3_VOLTAGE_DIVIDER ((uint32_t)2)
#define ADC_CHANNEL_4_VOLTAGE_DIVIDER ((uint32_t)2)
#define ADC_CHANNEL_5_VOLTAGE_DIVIDER ((uint32_t)1)

#define TIMEOUT_MS_POE_24                       ((uint32_t) 5000)

/* ADC threshold */
#define ADC_24V_LOW_TRHESHOLD_VALUE_MV          ((uint16_t)21600)  /* -10% from 24v */
#define ADC_24V_HIGH_TRHESHOLD_VALUE_MV         ((uint16_t)26400)  /* +10% from 24v */
#define ADC_5V_LOW_TRHESHOLD_VALUE_MV           ((uint16_t)4500)   /* -10% from 5v */
#define ADC_5V_HIGH_TRHESHOLD_VALUE_MV          ((uint16_t)5500)   /* +10% from 5v */
#define ADC_3V3_LOW_TRHESHOLD_VALUE_MV          ((uint16_t)2970)   /* -10% from 3.3v */
#define ADC_3V3_HIGH_TRHESHOLD_VALUE_MV         ((uint16_t)3630)   /* +10% from 3.3v */
#define ADC_2V5_LOW_TRHESHOLD_VALUE_MV          ((uint16_t)2250)   /* -10% from 2.5v */
#define ADC_2V5_HIGH_TRHESHOLD_VALUE_MV         ((uint16_t)2750)   /* +10% from 2.5v */
#define ADC_1V8_LOW_TRHESHOLD_VALUE_MV          ((uint16_t)1620)   /* -10% from 1.8v */
#define ADC_1V8_HIGH_TRHESHOLD_VALUE_MV         ((uint16_t)2500)   /* +10% from 1.8v */

#define POWER_RAIL_24V_RAISE_DELAY              ((uint32_t)(5000))
#define POWER_RAIL_24V_STAB_DELAY               ((uint32_t)(100))

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
} power_domain_state_e;


#define BLDL_POWER_DOMAIN_PRV_SIZE    (POWER_DOMAIN_CHANNELS_AMOUNT * (4 * (4 + POWER_DOMAIN_ADC_FILTER_LENGH) + 3) + 4)

typedef struct {
    hdl_module_t module;
    uint32_t adc_scale[POWER_DOMAIN_CHANNELS_AMOUNT];
    hdl_adc_source_t *adc_src[POWER_DOMAIN_CHANNELS_AMOUNT];
    // TODO: options for stabilization time, threholds
    PRIVATE(bldl, BLDL_POWER_DOMAIN_PRV_SIZE);
} bldl_power_domain_t;

hdl_module_state_t power_domain(void *desc, uint8_t enable);
void power_domain_set(bldl_power_domain_t *desc, atb3500_power_domain_e domain, uint8_t enable);
uint8_t power_domain_event_subscribe(bldl_power_domain_t *desc, atb3500_power_domain_e domain, event_handler_t handler, void *context);

#endif // BLDL_POWER_DOMAIN_H_
