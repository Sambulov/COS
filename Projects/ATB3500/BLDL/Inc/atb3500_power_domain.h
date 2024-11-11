#ifndef ATB3500_POWER_DOMAIN_H_
#define ATB3500_POWER_DOMAIN_H_

#define POWER_DOMAIN_ADC_FILTER_LENGH 		    7

#define ADC_REFERENCE_VOLTAGE_MV                ((uint32_t)2500)
#define ADC_RESOLUTION                          ((uint32_t)4096)

#define TIMEOUT_MS_POE_24                       ((uint32_t) 5000)

#define POWER_RAIL_ADC_SCALE_24V                ((uint32_t)11)
#define POWER_RAIL_ADC_SCALE_24VPOE             ((uint32_t)11)
#define POWER_RAIL_ADC_SCALE_5V                 ((uint32_t)3)
#define POWER_RAIL_ADC_SCALE_3V3                ((uint32_t)2)
#define POWER_RAIL_ADC_SCALE_2V5                ((uint32_t)2)
#define POWER_RAIL_ADC_SCALE_1V8                ((uint32_t)1)

#define POWER_RAIL_RAISE_DELAY_24V              ((uint32_t)(1000))
#define POWER_RAIL_RAISE_DELAY_24VPOE           ((uint32_t)(1000))
#define POWER_RAIL_RAISE_DELAY_5V               ((uint32_t)(1000))
#define POWER_RAIL_RAISE_DELAY_3V3              ((uint32_t)(1000))
#define POWER_RAIL_RAISE_DELAY_2V5              ((uint32_t)(1000))
#define POWER_RAIL_RAISE_DELAY_1V8              ((uint32_t)(1000))

#define POWER_RAIL_STAB_DELAY_24V               ((uint32_t)(100))
#define POWER_RAIL_STAB_DELAY_24VPOE            ((uint32_t)(100))
#define POWER_RAIL_STAB_DELAY_5V                ((uint32_t)(100))
#define POWER_RAIL_STAB_DELAY_3V3               ((uint32_t)(100))
#define POWER_RAIL_STAB_DELAY_2V5               ((uint32_t)(100))
#define POWER_RAIL_STAB_DELAY_1V8               ((uint32_t)(100))

/* ADC threshold */
#define POWER_RAIL_UV_TRHESHOLD_24V             ((uint16_t)19000)  /*  */
#define POWER_RAIL_UV_TRHESHOLD_24VPOE          ((uint16_t)15000)  /* ~ -30% from 24v poe */
#define POWER_RAIL_UV_TRHESHOLD_5V              ((uint16_t)4500)   /* -10% from 5v */
#define POWER_RAIL_UV_TRHESHOLD_3V3             ((uint16_t)2970)   /* -10% from 3v3 */
#define POWER_RAIL_UV_TRHESHOLD_2V5             ((uint16_t)2250)   /* -10% from 2v5 */
#define POWER_RAIL_UV_TRHESHOLD_1V8             ((uint16_t)1620)   /* -10% from 1v8 */

#define POWER_RAIL_OV_TRHESHOLD_24V             ((uint16_t)26400)  /* +10% from 24v */
#define POWER_RAIL_OV_TRHESHOLD_24VPOE          ((uint16_t)26400)  /* +10% from 24v poe */
#define POWER_RAIL_OV_TRHESHOLD_5V              ((uint16_t)5500)   /* +10% from 5v */
#define POWER_RAIL_OV_TRHESHOLD_3V3             ((uint16_t)3630)   /* +10% from 3v3 */
#define POWER_RAIL_OV_TRHESHOLD_2V5             ((uint16_t)2750)   /* +10% from 2v5 */
#define POWER_RAIL_OV_TRHESHOLD_1V8             ((uint16_t)2500)   /* +10% from 1v8 */

typedef enum {
  PD_STATE_OFF,
  PD_STATE_ENABLED,
  PD_STATE_STABLE_DELAY,
  PD_STATE_STABLE,
  PD_STATE_FAULT
} power_domain_state_e;

#define ATB3500_POWER_RAIL_PRV_SIZE      ((6 + POWER_DOMAIN_ADC_FILTER_LENGH) * 4) + 24

typedef struct {
  uint32_t adc_scale;
  uint32_t adc_ch;
  uint32_t ov_threshold;
  uint32_t uv_threshold;
  uint32_t raise_delay;
  uint32_t stabilization_delay;
} atb3500_power_rail_config_t;

#define ATB3500_POWER_RAIL_TIMER            0
#define ATB3500_POWER_RAIL_ADC              1
#define ATB3500_POWER_RAIL_ADC_PIN          2
#define ATB3500_POWER_RAIL_SOURSE_RAIL      3
#define ATB3500_POWER_RAIL_EN_PIN           4
#define ATB3500_POWER_RAIL_FEEDBACK_FAULT   5
#define ATB3500_POWER_RAIL_FEEDBACK_GOOD    6
/*
  depends on:
  - timer
  - adc
  - adc_pin
  - source power_rail
  - enable_pin
*/
typedef struct {
  hdl_module_t module;
  const atb3500_power_rail_config_t *config;
  PRIVATE(bldl, ATB3500_POWER_RAIL_PRV_SIZE);
} atb3500_power_rail_t;

hdl_module_state_t atb3500_power_rail(void *desc, uint8_t enable);
void atb3500_power_rail_set(atb3500_power_rail_t *desc, uint8_t enable);
uint8_t atb3500_power_rail_is_stable(atb3500_power_rail_t *desc);
uint8_t atb3500_power_rail_event_subscribe(atb3500_power_rail_t *desc, hdl_event_handler_t handler, void *context);


#endif // ATB3500_POWER_DOMAIN_H_
