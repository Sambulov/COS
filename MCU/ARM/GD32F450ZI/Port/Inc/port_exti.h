#ifndef PORT_EXTI_H_
#define PORT_EXTI_H_

#define EXTI_LINES_ALL            EXTI_0  | EXTI_1  | EXTI_2  |  EXTI_3 | \
                                  EXTI_4  | EXTI_5  | EXTI_6  |  EXTI_7 | \
                                  EXTI_8  | EXTI_9  | EXTI_10 | EXTI_11 | \
                                  EXTI_12 | EXTI_13 | EXTI_14 | EXTI_15 | \
                                  EXTI_16 | EXTI_17 | EXTI_19

#define EXTI_LINES_5_9                      EXTI_5  | EXTI_6  | EXTI_7  | \
                                  EXTI_8  | EXTI_9

#define EXTI_LINES_10_15                              EXTI_10 | EXTI_11 | \
                                  EXTI_12 | EXTI_13 | EXTI_14 | EXTI_15

typedef enum {
  HDL_EXTI_TRIGGER_FALLING        = 0x01,
  HDL_EXTI_TRIGGER_RISING         = 0x02,
  HDL_EXTI_TRIGGER_RISING_FALLING = HDL_EXTI_TRIGGER_RISING | HDL_EXTI_TRIGGER_FALLING,
  HDL_EXTI_TRIGGER_NONE           = !(HDL_EXTI_TRIGGER_RISING | HDL_EXTI_TRIGGER_FALLING)
} hdl_exti_trigger_t;

typedef enum {
  HDL_EXTI_SOURCE_PA = 0b0000,
  HDL_EXTI_SOURCE_PB = 0b0001,
  HDL_EXTI_SOURCE_PC = 0b0010,
  HDL_EXTI_SOURCE_PD = 0b0011,
  HDL_EXTI_SOURCE_PE = 0b0100,
  HDL_EXTI_SOURCE_PF = 0b0101,
  HDL_EXTI_SOURCE_PG = 0b0110,
} hdl_exti_source_t;

typedef enum {
  HDL_EXTI_LINE_0 = EXTI_0,   /*!< EXTI line 0 PA0, PB0, PF0 */
  HDL_EXTI_LINE_1 = EXTI_1,   /*!< EXTI line 1 PA1, PB1, PF1 */
  HDL_EXTI_LINE_2 = EXTI_2,   /*!< EXTI line 2 PA2, PB2 */
  HDL_EXTI_LINE_3 = EXTI_3,   /*!< EXTI line 3 PA3, PB3 */
  HDL_EXTI_LINE_4 = EXTI_4,   /*!< EXTI line 4 PA4, PB4 */
  HDL_EXTI_LINE_5 = EXTI_5,   /*!< EXTI line 5 PA5, PB5 */
  HDL_EXTI_LINE_6 = EXTI_6,   /*!< EXTI line 6 PA6, PB6, PF6 */
  HDL_EXTI_LINE_7 = EXTI_7,   /*!< EXTI line 7 PA7, PB7, PF7 */
  HDL_EXTI_LINE_8 = EXTI_8,   /*!< EXTI line 8 PA8, PB8 */
  HDL_EXTI_LINE_9 = EXTI_9,   /*!< EXTI line 9 PA9, PB9 */
  HDL_EXTI_LINE_10 = EXTI_10, /*!< EXTI line 10 PA10, PB10 */
  HDL_EXTI_LINE_11 = EXTI_11, /*!< EXTI line 11 PA11, PB11 */
  HDL_EXTI_LINE_12 = EXTI_12, /*!< EXTI line 12 PA12, PB12 */
  HDL_EXTI_LINE_13 = EXTI_13, /*!< EXTI line 13 PA13, PB13, PC13 */
  HDL_EXTI_LINE_14 = EXTI_14, /*!< EXTI line 14 PA14, PB14, PC14 */
  HDL_EXTI_LINE_15 = EXTI_15, /*!< EXTI line 15 PA15, PB15, PC15 */
  HDL_EXTI_LINE_16 = EXTI_16, /*!< EXTI line 16 LVD */
  HDL_EXTI_LINE_17 = EXTI_17, /*!< EXTI line 17 RTC alarm */
  HDL_EXTI_LINE_18 = EXTI_18, /*!< EXTI line 18 Reserved */
  HDL_EXTI_LINE_19 = EXTI_19, /*!< EXTI line 19 RTC tamper and timestamp */
  HDL_EXTI_LINE_20 = EXTI_20, /*!< EXTI line 20 */
  HDL_EXTI_LINE_21 = EXTI_21, /*!< EXTI line 21 */
  HDL_EXTI_LINE_22 = EXTI_22, /*!< EXTI line 22 */
} hdl_exti_line_t;

typedef enum {
  HDL_EXTI_MODE_INTERRUPT = EXTI_INTERRUPT,
  HDL_EXTI_MODE_EVENT = EXTI_EVENT
} hdl_exti_mode_t;

typedef struct {
  hdl_exti_line_t line;
  hdl_exti_source_t source;
  hdl_exti_mode_t mode;
  hdl_exti_trigger_t trigger;
} hdl_exti_t;

/* 
  depends on:
  nvic
*/

typedef struct {
  hdl_module_t module;
  hdl_exti_t **extis;
} hdl_exti_controller_t;

#define hdl_extis(...) ((hdl_exti_t *[]){__VA_ARGS__, NULL})

hdl_module_state_t hdl_exti(void *desc, uint8_t enable);

void hdl_exti_request(hdl_exti_controller_t *exti);

void hdl_exti_sw_trigger(hdl_exti_controller_t *desc, hdl_exti_line_t line);

#endif // PORT_EXTI_H_
