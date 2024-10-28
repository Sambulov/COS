#ifndef PORT_CLOCK_H_
#define PORT_CLOCK_H_

#define HDL_CLOCK_PRV_SIZE 8

typedef enum {
  HDL_CLOCK_TYPE_HXTAL,          /* property: freq (can be xx ~ yyMHz)*/
} hdl_clock_type_t;

typedef union {
  uint32_t freq;
  uint32_t mul;
  uint32_t div;
} hdl_clock_property_t;

typedef struct {
  hdl_clock_type_t type;
  hdl_clock_property_t property;
} hdl_clock_config_t;

#endif
