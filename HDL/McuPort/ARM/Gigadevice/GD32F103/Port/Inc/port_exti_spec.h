#ifndef PORT_EXTI_SPEC_H_
#define PORT_EXTI_SPEC_H_

/* cmsis compatibility define */
#define syscfg_exti_line_config   gpio_exti_source_select


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
} hdl_exti_line_t;

#endif // PORT_EXTI_SPEC_H_
