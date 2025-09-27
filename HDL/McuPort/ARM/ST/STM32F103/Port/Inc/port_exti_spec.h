#ifndef PORT_EXTI_SPEC_H_
#define PORT_EXTI_SPEC_H_

#define EXTI_LINES_4_15          (EXTI_LINE_4  | EXTI_LINE_5  | EXTI_LINE_6  | EXTI_LINE_7 | \
                                  EXTI_LINE_8  | EXTI_LINE_9  | EXTI_LINE_10 | EXTI_LINE_11 | \
                                  EXTI_LINE_12 | EXTI_LINE_13 | EXTI_LINE_14 | EXTI_LINE_15)

#if defined(EXTI_IMR_IM19)
#define EXTI_LINES_ALL            EXTI_LINE_0  | EXTI_LINE_1  | EXTI_LINE_2  | EXTI_LINE_3 | \
                                  EXTI_LINE_4  | EXTI_LINE_5  | EXTI_LINE_6  | EXTI_LINE_7 | \
                                  EXTI_LINE_8  | EXTI_LINE_9  | EXTI_LINE_10 | EXTI_LINE_11 | \
                                  EXTI_LINE_12 | EXTI_LINE_13 | EXTI_LINE_14 | EXTI_LINE_15 | \
                                  EXTI_LINE_16 | EXTI_LINE_17 | EXTI_LINE_19
#else 
#define EXTI_LINES_ALL            EXTI_LINE_0  | EXTI_LINE_1  | EXTI_LINE_2  | EXTI_LINE_3 | \
                                  EXTI_LINE_4  | EXTI_LINE_5  | EXTI_LINE_6  | EXTI_LINE_7 | \
                                  EXTI_LINE_8  | EXTI_LINE_9  | EXTI_LINE_10 | EXTI_LINE_11 | \
                                  EXTI_LINE_12 | EXTI_LINE_13 | EXTI_LINE_14 | EXTI_LINE_15 | \
                                  EXTI_LINE_16 | EXTI_LINE_17
#endif /* EXTI_IMR_IM19 */

typedef enum {
  HDL_EXTI_LINE_0 = EXTI_LINE_0,
  HDL_EXTI_LINE_1 = EXTI_LINE_1,
  HDL_EXTI_LINE_2 = EXTI_LINE_2,
  HDL_EXTI_LINE_3 = EXTI_LINE_3,
  HDL_EXTI_LINE_4 = EXTI_LINE_4,
  HDL_EXTI_LINE_5 = EXTI_LINE_5,
  HDL_EXTI_LINE_6 = EXTI_LINE_6,
  HDL_EXTI_LINE_7 = EXTI_LINE_7,
  HDL_EXTI_LINE_8 = EXTI_LINE_8,
  HDL_EXTI_LINE_9 = EXTI_LINE_9,
  HDL_EXTI_LINE_10 = EXTI_LINE_10,
  HDL_EXTI_LINE_11 = EXTI_LINE_11,
  HDL_EXTI_LINE_12 = EXTI_LINE_12,
  HDL_EXTI_LINE_13 = EXTI_LINE_13,
  HDL_EXTI_LINE_14 = EXTI_LINE_14,
  HDL_EXTI_LINE_15 = EXTI_LINE_15,
  HDL_EXTI_LINE_16 = EXTI_LINE_16,
  HDL_EXTI_LINE_17 = EXTI_LINE_17,
  HDL_EXTI_LINE_18 = EXTI_LINE_18,
#if defined(EXTI_IMR_IM19)
  HDL_EXTI_LINE_19 = EXTI_LINE_19,
#endif /* EXTI_IMR_IM19 */
} hdl_exti_line_t;

typedef enum {
  HDL_EXTI_SOURCE_PA = EXTI_GPIOA,
  HDL_EXTI_SOURCE_PB = EXTI_GPIOB,
  HDL_EXTI_SOURCE_PC = EXTI_GPIOC,
  HDL_EXTI_SOURCE_PD = EXTI_GPIOD,
  HDL_EXTI_SOURCE_PE = EXTI_GPIOE,
} hdl_exti_source_t;

#endif // PORT_EXTI_SPEC_H_