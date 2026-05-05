#ifndef HDL_MODBUS_TYPES_H_
#define HDL_MODBUS_TYPES_H_

typedef union {
    uint32_t abc;
  struct {
    union {
      uint8_t *u8;
      uint16_t *u16;
      int16_t *i16;
    } var_data;               /* Variable length payload data */
    uint8_t buffer_size;      /* For server response capability, var_data buffer width */
    uint8_t dev_addr;         /* Device address on bus */
    uint8_t func;             /* Function number */
    union {
      uint16_t reg_addr;
      uint16_t sub_function;
    };                        /* Register address & sub function share same field */
    union {
      uint16_t reg_value;
      uint16_t regs_amount;
    };                        /* Register value or amount share same field */
    union {
      uint16_t length;        /* low byte valuable */
      uint16_t code;          /* width depends on function */
      uint16_t error;         /* low byte valuable */
    };                        /* var_data length or code response or error */
  } common;

} hdl_modbus_frame_t;

typedef uint8_t (*hdl_modbus_custom_parser_t)(uint8_t *raw_data, uint16_t data_len, hdl_modbus_frame_t *frame);


#endif /* HDL_MODBUS_TYPES_H_ */
