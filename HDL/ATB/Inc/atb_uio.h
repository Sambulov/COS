#ifndef _ATB_UIO_H_
#define _ATB_UIO_H_

/* TODO: calibration values */

typedef enum {
  /* Reserved */
  ATB_IO_PORT_RESERVED = 0xFF000000,
  /* ... */
  /* ========================================= */

  /* Operations */
  ATB_IO_PORT_OP_FLAGS           = 0x00F00000,
  
  /* Update port configuration, bit cleared when complete */
  ATB_IO_PORT_OP_RESET_CONFIG    = 0x00100000,
  
  /* Reset value:
     - For counter mode: subtract [option] amount from counter value, if [option] = 0, reset to 0 
     - For latch mode: reset value to current port state 
     bit cleared when complete */
  ATB_IO_PORT_OP_RESET           = 0x00200000,
  
  /* Enable latch mode. Input register holds min/max value depending on ATB_IO_PORT_HW_MODE */
  ATB_IO_PORT_OP_LATCH_MODE      = 0x00400000,

  /* ========================================= */

  /* Status flags mask */
  ATB_IO_PORT_STATUS_FLAGS       = 0x000F0000,

  /* Port hardware failue, because of some reason like:
     usupported config, module failue, etc.
     Consider IO value as invalid.  */
  ATB_IO_PORT_STATUS_HARD_ERROR  = 0x00010000,
  /* ... */

  /* ========================================= */
  /* HW configuration flags mask */
    ATB_IO_PORT_HW_CONFIG          = 0x0000FFF0,

  /* | ATB_IO_PORT_HW_DIRECTION | 1(output)      | 0(input)  |
     |--------------------------+----------------+-----------|
     | ATB_IO_PORT_HW_MODE      ||||||||||||||||||||||||||||||
     |        0                 | push-pull      | latch min |
     |        1                 | open collector | latch max |
  */
  ATB_IO_PORT_HW_DIRECTION       = 0x00001000,
  ATB_IO_PORT_HW_MODE            = 0x00002000,
  
  /* Port mode, discrete - 0, analog - 1 */
  ATB_IO_PORT_HW_ANALOG          = 0x00008000,
  
  /* Enable weak pull up */
  ATB_IO_PORT_HW_PULL_UP_WEAK    = 0x00000100,
  
  /* Enable strong pull up */
  ATB_IO_PORT_HW_PULL_UP_STRONG  = 0x00000200,
  
  /* Enable voltage divider */
  ATB_IO_PORT_HW_VOLTAGE_DIVIDER = 0x00000400,
  
  /* Enable current shunt */
  ATB_IO_PORT_HW_CURRENT_SHUNT   = 0x00000800,
  
  /* ... */
  ATB_IO_PORT_HW_OUTPUT          = ATB_IO_PORT_HW_DIRECTION,
  ATB_IO_PORT_HW_INPUT           = 0x00000000,
/* ========================================= */

  /* SW configuration */
  ATB_IO_PORT_SW_CONFIG          = 0x0000000F,

  /* Port value as discrete (bool) */
  ATB_IO_PORT_SW_DISCRETE        = 0x00000000,
  /* Port value as voltage (uint32_t) uV */
  ATB_IO_PORT_SW_VOLTAGE         = 0x00000001,
  /* Port value as current (uint32_t) uA */
  ATB_IO_PORT_SW_CURRENT         = 0x00000002,
  /* Port value as resistance (uint32_t) mOhm */
  ATB_IO_PORT_SW_RESISTANSE      = 0x00000003,
  /* Port value as counter for each signal front (uint32_t) */
  ATB_IO_PORT_SW_FRONT_COUNTER   = 0x00000004,
  /* Port value as counter for each signal pulse (uint32_t) */
  ATB_IO_PORT_SW_PULSE_COUNTER   = 0x00000005,
  /* Port value as frequency (float) */
  ATB_IO_PORT_SW_FREQUENCY       = 0x00000006,
  /* Port value as duty cycle (float), option as period time (float) */
  ATB_IO_PORT_SW_PWM             = 0x00000007,
  /* Port value as period time (float), option as duty cycle (float) */
  ATB_IO_PORT_SW_PPM             = 0x00000008,
  /* Port value as ADC value (uint32_t) */
  ATB_IO_PORT_SW_RAW_ADC         = 0x0000000F,
  /* ... */

  /* ========================================= */

  /* Some port config predefinitions */
  /* Measure voltage 0-10V */
  ATB_IO_PORT_AI_0_10V = ATB_IO_PORT_HW_INPUT | ATB_IO_PORT_HW_ANALOG | ATB_IO_PORT_HW_VOLTAGE_DIVIDER | ATB_IO_PORT_SW_VOLTAGE,
  /* Measure current 0-20mA */
  ATB_IO_PORT_AI_0_20MA = ATB_IO_PORT_HW_INPUT | ATB_IO_PORT_HW_ANALOG | ATB_IO_PORT_HW_CURRENT_SHUNT | ATB_IO_PORT_SW_CURRENT,
  /* Measure resistance, PT1000/NI1000 optimized */
  ATB_IO_PORT_AI_PT1000 = ATB_IO_PORT_HW_INPUT | ATB_IO_PORT_HW_ANALOG | ATB_IO_PORT_HW_PULL_UP_STRONG | ATB_IO_PORT_SW_RESISTANSE,
  /* Measure resistance */
  ATB_IO_PORT_AI_NTC = ATB_IO_PORT_HW_INPUT | ATB_IO_PORT_HW_ANALOG | ATB_IO_PORT_HW_PULL_UP_WEAK | ATB_IO_PORT_SW_RESISTANSE,
  /* Discrete input */
  ATB_IO_PORT_DI = ATB_IO_PORT_HW_INPUT | ATB_IO_PORT_HW_VOLTAGE_DIVIDER | ATB_IO_PORT_SW_DISCRETE,
  /* Pulse counter */
  ATB_IO_PORT_DI_COUNTER = ATB_IO_PORT_HW_INPUT | ATB_IO_PORT_HW_VOLTAGE_DIVIDER | ATB_IO_PORT_SW_PULSE_COUNTER,
  /* Measure frrquency */
  ATB_IO_PORT_DI_FREQ = ATB_IO_PORT_HW_INPUT | ATB_IO_PORT_HW_VOLTAGE_DIVIDER | ATB_IO_PORT_SW_FREQUENCY,
  /* Discrete output */
  ATB_IO_PORT_DO = ATB_IO_PORT_HW_OUTPUT | ATB_IO_PORT_SW_DISCRETE,
  /* Analog output voltage 0-10V */
  ATB_IO_PORT_AO_0_10V = ATB_IO_PORT_HW_OUTPUT | ATB_IO_PORT_HW_ANALOG | ATB_IO_PORT_SW_VOLTAGE,
  /* Analog output current 0-20mA */
  ATB_IO_PORT_AO_0_20MA = ATB_IO_PORT_HW_OUTPUT | ATB_IO_PORT_HW_ANALOG | ATB_IO_PORT_SW_CURRENT,

  /* Button mode example, input will be latched (trigger on low) */
  ATB_IO_PORT_BUTTON = ATB_IO_PORT_HW_INPUT | ATB_IO_PORT_OP_LATCH_MODE | ATB_IO_PORT_HW_PULL_UP_STRONG | ATB_IO_PORT_SW_DISCRETE,
  /* ... */

} AtbPortDescriptor_t;

typedef union {
  float rValue;
  uint32_t ulValue;
} AtbPortValue_t;

typedef struct {
  AtbPortDescriptor_t ePortDesc;
  AtbPortValue_t xDefault;
} AtbIOPortConfig_t;

#define ATB_IO_PORTS_COUNT    8 /* Specified to hardware */

/* EEPROM mapping */ 
typedef struct {
  uint32_t ulReserved; /* Reserved for future use */
  uint32_t ulPortsCount; 
  AtbIOPortConfig_t aePorts[ATB_IO_PORTS_COUNT];
} AtbIOConfig_t;

#define IN    /* Data flow from MPC USPD to host */
#define OUT   /* Data flow from host to MPC USPD */
#define INOUT /* Bidirectional fields */

typedef struct {
  INOUT AtbPortDescriptor_t ePortDesc;
  IN AtbPortValue_t xInput;
  OUT AtbPortValue_t xOutput;
  OUT AtbPortValue_t xOption;
} AtbIOPort_t;

/* MCU ports mapping */
extern AtbIOPort_t axIOPortRegisterMap[ATB_IO_PORTS_COUNT];

#endif // _ATB_UIO_H_